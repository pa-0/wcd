/*
   filename: wcddir.c

   WCD - Chdir for Dos and Unix.

Author: Erwin Waterlander

======================================================================
= Copyright                                                          =
======================================================================
Copyright (C) 2002-2017 Erwin Waterlander

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

=======================================================================
*/

/* set tabstop=3 */

/* This file contains wrapper functions that operate on directories.
   These are needed to use the WIN32 API functions. WIN32 API functions
   are needed to get support for UNC paths.
*/

#include "config.h"

#include <stdlib.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#define S_ISDIR( m )    (((m) & _S_IFMT) == _S_IFDIR)
#endif
#if defined(UNIX) || defined(__DJGPP__) || defined(__OS2__)
# include <unistd.h>
#endif
#include "finddirs.h"
#include "wcddir.h"
#include "match.h"
#include <string.h>
#include <errno.h>
#include "wcd.h"

#ifdef _WCD_DOSFS
#  include <direct.h>
#endif

#if (defined(_WIN32) && defined(WCD_UNICODE))
#include <wchar.h>
#endif
#if (defined(__MSDOS__) && !defined(__DJGPP__)) || (defined(__OS2__) && !defined(__EMX__))
#  include <io.h> /* for unlink() */
#endif

#if (defined(_WIN32) || defined(__CYGWIN__))

/* WIN32:  Use WIN32 API to support UNC paths.
   CYGWIN: Use WIN32 API only to scan servers and find shared
           directories. The Cygwin POSIX interface already supports
           UNC paths directly. In Cygwin the Win32 api function don't work
           on cygwin (unix like) paths. */

#include "std_macr.h"
#include "structur.h"
#include "Text.h"
#include "nameset.h"
#include "wfixpath.h"
#include <string.h>
#include "display.h"
#include <windows.h>


/*
   int wcd_isServerPath (char* path)
   check if path is a possible UNC server path
   like \\servername
   RETURNS  1 if true
   RETURNS  0 if false
 */
int wcd_isServerPath (char* path)
{
   if ((strlen(path) > 2) && (wcd_is_slash(*path)) && (wcd_is_slash(*(path+1))) &&
       (strchr(path+2,'/') == NULL) && (strchr(path+2,'\\') == NULL))
      return(1);
   else
      return(0);
}

void exterr( void )
{
   /* This function was taken from Felix Kasza's Win32 Samples page
      at MVPS.ORG.  https://www.mvps.org/
      */
   char errbuf[2048], namebuf[2048];
   DWORD err;

   errbuf[0] = namebuf[0] = '\0';
   if ( WNetGetLastError( &err, errbuf, sizeof errbuf, namebuf, sizeof namebuf ) == NO_ERROR )
      print_error(_("%lu (\"%s\") reported by \"%s\".\n"),
         err, errbuf, namebuf );
   return;
}

#define MAX_NR_BUF 1000

