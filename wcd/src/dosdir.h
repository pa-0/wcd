/*      dosdir.h

	Defines structures, macros, and functions for dealing with
	directories and pathnames.

	Copyright (C) 1994 Jason Mathews
	This is free software; you can redistribute it and/or modify it under the
	terms of the GNU General Public License, see the file COPYING.

	Modification history:

	 V1.0  13-May-94, J Mathews	Original version.
	 V1.1   8-Jun-94, J Mathews	Added VMS definitions, added dd_
					prefix to functions/constants.
	 Jun 23 1998, Erwin Waterlander, DJGPP compatibilty
	 Oct 28 1999, Erwin Waterlander, update for WIN32 console appl.
	 Jan 3 2000, Erwin Waterlander, update for Mingw32 compiler.
	 Jul 14 2008, Erwin Waterlander, update for OS/2.
   Aug 2012, Major cleanup macros:
             * Use only C99 predefined macros.
             * Use __MSDOS__ only when it's real for MS-DOS.
             * UNIX is not defined on OS/2 EMX (fixed tailor.h, UNIX got defined,
               because _BSD_SOURCE is defined with EMX (GCC on OS/2)).
             * Make it compile with Watcom C for OS/2.
             * Borland C and LCC may have been broken, because these are not supported
               anymore.
        Erwin Waterlander
*/

#ifndef _DOSDIR_H
#define _DOSDIR_H

/* Set up portability */
#include <sys/stat.h>
#include "tailor.h"
#include "wcd.h"

#ifdef _MSC_VER
typedef unsigned int mode_t;
#endif
/* intptr_t is new since C99. An integer type capable of holding a pointer.
   When you use an older C standard your compiler may still support intptr_t.
   Older compilers may not support it. Wcd built with Microsoft Visual C++ 
   64 bit crashes if we don't use intptr_t */
#ifdef _WIN64
typedef intptr_t wcd_intptr_t;
#else
typedef int wcd_intptr_t;
#endif


#ifdef __MSDOS__
#  include <dos.h>
#  ifdef __TURBOC__
#    include <dir.h>
#  else
#    include <direct.h>
#  endif
#  define ALL_FILES_MASK "*.*"
#  define DIR_PARENT ".."
#  define DIR_END '\\'
#elif defined(_WIN32)
#  ifdef __WATCOMC__
#    include <dos.h> /* Watcom C does not have _getdrives(). We use the dos functions. */
#  endif
#  include <io.h>
#  include <direct.h>
#  ifdef WCD_UTF16
#    define ALL_FILES_MASK L"*.*"
#  else
#    define ALL_FILES_MASK "*.*"
#  endif
#  define DIR_PARENT ".."
#  define DIR_END '\\'
#elif (defined(__OS2__) && !defined(__EMX__))
#  include <io.h>
#  include <direct.h>
#  define INCL_DOSFILEMGR
#  include <os2.h>
#  define ALL_FILES_MASK "*"
#  define DIR_PARENT ".."
#  define DIR_END '\\'
#elif defined(VMS)
#  include <rms.h>
#  define ALL_FILES_MASK "*.*"
#  define DIR_PARENT "[-]"
#  define DIR_END ']'
#else /* ?UNIX or EMX */
#  ifdef __OS2__
#    define INCL_DOSFILEMGR
#    include <os2.h>
#    define ALL_FILES_MASK "*"
#    define DIR_END '\\'
#  else
#    define ALL_FILES_MASK "*"
#    define DIR_END '/'
#  endif
#  include <memory.h>
#  include <dirent.h>
#  define DIR_PARENT ".."
#endif /* ?__TURBOC__ */

/* MS-DOS flags equivalent to "dos.h" file attribute definitions */

#define DD_NORMAL   0x00	/* Normal file, no attributes */
#define DD_RDONLY   0x01	/* Read only attribute */
#define DD_HIDDEN   0x02	/* Hidden file */
#define DD_SYSTEM   0x04	/* System file */
#define DD_LABEL    0x08	/* Volume label */
#define DD_DIREC    0x10	/* Directory */
#define DD_ARCH	    0x20	/* Archive */
#define DD_DEVICE   0x40	/* Device */

#define DD_DOSATTRIBS 0x3f          /* DOS ATTRIBUTE MASK */

/*
 *  note that all DOS file attributes defined above do not overlap the
 *  DOS stat definitions, but will conflict will non-DOS machines, so
 *  use following macros to access the flags instead.
 */

#define DD_ISNORMAL(m)   ((m) & S_IFREG)
#if defined(__MSDOS__) || defined(_WIN32) || (defined(__OS2__) && !defined(__EMX__))
#  define DD_ISRDONLY(m) ((m) & DD_RDONLY)
#  define DD_ISHIDDEN(m) ((m) & DD_HIDDEN)
#  define DD_ISSYSTEM(m) ((m) & DD_SYSTEM)
#  define DD_ISLABEL(m)  ((m) & DD_LABEL)
#  define DD_ISDIREC(m)  ((m) & (DD_DIREC | S_IFDIR))
#  define DD_ISARCH(m)   ((m) & DD_ARCH)
#else
#  define DD_ISRDONLY(m) !((m) & S_IWRITE)
#  define DD_ISHIDDEN(m) (0)
#  define DD_ISSYSTEM(m) (0)
#  define DD_ISLABEL(m)  (0)
#  define DD_ISDIREC(m)  ((m) & S_IFDIR)
#  define DD_ISARCH(m)   (0)
#endif /* ?__MSDOS__ */

