/*
   filename: wcddir.c

   WCD - Chdir for Dos and Unix.

Author: Erwin Waterlander

======================================================================
= Copyright                                                          =
======================================================================
Copyright (C) 2002-2006 Erwin Waterlander

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

=======================================================================
*/

/* set tabstop=3 */

/* This file contains wrapper functions that operate on directories.
   These are needed to use the WIN32 API functions. WIN32 API functions
   are needed to get support for UNC paths.
*/

#include "config.h"

#if defined(UNIX) || defined(DJGPP) || defined(__OS2__)
# include <unistd.h>
#endif
#include "dosdir.h"

#if (defined(WIN32) && defined(WCD_UNICODE))
#include <wchar.h>
#endif

#if (defined(WIN32) || defined(__CYGWIN__))

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
#include "dosdir.h"
#include <string.h>
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
      at MVPS.ORG.  http://www.mvps.org/win32/
      */
   char errbuf[2048], namebuf[2048];
   DWORD err;

   errbuf[0] = namebuf[0] = '\0';
   if ( WNetGetLastError( &err, errbuf, sizeof errbuf, namebuf, sizeof namebuf ) == NO_ERROR )
      printf( _("Wcd: Error %lu (\"%s\") reported by \"%s\".\n"),
         err, errbuf, namebuf );
   return;
}

#define MAX_NR_BUF 1000