int doEnum( int level, NETRESOURCE *pnr, nameset n )
{
   /* This function was taken from Felix Kasza's Win32 Samples page
      at MVPS.ORG.  https://www.mvps.org/
      */
   DWORD rc, rc2;
   HANDLE hEnum;
   DWORD count, bufsize, ui;
   NETRESOURCE buf[MAX_NR_BUF];
   /* const char *type; */
   char path[WCD_MAXPATH];

   rc = WNetOpenEnum( RESOURCE_GLOBALNET, RESOURCETYPE_DISK, 0, pnr, &hEnum );
   if ( rc == ERROR_ACCESS_DENIED ) {
      /* printf( "%-6.6s %-4.4s%*s  Error 5 -- access denied\n", "", "", level * 2, "" ); */
      print_msg( _("access denied.\n"));
      return 1;
   }

   if ( rc ) {
      rc2 = GetLastError();
      /* printf( "WNOE(): rc = %lu, gle = %lu\n", rc, rc2 ); */
      if ( rc2 == ERROR_EXTENDED_ERROR )
         exterr();
      return 0;
   }

   while ( 1 ) {
      count = (DWORD) -1L;
      bufsize = sizeof buf;
      rc = WNetEnumResource( hEnum, &count, buf, &bufsize );
      if ( rc != NO_ERROR )
         break;
      for ( ui = 0; (ui < count); ++ ui ) {
         switch ( buf[ui].dwDisplayType ) {
            case RESOURCEDISPLAYTYPE_DOMAIN:
               /* type = "domain"; */ break;
            case RESOURCEDISPLAYTYPE_GENERIC:
               /* type = "generic"; */ break;
            case RESOURCEDISPLAYTYPE_SERVER:
               /* type = "server"; */ break;
            case RESOURCEDISPLAYTYPE_SHARE:
               /* type = "share"; */
               print_msg( "%s\n", buf[ui].lpRemoteName );
               wcd_strncpy(path, buf[ui].lpRemoteName, WCD_MAXPATH);
               wcd_fixpath(path, WCD_MAXPATH);
               addToNamesetArray(textNew(path), n);
               break;
            default:
               /* type = "unknown"; */ break;
         }
         /* now we recurse if it's a container */
         if ( buf[ui].dwUsage & RESOURCEUSAGE_CONTAINER )
            doEnum( level + 1, &buf[ui], n );
      }
   }

   if ( rc != ERROR_NO_MORE_ITEMS ) { /* bad things */
      rc2 = GetLastError();
      /* printf( "WNER(): rc = %lu, gle = %lu\n", rc, rc2 ); */
      if ( rc2 == ERROR_EXTENDED_ERROR )
         exterr();
   }

   WNetCloseEnum( hEnum );
   return 1;
}


/*************************************************************
  *
  *  wcd_getshares(char *path, nameset n)
  *
  *  In case path points to a server this function will
  *  search for shared directories on that server and add
  *  the shared directory names to n.
  *************************************************************/
void wcd_getshares(char* path, nameset n)
{
   NETRESOURCE  nr;

   if ((path == NULL) || (n == NULL))
      return;

   if (wcd_isServerPath(path)) {
      /* an UNC path, possibly pointing to a server */
      /* WIN32 API needs backslashes. */
      *path = '\\';
      *(path+1) = '\\';

      print_msg(_("Searching for shared directories on server %s\n"), path);

      nr.dwScope       = RESOURCE_GLOBALNET;
      nr.dwType        = RESOURCETYPE_ANY;
      nr.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
      nr.dwUsage       = RESOURCEUSAGE_CONTAINER;
      nr.lpLocalName   = NULL;
      nr.lpRemoteName  = path ;
      nr.lpProvider    = NULL;

      doEnum( 0, &nr, n );
   }
   print_msg(_("Found %lu shared directories on server %s\n"), (unsigned long)getSizeOfNamesetArray(n), path);
}

#endif


#if (defined(_WIN32) && !defined(__CYGWIN__))
/* WIN32, not CYGWIN
   Use WIN32 API */


void wcd_PrintError(DWORD dw)
{
    /* Retrieve the system error message for the last-error code */

    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    /* Display the error message */

    /* MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK); */
    fprintf(stderr, "%s\n",(LPCTSTR)lpMsgBuf);

    LocalFree(lpMsgBuf);
}



/*
   int wcd_isSharePath (char* path)
   check if path is a possible UNC share path
   like \\servername\sharename
   RETURNS  1 if true
   RETURNS  0 if false
 */
int wcd_isSharePath (char* path)
{
   /* Assume the path has been fixed by wcd_fixpath(). Only forward slashes. */
   if ((strlen(path) > 2) && (wcd_is_slash(*path)) && (wcd_is_slash(*(path+1)))) {
      char *ptr = strchr(path+2,'/');
      if (ptr == NULL)
        return(0);
      else {
        ptr = strchr(ptr+1,'/');
        if (ptr == NULL)
           return(1);
        else
           return(0);
      }
   }
   else
     return(0);
}
/*
 * wcd_islink()
 * Check if directory is a symbolic link.
 * Returns: 1 if dir is a symlink, 0 if dir is not a symlink.
 */