#if (defined(UNIX) || defined(VMS) || defined(__EMX__))
#  include <errno.h>
#  ifndef ENOENT
#    define ENOENT -1
#  endif
#  ifndef ENMFILE
#    define ENMFILE ENOENT
#  endif
#endif /* ?UNIX/VMS */

/* flags used by fnsplit */

#ifndef __TURBOC__
#  define WILDCARDS	0x01
#  define EXTENSION	0x02
#  define FILENAME	0x04
#  define DIRECTORY	0x08
#  define DRIVE		0x10
#endif

/* definitions and types corresponding to those in dir.h */

#include <time.h> /* for time_t definition */

#if defined(__GO32__) || defined(_WIN32) || defined(__OS2__) || defined(__386__)
/* flat memory, _long_ directory names */
#  define __FLAT__   1
#endif

/*
                     MAXPATH   MAXDIR  MAXFILE  MAXEXT
   Borland DOS16       80        66       9        5
   Watcom  DOS16      144       130       9        5
   Watcom  DOS32      144       130       9        5
   DJGPP   DOS32      260       256     256      256
   *       WIN32      260       256     256      256
*/


#if defined(__MSDOS__) || defined(_WIN32) || (defined(__OS2__) && !defined(__EMX__))
#  define DD_MAXDRIVE	3
#  ifndef __FLAT__
   /* DOS 16 bit */
#    define DD_MAXPATH	80
#    define DD_MAXDIR	66
#    define DD_MAXFILE  16
#    define DD_MAXEXT   10 /* allow for wildcards .[ch]*, .etc */
#  else
   /* DOS 32 bit or Windows */
   /* Big values to be able to read treedata files of Unix network drives */
#    define DD_MAXPATH	1028
#    define DD_MAXDIR	1024
#    define DD_MAXFILE	256
#    define DD_MAXEXT	256
#  endif /* ?__FLAT__ */

#  if defined(__TURBOC__) || defined(_MSC_VER)
     typedef long    off_t;
#  endif
#  ifdef __TURBOC__
     typedef short   mode_t;
#  endif /* ?__TURBOC__ */

#elif defined(VMS)
#  define DD_MAXPATH	NAM$C_MAXRSS
#  define DD_MAXDRIVE	1
#  define DD_MAXDIR	NAM$C_MAXRSS
#  define DD_MAXFILE	80
#  define DD_MAXEXT	32
typedef unsigned short mode_t;
#else /* ?unix or EMX */
/*
 * DD_MAXPATH defines the longest permissable path length,
 * including the terminating null. It should be set high
 * enough to allow all legitimate uses, but halt infinite loops
 * reasonably quickly.
 */
#  define DD_MAXPATH	1024
#  define DD_MAXDIR	1024
#  define DD_MAXFILE	255
#  ifdef __OS2__
#    define DD_MAXDRIVE	3
#    define DD_MAXEXT	256
#  else
#    define DD_MAXDRIVE	1
#    define DD_MAXEXT	1
#  endif
   typedef struct dirent DIR_ENT;
#endif /* ?__MSDOS__ */

typedef struct {
#ifdef WCD_UTF16
    /* We need a buffer, because we convert the UTF-16 wide character
     * name to an UTF-8 multi-byte string. */
    char   dd_name[DD_MAXPATH]; /* File name */
#else
    char*  dd_name;             /* File name */
#endif

    time_t dd_time;             /* File time stamp */
    off_t  dd_size;             /* File length */
    mode_t dd_mode;             /* Attributes of file */

    /*  Below is private (machine specific) data, which should
     *  only be accessed by dosdir modules.
     */
#if defined(__MSDOS__) || defined(_WIN32) || (defined(__OS2__) && !defined(__EMX__))
#  ifdef __TURBOC__
    struct ffblk  dos_fb;
#  elif defined(_WIN32) || (defined(__OS2__) && !defined(__EMX__))
#   ifdef WCD_UTF16
    struct _wfinddata_t dos_fb;
#   else
    struct _finddata_t dos_fb;
#   endif
    wcd_intptr_t nHandle;
    char dd_attribs;
#  else /* ?MSC */
    struct find_t dos_fb;
#  endif /* ?TURBOC */
#elif defined (VMS)
    struct FAB  fab;
    struct NAM  nam;
    char        rms_filespec[DD_MAXPATH];
    char        esa[DD_MAXPATH];
    char        rsa[DD_MAXPATH];
    char        path[DD_MAXPATH];
    char*       filespec;
    char        dd_attribs;
#else /* ?unix */
    DIR*          dd_dirp;                /* Directory ptr */
    DIR_ENT*      dd_dp;                  /* Directory entry */
    char          dd_attribs;             /* File search attributes */
    char          dd_filespec[DD_MAXFILE];   /* File search mask */
#endif /* ?__MSDOS__ */
}   dd_ffblk;


#ifdef __cplusplus
extern "C" {
#endif

wcd_intptr_t  dd_findfirst(const wcd_char *path, dd_ffblk *fb, int attrib);
wcd_intptr_t  dd_findnext(dd_ffblk *fb);
int  dd_fnsplit(const char *path, char * drive, char * dir,
		    char * name, char * ext);

#ifndef __TURBOC__
int  getdisk(void);
int  setdisk(int drive);
#endif /* ?!__TURBOC__ */

#ifdef __cplusplus
}
#endif

#ifdef WCD_UTF16
extern struct _stat dd_sstat;
#else
extern struct stat dd_sstat;
#endif

#endif /* _DOSDIR_H */
