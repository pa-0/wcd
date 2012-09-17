/* tailor.h

     Copyright (C) 1998-2012 Erwin Waterlander.
	 Was original written by Mark Adler (Not copyrighted 1993)

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


#ifndef _TAILOR_H
#define _TAILOR_H

/* Define __MSDOS__ for Turbo C and Power C */
#ifdef __POWERC
#  define __TURBOC__
#  define __MSDOS__
#endif /* __POWERC */

#if (defined(__DOS__) && !defined(__MSDOS__))
#  define __MSDOS__
#endif /* Watcom C DOS target.  EW */

#if (defined(__MSDOS__) && !defined(MSDOS))
#  define MSDOS
#endif

#if (defined(__NT__) && !defined(_WIN32))
#  define _WIN32
#endif /* Watcom C Windows NT and 95 target.  EW */

#if (defined(_WIN32) && !defined(WIN32))
#  define WIN32
#endif

#ifdef ATARI_ST
#  undef __MSDOS__   /* avoid the MS-DOS specific includes */
#endif

/* Use prototypes and ANSI libraries if _STDC__, or Microsoft or Borland C,
 * or Silicon Graphics, or IBM C Set/2, or Watcom C, or GNU gcc under emx.
 */
#if defined(__STDC__) || defined(__MSDOS__) || defined(ATARI_ST) || defined(sgi)
#  ifndef PROTO
#    define PROTO
#  endif /* !PROTO */
#  define MODERN
#endif

#if defined(__IBMC__) || defined(__EMX__) || defined(__WATCOMC__)
#  ifndef PROTO
#    define PROTO
#  endif /* !PROTO */
#  define MODERN
#endif

#if defined(__BORLANDC__) || (defined(__alpha) && defined(VMS))
#  ifndef PROTO
#    define PROTO
#  endif /* !PROTO */
#  define MODERN
#endif

#ifdef __IBMC__
#  define S_IFMT 0xF000
#endif /* __IBMC__ */

#if defined(__EMX__) || defined(__WATCOMC__) || defined(__BORLANDC__)
#  if (defined(__OS2__) && !defined(__32BIT__))
#    define __32BIT__
#  endif
#endif

#if (defined(__OS2__) && !defined(OS2))
#  define OS2
#endif

#if defined(__DJGPP__)   /* MS-DOS extender:  NOT Unix */
#  undef unix
#  undef __unix
#  undef __unix__
#endif

#if (defined(unix) || defined(__unix) || defined(__unix__))
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* unix || __unix || __unix__ */

/* GCC on OS/2 defines _BSD_SOURCE in /usr/inlcude/features.h   EW. */
#if (defined(linux) || defined(_BSD_SOURCE)) && !defined(__OS2__)
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* linux || _BSD_SOURCE  */

#if (defined(M_XENIX) || defined(COHERENT) || defined(__hpux))
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* M_XENIX || COHERENT || __hpux */

#if defined(__convexc__) || defined(MINIX) || defined(sgi)
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* __convexc__ || MINIX || sgi */

#ifdef __COMPILER_KCC__
#  define TOPS20
#  define NOPROTO
#endif

/* Turn off prototypes if requested */
#if (defined(NOPROTO) && defined(PROTO))
#  undef PROTO
#endif

/* Used to remove arguments in function prototypes for non-ANSI C */
#ifdef PROTO
#  define OF(a) a
#else /* !PROTO */
#  define OF(a) ()
#endif /* ?PROTO */


#ifdef MACOS
#  define DYN_ALLOC
#endif


/* Define MSVMS if MSDOS or VMS defined -- ATARI also does, Amiga could */
#if defined(__MSDOS__) || defined(VMS)
#  define MSVMS
#endif




#endif /* end of tailor.h */
