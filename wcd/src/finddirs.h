/*
Copyright (C) 1996-2017 Erwin Waterlander

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
*/

#ifndef _WCD_FINDDIRS_H
#define _WCD_FINDDIRS_H

#include "tailor.h"
#include "wcd.h"

/*
                     MAXPATH   MAXDIR  MAXFILE  MAXEXT
   Borland DOS16       80        66       9        5
   Watcom  DOS16      144       130       9        5
   Watcom  DOS32      144       130       9        5
   DJGPP   DOS32      260       256     256      256
   *       WIN32      260       256     256      256
*/

#if defined(__GO32__) || defined(_WIN32) || defined(__OS2__) || defined(__386__)
/* flat memory, _long_ directory names */
#  define __FLAT__   1
#endif

#if defined(__MSDOS__) || defined(_WIN32) || (defined(__OS2__) && !defined(__EMX__))
#  define WCD_MAXDRIVE	3
#  ifndef __FLAT__
   /* DOS 16 bit */
#    define WCD_MAXPATH	80
#    define WCD_MAXDIR	66
#    define WCD_MAXFILE  16
#    define WCD_MAXEXT   10 /* allow for wildcards .[ch]*, .etc */
#  else
   /* DOS 32 bit or Windows */
   /* Big values to be able to read treedata files of Unix network drives */
#    define WCD_MAXPATH	1028
#    define WCD_MAXDIR	1024
#    define WCD_MAXFILE	256
#    define WCD_MAXEXT	256
#  endif /* ?__FLAT__ */

#else /* ?unix or EMX */
/*
 * WCD_MAXPATH defines the longest permissable path length,
 * including the terminating null. It should be set high
 * enough to allow all legitimate uses, but halt infinite loops
 * reasonably quickly.
 */
#  define WCD_MAXPATH	1024
#  define WCD_MAXDIR	1024
#  define WCD_MAXFILE	255
#  ifdef __OS2__
#    define WCD_MAXDRIVE	3
#    define WCD_MAXEXT	256
#  else
#    define WCD_MAXDRIVE	1
#    define WCD_MAXEXT	1
#  endif
#endif /* ?__MSDOS__ */


#ifdef __MSDOS__
#  define ALL_FILES_MASK "*.*"
#  define DIR_END '\\'
#elif defined(_WIN32)
#  ifdef WCD_UTF16
#    define ALL_FILES_MASK L"*.*"
#  else
#    define ALL_FILES_MASK "*.*"
#  endif
#  define DIR_END '\\'
#elif (defined(__OS2__) && !defined(__EMX__))
#  define ALL_FILES_MASK "*"
#  define DIR_END '\\'
#else /* ?UNIX or EMX */
#  ifdef __OS2__
#    define ALL_FILES_MASK "*"
#    define DIR_END '\\'
#  else
#    define ALL_FILES_MASK "*"
#    define DIR_END '/'
#  endif
#endif /* ?__TURBOC__ */

#define DIR_PARENT ".."

#ifndef __TURBOC__
int  getdisk(void);
int  setdisk(int drive);
#endif /* ?!__TURBOC__ */
void rmTree(char *dir);
void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet);

#endif
