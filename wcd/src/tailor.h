/* tailor.h

     Copyright (C) 1998-2017 Erwin Waterlander.
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

#ifndef _WCD_TAILOR_H
#define _WCD_TAILOR_H

/* Define __MSDOS__ for Turbo C and Power C */
#ifdef __POWERC
#  define __TURBOC__
#  define __MSDOS__
#endif /* __POWERC */

#if (defined(__DOS__) && !defined(__MSDOS__))
#  define __MSDOS__
#endif /* Watcom C DOS target.  EW */

#if (defined(__NT__) && !defined(_WIN32))
#  define _WIN32
#endif /* Watcom C Windows NT and 95 target.  EW */

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

#endif /* end of tailor.h */