int doEnum( int level, NETRESOURCE *pnr, nameset n )
{
   /* This function was taken from Felix Kasza's Win32 Samples page
      at MVPS.ORG.  http://www.mvps.org/win32/
      */
   DWORD rc, rc2;
   HANDLE hEnum;
   DWORD count, bufsize, ui;
   NETRESOURCE buf[MAX_NR_BUF];
   const char *type;
   char path[DD_MAXPATH];

   rc = WNetOpenEnum( RESOURCE_GLOBALNET, RESOURCETYPE_DISK, 0, pnr, &hEnum );
   if ( rc == ERROR_ACCESS_DENIED )
   {
      /* printf( "%-6.6s %-4.4s%*s  Error 5 -- access denied\n", "", "", level * 2, "" ); */
      printf( _("Wcd: access denied.\n"));
      return 1;
   }

   if ( rc )
   {
      rc2 = GetLastError();
      /* printf( "WNOE(): rc = %lu, gle = %lu\n", rc, rc2 ); */
      if ( rc2 == ERROR_EXTENDED_ERROR )
         exterr();
      return 0;
   }

   while ( 1 )
   {
      count = (DWORD) -1L;
      bufsize = sizeof buf;
      rc = WNetEnumResource( hEnum, &count, buf, &bufsize );
      if ( rc != NO_ERROR )
         break;
      for ( ui = 0; (ui < count); ++ ui )
      {
         switch ( buf[ui].dwDisplayType )
         {
            case RESOURCEDISPLAYTYPE_DOMAIN:
               type = "domain"; break;
            case RESOURCEDISPLAYTYPE_GENERIC:
               type = "generic"; break;
            case RESOURCEDISPLAYTYPE_SERVER:
               type = "server"; break;
            case RESOURCEDISPLAYTYPE_SHARE:
               type = "share"; 
               printf( "Wcd: %s\n", buf[ui].lpRemoteName );
               strncpy(path, buf[ui].lpRemoteName, DD_MAXPATH);
               wcd_fixpath(path, DD_MAXPATH);
               addToNamesetArray(textNew(path), n);
               break;
            default:
               type = "unknown"; break;
         }
         // now we recurse if it's a container
         if ( buf[ui].dwUsage & RESOURCEUSAGE_CONTAINER )
            doEnum( level + 1, &buf[ui], n );
      }
   }

   if ( rc != ERROR_NO_MORE_ITEMS )  // bad things
   {
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
   
   if (wcd_isServerPath(path))
   {
      /* an UNC path, possibly pointing to a server */
      /* WIN32 API needs backslashes. */
      *path = '\\';
      *(path+1) = '\\';

      printf(_("Wcd: Searching for shared directories on server %s\n"), path);
      
      nr.dwScope       = RESOURCE_GLOBALNET;
      nr.dwType        = RESOURCETYPE_ANY;
      nr.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
      nr.dwUsage       = RESOURCEUSAGE_CONTAINER;
      nr.lpLocalName   = NULL;
      nr.lpRemoteName  = path ;
      nr.lpProvider    = NULL;
      
      doEnum( 0, &nr, n );
   }
   printf(_("Wcd: Found %d shared directories on server %s\n"), getSizeOfNamesetArray(n), path);
}

#endif


#if (defined(WIN32) && !defined(__CYGWIN__))
/* WIN32, not CYGWIN
   Use WIN32 API */

/* If WCD_UNICODE is defined we assume that all multi-byte
 * strings are encoded in UTF-8.
 */

char *wcd_getcwd(char *buf, int size)
{
   BOOL err;
#ifdef WCD_UNICODE
   static wchar_t wstr[DD_MAXPATH];

   err = GetCurrentDirectoryW(size, wstr);
   if ( err != 0)
      if (wcstoutf8(buf, wstr, DD_MAXPATH) < 0)
         err = 0;
#else
   err = GetCurrentDirectory(size, buf);
#endif

   if (err == 0)
      return(NULL);  /* fail */
   else
      return(buf);   /* success */
}

int wcd_chdir(char *buf)
{
   BOOL err;
#ifdef WCD_UNICODE
   static wchar_t wstr[DD_MAXPATH];

   if (utf8towcs(wstr, buf, DD_MAXPATH) < 0)
      err = 0;
   else
      err = SetCurrentDirectoryW(wstr); 
#else
   err = SetCurrentDirectory(buf); 
#endif

   if (err == 0)
      return(1);   /* fail */
   else
      return(0);   /* success */
}

int wcd_mkdir(char *buf)
{
   BOOL err;
#ifdef WCD_UNICODE
   static wchar_t wstr[DD_MAXPATH];

   if (utf8towcs(wstr, buf, DD_MAXPATH) < 0)
      err = FALSE;
   else
      err = CreateDirectoryW(wstr, NULL);
#else
   err = CreateDirectory(buf, NULL);
#endif

   if (err == TRUE)
      return(0);  /* success */
   else
      return(1);  /* fail */
}

int wcd_rmdir(char *buf)
{
   BOOL err;
#ifdef WCD_UNICODE
   static wchar_t wstr[DD_MAXPATH];

   if (utf8towcs(wstr, buf, DD_MAXPATH) < 0)
      err = FALSE;
   else
      err = RemoveDirectoryW(wstr);
#else
   err = RemoveDirectory(buf);
#endif

   if (err == TRUE)
      return(0);  /* success */
   else
      return(1);  /* fail */
}

/******************************************************************
 *
 * int wcd_isdir(char *dir)
 *
 * test if *dir points to a directory.
 *
 * returns 0 on success, -1 when it fails.
 *
 * - The following method using POSIX API fails on UNC share paths
 *   like //servername/sharename on MS-Windows systems.
 *
 *       stat(path, &buf) ;
 *          if (S_ISDIR(buf.st_mode)) { ... }
 *
 * - The function 'opendir()' from <dirent.h> works on all systems, 
 *   also on Windows UNC paths as above, but not all compilers have 'dirent'
 *   included. E.g. LCC 3.8 and Open Watcom 1.3 don't have it.
 *
 * - Using 'wcd_chdir()' is a portable solution.
 *
 ******************************************************************/
int wcd_isdir(char *dir)
{
   char tmp[DD_MAXDIR];

   wcd_getcwd(tmp, sizeof(tmp)); /* remember current dir */

   if (wcd_chdir(dir) == 0) /* just try to change to dir */
   {
     wcd_chdir(tmp); /* go back */
     return(0);
   }
   else
      return(-1);
}


#else   /************************************************************/

  /*  Use POSIX API  */

#if defined(UNIX) || defined(DJGPP) || defined(__OS2__)

int wcd_mkdir(char *buf, mode_t m)
{
   return(mkdir(buf, m));
}

#else

int wcd_mkdir(char *buf)
{
   return(mkdir(buf));
}

#endif

char *wcd_getcwd(char *buf, int size)
{
   return(getcwd(buf, size));
}

int wcd_chdir(char *buf)
{
   return(chdir(buf)); 
}

int wcd_rmdir(char *buf)
{
   return(rmdir(buf)); 
}

/******************************************************************
 *
 * int wcd_isdir(char *dir)
 *
 * test if *dir points to a directory.
 *
 * returns 0 on success, -1 when it fails.
 *
 ******************************************************************/
int wcd_isdir(char *dir)
{
   struct stat buf;

   if ((stat(dir, &buf) == 0) && S_ISDIR(buf.st_mode))
      return(0); 
   else
      return(-1);
}


#endif