int wcd_islink(const char *dir, int quiet)
{
   DWORD attrs;
   DWORD dw;
#ifdef WCD_UNICODE
   static wchar_t wstr[WCD_MAXPATH];

   if (utf8towcs(wstr, dir, WCD_MAXPATH) == (size_t)(-1))
      attrs = INVALID_FILE_ATTRIBUTES;
   else
      attrs = GetFileAttributesW(wstr);
#else
   attrs = GetFileAttributes(dir);
#endif

   if (attrs == INVALID_FILE_ATTRIBUTES) {
      if ( !quiet ) {
         dw = GetLastError();
         print_error("wcd_islink(): ");
         wcd_PrintError(dw);
      }
      return(0);
   }

   return ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
}


/* If WCD_UNICODE is defined we assume that all multi-byte
 * strings are encoded in UTF-8.
 */

char *wcd_getcwd(char *buf, size_t size)
{
   DWORD err;
   DWORD dw;
#ifdef WCD_UNICODE
   static wchar_t wstr[WCD_MAXPATH];

   err = GetCurrentDirectoryW((DWORD)size, wstr);
   if ( err != 0)
      if (wcstoutf8(buf, wstr, WCD_MAXPATH) == (size_t)(-1))
         err = 0;
#else
   err = GetCurrentDirectory(size, buf);
#endif

   if (err == 0) {
     dw = GetLastError();
     print_error("%s", _("Unable to get current working directory: "));
     wcd_PrintError(dw);
     return(NULL);  /* fail */
   }
   else
      return(buf);   /* success */
}

int wcd_chdir(const char *path, int quiet)
{
   BOOL err;
   DWORD dw;
#ifdef WCD_UNICODE
   static wchar_t wstr[WCD_MAXPATH];

   if (utf8towcs(wstr, path, WCD_MAXPATH) == (size_t)(-1))
      err = 0;
   else
      err = SetCurrentDirectoryW(wstr);
#else
   err = SetCurrentDirectory(path);
#endif

   if (err == 0) {
      if ( !quiet ) {
         dw = GetLastError();
         print_error("");
         wcd_printf(_("Unable to change to directory %s: "), path);
         wcd_PrintError(dw);
      }
      return(1);   /* fail */
   }
   else
      return(0);   /* success */
}

int wcd_mkdir(const char *path, int quiet)
{
   BOOL err;
   DWORD dw;
#ifdef WCD_UNICODE
   static wchar_t wstr[WCD_MAXPATH];

   if (utf8towcs(wstr, path, WCD_MAXPATH) == (size_t)(-1))
      err = FALSE;
   else
      err = CreateDirectoryW(wstr, NULL);
#else
   err = CreateDirectory(path, NULL);
#endif

   if (err == TRUE)
      return(0);  /* success */
   else {
     if ( !quiet ) {
       dw = GetLastError();
       print_error("");
       wcd_printf(_("Unable to create directory %s: "), path);
       wcd_PrintError(dw);
     }
     return(1);  /* fail */
   }
}

int wcd_rmdir(const char *path, int quiet)
{
   BOOL err;
   DWORD dw;
#ifdef WCD_UNICODE
   static wchar_t wstr[WCD_MAXPATH];

   if (utf8towcs(wstr, path, WCD_MAXPATH) == (size_t)(-1))
      err = FALSE;
   else
      err = RemoveDirectoryW(wstr);
#else
   err = RemoveDirectory(path);
#endif

   if (err == TRUE)
      return(0);  /* success */
   else {
     if ( !quiet ) {
       dw = GetLastError();
       print_error("");
       wcd_printf(_("Unable to remove directory %s: "), path);
       wcd_PrintError(dw);
     }
     return(1);  /* fail */
   }
}

int wcd_unlink(const char *path)
{
#ifdef WCD_UNICODE
   wchar_t pathw[WCD_MAXPATH];
   if (utf8towcs(pathw, path, WCD_MAXPATH) == (size_t)(-1))
      return -1;
   return _wunlink(pathw);
#else
   return unlink(path);
#endif
}


