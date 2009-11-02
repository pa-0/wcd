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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

=======================================================================
*/

/* set tabstop=3 */

/* This file contains wrapper functions that operate on directories.
   These are needed to use the WIN32 API functions. WIN32 API functions
   are needed to get support for UNC paths.
*/

#include <stdio.h>

#if defined(UNIX) || defined(DJGPP) || defined(__OS2__)
# include <unistd.h>
#endif

#if (defined(WIN32) && defined(WCD_UNICODE))
#include <wchar.h>
#endif

#if (defined(WIN32) || defined(__CYGWIN__))

/* WIN32:  Use WIN32 API to support UNC paths.
   CYGWIN: Use WIN32 API only to scan servers and find shared
           directories. The Cygwin POSIX interface already supports
           UNC paths directly. In Cygwin the Win32 api function don't work
           on cygwin (unix like) paths. */

#include <string.h>
#include <windows.h>

#endif

#define DD_MAXPATH 1024
#define DD_MAXDIR 1024

#if (defined(WIN32) && !defined(__CYGWIN__))
/* WIN32, not CYGWIN
   Use WIN32 API */

char *wcd_getcwd(char *buf, int size)
{
   BOOL err;
#ifdef WCD_UNICODE
   static wchar_t wstr[DD_MAXPATH];
   int len;
/*   UINT codepage = GetConsoleOutputCP();
   HANDLE stduit = GetStdHandle(STD_OUTPUT_HANDLE) ; */


   err = GetCurrentDirectoryW(size, wstr);
   len = wcslen(wstr);
/*   WriteConsoleW(stduit, wstr, wcslen(wstr), NULL, NULL);
   WriteConsoleW(stduit, L"\n\r", 1, NULL, NULL);  */

   if ( err != 0)
      if (wcstombs(buf, wstr, DD_MAXPATH) < 0)
/*      if (WideCharToMultiByte(codepage, 0, wstr, -1, buf, DD_MAXPATH, NULL, NULL) == 0) */
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
   int len;
   len = strlen(buf);
   printf("buf char len=%d\n",len);
   printf("buf char last=%x\n",(unsigned char)buf[len-1]);
   static wchar_t wstr[DD_MAXPATH];

/*   UINT codepage = GetConsoleOutputCP();  // console OEM code page
   UINT codepage = GetACP();              // system ANSI code page

   if (MultiByteToWideChar(codepage,0, buf, -1, wstr,DD_MAXPATH) == 0  ) */
   if (mbstowcs(wstr, buf, DD_MAXPATH) < 0)
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

   if (mbstowcs(wstr, buf, DD_MAXPATH) < 0)
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

   if (mbstowcs(wstr, buf, DD_MAXPATH) < 0)
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

