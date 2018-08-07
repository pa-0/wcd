/*
   filename: WCD.C

   WCD - Chdir for Dos and Unix.


Author: Erwin Waterlander

E-mail  : waterlan@xs4all.nl
WWW     : http://waterlan.home.xs4all.nl/

======================================================================
= Copyright                                                          =
======================================================================
Copyright (C) 1996-2018 Erwin Waterlander

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

Jason Mathews' file filelist.c was a starting point for this file.


*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#if !defined(__TURBOC__) && !defined(_MSC_VER)
# include <unistd.h>
#endif
#ifdef __TURBOC__
#define __FLAT__
#endif
#include <sys/stat.h>
#ifdef _MSC_VER
#include <malloc.h>
#define S_ISDIR( m )    (((m) & _S_IFMT) == _S_IFDIR)
#define S_ISREG( m )    (((m) & _S_IFMT) == _S_IFREG)
#endif
#ifdef __DJGPP__
# include <dir.h>
#endif
#include "wcd.h"

#ifndef __MSDOS__
/* Win32, OS/2, Unix, Cygwin */
#include <locale.h>
#endif

/* MSYS1 has no langinfo.h and does not support Unix locales */
/* MSYS1 has __GNUC__ = 3, MSYS2 has __GNUC__ = 4 */
#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__) && !(defined(__MSYS__) && (__GNUC__ == 3))/* Unix, Cygwin, MSYS2 */
# include <langinfo.h>
#endif

#include "querycp.h"
#include "finddirs.h"
#include "match.h"
#ifdef WCD_UNICODE
#  include "matchw.h"
#else
#  include "matchl.h"
#endif
#include "std_macr.h"
#include "structur.h"
#include "Error.h"
#include "Text.h"
#include "nameset.h"
#include "WcdStack.h"
#include "dirnode.h"
#include "stack.h"
#include "display.h"
#include "wfixpath.h"
#include "graphics.h"
#include "wcddir.h"
#include "config.h"
#if defined(_WIN32) || defined(WCD_UNICODE)
#  include <wchar.h>
#endif
#if defined (_WIN32) && !defined(__CYGWIN__)
#  include <windows.h>
#endif

#ifdef __CYGWIN__
#  undef _WIN32
#endif

/* Global variables */

/* Disable wildcard globbing */
#ifdef __MINGW32__
# ifdef __MINGW64__
   int _dowildcard = 0;
# else
   int _CRT_glob = 0;
# endif
#endif
#ifdef __DJGPP__
#include <crt0.h>

 char **__crt0_glob_function (char *arg)
 {
   return 0;
 }
#endif

/* Copy string src to dest, and null terminate dest.
   dest_size must be the buffer size of dest. */
char *wcd_strncpy(char *dest, const char *src, size_t dest_size)
{
   strncpy(dest,src,dest_size);
   dest[dest_size-1] = '\0';
#if DEBUG
   if(strlen(src) > (dest_size-1)) {
     print_error("Text %s has been truncated from %d to %d characters in %s to prevent a buffer overflow.\n", src, (int)strlen(src), (int)dest_size, "wcd_strncpy()");
   }
#endif
   return dest;
}

/* Append string src to dest, and null terminate dest.
   dest_size must be the buffer size of dest. */
char *wcd_strncat(char *dest, const char *src, size_t dest_size)
{
#if DEBUG
   if((strlen(dest) + strlen(src)) > (dest_size-1)) {
     print_error("Text %s + %s has been truncated to %d characters in %s to prevent a buffer overflow.\n", dest,  src, (int)dest_size,"wcd_strncat()");
   }
#endif
   strncat(dest,src,dest_size-strlen(dest));
   dest[dest_size-1] = '\0';
   return dest;
}

int print_msg(const char *format, ...)
{
   va_list args;
   int rc;

   printf("Wcd: ");
   va_start(args, format);
   rc = vprintf(format, args);
   va_end(args);

   return(rc);
}

int print_error(const char *format, ...)
{
   va_list args;
   int rc;

   fprintf(stderr,_("Wcd: error: "));
   va_start(args, format);
   rc = vfprintf(stderr, format, args);
   va_end(args);

   return(rc);
}

void wcd_read_error(const char *filename)
{
    char *errstr;

    errstr = strerror(errno);
    print_error(_("Unable to read file %s: %s\n"), filename, errstr);
}


/* Use wcd_fprintf() when we write to files. To get an
 * error message when the disk is full or quota is exceeded.

 Because the output is buffered, you may not get the disk-full error during
 fprintf(), but during fclose() when the buffer is flushed to disk. When you
 write a lot of data, more than fits in the buffer, the buffer is flushed in
 the meantime, before closing the file and you get the disk-full error during
 fprintf(). You may not get the error message during fclose() then. Therefore
 we have to check for errors during writing to file AND when the file is
 closed.
 */
int wcd_fprintf(FILE *stream, const char *format, ...)
{
   va_list args;
   int rc;

   va_start(args, format);
   rc = vfprintf(stream, format, args);
   va_end(args);

   if (rc < 0)
   {
      char *errstr = strerror(errno);
      print_error("%s\n", errstr);
   }

   return(rc);
}

FILE *wcd_fopen(const char *filename, const char *m, int quiet)
{
  struct stat buf;
  FILE *f;
  char *errstr;
  int loc_quiet;

#if DEBUG
  fprintf(stderr, "Wcd: wcd_fopen: Opening file \"%s\" mode=%s quiet=%d\n",filename, m, quiet);
  loc_quiet = 0;
#else
  loc_quiet = quiet;
#endif

  if (m[0] == 'r') /* we try to read an existing file */
  {
    if (stat(filename, &buf) != 0) /* check if file exists */
    {
      if ( !loc_quiet )
      {
        errstr = strerror(errno);
        print_error(_("Unable to read file %s: %s\n"), filename, errstr);
      }
      return(NULL);
    }

    if (!S_ISREG(buf.st_mode)) /* check if it is a file */
    {
      if ( !loc_quiet )
      {
        print_error(_("Unable to read file %s: Not a regular file.\n"), filename);
      }
      return(NULL);
    }
  }

  f = fopen(filename, m); /* open the file */
  if ( !loc_quiet && (f == NULL))
  {
    errstr = strerror(errno);
    if (m[0] == 'r')
      print_error(_("Unable to read file %s: %s\n"), filename, errstr);
    else
      print_error(_("Unable to write file %s: %s\n"), filename, errstr);
  }
#if DEBUG
  fprintf(stderr, "Wcd: wcd_fopen: Opening file \"%s\" OK.\n", filename);
#endif
  return(f);
}

/* wcd_fopen_type, similar as wcd_fopen, but also returns file
 * type based on Unicode BOM.
   Returns file pointer or NULL in case of a read error */

FILE *wcd_fopen_bom(const char *filename, const char *m, int quiet, int *bomtype)
{
  FILE *f;
  /* BOMs
   * UTF16-LE  ff fe
   * UTF16-BE  fe ff
   * UTF-8     ef bb bf
   */

   *bomtype = FILE_MBS;

   if ((f = wcd_fopen(filename, m, quiet)) == NULL) return NULL;

   /* Check for BOM */
   if (m[0] == 'r')
   {
      int bom[3];
      if ((bom[0] = fgetc(f)) == EOF) {
         if (ferror(f)) goto read_failed;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] != 0xff) && (bom[0] != 0xfe) && (bom[0] != 0xef)) {
         if (ungetc(bom[0], f) == EOF) goto read_failed;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[1] = fgetc(f)) == EOF) {
         if (ferror(f)) goto read_failed;
         if (ungetc(bom[1], f) == EOF) goto read_failed;
         if (ungetc(bom[0], f) == EOF) goto read_failed;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xff) && (bom[1] == 0xfe)) { /* UTF16-LE */
         *bomtype = FILE_UTF16LE;
         return(f);
      }
      if ((bom[0] == 0xfe) && (bom[1] == 0xff)) { /* UTF16-BE */
         *bomtype = FILE_UTF16BE;
         return(f);
      }
      if ((bom[2] = fgetc(f)) == EOF) {
         if (ferror(f)) goto read_failed;
         if (ungetc(bom[2], f) == EOF) goto read_failed;
         if (ungetc(bom[1], f) == EOF) goto read_failed;
         if (ungetc(bom[0], f) == EOF) goto read_failed;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xef) && (bom[1] == 0xbb) && (bom[2]== 0xbf)) { /* UTF-8 */
         *bomtype = FILE_UTF8;
         return(f);
      }
      if (ungetc(bom[2], f) == EOF) goto read_failed;
      if (ungetc(bom[1], f) == EOF) goto read_failed;
      if (ungetc(bom[0], f) == EOF) goto read_failed;
      *bomtype = FILE_MBS;
      return(f);
   }
  return(f);

  read_failed:
    wcd_read_error(filename);
    wcd_fclose(f, filename, "r", "wcd_fopen_bom: ");
    return NULL;
}


void wcd_fclose(FILE *fp, const char *filename, const char *m, const char *functionname)
{
   if (fclose(fp) != 0)
   {
     char *errstr = strerror(errno);
     if (m[0] == 'w')
       print_error(_("Unable to write file %s: %s\n"), filename, errstr);
     else
       print_error(_("Unable to close file %s: %s\n"), filename, errstr);
   }
#if DEBUG
   else
     fprintf(stderr, "Wcd: %sClosing file \"%s\" OK.\n", functionname, filename);
#endif
}

/********************************************************************
 * void cleanPath(char path[], int len, minlength)
 *
 * Clean path string from ending '/' character.
 *
 * Borland's chdir() function cannot change to paths ending with '/'
 * or '\' except root itself (/ or \).
 *
 ********************************************************************/

void cleanPath(char path[], int len, int minlength)
{
   /* strip ending separator (if present) */
   if (len > minlength && path[len-1]==DIR_SEPARATOR && path[len-2]!=':') path[len-1] = '\0';
}

/* trimPath(char* path, size_t n)
 * Remove n characters from beginning of path.
 */
void trimPath(char* path, size_t n)
{
  if ((path != NULL) && (n <= strlen(path)))
  {
       char* dest = path;
       char* src = path + n;
       while (*src)
          *dest++ = *src++;
       *dest = *src;
  }
}

#ifdef _WCD_DOSFS
void rmDriveLetter(char path[], int *use_HOME)
{
   if (path == NULL) return;

   /* remove drive letter */
   if (*use_HOME == 0 ) {
      char *ptr;
      if ( (ptr=strstr(path,"/")) != NULL)
         trimPath(path,ptr-path);
   }
}
#endif

void rmDirFromList(char *string, nameset n)
{
   size_t len = strlen(string) + 1;
   char *dir = (char *)malloc(len);
   char *subdir;
   size_t i;

   if (dir==NULL) {
      print_error(_("Memory allocation error in %s: %s\n"),"rmDirFromList()",strerror(errno));
      return;
   }
   subdir = (char *)malloc(len+2);
   if (subdir==NULL) {
      print_error(_("Memory allocation error in %s: %s\n"),"rmDirFromList()",strerror(errno));
      free(dir);
      return;
   }
   wcd_strncpy(dir,string,len);

   wcd_fixpath(dir,len);

   wcd_strncpy(subdir,dir,len+2);
   wcd_strncat(subdir,"/*",len+2);

   i = 0;
   while (i < n->size )
   {
#ifdef _WCD_DOSFS
      if( dd_match(n->array[i],dir,1) || dd_match(n->array[i],subdir,1))
#else
      if( dd_match(n->array[i],dir,0) || dd_match(n->array[i],subdir,0))
#endif
       removeElementAtNamesetArray(i,n,true);
      else
      ++i;
   }
   free(subdir);
   free(dir);
}