/******************************************************************
 *
 * int wcd_isdir(char *dir)
 *
 * test if *dir points to a directory.
 *
 * returns 1 when *dir is a directory path, 0 when it isn't.
 *
 * - The following method using POSIX API fails on UNC share paths
 *   like //servername/sharename on MS-Windows systems.
 *
 *       stat(path, &buf) ;
 *          if (S_ISDIR(buf.st_mode)) { ... }
 *
 * See also bug report at https://sourceforge.net/projects/mingw/
 *   https://sourceforge.net/tracker/?func=detail&atid=102435&aid=3304800&group_id=2435
 *
 * - The function 'opendir()' from <dirent.h> works on all systems,
 *   also on Windows UNC paths as above, but not all compilers have 'dirent'
 *   included. E.g. LCC 3.8 and Open Watcom 1.3 don't have it.
 *
 * - Using 'wcd_chdir()' is a portable solution to check \\servername\sharename.
 *
 ******************************************************************/
int wcd_isdir(char *dir, int quiet)
{

#ifdef WCD_UTF16
   BOOL err;
   DWORD dw;
   struct _stat buf;
#else
   struct stat buf;
#endif

   if (wcd_isSharePath(dir)) {
      char tmp[WCD_MAXDIR];
      wcd_getcwd(tmp, sizeof(tmp)); /* remember current dir */

      if (wcd_chdir(dir, quiet) == 0) { /* just try to change to dir */
        wcd_chdir(tmp, quiet); /* go back */
        return 1 ;
      }
      else
         return 0;
   } else {
#ifdef WCD_UTF16
      static wchar_t wstr[WCD_MAXPATH];
      if (utf8towcs(wstr, dir, WCD_MAXPATH) == (size_t)(-1))
         err = FALSE;
      else
         err = TRUE;

      if (err == TRUE) {
         if (_wstat(wstr, &buf) == 0) {
            if (S_ISDIR(buf.st_mode))
               return 1;
            else
               return(0);
         } else {
            if (!quiet) {
              char *errstr = strerror(errno);
              wcd_printf("Wcd: %s: %s\n", dir, errstr);
            }
            return 0;
         }
      } else {
        if ( !quiet ) {
          dw = GetLastError();
          wcd_printf("Wcd: %s: ", dir);
          wcd_PrintError(dw);
        }
        return 0;  /* fail */
      }
#else
      if (stat(dir, &buf) == 0) {
         if (S_ISDIR(buf.st_mode))
            return 1;
         else
            return(0);
      } else {
         if (!quiet) {
           char *errstr = strerror(errno);
           print_error("%s: %s\n", dir, errstr);
         }
         return 0;
      }
#endif
   }
}


#else   /************************************************************/

  /*  Use POSIX API  */

#if defined(UNIX) || defined(__DJGPP__) || defined(__EMX__)

int wcd_mkdir(const char *path, mode_t m, int quiet)
{
  int err = mkdir(path, m);

  if ( !quiet && err)
    print_error(_("Unable to create directory %s: %s\n"), path, strerror(errno));
  return(err);
}

#else

int wcd_mkdir(const char *path, int quiet)
{
  int err = mkdir(path);

  if ( !quiet && err)
    print_error(_("Unable to create directory %s: %s\n"), path, strerror(errno));
  return(err);
}

#endif

#ifdef UNIX

/* The $HOME directory can be mounted to a long absolute path by
 * a volume manager. Replace the absolute volume path by $HOME.
 */

char *replace_volume_path_HOME(char *buf, size_t size)
{
   static char *home = NULL;      /* value of $HOME env variable */
   static char home_abs[WCD_MAXPATH];  /* absolute volume path of $HOME */
   static char status = 0;
   static size_t  len_home = 0;
   static size_t  len_home_abs = 0;
   size_t i, len_buf;
   size_t j;
   char tmp[WCD_MAXPATH];
   static char pattern[WCD_MAXPATH];
   char *ptr1, *ptr2;


   if (buf == NULL)
      return(NULL);

   if ( status == 0 ) {
   /* not intialised. */
   /* initialize only once. */
      if ((home = getenv("HOME")) != NULL ) {
         getcwd(tmp, sizeof(tmp)); /* remember current dir */
         if (wcd_chdir(home,0) == 0) {
            if (getcwd(home_abs, sizeof(home_abs)) == NULL) {
               status = 3; /* fail */
            } else {
               if (strcmp(home,home_abs) == 0) {
                  status = 4; /* home and home_abs are equal */
               } else {
                  if ( ((ptr1 = strrchr(home,DIR_SEPARATOR)) != NULL) && ((ptr2 = strrchr(home_abs,DIR_SEPARATOR)) != NULL)) {
                     if (strcmp(ptr1,ptr2) == 0) {
                         /* Both paths have same user name. Strip user name, so that
                          * also paths from other users benefit if users have a common
                          * volume prefix. Depends on how Volume Manager was configured. */
                         *ptr1 = '\0';
                         *ptr2 = '\0';
                     }
                  }
                  wcd_strncpy(pattern, home_abs, sizeof(pattern));
                  wcd_strncat(pattern,"*", sizeof(pattern));
                  len_home = strlen(home);
                  len_home_abs = strlen(home_abs);
                  if (len_home <= len_home_abs) {
                     status = 1; /* home and home_abs are not equal */
                  } else {
                     status = 2; /* home and home_abs are not equal */
                  }
               }
            }
         wcd_chdir(tmp,0); /* go back */
         } else {
            status = 5; /* fail */
         }
      } else {
         status = 6; /* fail */
      }
      /* printf("status = %d\n", status); */
   }
   if ( status == 1 ) { /* $HOME is shorter or equal length than volume name */
      if (dd_match(buf, pattern , 0)) {
         len_buf = strlen(buf);
         for (i=0; i < len_home; i++)
            buf[i] = home[i];
         j = i;
         for (i=len_home_abs; i < len_buf; i++) {
            buf[j] = buf[i];
            ++j;
         }
         buf[j] = '\0';
      }
   }
   if ( status == 2 ) { /* $HOME is longer than volume name */
      if (dd_match(buf, pattern, 0)) {
         len_buf = strlen(buf);
         for (i=0; (i < len_home) && (i < size); i++)
            tmp[i] = home[i];
         j = i;
         for (i=len_home_abs; (i < len_buf) && (i < size); i++) {
            tmp[j] = buf[i];
            ++j;
         }
         tmp[j] = '\0';
         strcpy(buf,tmp);
      }
   }
   return(buf);
}
#endif

char *wcd_getcwd(char *buf, size_t size)
{
   char *err = getcwd(buf, size);
   if ( err == NULL) {
     char *errstr = strerror(errno);
     print_error(_("Unable to get current working directory: %s\n"), errstr);
   }
#ifdef UNIX
   else
     replace_volume_path_HOME(buf,size);
#endif
   return(err);
}

int wcd_chdir(const char *path, int quiet)
{
  int err = chdir(path);

  if (!quiet && err)
    print_error(_("Unable to change to directory %s: %s\n"), path, strerror(errno));
  return(err);
}

int wcd_rmdir(const char *path, int quiet)
{
  int err = rmdir(path);

  if (!quiet && err)
    print_error(_("Unable to remove directory %s: %s\n"), path, strerror(errno));
  return(err);
}

int wcd_unlink(const char *path)
{
   return unlink(path);
}

/******************************************************************
 *
 * int wcd_isdir(char *dir)
 *
 * test if *dir points to a directory.
 *
 * returns 1 when *dir is a directory path, 0 when it isn't.
 *
 ******************************************************************/
int wcd_isdir(char *dir, int quiet)
{
   struct stat buf;

   if (stat(dir, &buf) == 0) {
      if (S_ISDIR(buf.st_mode))
         return 1;
      else
         return 0;
   } else {
      if (!quiet)
         print_error("%s: %s\n", dir, strerror(errno));
      return 0;
   }
}

#endif