/********************************/
void writeList(char * filename, nameset n, int bomtype)
{
   FILE *outfile;

   if ( (outfile = wcd_fopen(filename,"w",0)) != NULL)
   {
#ifdef WCD_UTF16
      wcd_fprintf(outfile, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
#endif
#ifdef WCD_ANSI
      /* non-Unicode Windows version */
      /* When the treefile was in Unicode the non-Unicode Windows version of wcd
         translated the Unicode directory names to the system default ANSI code page. */
      if (bomtype > 0) /* Unicode, write back in UTF-8 */
        wcd_fprintf(outfile, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
#endif
      size_t i;
      int rc = 0;
#ifdef WCD_ANSI
      char    path[WCD_MAXPATH];
      wchar_t pathw[WCD_MAXPATH];
#endif
      for(i=0;((i<n->size)&&(rc>=0));i++)
      {
#ifdef WCD_ANSI
         if (bomtype > 0)
         {
           /* Unicode, write back in UTF-8.
              Characters that are not supported by the ANSI code page get broken. */
           wcd_strncpy(path, n->array[i], sizeof(path));
           MultiByteToWideChar(CP_ACP, 0, path, -1, pathw, sizeof(pathw));
           WideCharToMultiByte(CP_UTF8, 0, pathw, -1, path, sizeof(path), NULL, NULL);
           rc = wcd_fprintf(outfile,"%s\n",path);
         }
         else
           rc = wcd_fprintf(outfile,"%s\n",n->array[i]);
#else
         rc = wcd_fprintf(outfile,"%s\n",n->array[i]);
#endif
      }
      wcd_fclose(outfile, filename, "w", "writeList: ");
   }
}
#ifdef UNIX
/*************************************************************
 * void stripTmpMnt(char* path)
 *
 * This function stips /tmp_mnt from path if it begins with it.
 * /tmp_mnt is used by the automounter.
 *
 *************************************************************/
void stripTmpMnt(char* path)
{
   if (path != NULL)
   {
      if (strncmp(path,TMP_MNT_STR,strlen(TMP_MNT_STR)) == 0)
         trimPath(path,strlen(TMP_MNT_STR) - 1);
   }
}
#endif

#ifdef WCD_UNIXSHELL

/*************************************************************
 *
 * void quoteString(char *string)
 *
 * -----
 * quote all metacharacters (; & $ ( ) | < > space), quotes ("),
 * apostrophes ('), grave accents (`), wildcard characters
 * (* ? [ ]), and backslashes (\) with a backslash.
 *
 * Quoting every character with a backslash seems to be
 * the most portable solution. Using apostrophes ('...') to quote the whole
 * string makes it impossible to quote an apostrophe in the path. Using quotes
 * ("...") does not work well in csh, because you can't escape a $ character
 * inside quotes to stop variable substitution, and you can't escape a grave
 * accent.
 *
 * Functions is_slash() and is_term() in file wfixpath.c have also
 * been modified so that a backslash is not replaced by slash anymore
 * on UNIX systems.
 *
 * Erwin Waterlander Oct 17 2001
 *************************************************************/

void quoteString(char *string)
{
 size_t i, j;
 int k,kmax;
 char help1_str[WCD_MAXPATH];

 j = strlen(string);
 k = 0;
 kmax = WCD_MAXPATH -3;

 for (i=0; (i < j)&&(k < kmax) ; i++)
 {
   if ( (string[i] == '"') ||
        (string[i] == '$') ||
        (string[i] == '&') ||
        (string[i] == '\'') ||
        (string[i] == '(') ||
        (string[i] == ')') ||
        (string[i] == '*') ||
        (string[i] == ';') ||
        (string[i] == '<') ||
        (string[i] == '>') ||
        (string[i] == '?') ||
        (string[i] == '[') ||
        (string[i] == '\\') ||
        (string[i] == ']') ||
        (string[i] == ' ') ||
        (string[i] == '`') ||
        (string[i] == '|')
      )
   {
      help1_str[k] = '\\';
     k++;
   }
   help1_str[k] = string[i];
   k++ ;
 }
 help1_str[k] = '\0' ;

 wcd_strncpy(string,help1_str,(size_t)WCD_MAXPATH);
}
#endif

/*************************************************************
 *
 * void quoteString(char *string)
 *
 * For cmd.exe on Windows and OS/2, and Windows PowerShell.
 * We need backslashes instead of forward slashes.
 *
 * -----
 * Quoting ("...") is needed for paths that contain '&' characters
 * Quote '%' characters with '%', needed when cd command is executed
 * via batch file.
 * Oct 16 2001
 *
 *************************************************************/
#if (defined(_WIN32) && !defined(WCD_WINZSH)) || (defined(__OS2__) && !defined(WCD_OS2BASH))
void quoteString(char *string)
{
 size_t i, j;
 int k,kmax;
 char help1_str[WCD_MAXPATH];

 j = strlen(string);
 help1_str[0] = '"';
 k = 1;
 kmax = WCD_MAXPATH -3;

 for (i=0; (i < j)&&(k < kmax) ; i++)
 {
#ifndef WCD_WINPWRSH
   if (string[i] == '%')
   {
      help1_str[k] = '%';
     k++;
   }
#endif

   if (string[i] == '/')
      help1_str[k] = '\\';
   else
      help1_str[k] = string[i];
   k++ ;
 }
 help1_str[k] = '"' ;
 k++;
 help1_str[k] = '\0' ;

 wcd_strncpy(string,help1_str,(size_t)WCD_MAXPATH);

}
#endif

/*************************************************************
 *
 *  changeDisk
 *
 *  Returns:
 *  On succes : the new drive number >=0, or the current
 *              disk number >=0 if there was no drive to go to.
 *  Fail      : -1
 *
 *************************************************************/

#ifdef _WCD_DOSFS
int changeDisk(char *path, int *changed, char *newdrive, int *use_HOME)
{
   char drive[WCD_MAXDRIVE];


   int i = getdisk();  /* current disk */

   if (strlen(path)>1)
   {

      wcd_strncpy(drive,path,sizeof(drive));

      if (dd_match(drive,"[a-z]:",1))
      {
         int destDisk = islower(*drive) ? *drive-'a' : *drive-'A';

         if (destDisk >= 0)
         {
            setdisk(destDisk);
            i = getdisk();    /* new disk */
         }

         if ((i==destDisk) && (i>=0))  /* succes ? */
         {
            *changed = 1 ;
            wcd_strncpy(newdrive,drive,(size_t)WCD_MAXDRIVE) ;

           if((use_HOME == NULL)||(*use_HOME == 0))
           {
            char *ptr = path + 2;
            if (strcmp(ptr,"") == 0)
            {
               wcd_strncpy(path,"/",(size_t)WCD_MAXPATH);
            }
            else
            {
               wcd_strncpy(path,ptr,(size_t)WCD_MAXPATH);
            }
           }
         }
         else
           i = -1;
      }
   }
   return(i);
}
#endif
/*****************************************************************/
char *getCurPath(char *buffer, size_t size, int *use_HOME)
{
 char *path = wcd_getcwd(buffer, size);
 if(path != NULL)
 {
   size_t len = strlen(buffer);
   if (len==0)
      buffer[len] = '\0';
#ifdef _WCD_DOSFS
   wcd_fixpath(buffer,size);
   rmDriveLetter(buffer,use_HOME);
#endif
 }
 return path;
}
/*****************************************************************
 *  add current path to file.
 *
 *****************************************************************/
void addCurPathToFile(char *filename,int *use_HOME, int parents)
{

 char tmp[WCD_MAXPATH];      /* tmp string buffer */

 char *path = getCurPath(tmp,(size_t)WCD_MAXPATH,use_HOME);

 if(path != NULL)
 {
   /* open the treedata file */
   FILE *outfile;
   if  ((outfile = wcd_fopen(filename,"a",0)) != NULL)
   {
     wcd_fprintf(outfile,"%s\n",tmp);
     print_msg("");
     wcd_printf(_("%s added to file %s\n"),tmp,filename);

     if (parents)
     {
     char *ptr ;

      while ( (ptr = strrchr(tmp,DIR_SEPARATOR)) != NULL)
      {
         *ptr = '\0' ;
         /* keep one last separator in the path */
         if (strrchr(tmp,DIR_SEPARATOR) != NULL)
         {
            wcd_fprintf(outfile,"%s\n",tmp);
            print_msg("");
            wcd_printf(_("%s added to file %s\n"),tmp,filename);
         }
      }
     }
     wcd_fclose(outfile, filename, "w", "addCurPathToFile: ");
   }
 }

}

/********************************************************************
 *
 *     scanDisk(char *path, char *treefile, int scanreldir, size_t append, int *use_HOME)
 *
 *     append == 1        : append to treefile
 *     scanreldir == 1    : write relative paths in rtdata.wcd
 *
 ********************************************************************/
void scanDisk(char *path, char *treefile, int scanreldir, size_t append, int *use_HOME, nameset exclude)
{
   size_t  offset = 0 ;     /* offset to remove scanned from path */
   char tmp2[WCD_MAXPATH];   /* tmp string buffer */
   FILE *outfile;
#ifdef _WCD_DOSFS
   char drive[WCD_MAXDRIVE];
   int  changedrive = 0;
#endif

   wcd_fixpath(path,(size_t)WCD_MAXPATH);
   wcd_fixpath(treefile,(size_t)WCD_MAXPATH);
   wcd_getcwd(tmp2, sizeof(tmp2)); /* remember current dir */

   if(! wcd_isdir(path,0))
   {
      print_msg("");
      wcd_printf(_("%s is not a directory.\n"),path);
      return ;
   }

   print_msg("");
   wcd_printf(_("Please wait. Scanning disk. Building treedata-file %s from %s\n"),treefile, path);

#ifdef _WCD_DOSFS
      changeDisk(path,&changedrive,drive,use_HOME);
#endif
   if (scanreldir)
   {
     if ( !wcd_chdir(path,0) )
     {
      char tmp[WCD_MAXPATH];    /* tmp string buffer */
      wcd_getcwd(tmp, sizeof(tmp)); /* get full path */
#ifdef _WCD_DOSFS
          wcd_fixpath(tmp,sizeof(tmp));
          rmDriveLetter(tmp,use_HOME);
#endif
      offset = strlen(tmp);
      /* do not count ending separator (if present) */
      if (offset > 0 && tmp[offset-1]==DIR_SEPARATOR)
        offset--;
      offset++;
     }
     wcd_chdir(tmp2,0);          /* go back */
    }

#ifdef _WCD_DOSFS

   /* open the output file */
   if (append)
     outfile = wcd_fopen(treefile,"a",0);  /* append to database */
   else
     outfile = wcd_fopen(treefile,"w",0);  /* create new database */

   if (!outfile) /* Try to open in a temp dir */
   {
      char *ptr;

      if  ( (ptr = getenv("TEMP")) != NULL )
      {
      wcd_strncpy(treefile,ptr,(size_t)WCD_MAXPATH);
      wcd_strncat(treefile,TREEFILE,(size_t)WCD_MAXPATH);
      outfile = wcd_fopen(treefile,"w",1);
      }

      if (outfile == NULL)
      {
         if  ( (ptr = getenv("TMP")) != NULL )
            {
            wcd_strncpy(treefile,ptr,(size_t)WCD_MAXPATH);
            wcd_strncat(treefile,TREEFILE,(size_t)WCD_MAXPATH);
            outfile = wcd_fopen(treefile,"w",1);
            }

         if (outfile == NULL)
         {
            if  ( (ptr = getenv("TMPDIR")) != NULL )
               {
               wcd_strncpy(treefile,ptr,(size_t)WCD_MAXPATH);
               wcd_strncat(treefile,TREEFILE,(size_t)WCD_MAXPATH);
               outfile = wcd_fopen(treefile,"w",1);
               }
         }
      }
      print_msg(_("Writing file \"%s\"\n"), treefile);
   }

   if (outfile == NULL) /* Did we succeed? */
   {
      print_error("%s", _("Write access to tree-file denied.\n"));
      print_error("%s", _("Set TEMP environment variable if this is a read-only disk.\n"));
      return ;
   }
#else
   /* open the output file */
   if (append)
     outfile = wcd_fopen(treefile,"a",0);  /* append to database */
   else
     outfile = wcd_fopen(treefile,"w",0);  /* create new database */

   if  (outfile == NULL)
      return ;
#endif
#ifdef WCD_UTF16
   /* Add UTF-8 BOM to make it readable by notepad. */
   if (append == 0)
     wcd_fprintf(outfile, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
#endif
   finddirs( path, &offset, outfile, use_HOME, exclude, 0); /* Build treedata-file */
   wcd_fclose(outfile, treefile, "w", "scanDisk: ");
   wcd_chdir(tmp2,0);          /* go back */
}

#if (defined(_WIN32) || defined(__CYGWIN__))
/***********************************************************************
 * scanServer()
 * scan all the shared directories on a server
 ***********************************************************************/
void scanServer(char *path, char *treefile, size_t append, int *use_HOME, nameset exclude)
{
   size_t i;
   nameset shares;

   shares = namesetNew();
   wcd_getshares(path, shares);

   i = 0;
   while (i<getSizeOfNamesetArray(shares))
   {
      scanDisk(elementAtNamesetArray(i,shares),treefile, 0, i + append, use_HOME, exclude);
      ++i;
   }
   freeNameset(shares, true);
}
#endif

/********************************************************************
 *
 *     makeDir(char *path, char *treefile, int *use_HOME)
 *
 ********************************************************************/
void makeDir(char *path, char *treefile, int *use_HOME)
{
#if (defined(UNIX) || defined(__DJGPP__) || defined(__EMX__))
   mode_t m;
#endif
#ifdef _WCD_DOSFS
   char drive[WCD_MAXDRIVE];
   int  changedrive = 0;
#endif

   wcd_fixpath(path,(size_t)WCD_MAXPATH);

#if (defined(__DJGPP__) || defined(__EMX__))
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
   m = S_IRWXU | S_IRWXG | S_IRWXO;
   if (wcd_mkdir(path,m,0)==0)
#elif defined(UNIX)
   m = S_IRWXU | S_IRWXG | S_IRWXO;
   if (wcd_mkdir(path,m,0)==0)
#else
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
   if (wcd_mkdir(path,0)==0)
#endif
   {
      char tmp2[WCD_MAXPATH];
      wcd_getcwd(tmp2, (size_t)WCD_MAXPATH);  /* remember current dir */
      if(wcd_chdir(path,0)==0)        /* goto dir and add */
       addCurPathToFile(treefile,use_HOME,0);
      wcd_chdir(tmp2,0) ;                /* go back */
   }
}

/********************************************************************
 *
 *     deleteLink(char *path, char *treefile)
 *
 ********************************************************************/

#if defined(UNIX) || defined(_WIN32) || ((defined(__OS2__) && defined(__EMX__)))
void deleteLink(char *path, char *treefile)
{
   static struct stat buf ;
   char *errstr;

 if (stat(path, &buf) == 0)
 {
   if (S_ISDIR(buf.st_mode)) /* does the link point to a dir */
   {
        char *line_end ;
        char tmp2[WCD_MAXPATH];

        /* get the parent path of the link */

        if( (line_end = strrchr(path,DIR_SEPARATOR)) != NULL)
          {
            *line_end = '\0' ;
            line_end++;
            wcd_chdir(path,0);  /* change to parent dir of link */
          }
        else
          line_end = path;  /* we were are already there */

        wcd_strncpy(tmp2,line_end,sizeof(tmp2));
        wcd_getcwd(path, (size_t)WCD_MAXPATH);  /* get the full path of parent dir*/
        wcd_strncat(path,"/",(size_t)WCD_MAXPATH);
        wcd_strncat(path,tmp2,(size_t)WCD_MAXPATH);
        wcd_fixpath(path,(size_t)WCD_MAXPATH);
#ifdef _WIN32
        /* When we use unlink() on a Windows symbolic directory link
         * we get 'permission denied'. Use rmdir. */
        if (wcd_rmdir(tmp2,0)==0) /* delete the link */
#else
        if (wcd_unlink(tmp2)==0)    /* delete the link */
#endif
        {
            print_msg("");
            wcd_printf(_("Removed symbolic link %s\n"),path);
            cleanTreeFile(treefile,path);
        } else {
          errstr = strerror(errno);
          print_error(_("Unable to remove symbolic link %s: %s\n"),path, errstr);
        }
   } else {
      print_msg("");
      wcd_printf(_("%s is a symbolic link to a file.\n"),path);
   }
 }
 else
 {
   errstr = strerror(errno);
   print_error("%s: %s\n",path,errstr);
 }

}
#endif

/********************************************************************
 *
 *  deleteDir(char *path, char *treefile, int recursive, int *use_HOME, int assumeYes)
 *
 ********************************************************************/
void deleteDir(char *path, char *treefile, int recursive, int *use_HOME, int assumeYes)
{
#if defined(UNIX) || ((defined(__OS2__) && defined(__EMX__)))
   static struct stat buf ;
#endif
#ifdef _WCD_DOSFS
   char drive[WCD_MAXDRIVE];
   int  changedrive = 0;
#endif

   wcd_fixpath(path,(size_t)WCD_MAXPATH);

#if defined(UNIX) || ((defined(__OS2__) && defined(__EMX__)))
   if (lstat(path, &buf) != 0)
   {
     char *errstr = strerror(errno);
     print_error("%s: %s\n",path,errstr);
     return;
   }

   if (S_ISLNK(buf.st_mode))  /* is it a link? */
   {
      deleteLink(path,treefile);
      return;
   }
#elif defined(_WIN32) && !defined(__CYGWIN__)
     if (wcd_islink(path, 0))  /* is it a link? */
     {
        deleteLink(path,treefile);
        return;
     }
#endif

#ifdef _WCD_DOSFS
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
#endif

   if (wcd_isdir(path,0)) /* is it a dir */
   {
      char tmp2[WCD_MAXPATH];
      wcd_getcwd(tmp2, (size_t)WCD_MAXPATH);  /* remember current path */

      if(wcd_chdir(path,0)==0)
      {
         wcd_getcwd(path, (size_t)WCD_MAXPATH);   /* path to remove */

#ifdef _WCD_DOSFS
         wcd_fixpath(path,WCD_MAXPATH);
         rmDriveLetter(path,use_HOME);
#endif
         wcd_chdir(tmp2,0);
      }

      if(recursive)
      {
         int c ;

         c = 'x' ;

         if (assumeYes)
            c = 'y';
         else
         {
            while ( (c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N'))
            {
               print_msg(_("Recursively remove %s? Are you sure? y/n :"),path);

                   /* Note that getchar reads from stdin and
                      is line buffered; this means it will
                      not return until you press ENTER. */

               c = getchar(); /* get first char */
               if (c != '\n')
                  while ((getchar()) != '\n') ; /* skip the rest */
            }
          }
          if ( (c == 'y') ||  (c == 'Y') )
          {

            wcd_chdir(tmp2,0);  /* go back */
            rmTree(path);       /* delete the stuff */
            wcd_chdir(tmp2,0);  /* go back */

            /* rmTree leaves an empty directory */
            if (wcd_rmdir(path,0)==0)
            {
                print_msg("");
                wcd_printf(_("Removed directory %s\n"),path);
                cleanTreeFile(treefile,path);
            }

          } /* ( (c != 'y') ||  (c != 'Y') ) */
      }
      else
        if (wcd_rmdir(path,0)==0)
        {
          print_msg("");
          wcd_printf(_("Removed directory %s\n"),path);
          cleanTreeFile(treefile,path);
        }
   }
   else
   {
     print_msg("");
     wcd_printf(_("%s is not a directory.\n"),path);
   }
}
/********************************************************************
 *
 * Read a line from an already opened file.
 * Returns: length of the string , not including the ter-
 *          minating `\0' character.
 ********************************************************************/

int wcd_getline(char s[], int lim, FILE* infile, const char* file_name, const int* line_nr)
{
   int c, i;

   for (i=0; i<lim-1 && ((c=fgetc(infile)) != '\n') && (c != EOF) ; ++i)
      {
      s[i] = (char)c ;
      if (c == '\r') i--;
   }

   s[i] = '\0' ;

   if (i >= lim-1)
   {
      int j;
      print_error(_("line too long in %s ( > %d). The treefile could be corrupt, else fix by increasing WCD_MAXPATH in source code.\n"),"wcd_getline()",(lim-1));
      print_error(_("file: %s, line: %d,"),file_name, *line_nr);
      /* Continue reading until end of line */
      j = i+1;
      while (((c=getc(infile)) != '\n') && (c != EOF))
      {
         ++j;
      }
      fprintf(stderr,_(" length: %d\n"),j);
   }

   if (c == EOF) {
      if (ferror(infile)) {
        wcd_read_error(file_name);
      }
   }

   return i ;
}

#if defined(_WIN32) || defined(WCD_UNICODE)
/* UTF16 little endian */
int wcd_wgetline(wchar_t s[], int lim, FILE* infile, const char* file_name, const int* line_nr)
{
   int i;
   int c_high=EOF, c_low=EOF;
#if !defined(_WIN32) && !defined(__CYGWIN__)
   wchar_t lead, trail;
#endif

   for (i=0; i<lim-1 && ((c_low=fgetc(infile)) != EOF)  && ((c_high=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')) ; ++i)
   {
      c_high <<=8;
      s[i] = (wchar_t)(c_high + c_low) ;
      if (s[i] == L'\r') {
         i--;
         continue;
      }
#if !defined(_WIN32) && !defined(__CYGWIN__)
      /* wcstombs() on Unix ignores UTF-16 surrogate pairs. Therefore we have to decode the UTF-16 surrogate pair ourselves.
       * If we don't do it wcstombs() will convert the lead and trail halves individually to surrogate halves in UTF-8,
       * which are invalid in UTF-8. */
      if ((sizeof(wchar_t) >= 4) && (s[i] >= 0xd800) && (s[i] < 0xdc00))
      {
         lead = s[i];
         if (((c_low=fgetc(infile)) != EOF)  && ((c_high=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')))
         {
            c_high <<=8;
            trail = (wchar_t)(c_high + c_low) ;
            if ((trail >= 0xdc00) && (trail < 0xe000))
            {
               s[i] = 0x10000;
               s[i] += (lead & 0x03FF) << 10;
               s[i] += (trail & 0x03FF);
            }
            else /* not a UTF-16 surrogate pair trail. */
            {
               s[i] = trail;
               if (s[i] == L'\r') i--;
            }
         }
      }
#endif
   }

   s[i] = L'\0' ;

   if (i >= lim-1)
   {
      print_error(_("line too long in %s ( > %d). The treefile could be corrupt, else fix by increasing WCD_MAXPATH in source code.\n"),"wcd_wgetline()",(lim-1));
      print_error(_("file: %s, line: %d,"),file_name, *line_nr);
      /* Continue reading until end of line */
      int j = i+1;
      while (((c_low=fgetc(infile)) != EOF)  && ((c_high=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')))
      {
         ++j;
      }
      fprintf(stderr,_(" length: %d\n"),j);
   }

   if ((c_low == EOF) || (c_high == EOF)) {
      if (ferror(infile)) {
        wcd_read_error(file_name);
      }
   }

   return i ;
}

/* UTF16 big endian */
int wcd_wgetline_be(wchar_t s[], int lim, FILE* infile, const char* file_name, const int* line_nr)
{
   int i;
   int c_high=EOF, c_low=EOF;
#if !defined(_WIN32) && !defined(__CYGWIN__)
   wchar_t lead, trail;
#endif

   for (i=0; i<lim-1 && ((c_high=fgetc(infile)) != EOF)  && ((c_low=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')) ; ++i)
   {
      c_high <<=8;
      s[i] = (wchar_t)(c_high + c_low) ;
      if (s[i] == L'\r') {
         i--;
         continue;
      }
#if !defined(_WIN32) && !defined(__CYGWIN__)
      /* wcstombs() on Unix ignores UTF-16 surrogate pairs. Therefore we have to decode the UTF-16 surrogate pair ourselves.
       * If we don't do it wcstombs() will convert the lead and trail halves individually to surrogate halves in UTF-8,
       * which are invalid in UTF-8. */
      if ((sizeof(wchar_t) >= 4) && (s[i] >= 0xd800) && (s[i] < 0xdc00))
      {
         lead = s[i];
         if (((c_high=fgetc(infile)) != EOF)  && ((c_low=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')))
         {
            c_high <<=8;
            trail = (wchar_t)(c_high + c_low) ;
            if ((trail >= 0xdc00) && (trail < 0xe000))
            {
               s[i] = 0x10000;
               s[i] += (lead & 0x03FF) << 10;
               s[i] += (trail & 0x03FF);
            }
            else /* not a UTF-16 surrogate pair trail. */
            {
               s[i] = trail;
               if (s[i] == L'\r') i--;
            }
         }
      }
#endif
   }

   s[i] = L'\0' ;

   if (i >= lim-1)
   {
      print_error(_("line too long in %s ( > %d). The treefile could be corrupt, else fix by increasing WCD_MAXPATH in source code.\n"),"wcd_wgetline_be()",(lim-1));
      print_error(_("file: %s, line: %d,"),file_name, *line_nr);
      /* Continue reading until end of line */
      int j = i+1;
      while (((c_high=fgetc(infile)) != EOF)  && ((c_low=fgetc(infile)) != EOF) && !((c_high == '\0') && (c_low == '\n')))
      {
         ++j;
      }
      fprintf(stderr,_(" length: %d\n"),j);
   }

   if ((c_low == EOF) || (c_high == EOF)) {
      if (ferror(infile)) {
        wcd_read_error(file_name);
      }
   }

   return i ;
}
#endif

/********************************************************************
 *
 * Read treefile in a structure.
 *
 ********************************************************************/
void read_treefileA(FILE *f, nameset bd, const char* file_name)
{
    int line_nr=1;
    char path[WCD_MAXPATH];

    while (!feof(f) && !ferror(f))
    {
       /* read a line */
       int len = wcd_getline(path,WCD_MAXPATH,f,file_name,&line_nr);
       ++line_nr;

       if (len > 0 )
       {
          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) && !ferror(f)) */
}
#if defined(_WIN32) || defined(WCD_UNICODE)
void read_treefileUTF16LE(FILE *f, nameset bd, const char* file_name)
{
    int line_nr=1;
    char path[WCD_MAXPATH];
    wchar_t pathw[WCD_MAXPATH];

    while (!feof(f) && !ferror(f))
    {
       /* read a line */
       int len = wcd_wgetline(pathw,WCD_MAXPATH,f,file_name,&line_nr);
       ++line_nr;

       if (len > 0 )
       {
          WCSTOMBS(path, pathw, sizeof(path));

          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) && !ferror(f)) */
}
void read_treefileUTF16BE(FILE *f, nameset bd, const char* file_name)
{
    int line_nr=1;
    char path[WCD_MAXPATH];
    wchar_t pathw[WCD_MAXPATH];

    while (!feof(f) && !ferror(f))
    {
       /* read a line */
       int len = wcd_wgetline_be(pathw,WCD_MAXPATH,f,file_name,&line_nr);
       ++line_nr;

       if (len > 0 )
       {
          WCSTOMBS(path, pathw, sizeof(path));

          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) && !ferror(f)) */
}
/* read_treefileUTF8() was made to make it possible that a non-Unicode
   Windows version of Wcd can read UTF-8 encoded tree-data files.
   Handy when a person uses also the Windows version for PowerShell with
   Unicode support.
   Most non-ASCII characters are likely part of the default system
   ANSI code page.
   */
void read_treefileUTF8(FILE *f, nameset bd, const char *file_name)
{
    int line_nr=1;
    char path[WCD_MAXPATH];
#ifdef WCD_ANSI
    wchar_t pathw[WCD_MAXPATH];
#endif

    while (!feof(f) && !ferror(f))
    {
       /* read a line */
       int len = wcd_getline(path,WCD_MAXPATH,f,file_name,&line_nr);
       ++line_nr;

       if (len > 0 )
       {
#ifdef WCD_ANSI
          /* Convert UTF-8 to ANSI */
          MultiByteToWideChar(CP_UTF8, 0, path, -1, pathw, sizeof(pathw));
          WideCharToMultiByte(CP_ACP, 0, pathw, -1, path, sizeof(path), NULL, NULL);
#endif
          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) && !ferror(f)) */
}
#endif

int read_treefile(char* filename, nameset bd, int quiet)
{
   FILE *infile;
   int bomtype;

   /* open treedata-file */
   if  ((infile = wcd_fopen_bom(filename,"rb", quiet, &bomtype)) != NULL) {
       switch (bomtype)
       {
         case FILE_MBS:
           read_treefileA(infile, bd, filename);
           break;
#if defined(_WIN32) || defined(WCD_UNICODE)
         case FILE_UTF16LE:
           read_treefileUTF16LE(infile, bd, filename);
           break;
         case FILE_UTF16BE:
           read_treefileUTF16BE(infile, bd, filename);
           break;
         case FILE_UTF8:
           read_treefileUTF8(infile, bd, filename);
           break;
#endif
         default:
           read_treefileA(infile, bd, filename);
      }
      wcd_fclose(infile, filename, "r", "read_treefile: ");
   } else {
      return -1;
   }
   return bomtype;
}
/********************************************************************
 *
 *    void cleanTreeFile(char *filename, char *dir)
 *
 *  remove path from treefile
 *
 ********************************************************************/

void cleanTreeFile(char *filename, char *dir)
{
   nameset dirs;
   int bomtype;

   dirs = namesetNew();
   if ((bomtype = read_treefile(filename,dirs,0)) >= 0) {
      rmDirFromList(dir,dirs);
      writeList(filename, dirs, bomtype);
   }
   freeNameset(dirs, true);
}


/********************************************************************
 *
 *                    check_double_match(char *dir, nameset set)
 *
 *  Returns 0 if directory had no match before.
 *  Returns 1 if directory is double matched.
 *
 ********************************************************************/

int check_double_match(char *dir, nameset set)
{
   size_t i = 0;

   if ((dir == NULL) || (set == NULL))
      return(0);

   while(i < set->size)
   {
#ifdef _WCD_DOSFS
      if( stricmp(set->array[i],dir) == 0) return(1);
#else
      if( strcmp(set->array[i],dir) == 0) return(1);
#endif
      ++i;
   }

   return(0);
}

/********************************************************************
 *
 *                    check_filter(char *dir, nameset filter)
 *
 *  Returns 0 if directory is in filter list or filter list is empty.
 *  Returns 1 if directory is in filter list.
 *
 ********************************************************************/
int check_filter(char *dir, nameset filter)
{
   size_t index = 0;

   if (filter->size == 0) return (0);

   while (index < filter->size)
   {
#ifdef _WCD_DOSFS
      if(dd_match(dir,filter->array[index],1)) return(0);
#else
      if(dd_match(dir,filter->array[index],0)) return(0);
#endif
         ++index;
   }
   return(1);
}

/********************************************************************
 *
 *  read_treefile_line()
 *
 *  Read a line from a tree file. Convert the line when the input is UTF-16.
 *
 *  Returns the number of characters read.
 *
 ********************************************************************/
int read_treefile_line (char line[], FILE* infile, const char* filename, const int* line_nr, int bomtype)
{
   int len;
#if defined(_WIN32) || defined(WCD_UNICODE)
   wchar_t linew[WCD_MAXPATH];            /* database path */
#endif

#if defined(_WIN32) || defined(WCD_UNICODE)
      /* read a line */
      switch (bomtype)
      {
         case FILE_MBS:
           len = wcd_getline(line,WCD_MAXPATH,infile,filename,line_nr);
           break;
         case FILE_UTF16LE:
           len = wcd_wgetline(linew,WCD_MAXPATH,infile,filename,line_nr);
           WCSTOMBS(line, linew, (size_t)WCD_MAXPATH);
           break;
         case FILE_UTF16BE:
           len = wcd_wgetline_be(linew,WCD_MAXPATH,infile,filename,line_nr);
           WCSTOMBS(line, linew, (size_t)WCD_MAXPATH);
           break;
         case FILE_UTF8:
           len = wcd_getline(line,WCD_MAXPATH,infile,filename,line_nr);
#ifdef WCD_ANSI
           /* convert UTF-8 to ANSI */
           MultiByteToWideChar(CP_UTF8, 0, line, -1, linew, sizeof(linew));
           len = WideCharToMultiByte(CP_ACP, 0, linew, -1, line, WCD_MAXPATH, NULL, NULL) -1;
#endif
           break;
         default:
           len = wcd_getline(line,WCD_MAXPATH,infile,filename,line_nr);
      }
#else
      len = wcd_getline(line,WCD_MAXPATH,infile,filename,line_nr);
#endif

   if (len<0)
     len = 0;
   return len;
}


/********************************************************************
 *
 *  pathInNameset(char *path, nameset set)
 *
 *  Test if *path is a directory or a subdirectory of one
 *  of the paths in set.
 *
 *  set is a list of paths.
 *  wildcards are supported.
 *
 *
 *  Returns (size_t)-1 if directory is not in nameset.
 *  Returns index number (>=0) of first match found if directory is in nameset.
 *
 ********************************************************************/
size_t pathInNameset (text path, nameset set)
{
   char tmp[WCD_MAXPATH];
   size_t size, index = 0;

   if ((path == NULL)||(set == NULL))
      return((size_t)-1);

   size = getSizeOfNamesetArray(set);

   while (index < size)
   {
      wcd_strncpy(tmp,set->array[index],sizeof(tmp));
      wcd_strncat(tmp,"/*",sizeof(tmp));

#ifdef _WCD_DOSFS
      if ((dd_match(path,set->array[index],1)) || (dd_match(path, tmp,1)))
#else
      if ((dd_match(path,set->array[index],0)) || (dd_match(path, tmp,0)))
#endif
      {
         return(index);
      }
      ++index;
   }
   return((size_t)-1);
}

/********************************************************************
 *
 *    scanfile(char *org_dir, char *filename, int
 *             ignore_case, nameset pm, nameset wm, nameset bd, int nfs)
 *
 *
 ********************************************************************/

void scanfile(char *org_dir, char *filename, int ignore_case,
              nameset pm, nameset wm, nameset bd, nameset filter, int relative, int wildOnly, int ignore_diacritics)
{
   FILE *infile;
   char line[WCD_MAXPATH];            /* database path */
   int  bomtype ;
   char *line_end;                  /* database directory */
   char path_str[WCD_MAXPATH];        /* path name to match */
   char dirwild_str[WCD_MAXPATH];     /* directory name to wild match */
   char *dir_str ;                   /* directory name to perfect match */
   char relative_prefix[WCD_MAXPATH]; /* relative prefix */
   char tmp[WCD_MAXPATH];
   int wild = 0;
   int line_nr =1;

   /* open treedata-file */
   if  ((infile = wcd_fopen_bom(filename,"rb",0,&bomtype)) == NULL) {
      return;
   }

   if( (dir_str = strrchr(org_dir,DIR_SEPARATOR)) != NULL)
      dir_str++;
   else dir_str = org_dir;

   wcd_strncpy(dirwild_str,dir_str,sizeof(dirwild_str));

#ifdef _WCD_DOSFS
   if ((strlen(org_dir)>1) && (dd_match(org_dir,"[a-z]:*",1)))
   {
     /* If user searches "c:bin" (a directory "bin" on drive c:) set path_str to "c:*bin" */
     wcd_strncpy(path_str,org_dir,WCD_MAXDRIVE-1);
     path_str[WCD_MAXDRIVE-1] = '\0';
     line_end = org_dir + WCD_MAXDRIVE ;
     wcd_strncat(path_str,"*",sizeof(path_str));
     wcd_strncat(path_str,line_end,sizeof(path_str));
   }
   else
#endif
   {
     wcd_strncpy(path_str,"*",sizeof(path_str));
     wcd_strncat(path_str,org_dir,sizeof(path_str));
   }

   if (!dd_iswild(dir_str))
   {
      wcd_strncat(dirwild_str,"*",sizeof(dirwild_str));
      wcd_strncat(path_str,"*",sizeof(path_str));
      wild = 0;
   }
   else
      wild = 1;

   if (wildOnly)
      wild = 1;

   if (relative)
   {
      wcd_strncpy(relative_prefix,filename,sizeof(relative_prefix));
      if( (line_end = strrchr(relative_prefix,DIR_SEPARATOR)) != NULL)
      {
         line_end++ ;
         *line_end = '\0';
      }
      else
        relative_prefix[0] = '\0';
   }


   while (!feof(infile) && !ferror(infile))  /* parse the file */
   {
      int len;

      len = read_treefile_line(line,infile,filename,&line_nr, bomtype);
      if (ferror(infile)) {
         wcd_read_error(filename);
         wcd_fclose(infile, filename, "r", "scanfile: ");
         return;
      }
      ++line_nr;

      cleanPath(line,len,1) ;

      if( (line_end = strrchr(line,DIR_SEPARATOR)) != NULL)
         line_end++;
      else
         line_end = line;

      /* test for a perfect match */

#ifdef WCD_UNICODE
      if ( (wild == 0) && (dd_matchmbs(line_end,dir_str,ignore_case,ignore_diacritics))  &&
           (dd_matchmbs(line,path_str,ignore_case,ignore_diacritics)) )
#else
      if ( (wild == 0) && (dd_matchl(line_end,dir_str,ignore_case,ignore_diacritics))  &&
           (dd_matchl(line,path_str,ignore_case,ignore_diacritics)) )
#endif
            {

               if (relative)
               {
                  wcd_strncpy(tmp,relative_prefix,sizeof(tmp));
                  wcd_strncat(tmp,line,sizeof(tmp));
                  wcd_strncpy(line,tmp,sizeof(line));
               }

               if ((pathInNameset(line,bd) == (size_t)-1) &&
                  (check_double_match(line,pm)==0)&&
                  (check_filter(line,filter)==0))
               {
                  addToNamesetArray(textNew(line),pm);
               }
            }
         else
         {

            /* test for a wild match if no perfect match */

#ifdef WCD_UNICODE
            if ( (dd_matchmbs(line_end,dirwild_str,ignore_case,ignore_diacritics)) &&
                 (dd_matchmbs(line,path_str,ignore_case,ignore_diacritics)) && (pm->size == 0))
#else
            if ( (dd_matchl(line_end,dirwild_str,ignore_case,ignore_diacritics)) &&
                 (dd_matchl(line,path_str,ignore_case,ignore_diacritics)) && (pm->size == 0))
#endif
               {

                   if (relative)
                   {
                      wcd_strncpy(tmp,relative_prefix,sizeof(tmp));
                      wcd_strncat(tmp,line,sizeof(tmp));
                      wcd_strncpy(line,tmp,sizeof(line));
                   }

                  if((pathInNameset(line,bd) == (size_t)-1) &&
                     (check_double_match(line,wm)==0) &&
                     (check_filter(line,filter)==0))
                  {
                     addToNamesetArray(textNew(line),wm);
                  }
               }
         }
   }   /* while (!feof(infile) && !ferror(f)) */
   wcd_fclose(infile, filename, "r", "scanfile: ");
}
/********************************************************************
 *
 *     scanaliasfile(char *org_dir, char *filename,
 *               nameset *pm, nameset *wm, int wildOnly)
 *
 *
 ********************************************************************/

void scanaliasfile(char *org_dir, char *filename,
              nameset pm, nameset wm, int wildOnly)
{
   FILE *infile;
   char *dir;
   char line[WCD_MAXPATH];
   int line_nr=1;
   int bomtype;

   dir = org_dir;

#ifdef _WCD_DOSFS
   /* wcd_fixpath() could have added a '/' before the alias
      e.g.  wcd d:alias   =>  /alias */
   if (*dir == '/')
    dir++;
#endif

   /* open treedata-file */
   if  ((infile = wcd_fopen_bom(filename,"r",1,&bomtype)) == NULL)
      return;
   if ((bomtype == FILE_UTF16LE)||(bomtype == FILE_UTF16BE)) {
      wcd_fclose(infile, filename, "r", "scanaliasfile: ");
      print_error("%s", _("Alias file in UTF-16 format is not supported.\n"));
      return;
   }

   while (!feof(infile) && !ferror(infile)) {
      char alias[256];
      int len;
      char *ptr;
      size_t j;

      /* skip spaces at the beginning of the line */
      while ((line[0]=(char)fgetc(infile)) == ' '){};
      ungetc(line[0], infile);

      /* read a line */
      len = wcd_getline(line,WCD_MAXPATH,infile,filename,&line_nr);
      ++line_nr;

      if (len == 0 ) continue;
      ptr = line;
      j=0;
      while ((*ptr != ' ') && (*ptr != '\0') && (j<(sizeof(alias)-1))) {
         alias[j] = *ptr;
         ++j;
         ++ptr;
      }
      alias[j]='\0';
      while ((*ptr!=' ')&&(*ptr!='\0')){++ptr;}; /* alias longer than 256 chars */
      while (*ptr==' '){++ptr;}; /* skip spaces between alias and dir */

      if (strlen(ptr) > 0 )
      /* Only a perfect match counts, case sensitive */
         if  ((strcmp(alias,dir)==0) &&
              (check_double_match(line,pm)==0) /* &&
              (check_filter(line,filter)==0) */ )
            {
               if(wildOnly)
                  addToNamesetArray(textNew(ptr),wm);
               else
                  addToNamesetArray(textNew(ptr),pm);
            }
   }   /* while (!feof(infile) && !ferror(infile)) */
   if (ferror(infile)) {
     wcd_read_error(filename);
   }

   wcd_fclose(infile, filename, "r", "scanaliasfile: ");
}
/********************************************************************
 *
 *     list_alias_file(char *filename);
 *     List the aliases in alphabetical order.
 *
 ********************************************************************/

void list_alias_file(char *filename)
{
   FILE *infile;
   char alias[256];
   int line_nr=1;
   size_t i;
   nameset aliaslines;
   int bomtype;

   /* open treedata-file */
   if  ((infile = wcd_fopen_bom(filename,"r",1,&bomtype)) == NULL)
      return;
   if ((bomtype == FILE_UTF16LE)||(bomtype == FILE_UTF16BE)) {
      wcd_fclose(infile, filename, "r", "list_alias_file: ");
      print_error("%s", _("Alias file in UTF-16 format is not supported.\n"));
      return;
   }

   aliaslines = namesetNew();

   /* First read all the lines in a nameset so that we can sort it later */
   while (!feof(infile) && !ferror(infile)) {
      char line[WCD_MAXPATH];
      int len;

      /* skip spaces at the beginning of the line */
      while ((line[0]=(char)fgetc(infile)) == ' '){};
      ungetc(line[0], infile);

      /* read a line */
      len = wcd_getline(line,WCD_MAXPATH,infile,filename,&line_nr);
      ++line_nr;
      if (len > 0 )
         addToNamesetArray(textNew(line),aliaslines);
   }   /* while (!feof(infile) && !ferror(infile)) */
   if (ferror(infile)) {
     wcd_read_error(filename);
   }
   wcd_fclose(infile, filename, "r", "list_alias_file: ");

   sort_list(aliaslines);

   for (i=0;i<aliaslines->size;i++) {
      char *ptr = aliaslines->array[i];
      size_t j=0;
      while ((*ptr != ' ') && (*ptr != '\0') && (j<(sizeof(alias)-1))) {
         alias[j] = *ptr;
         ++j;
         ++ptr;
      }
      alias[j]='\0';
      while ((*ptr!=' ')&&(*ptr!='\0')){++ptr;}; /* alias longer than 256 chars */
      while (*ptr==' '){++ptr;}; /* skip spaces between alias and dir */

      if (strlen(ptr) > 0 )
         wcd_printf("%s\t%s\n",alias,ptr);
   }
   freeNameset(aliaslines, true);
}
/********************************************************************
 *
 *                 Get int
 *
 ********************************************************************/

int wcd_get_int(void)
{
   int i;
   char string[32];

   fgets(string,(int)sizeof(string),stdin);
   fflush(stdin); /* flush the input stream in case of bad input */

   /* fgets retains the newline character (LF) at the end of string
      and appends a null byte to string to mark the end of the
      string. */
   string[strlen(string)-1] = '\0'; /* remove LF */

   i=atoi(string);

   return(i);
}

/********************************************************************
 *
 *                 exit
 *
 ********************************************************************/

int wcd_exit(nameset pm, nameset wm, nameset ef, nameset bd, nameset nfs, WcdStack ws, nameset excl,
             nameset scan_dirs, nameset filter)
{

   /* free datastructures */
   freeNameset(pm, true); /* perfect list */
   freeNameset(wm, true); /* wild list */
   freeNameset(ef, true); /* extra files */
   freeNameset(bd, true); /* banned dirs */
   freeNameset(nfs, true); /* relative files */
   freeWcdStack(ws, true); /* directory stack */
   freeNameset(excl, true); /* excluded paths */
   freeNameset(scan_dirs, true);
   freeNameset(filter, true);

   return(0);
}

/********************************************************************
 *
 *                 Print help
 *
 ********************************************************************/
void print_help(void)
{
   printf(_("Usage: wcd [options] [directory]\n\n"));
   printf(_("directory:  Name of directory to change to.\n"));
   printf(_("            Wildcards *, ? and [SET] are supported.\n\n"));

   printf(_("options:\n"));
   printf(_("  -a                      Add current path to treefile\n"));
   printf(_("  -aa                     Add current and all parent paths to treefile\n"));
   printf(_("  -A PATH                 Add tree from PATH\n"));
   printf(_("  -b                      Ban current path\n"));
   printf(_("  -c,  --direct-cd        direct CD mode\n"));
   printf(_("  -d DRIVE                set DRIVE for stack & go files (DOS)\n"));
   printf(_("  -e                      add current path to Extra treefile\n"));
   printf(_("  -ee                     add current and all parent paths to Extra treefile\n"));
   printf(_("  -E PATH                 add tree from PATH to Extra treefile\n"));
   printf(_("  -f FILE                 use extra treeFile\n"));
   printf(_("  +f FILE                 add extra treeFile\n"));
   printf(_("  -g                      Graphics\n"));
   printf(_("  -gd                     Graphics, dump tree\n"));
   printf(_("  -G PATH                 set PATH Go-script\n"));
   printf(_("  -GN, --no-go-script     No Go-script\n"));
   printf(_("  -h,  --help             show this Help\n"));

#ifdef _WCD_DOSFS
   printf(_("  -i,  --ignore-case      Ignore case (default)\n"));
   printf(_("  +i,  --no-ignore-case   regard case\n"));
#else
   printf(_("  -i,  --ignore-case      Ignore case\n"));
   printf(_("  +i,  --no-ignore-case   regard case (default)\n"));
#endif

   printf(_("  -I,  --ignore-diacritics     Ignore diacritics\n"));
   printf(_("  +I,  --no-ignore-diacritics  regard diacritics (default)\n"));
   printf(_("  -j,  --just-go          Just go mode\n"));
   printf(_("  -k,  --keep-paths       Keep paths\n"));
   printf(_("  -K,  --color            colors\n"));
   printf(_("  -l ALIAS                aLias current directory\n"));
   printf(_("  -ls                     List the aliases\n"));
   printf(_("  -L,  --license          show software License\n"));
   printf(_("  -m DIR                  Make DIR, add to treefile\n"));
   printf(_("  -M DIR                  Make DIR, add to extra treefile\n"));
   printf(_("  -n PATH                 use relative treefile in PATH\n"));
   printf(_("  +n PATH                 add relative treefile in PATH\n"));
   printf(_("  -N,  --numbers          use Numbers\n"));
   printf(_("  -o                      use stdOut\n"));
   printf(_("  -od, --to-stdout        dump matches\n"));
   printf(_("  -q,  --quiet            Quieter operation\n"));
   printf(_("  -r DIR                  Remove DIR\n"));
   printf(_("  -rmtree DIR             Remove DIR recursive\n"));
   printf(_("  -s                      Scan disk from $HOME\n"));
   printf(_("  -S PATH                 Scan disk from PATH\n"));
   printf(_("  +S PATH                 Scan disk from PATH, create relative treefile\n"));
   printf(_("  -t                      don't strip /tmp_mnt from paths\n"));
   printf(_("  -T,  --ascii-tree       draw tree with ASCII characters\n"));
   printf(_("  -Ta, --alt-tree-nav     Alternative tree navigation\n"));
   printf(_("  -TC, --center-tree      Centered tree view\n"));
   printf(_("  -Tc, --compact-tree     Compact tree\n"));
   printf(_("  -Td, --cjk-width        support legacy CJK fonts\n"));
   printf(_("  -u USER                 use USER's treefile\n"));
   printf(_("  +u USER                 add USER's treefile\n"));
   printf(_("  -v,  --verbose          Verbose operation\n"));
   printf(_("  -V,  --version          show Version info\n"));
   printf(_("  -w,  --wild-match-only  Wild matching only\n"));
   printf(_("  -x PATH                 eXclude PATH during disk scan\n"));
   printf(_("  -xf FILE                eXclude paths from FILE\n"));
   printf(_("  -y,  --assume-yes       assume Yes on all queries\n"));
   printf(_("  -z NUMBER               set max stack siZe\n"));
   printf(_("  -[NUMBER]               push dir (NUMBER times)\n"));
   printf(_("  +[NUMBER]               pop dir (NUMBER times)\n"));
   printf(_("  =                       show stack\n"));
}

#ifdef ENABLE_NLS
void print_version(char *localedir)
#else
void print_version(void)
#endif
{
   printf(_("wcd %s (%s) - Wherever Change Directory\n"),VERSION,VERSION_DATE);
   printf(_("Chdir for Dos and Unix.\n\n"));

#ifdef __MSDOS__
#if defined(__WATCOMC__) && defined(__I86__)
  printf(_("DOS 16 bit version (WATCOMC).\n"));
#elif defined(__TURBOC__)
  printf( _("DOS 16 bit version (TURBOC).\n"));
#elif defined(__GO32__)
   printf(_("DOS 32 bit version (DJGPP).\n"));
#elif defined(__WATCOMC__) && defined(__386__)
   printf(_("DOS 32 bit version (WATCOMC).\n"));
#endif
#endif

#ifdef _WIN32
# ifdef _WIN64
#   ifdef _MSC_VER
    printf(_("Win64 version (MSVC %d).\n"),_MSC_VER);
#   else
    printf(_("Win64 version (MinGW-w64).\n"));
#   endif
# else
#   if defined(__WATCOMC__) && defined(__NT__)
    printf(_("Win32 version (WATCOMC).\n"));
#   elif defined(_MSC_VER)
    printf(_("Win32 version (MSVC %d).\n"),_MSC_VER);
#   elif defined(__MINGW32__) && defined(WCD_MINGW32_W64)
    printf(_("Win32 version (MinGW-w64).\n"));
#   else
    printf(_("Win32 version (MinGW).\n"));
#   endif
# endif
#endif

#ifdef _WIN32
# ifdef WCD_WINZSH
   printf(_("This version is for MSYS and WinZsh.\n"));
# elif defined(WCD_WINPWRSH)
   printf(_("This version is for Windows PowerShell.\n"));
# else
   printf(_("This version is for Windows Command Prompt (cmd.exe).\n"));
# endif
#endif

#ifdef __OS2__
  printf(_("OS/2 version"));
# ifdef __WATCOMC__
  printf(" (WATCOMC).\n");
# elif defined(__EMX__)
  printf(" (EMX).\n");
# else
  printf( ".\n");
# endif
#endif

#ifdef __MSYS__
   printf(_("This version is for native MSYS.\n"));
#endif
#if defined(__CYGWIN__) && !defined(__MSYS__)
   printf(_("Cygwin version.\n"));
#endif
#ifdef WCD_DOSBASH
   printf(_("This version is for DJGPP DOS bash.\n"));
#endif
#ifdef WCD_OS2BASH
   printf(_("This version is for OS/2 bash.\n"));
#endif

   printf(_("Interface: "));
#ifdef WCD_USECONIO
   printf("conio\n");
#else
# ifdef WCD_USECURSES
#  ifdef NCURSES_VERSION
   printf(_("ncurses version %s.%d\n"),NCURSES_VERSION,NCURSES_VERSION_PATCH);
#  else
#   ifdef __PDCURSES__
   printf(_("PDCurses build %d\n"),PDC_BUILD);
#   else
   printf(_("curses\n"));
#   endif
#  endif
#  ifdef ASCII_TREE
   printf("ASCII_TREE=1\n");
#  else
   printf("ASCII_TREE=0\n");
#  endif
# else
   printf(_("stdout\n"));
# endif
#endif
#ifdef ENABLE_NLS
   printf(_("Native language support included.\n"));
   printf(_("LOCALEDIR=%s\n"),localedir);
#else
   printf(_("No native language support included.\n"));
#endif
#if defined(_WCD_DOSFS) || (defined(__MSYS__) && (__GNUC__ == 3 )) /* DOS, OS/2, Windows, or MSYS 1 */
   printf(_("Current locale uses CP%u encoding.\n"),query_con_codepage());
#else
   printf(_("Current locale uses %s encoding.\n"),nl_langinfo(CODESET));
#endif
#ifdef WCD_UNICODE
   printf(_("With Unicode support.\n"));
#ifndef WCD_UTF16
   if (strcmp(nl_langinfo(CODESET), "UTF-8") == 0)
#endif
   {
      printf(_("  Euro symbol: "));
      wcd_printf ("%s\n","€"); /* Put real UTF-8. Visual C does not support C99 unicode notation. */
      printf(_("  Chinese characters: "));
      wcd_printf ("%s\n","中文");
   }
#else
   printf(_("Without Unicode support.\n"));
#endif
#ifdef WCD_UNINORM
   printf(_("With Unicode normalization.\n"));
#else
   printf(_("Without Unicode normalization.\n"));
#endif
   printf("\n");
   printf(_("Download the latest executables and sources from:\n"));
   printf("http://waterlan.home.xs4all.nl/\n");
}

void print_license(void)
{
   printf(_("wcd %s (%s) - Wherever Change Directory\n"),VERSION,VERSION_DATE);
   printf(_("\
Copyright (C) 1996-%d Erwin Waterlander\n\
Copyright (C) 1994-2002 Ondrej Popp on C3PO\n\
Copyright (C) 1995-1996 DJ Delorie on _fixpath()\n\
Copyright (C) 1995-1996 Borja Etxebarria & Olivier Sirol on Ninux Czo Directory\n\
Copyright (C) 1994-1996 Jason Mathews on DOSDIR\n\
Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,\n\
Kai Uwe Rommel and Igor Mandrichenko on recmatch()\n"),2018);
   printf(_("\
Source code to scan Windows LAN was originally written and placed\n\
in the public domain by Felix Kasza.\n\
Markus Kuhn's free wcwidth() and wcswidth() implementations are used.\n\
Rugxulo is the original author of query_con_codepage() (public domain).\n\n"));

   printf(_("\
This program is free software; you can redistribute it and/or\n\
modify it under the terms of the GNU General Public License\n\
as published by the Free Software Foundation; either version 2\n\
of the License, or (at your option) any later version.\n\n"));

   printf(_("\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\n"));

   printf(_("\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n"));
}

/* Recurively create directory path, to enable writing
 * the file */
void create_dir_for_file(const char *f)
{
   char path[WCD_MAXPATH];
   char *ptr ;

   wcd_strncpy(path, f, sizeof(path));
   if ( (ptr = strrchr(path,DIR_SEPARATOR)) != NULL)
   {
      *ptr = '\0' ;
       if (
           (path[0] != '\0') && /* not an empty string */
#ifdef _WCD_DOSFS
           (!dd_match(path,"[a-z]:",1)) && /* not a drive letter */
#endif
           (! wcd_isdir(path,1)) /* dir does not exist */
          )
       {
          create_dir_for_file(path);
#if (defined(UNIX) || defined(__DJGPP__) || defined(__EMX__))
          mode_t m;
          m = S_IRWXU | S_IRWXG | S_IRWXO;
          if (wcd_mkdir(path,m,0)==0)
#else
          if (wcd_mkdir(path,0)==0)
#endif
             print_msg(_("creating directory %s\n"), path);
       }
   }
}

/********************************************************************
 *
 *             empty wcd.go file
 *
 ********************************************************************/

#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)
void empty_wcdgo(char *go_file, int use_GoScript, int verbose)
{
   FILE *outfile;

   if (use_GoScript == 0)
      return;

   if (verbose)
     print_msg(_("Writing file \"%s\"\n"), go_file);

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      exit(0);

   wcd_fprintf(outfile, "%s", "\n");
   wcd_fclose(outfile, go_file, "w", "empty_wcdgo: ");

}
#endif

#ifdef WCD_DOSBASH
void empty_wcdgo(char *go_file, int changedrive, char *drive, int use_GoScript, int verbose)
{
   FILE *outfile;

   if (use_GoScript == 0)
      return;

   if (verbose)
     print_msg(_("Writing file \"%s\"\n"), go_file);

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      exit(0);

   if(changedrive == 1)
      wcd_fprintf(outfile,"cd %s\n",drive);
   else
      wcd_fprintf(outfile, "%s", "\n");
   wcd_fclose(outfile, go_file, "w", "empty_wcdgo: ");
}
#endif


/********************************************************************
 *
 * pickDir()
 *
 * gets current dirname.
 * picks dirname from a nameset. The next one after the current,
 * otherwise the first dirname.
 *
 * Returns 0, if no dir found. Otherwise the indexnumber of the
 *         list + 1.
 *
 * ******************************************************************/


size_t pickDir(nameset list, int *use_HOME)
{
   char curDir[WCD_MAXPATH];
   size_t i;
   char *path;

   if (list == NULL) /* there is no list */
      return(0);

   sort_list(list);

   path = getCurPath(curDir,(size_t)WCD_MAXPATH,use_HOME);

   if (path == NULL)  /* no dirname found */
      return(1);            /* return first of list */

   if ((i = inNameset(curDir,list)) == (size_t)-1)  /* not in list */
      return(1);                            /* return first of list */

   i++;  /* next dirname */
   if (i >= getSizeOfNamesetArray(list))
      i = 0;             /* wrap to beginning */

   return(i+1);
}

/********************************************************************
 *
 *
 ********************************************************************/

#ifdef WCD_SHELL
void writeGoFile(char *go_file, int *changedrive, char *drive, char *best_match, int use_GoScript, int verbose)
{
   FILE *outfile;
#ifdef WCD_UNIXSHELL
   char *ptr ;
#endif
#if defined (_WIN32) && !defined(__CYGWIN__)
   unsigned int codepage_ansi, codepage_dos;

   codepage_ansi = GetACP();
   codepage_dos  = GetConsoleOutputCP();
#endif

   if (use_GoScript == 0)
      return;

   if (verbose)
     print_msg(_("Writing file \"%s\"\n"), go_file);

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   /* open go-script */
   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      return;

# ifdef WCD_UNIXSHELL
   /* unix shell */
#  ifdef WCD_DOSBASH
   /* In DOS Bash $SHELL points to the windows command shell.
      So we use $BASH instead. */
   if ((ptr = getenv("BASH")) != NULL)
      wcd_fprintf(outfile,"#!%s\n",ptr);
   if (*changedrive)
      wcd_fprintf(outfile,"cd %s ; ",drive);
#  else
   /* Printing of #!$SHELL is needed for 8 bit characters.
      Some shells otherwise think that the go-script is a binary file
      and will not source it. */
   if ((ptr = getenv("SHELL")) != NULL)
      wcd_fprintf(outfile,"#!%s\n",ptr);
#  endif
   wcd_fprintf(outfile,"cd %s\n", best_match);
# else /* WCD_UNIXSHELL */
   /* Go-script required, but not unix shell type. */
#  ifdef WCD_WINPWRSH
   /* Windows powershell */
#    ifdef WCD_UTF16
   /* PowerShell can run UTF-8 encoded scripts when the UTF-8 BOM is in. */
   wcd_fprintf(outfile, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
#    endif
   if (codepage_ansi != codepage_dos)
      wcd_fprintf(outfile,"chcp %d | Out-Null\n", codepage_ansi);
   wcd_fprintf(outfile,"set-location %s\n", best_match);
   if (codepage_ansi != codepage_dos)
      wcd_fprintf(outfile,"chcp %d | Out-Null\n", codepage_dos);
#  else
   /* Windows Command Prompt, os/2 */
   wcd_fprintf(outfile, "%s", "@echo off\n");
   if (*changedrive)
      wcd_fprintf(outfile,"%s\n",drive);
#   ifdef WCD_UTF16
   if (codepage_ansi != 65001)
      wcd_fprintf(outfile,"chcp 65001 > nul\n");
#   endif
#   ifdef WCD_ANSI
   if (codepage_ansi != codepage_dos)
      wcd_fprintf(outfile,"chcp %d > nul\n", codepage_ansi);
#   endif
   if (strncmp(best_match,"\"\\\\",3) == 0)
      wcd_fprintf(outfile,"pushd %s\n", best_match); /* UNC path */
   else
      wcd_fprintf(outfile,"cd %s\n", best_match);
#   ifdef WCD_UTF16
   if (codepage_ansi != 65001)
      wcd_fprintf(outfile,"chcp %d > nul\n", codepage_dos);
#   endif
#   ifdef WCD_ANSI
   if (codepage_ansi != codepage_dos)
      wcd_fprintf(outfile,"chcp %d > nul\n", codepage_dos);
#   endif
#  endif
# endif
   wcd_fclose(outfile, go_file, "w", "writeGoFile: ");
}
#endif


void addListToNameset(nameset set, char *list)
{
   if (list != NULL)
   {
      char tmp[WCD_MAXPATH];      /* tmp string buffer */
      list = strtok(list, LIST_SEPARATOR);
      while (list != NULL)
      {
         if (strlen(list) < (WCD_MAXPATH-2)) /* prevent buffer overflow */
         {
            wcd_strncpy(tmp,list,sizeof(tmp));
            wcd_fixpath(tmp,sizeof(tmp));
            addToNamesetArray(textNew(tmp),set);
         }
         list = strtok(NULL, LIST_SEPARATOR);
      }
   }
}

void addListToNamesetFilter(nameset set, char *list)
{
   if (list != NULL)
   {
      char tmp[WCD_MAXPATH];      /* tmp string buffer */
      list = strtok(list, LIST_SEPARATOR);
      while (list != NULL)
      {
         if (strlen(list) < (WCD_MAXPATH-2)) /* prevent buffer overflow */
         {
            wcd_strncpy(tmp,"*",sizeof(tmp));
            wcd_strncat(tmp,list,sizeof(tmp));
            wcd_strncat(tmp,"*",sizeof(tmp));
            wcd_fixpath(tmp,sizeof(tmp));
            addToNamesetArray(textNew(tmp),set);
         }
         list = strtok(NULL, LIST_SEPARATOR);
      }
   }
}

/********************************************************************
 *
 *                             MAIN
 *
 ********************************************************************/
#if defined(XCURSES)
   char *XCursesProgramName = "wcd";
#endif

int main(int argc,char** argv)
{
   FILE *infile;
   FILE *outfile;
   char best_match[WCD_MAXPATH];
   char verbose = 0;
   int i;
   size_t ii, j;
   int exit_wcd = 0;
   int use_default_treedata = 1;
   int use_numbers = 0; /* use numbers instead of letters in conio or curses interface */
   int use_stdout = WCD_STDOUT_NO; /* use stdout interface instead of curses */
#ifdef UNIX
   int strip_mount_string = 1 ;     /* remove mount string prefix( e.g. /tmp_mnt ) from path before /home */
#endif
   int  stack_hit = 0, stack_index;
   char rootdir[WCD_MAXPATH],treefile[WCD_MAXPATH],banfile[WCD_MAXPATH],aliasfile[WCD_MAXPATH];
   char stackfile[WCD_MAXPATH];
   char scandir[WCD_MAXPATH];
   char rootscandir[WCD_MAXPATH];
   char extratreefile[WCD_MAXPATH];
   char homedir[WCD_MAXPATH];
   char dir[WCD_MAXPATH];  /* directory to go to, or dir to scan, make or remove */
   char scan_mk_rm = 0; /* scan disk, or make dir, or remove dir */
   char *ptr, *stackptr;
   int  quieter = 0, cd = 0 ;
   size_t len;
   char tmp[WCD_MAXPATH];      /* tmp string buffer */
   char tmp2[WCD_MAXPATH];      /* tmp string buffer */
   int  stack_is_read = 0;
   WcdStack DirStack;
   nameset extra_files, banned_dirs, scan_dirs;
   nameset perfect_list, wild_list ;  /* match lists */
   nameset relative_files;
   nameset exclude; /* list of paths to exclude from scanning */
   nameset filter;
   int use_HOME = 0 ;
   int wildOnly = 0 ;
   int justGo = 0;
   int assumeYes = 0;
   int graphics = WCD_GRAPH_NO ;
   int keep_paths =0; /* Keep paths in treedata.wcd when wcd can't change to them */
#ifdef WCD_USECURSES
   dirnode rootNode ;
#endif
   int  changedrive = 0;
   char drive[WCD_MAXDRIVE];
   int ignore_diacritics = 0;
#ifdef _WCD_DOSFS
   int ignore_case = 1;
#else
   int ignore_case = 0;
#endif
#ifdef WCD_SHELL
   char go_file[WCD_MAXPATH];
   int use_GoScript = 1;
#endif
#ifdef WCD_UTF16
    wchar_t *cmdstr;
    wchar_t **wargv;
#endif

#ifndef __MSDOS__ /* Win32, OS/2, Unix, Cygwin */
   /* setlocale is required for correct working of nl_langinfo()
      DOS versions of wcd will use query_con_codepage(). */
#if (defined(_WIN32) && !defined(__CYGWIN__))
/* When the locale is set to "" on Windows all East-Asian multi-byte ANSI encoded
   text is printed wrongly when you use standard printf() on Windows with East-
   Asian regional setting.  When we set the locale to "C" gettext still
   translates the messages on Windows. On Unix this would disable gettext. */
   setlocale (LC_ALL, "C");
#else
   setlocale (LC_ALL, "");
#endif
#endif
#ifdef ENABLE_NLS
   char localedir[WCD_MAXPATH];
   ptr = getenv("WCDLOCALEDIR");
   if (ptr == NULL)
      wcd_strncpy(localedir,LOCALEDIR,sizeof(localedir));
   else
   {
      if (strlen(ptr) < sizeof(localedir))
      {
         wcd_strncpy(localedir,ptr,sizeof(localedir));
         wcd_fixpath(localedir,sizeof(localedir));
      }
      else
      {
         print_error(_("Value of environment variable %s is too long.\n"),"WCDLOCALEDIR");
         wcd_strncpy(localedir,LOCALEDIR,sizeof(localedir));
      }
   }
   bindtextdomain (PACKAGE, localedir);
   textdomain (PACKAGE);
#endif

#ifdef __PDCURSES__
  /* if ( getenv("PDC_RESTORE_SCREEN") == NULL )
      putenv("PDC_RESTORE_SCREEN=1");  */ /* restore screen after exit */
   if ( getenv("PDC_ORIGINAL_COLORS") == NULL )
      putenv("PDC_ORIGINAL_COLORS=1");  /* use command prompt original colours */

   /* When PDC_RESTORE_SCREEN is set, wcd scrolls away if command window buffer
      is too large. This has been fixed in PDCurses 2.7. Bug 1144353.
      Don't assume user has PDCurses 2.7, so don't set PDC_RESTORE_SCREEN by default.
      Erwin */
#endif
#if defined(_WIN32) && !defined(__CYGWIN__) && defined(NCURSES_VERSION)
   /* On Windows TERM is not standardized and may be set to any value.
    * When ncurses does not understand the value of TERM it will exit
    * right away. On Windows (not Cygwin) it is best to not set TERM at all.
    */
    if (getenv("TERM") != NULL)
    {
        if (putenv("TERM=") != 0)
        {
             ptr = strerror(errno);
             print_error(_("Failed to unset environment variable TERM: %s\n"), ptr);
        }
    }
#endif

    if ((ptr = getenv("HOME")) == NULL)
    {
       wcd_strncpy(rootscandir,ROOTDIR,sizeof(rootscandir));
    } else {
       if (strlen(ptr) > (WCD_MAXPATH -20))
       {
         print_error(_("Value of environment variable %s is too long.\n"),"HOME");
         return(1);
       }
       wcd_strncpy(rootscandir,ptr,sizeof(rootscandir));
    }

#ifdef _WCD_DOSFS

   if ((ptr = getenv("WCDHOME")) == NULL)
       ptr = getenv("HOME");

   if (ptr != NULL)
   {
      if (strlen(ptr) > (WCD_MAXPATH -20))
      {
         print_error(_("Value of environment variable %s is too long.\n"),"HOME or WCDHOME");
         return(1);
      }

      use_HOME = 1 ;
      wcd_strncpy(rootdir,ptr,sizeof(rootdir));
      wcd_fixpath(rootdir,sizeof(rootdir));
      wcd_strncpy(treefile,rootdir,sizeof(treefile));
      wcd_strncat(treefile,TREEFILE,sizeof(treefile));
      wcd_strncpy(extratreefile,rootdir,sizeof(extratreefile));
      wcd_strncat(extratreefile,EXTRA_TREEFILE,sizeof(extratreefile));
# if (defined(_WIN32) || defined(WCD_DOSBASH) || defined(__OS2__))
      wcd_strncpy(go_file,rootdir,sizeof(go_file));
      wcd_strncat(go_file,GO_FILE,sizeof(go_file));
# endif
      wcd_strncpy(banfile,rootdir,sizeof(banfile));
      wcd_strncat(banfile,BANFILE,sizeof(banfile));
      wcd_strncpy(aliasfile,rootdir,sizeof(aliasfile));
      wcd_strncat(aliasfile,ALIASFILE,sizeof(aliasfile));
      if ((ptr = getenv("WCDSTACKFILE")) == NULL)
      {
         wcd_strncpy(stackfile,rootdir,sizeof(stackfile));
         wcd_strncat(stackfile,STACKFILE,sizeof(stackfile));
      }
      else
      {
         wcd_strncpy(stackfile,ptr,sizeof(stackfile));
      }
   }
   else
   {
# if (defined(WCD_WINZSH) || defined(WCD_WINPWRSH))
     print_error("%s", _("Environment variable HOME or WCDHOME is not set.\n"));
      return(1);
# endif
# if (defined(_WIN32) || defined(WCD_DOSBASH) || defined(__OS2__))
      wcd_strncpy(go_file,STACK_GO_DRIVE,sizeof(go_file));
      wcd_strncat(go_file,GO_FILE,sizeof(go_file));
# endif
      wcd_strncpy(rootdir,ROOTDIR,sizeof(rootdir));
      wcd_strncpy(treefile,TREEFILE,sizeof(treefile));
      wcd_strncpy(extratreefile,EXTRA_TREEFILE,sizeof(extratreefile));
      wcd_strncpy(banfile,BANFILE,sizeof(banfile));
      wcd_strncpy(aliasfile,ALIASFILE,sizeof(aliasfile));
      if ((ptr = getenv("WCDSTACKFILE")) == NULL)
      {
         wcd_strncpy(stackfile,STACK_GO_DRIVE,sizeof(stackfile));
         wcd_strncat(stackfile,STACKFILE,sizeof(stackfile));
      }
      else
      {
         wcd_strncpy(stackfile,ptr,sizeof(stackfile));
      }
   }
#else

   if ((ptr = getenv("WCDHOME")) == NULL)
      ptr = getenv("HOME");

   if (ptr == NULL)
   {
      print_error("%s", _("Environment variable HOME or WCDHOME is not set.\n"));
      return(1);
   }
   else
   {
      if (strlen(ptr) > (WCD_MAXPATH -20))
      {
         print_error(_("Value of environment variable %s is too long.\n"),"HOME or WCDHOME");
         return(1);
      }
      wcd_strncpy(rootdir,ptr,sizeof(rootdir));
   }
   wcd_fixpath(rootdir,sizeof(rootdir));
   wcd_strncpy(treefile,rootdir,sizeof(treefile));
   wcd_strncat(treefile,TREEFILE,sizeof(treefile));
   wcd_strncpy(extratreefile,rootdir,sizeof(extratreefile));
   wcd_strncat(extratreefile,EXTRA_TREEFILE,sizeof(extratreefile));
   wcd_strncpy(go_file,rootdir,sizeof(go_file));
   wcd_strncat(go_file,GO_FILE,sizeof(go_file));
   wcd_strncpy(banfile,rootdir,sizeof(banfile));
   wcd_strncat(banfile,BANFILE,sizeof(banfile));
   wcd_strncpy(aliasfile,rootdir,sizeof(aliasfile));
   wcd_strncat(aliasfile,ALIASFILE,sizeof(aliasfile));
   if ((ptr = getenv("WCDSTACKFILE")) == NULL)
   {
      wcd_strncpy(stackfile,rootdir,sizeof(stackfile));
      wcd_strncat(stackfile,STACKFILE,sizeof(stackfile));
   }
   else
   {
      wcd_strncpy(stackfile,ptr,sizeof(stackfile));
   }
#endif

   create_dir_for_file(treefile);
   wcd_strncpy(scandir,rootdir,sizeof(scandir));

   strcpy(dir,"");

   perfect_list = namesetNew();
   wild_list = namesetNew();
   extra_files = namesetNew();
   banned_dirs = namesetNew();
   scan_dirs = namesetNew();
   relative_files = namesetNew();
   exclude = namesetNew();
   filter = namesetNew();
   DirStack = WcdStackNew(WCD_STACK_SIZE);

   read_treefile(banfile,banned_dirs,1);

   ptr = getenv("WCDSCAN");

   addListToNameset(scan_dirs, ptr);

   ptr = getenv("WCDEXCLUDE");

   addListToNameset(exclude, ptr);

   ptr = getenv("WCDBAN");

   addListToNameset(banned_dirs, ptr);

   ptr = getenv("WCDFILTER");

   addListToNamesetFilter(filter, ptr);

   stackptr = NULL;

   ptr = getenv("WCDUSERSHOME");
   if (ptr == NULL)
      wcd_strncpy(homedir,HOMESTRING,sizeof(homedir));
   else
   {
      if (strlen(ptr) > WCD_MAXPATH)
      {
         print_error(_("Value of environment variable %s is too long.\n"),"WCDUSERSHOME");
         return(1);
      }
      wcd_strncpy(homedir,ptr,sizeof(homedir));
      wcd_fixpath(homedir,sizeof(homedir));
   }

#ifdef WCD_UTF16
    /* Get Unicode parameters. */
    cmdstr = GetCommandLineW();
    wargv = CommandLineToArgvW(cmdstr, &argc);
#endif
   /* ---------------------- parse the commandline ------------*/

   for (i=1; i < argc; i++)
   {
      if (*argv[i]=='-') /* is it a switch? */
         switch (argv[i][1]) {
         case '\0':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }

            stackptr = stack_push(DirStack,1);
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            ptr = argv[i];
            ptr++;
            stackptr = stack_push(DirStack,atoi(ptr));
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case 's':
            if (getSizeOfNamesetArray(scan_dirs) == 0)
               scanDisk(rootscandir,treefile,0,(size_t)0,&use_HOME,exclude);
            else
            {
               j = 0;
               while (j<getSizeOfNamesetArray(scan_dirs))
               {
#if (defined(_WIN32) || defined(__CYGWIN__))
                  if (wcd_isServerPath(elementAtNamesetArray(j,scan_dirs)))
                  {
                     scanServer(elementAtNamesetArray(j,scan_dirs),treefile,j,&use_HOME,exclude);
                  }
                  else
#endif
                     scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, j, &use_HOME, exclude);
                  ++j;
               }
            }
            scan_mk_rm = 1;
            break ;
         case 'S':
         case 'A':
         case 'E':
         case 'm':
         case 'M':
         case 'r':
            scan_mk_rm = 1;
            break;
         case 'a':
            if (argv[i][2] == 'a')
               addCurPathToFile(treefile,&use_HOME,1);
            else
               addCurPathToFile(treefile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'e':
            if (argv[i][2] == 'e')
               addCurPathToFile(extratreefile,&use_HOME,1);
            else
               addCurPathToFile(extratreefile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'B':
         case 'b':
            addCurPathToFile(banfile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'l':
            if (argv[i][2] == 's') {
                print_msg("");
                wcd_printf(_("aliasfile: %s\n"),aliasfile);
               list_alias_file(aliasfile);
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)       /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
            }
            break;
         case 'v':
            verbose = 1;
            if ((ptr = getenv("HOME")) == NULL)
               print_msg(_("HOME is not defined\n"));
            else
               print_msg("HOME=\"%s\"\n",ptr);
            if ((ptr = getenv("WCDHOME")) == NULL)
               print_msg(_("WCDHOME is not defined\n"));
            else
               print_msg("WCDHOME=\"%s\"\n",ptr);
            if ((ptr = getenv("WCDSCAN")) == NULL)
               print_msg(_("WCDSCAN is not defined\n"));
            break;
         case 'q':
            quieter = 1;
            break;
         case 'V':
#ifdef ENABLE_NLS
            print_version(localedir);
#else
            print_version();
#endif

#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)       /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         case 'g':
#ifdef WCD_USECURSES
            graphics |= WCD_GRAPH_NORMAL ;
            if (argv[i][2] == 'd') /* dump tree to stdout */
               graphics |= WCD_GRAPH_DUMP ;
#else
            print_msg("%s", _("Graphics mode only supported in wcd with curses-based interface.\n"));
#endif
            break;
         case 'L':
            print_license();
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)       /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         case 'c':
         case 'C':
            cd = 1;
            break;
         case 'x':
         case 'f':
         case 'z':
         case 'n':
            break;
         case 'G':
#ifdef WCD_SHELL
            if (argv[i][2] == 'N') /* No Go-script */
            {
               use_GoScript = 0;
               if (verbose)
                  print_msg("use_GoScript = 0\n");
            }
#endif
            break;
         case 'N':
            use_numbers = 1;
            break;
         case 'o':
            use_stdout |= WCD_STDOUT_NORMAL ;
            if (argv[i][2] == 'd') /* dump matches to stdout */
               use_stdout |= WCD_STDOUT_DUMP ;
            break;
         case 'i':
            ignore_case = 1;
            break;
         case 'I':
            ignore_diacritics = 1;
            break;
         case 'k':
            keep_paths = 1;
            break;
         case 'K':
            graphics |= WCD_GRAPH_COLOR ;
            break;
         case 'w':
            wildOnly = 1;
            break;
         case 'j':
            justGo = 1;
            break;
         case 'y':
            assumeYes = 1;
            break;
         case 'u':
             break;
#ifdef UNIX
         case 't':
                strip_mount_string = 0;
            break;
#endif
         case 'T':
               if (argv[i][2] == 'c') /* Compact tree */
                  graphics |= WCD_GRAPH_COMPACT ;
               else if (argv[i][2] == 'a') /* Alternative navigation */
                  graphics |= WCD_GRAPH_ALT ;
               else if (argv[i][2] == 'C') /* Centered view */
                  graphics |= WCD_GRAPH_CENTER ;
               else if (argv[i][2] == 'd') /* Double width line drawing symbols */
                  graphics |= WCD_GRAPH_CJK ;
               else
                  graphics |= WCD_GRAPH_ASCII ;
            break;
#ifdef _WCD_DOSFS
         case 'd':
            break;
#endif
         case '-':  /* long option */
            if (strcmp(argv[i]+2,"version") == 0) {
#ifdef ENABLE_NLS
               print_version(localedir);
#else
               print_version();
#endif
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
            } else if (strcmp(argv[i]+2,"verbose") == 0) {
               verbose = 1;
            } else if (strcmp(argv[i]+2,"direct-cd") == 0) {
               cd = 1;
#ifdef WCD_SHELL
            } else if (strcmp(argv[i]+2,"no-go-script") == 0) {
               use_GoScript = 0;
#endif
            } else if (strcmp(argv[i]+2,"ignore-case") == 0) {
               ignore_case = 1;
            } else if (strcmp(argv[i]+2,"no-ignore-case") == 0) {
               ignore_case = 0;
            } else if (strcmp(argv[i]+2,"ignore-diacritics") == 0) {
               ignore_diacritics = 1;
            } else if (strcmp(argv[i]+2,"no-ignore-diacritics") == 0) {
               ignore_diacritics = 0;
            } else if (strcmp(argv[i]+2,"just-go") == 0) {
               justGo = 1;
            } else if (strcmp(argv[i]+2,"keep-paths") == 0) {
               keep_paths = 1;
            } else if (strcmp(argv[i]+2,"color") == 0) {
               graphics |= WCD_GRAPH_COLOR ;
            } else if (strcmp(argv[i]+2,"license") == 0) {
               print_license();
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)       /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
            } else if (strcmp(argv[i]+2,"numbers") == 0) {
               use_numbers = 1;
            } else if (strcmp(argv[i]+2,"to-stdout") == 0) {
               use_stdout |= WCD_STDOUT_DUMP ;
            } else if (strcmp(argv[i]+2,"quiet") == 0) {
               quieter = 1;
            } else if (strcmp(argv[i]+2,"ascii-tree") == 0) {
               graphics |= WCD_GRAPH_ASCII ;
            } else if (strcmp(argv[i]+2,"alt-tree-nav") == 0) {
               graphics |= WCD_GRAPH_ALT ;
            } else if (strcmp(argv[i]+2,"center-tree") == 0) {
               graphics |= WCD_GRAPH_CENTER ;
            } else if (strcmp(argv[i]+2,"compact-tree") == 0) {
               graphics |= WCD_GRAPH_COMPACT ;
            } else if (strcmp(argv[i]+2,"cjk-width") == 0) {
               graphics |= WCD_GRAPH_CJK ;
            } else if (strcmp(argv[i]+2,"wild-match-only") == 0) {
               wildOnly = 1;
            } else if (strcmp(argv[i]+2,"assume-yes") == 0) {
               assumeYes = 1;
            } else {
               print_help();
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
            }
            break;
         default:               /* any switch except the above */
            print_help();

#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         }
      else
      if (*argv[i]=='+') /* Pop dir */
         switch (argv[i][1]) {
         case '\0':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            stackptr = stack_pop(DirStack,1);
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            ptr = argv[i];
            ptr++;
            stackptr = stack_pop(DirStack,atoi(ptr));
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case 'f':
            break;
         case 'S':
            scan_mk_rm = 1;
            break;
         case 'n':
            break;
         case 'i':
            ignore_case = 0;
            break;
         case 'I':
            ignore_diacritics = 0;
            break;
         case 'u':
             break;
         default:
            break;
         }
      else
      if (*argv[i]=='=') /* Print stack */
         {
           if (stack_is_read == 0)
           {
            stack_read(DirStack,stackfile);
            stack_is_read = 1;
           }
            stack_index = stack_print(DirStack,use_numbers,use_stdout);
         if (stack_index >= 0)
            {
               stackptr = DirStack->dir[stack_index] ;
               if (stackptr != NULL)
               {
                  stack_hit = 1;
                  stack_write(DirStack,stackfile);
               }
            }
            else
            {
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
            }
         }
      else /* Not a switch. Must be a dir or filename. */
      {
         if (strcmp(argv[i-1],"-l") == 0 )
         {
            if (wcd_getcwd(tmp, sizeof(tmp)) != NULL)
            {
               len = strlen(tmp);
               if (len==0)
                  tmp[len] = '\0';

               /* open the treedata file */
               if  ((outfile = wcd_fopen(aliasfile,"a",0)) != NULL)
               {
#ifdef WCD_UTF16
                  wcstoutf8(tmp2,wargv[i],sizeof(tmp2));
#else
                  wcd_strncpy(tmp2,argv[i],sizeof(tmp2));
#endif
#ifdef _WCD_DOSFS
                  wcd_fixpath(tmp,sizeof(tmp)) ;
                  rmDriveLetter(tmp,&use_HOME);
#endif
                  wcd_fprintf(outfile,"%s %s\n",tmp2,tmp);
                  print_msg("");
                  wcd_printf(_("%s added to aliasfile %s\n"),tmp,aliasfile);
                  wcd_fclose(outfile, aliasfile, "w", "main: ");
               }
            }
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         }
         else
         if ((strcmp(argv[i-1],"-f") == 0 ) || (strcmp(argv[i-1],"+f") == 0 ))
         {
            if (argv[i-1][0] == '-')
               use_default_treedata = 0;
            addToNamesetArray(textNew(argv[i]),extra_files);
         }
         else
         if (strncmp(argv[i-1],"-x",(size_t)2) == 0 )  /* exclude paths from scanning */
         {
            wcd_strncpy(tmp,argv[i],sizeof(tmp));
            wcd_fixpath(tmp,sizeof(tmp));

            if (argv[i-1][2] == 'f')
               read_treefile(tmp,exclude,0);   /* read exclude paths from file */
            else
               addToNamesetArray(textNew(tmp),exclude); /* get path from argument */

             /* printNameset("exclude ==>",exclude,stderr,true);  */
         }
            else
            if ((strcmp(argv[i-1],"-n") == 0 ) || (strcmp(argv[i-1],"+n") == 0 ))
            {
               if (argv[i-1][0] == '-')
                  use_default_treedata = 0;

               wcd_strncpy(tmp,argv[i],sizeof(tmp));
               wcd_fixpath(tmp,sizeof(tmp));
#ifdef _WCD_DOSFS
               /* is there a drive to go to ? */
               changeDisk(tmp,&changedrive,drive,&use_HOME);
#endif
               if (wcd_isdir(tmp,1)) /* is it a dir */
               {
                  wcd_strncat(tmp,RELTREEFILE,sizeof(tmp));
                  wcd_fixpath(tmp,sizeof(tmp));
                  addToNamesetArray(textNew(tmp),relative_files);
               }
               else  /* it is a file */
               {
                  addToNamesetArray(textNew(tmp),relative_files);
               }
            }
            else
            if ((strcmp(argv[i-1],"-u") == 0 ) || (strcmp(argv[i-1],"+u") == 0 ))
            {
               if (argv[i-1][0] == '-')
                  use_default_treedata = 0;

               if (strcmp(argv[i],"root") == 0)
                  strcpy(tmp,"/");
               else
                  wcd_strncpy(tmp,homedir,sizeof(tmp));
               if ((strlen(tmp)+strlen(argv[i])+strlen(TREEFILE)+1) > WCD_MAXPATH )
               {
                  print_error(_("Value of environment variable %s is too long.\n"),"WCDUSERSHOME");
                  return(1);
               }
               wcd_strncat(tmp,"/",     sizeof(tmp));
               wcd_strncat(tmp,argv[i], sizeof(tmp));
               wcd_strncat(tmp,TREEFILE,sizeof(tmp));
               if ((infile = wcd_fopen(tmp,"r",1)) != NULL)
               {
                  wcd_fclose(infile, tmp, "r", "main: ");
                  addToNamesetArray(textNew(tmp),extra_files);
               }
               else
               {
                  if (strcmp(argv[i],"root") == 0)
                     strcpy(tmp2,"/");
                  else
                     wcd_strncpy(tmp2,homedir,sizeof(tmp2));
                  if ((strlen(tmp2)+strlen(argv[i])+strlen(TREEFILE)+1+5) > WCD_MAXPATH )
                  {
                     print_error(_("Value of environment variable %s is too long.\n"),"WCDUSERSHOME");
                     return(1);
                  }
                  wcd_strncat(tmp2,"/",     sizeof(tmp2));
                  wcd_strncat(tmp2,argv[i], sizeof(tmp2));
                  wcd_strncat(tmp2,"/.wcd", sizeof(tmp2));
                  wcd_strncat(tmp2,TREEFILE,sizeof(tmp2));
                  if ((infile = wcd_fopen(tmp2,"r",1)) != NULL)
                  {
                     wcd_fclose(infile, tmp2, "r", "main: ");
                     addToNamesetArray(textNew(tmp2),extra_files);
                  }
                  else
                     print_error(_("Unable to read file %s or %s\n"), tmp, tmp2);
               }
            }
            else
#ifdef _WCD_DOSFS
            if (strcmp(argv[i-1],"-d") == 0 )
            {
              if (stack_is_read == 0)
              {
                  stackfile[0] = argv[i][0];
#ifdef WCD_DOSBASH
                  go_file[0] = argv[i][0];
#endif
               }
            }
            else
#endif

            if (strcmp(argv[i-1],"-S") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               wcd_strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(_WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,treefile,0,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,treefile,0,(size_t)0,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"+S") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               wcd_strncpy(scandir,argv[i],sizeof(scandir));
#endif
               wcd_strncpy(treefile,scandir,sizeof(treefile));
               wcd_strncat(treefile,RELTREEFILE,sizeof(treefile));

               scanDisk(scandir,treefile,1,(size_t)0,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-z") == 0 )
            {
             if ((stack_is_read == 0) && (atoi(argv[i]) >= 0))
               DirStack->maxsize = atoi(argv[i]);
            }
            else
            if (strcmp(argv[i-1],"-A") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               wcd_strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(_WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,treefile,1,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,treefile,0,(size_t)1,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-E") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               wcd_strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(_WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,extratreefile,1,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,extratreefile,0,(size_t)1,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-m") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(tmp,wargv[i],sizeof(tmp));
#else
               wcd_strncpy(tmp,argv[i],sizeof(tmp));
#endif
               makeDir(tmp,treefile,&use_HOME) ;
            }
            else
            if (strcmp(argv[i-1],"-M") == 0 )
            {
               wcd_strncpy(tmp,argv[i],sizeof(tmp));
               makeDir(tmp,extratreefile,&use_HOME) ;
            }
            else
            if (strcmp(argv[i-1],"-r") == 0 )  /* remove one dir, or link to dir */
            {
#ifdef WCD_UTF16
               wcstoutf8(tmp,wargv[i],sizeof(tmp));
#else
               wcd_strncpy(tmp,argv[i],sizeof(tmp));
#endif
               deleteDir(tmp,treefile,0,&use_HOME, assumeYes) ;
            }
            else
            if (strcmp(argv[i-1],"-rmtree") == 0 )  /* remove dir recursive */
            {
               wcd_strncpy(tmp,argv[i],sizeof(tmp));
               deleteDir(tmp,treefile,1,&use_HOME, assumeYes) ;
            }
#ifdef WCD_SHELL
            else
            if (strcmp(argv[i-1],"-G") == 0)
            {
               wcd_strncpy(go_file,argv[i],sizeof(go_file));
               wcd_fixpath(go_file,sizeof(go_file)) ;
# ifdef UNIX
               wcd_strncat(go_file,GO_FILE2,sizeof(go_file));
# else
               wcd_strncat(go_file,GO_FILE,sizeof(go_file));
# endif
            }
#endif
            else
               {
#ifdef WCD_UTF16
                  wcstoutf8(dir,wargv[i],sizeof(dir));
#else
                  wcd_strncpy(dir,argv[i],sizeof(dir));
#endif
                  wcd_fixpath(dir,sizeof(dir));
               }
      }
   } /* for */


   /*--- end parsing commandline -----------*/

   if (verbose > 0)
   {
      for (ii=0; ii<scan_dirs->size; ++ii)
         print_msg(_("WCDSCAN directory {%s}\n"),elementAtNamesetArray(ii, scan_dirs));
      for (ii=0; ii<banned_dirs->size; ++ii)
         print_msg(_("banning {%s}\n"),elementAtNamesetArray(ii, banned_dirs));
      for (ii=0; ii<exclude->size; ++ii)
         print_msg(_("excluding {%s}\n"),elementAtNamesetArray(ii, exclude));
      for (ii=0; ii<filter->size; ++ii)
         print_msg(_("filtering {%s}\n"),elementAtNamesetArray(ii, filter));
   }



   if (stack_is_read == 0)
   {
      stack_read(DirStack,stackfile);
   }

   if ((strcmp(dir,"") == 0 )&& !(graphics & WCD_GRAPH_NORMAL)) /* no directory given, no graphics, so we go HOME */
      addToNamesetArray(textNew(rootscandir),perfect_list);


   /*--- stack hit ? ------------------------*/

   if (stack_hit==1)
   {
      wcd_strncpy(best_match,stackptr,sizeof(best_match));
#ifdef UNIX
        /* strip the /tmp_mnt string */
      if (strip_mount_string)
         stripTmpMnt(best_match);
#endif
      if ((!quieter)&&(!justGo))
         wcd_printf("-> %s\n",best_match);
#ifdef _WCD_DOSFS
       /* is there a drive to go to ? */
       changeDisk(best_match,&changedrive,drive,&use_HOME);
#endif

#ifdef WCD_SHELL
         quoteString(best_match);
         if (justGo)
            wcd_printf("%s\n",best_match);
         writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript,verbose);
#else
         wcd_chdir(best_match,0); /* change to directory */
#endif

         stack_write(DirStack,stackfile);
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
      }

   /*--- end stack hit ? ------------------------*/

   /*--- Direct CD mode -------------------------*/

   if ((cd==1)&&(strcmp(dir,"")!=0)) /* Try open dir direct. */
   {
      if(wcd_isdir(dir,1)) /* GJM */
      {
         if ( use_stdout & WCD_STDOUT_DUMP ) /* just dump the match and exit */
         {
            wcd_printf("%s\n", dir);
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         }
         wcd_strncpy(best_match,dir,sizeof(best_match));
         if ((!quieter)&&(!justGo))
            wcd_printf("-> %s\n",best_match);

#ifdef _WCD_DOSFS
         /* is there a drive to go to ? */
         changeDisk(best_match,&changedrive,drive,&use_HOME);
#endif
         wcd_chdir(dir,0);
         if(wcd_getcwd(tmp, sizeof(tmp)) != NULL)
         {
            len = strlen(tmp);
            if (len==0)
                  tmp[len] = '\0';

            wcd_fixpath(tmp,sizeof(tmp));

            if ( (ptr=strstr(tmp,"/")) != NULL)
            {
                 wcd_strncpy(best_match,tmp,sizeof(best_match));
#ifdef UNIX
                   /* strip the /tmp_mnt string */
                  if (strip_mount_string)
                  stripTmpMnt(best_match);
#endif
               stack_add(DirStack,tmp);      /* stack include /tmp_mnt string */
               stack_write(DirStack,stackfile);
            }

         }

#ifdef WCD_SHELL
         quoteString(best_match);
         if (justGo)
            wcd_printf("%s\n",best_match);
         writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript,verbose);
#else
         wcd_chdir(best_match,0); /* change to directory */
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
      } /* ? (wcd_isdir(dir)) */



   } /* ? direct cd mode  ((cd==1)&&(strcmp(dir,"")!=0)) */

   /*--- end Direct CD mode -------------------------*/

#ifdef _WCD_DOSFS
   /* is there a drive to go to ? */
   changeDisk(dir,&changedrive,drive,&use_HOME);
#endif

   /* does default treedata-file exist? */

   if  (use_default_treedata)
   {
     if ((infile = wcd_fopen(treefile,"r",1)) == NULL)
     {
        /* create treefile */
        if (getSizeOfNamesetArray(scan_dirs) == 0)
           scanDisk(rootscandir,treefile,0,(size_t)0,&use_HOME,exclude);
        else
        {
           j = 0;
           while (j<getSizeOfNamesetArray(scan_dirs))
           {
              if (j == 0)
                 scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, (size_t)0, &use_HOME, exclude);
              else
                 scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, (size_t)1, &use_HOME, exclude);
              ++j;
           }
        }
     }
     else
       wcd_fclose(infile, treefile, "r", "main: ");
   }



   if((strcmp(dir,"")==0) && (scan_mk_rm))
   {
   /* No directory given. Don't go HOME. Exit and stay in current dir,
      because we only wanted to scan the disk or make or remove a directory */
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
   }


   /*********** scan treedata files ***************/

   if (strcmp(dir,"")!=0) /* Is there a directory to go to? */
   {
      if (use_default_treedata)
         scanfile(dir, treefile,ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the treedata file */


      if  ((infile = wcd_fopen(extratreefile,"r",1)) != NULL)
      {
         wcd_fclose(infile, extratreefile, "r", "main: ");
         scanfile(dir, extratreefile,ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the extra treedata file */
      }


      /* search extra files */

      for (ii=0;ii<extra_files->size;ii++)
      {
         scanfile(dir, extra_files->array[ii],ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the extra treedata file */
      }
      /* search relative files */

      for (ii=0;ii<relative_files->size;ii++)
      {
         scanfile(dir, relative_files->array[ii],ignore_case,perfect_list,wild_list,banned_dirs,filter,1,wildOnly,ignore_diacritics); /* scan the nfs treedata file */
      }

      /* search alias file */

      scanaliasfile(dir, aliasfile, perfect_list, wild_list,wildOnly);
    }
#ifdef WCD_USECURSES
   else
   {
      /* graphics? */
      if (graphics & WCD_GRAPH_NORMAL)
      {
         ptr = NULL;
         rootNode = createRootNode();
         if(rootNode != NULL)
         {
            if (use_default_treedata)
               buildTreeFromFile(treefile,rootNode,0);
            buildTreeFromFile(extratreefile,rootNode,1);
            for (ii=0;ii<extra_files->size;ii++)
            {
               buildTreeFromFile(extra_files->array[ii],rootNode,0);
            }
            for (ii=0;ii<relative_files->size;ii++)
            {
               buildTreeFromFile(relative_files->array[ii],rootNode,0);
            }

            sortTree(rootNode);
            setXYTree(rootNode, &graphics);

            if (graphics & WCD_GRAPH_DUMP)
               dumpTree(rootNode, &graphics);
            else
               ptr = selectANode(rootNode,&use_HOME,ignore_case,graphics,ignore_diacritics);
            freeDirnode(rootNode,true);
         }

         if (ptr != NULL) {
            addToNamesetArray(textNew(ptr),perfect_list);
            free(ptr);
         } else {
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
         }

      }
   }
#endif


   /*********** search is done ***************/


   if ((perfect_list->size==0)&&(wild_list->size == 0)&&(cd==0)) /* No match at all & no direct CD mode */
   {
      if(wcd_isdir(dir,1)) /* GJM */
      {
         /* typed directory exists */
         addToNamesetArray(textNew(dir),perfect_list);
      }
   }


   /*******************************/


   if ((perfect_list->size==0)&&(wild_list->size == 0))  /* No match at all */
   {
      if ( !(use_stdout & WCD_STDOUT_DUMP) ) { /* don't print message if option -od is used */
         print_msg("");
         wcd_printf(_("No directory found matching %s\n"),dir);
         print_msg(_("Perhaps you need to rescan the disk or path is banned.\n"));
      }
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
   }
   else if (perfect_list->size==1)   /* one perfect match */
   {
      wcd_strncpy(best_match,perfect_list->array[0],sizeof(best_match));
   }
   else if ((perfect_list->size==0) && (wild_list->size==1))   /* one wild match, zero perfect matches */
   {
      wcd_strncpy(best_match,wild_list->array[0],sizeof(best_match));
   }
   else
   {
       nameset match_list;
       if (perfect_list->size > 1)
          match_list = perfect_list;  /* choose from perfect list */
       else
          match_list = wild_list;     /* choose from wild list */
#ifdef WCD_USECURSES
      if(graphics & WCD_GRAPH_NORMAL)
      {
         ptr = NULL;
         rootNode = createRootNode();
         if(rootNode != NULL) {
            buildTreeFromNameset(match_list, rootNode);
            setXYTree(rootNode, &graphics);
            if (graphics & WCD_GRAPH_DUMP)
               dumpTree(rootNode, &graphics);
            else
               ptr = selectANode(rootNode,&use_HOME,ignore_case,graphics,ignore_diacritics);
            freeDirnode(rootNode,true);
         }
         if (ptr != NULL) {
            wcd_strncpy(best_match,ptr,sizeof(best_match));
            free(ptr);
         } else
            exit_wcd = 1;
      }
      else
#endif
      {
         if (justGo)
            i = (int)pickDir(match_list,&use_HOME);
         else
            i = display_list(match_list,(int)perfect_list->size,use_numbers,use_stdout);

         if ( (i>0) && (i <= (int)match_list->size))
         {
            i--;
            wcd_strncpy(best_match,match_list->array[i],sizeof(best_match));
         }
         else
            exit_wcd = 1;
      }
      if (exit_wcd)
      {
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
         empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
         empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
      }
   }


   /*******************************/

   /* Yes, a match (best_match) */
   if ( use_stdout & WCD_STDOUT_DUMP ) /* just dump the match and exit */
   {
      wcd_printf("%s\n", best_match);
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)    /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
   }

   wcd_strncpy(tmp,best_match,sizeof(tmp)); /* remember path (with /tmp_mnt) */
#ifdef UNIX
   /* strip the /tmp_mnt string */
   if (strip_mount_string)
      stripTmpMnt(best_match);
#endif
#ifdef _WCD_DOSFS
   changeDisk(tmp,&changedrive,drive,&use_HOME);
#endif

   if ( wcd_chdir(best_match,0) == 0)  /* change to dir to get full path */
   {
      if(wcd_getcwd(tmp, sizeof(tmp)) != NULL)
      {
         if ((!quieter)&&(!justGo))
            wcd_printf("-> %s\n",best_match); /* print match without /tmp_mnt string */

         len = strlen(tmp);
         if (len==0)
            tmp[len] = '\0';

         wcd_fixpath(tmp,sizeof(tmp));

         if ( (ptr=strstr(tmp,"/")) != NULL)
         {
            wcd_strncpy(best_match,tmp,sizeof(best_match));
#ifdef UNIX
            /* strip the /tmp_mnt string */
            if (strip_mount_string)
               stripTmpMnt(best_match);
#endif
            stack_add(DirStack,tmp);     /* stack includes /tmp_mnt string */
            stack_write(DirStack,stackfile);
         }

      }
   }
   else
   {
      print_msg("");
      wcd_printf(_("Cannot change to %s\n"),best_match);
#if defined(UNIX) || defined(_WIN32) || defined(__OS2__)     /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript,verbose);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript,verbose);
#endif
      if (keep_paths == 0)
         cleanTreeFile(treefile,tmp);
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
   }

#ifdef WCD_SHELL
   quoteString(best_match);
   if (justGo)
      wcd_printf("%s\n",best_match);
   writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript,verbose);
#else
   wcd_chdir(best_match,0); /* change to directory */
#endif


   return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude,scan_dirs,filter);
}
