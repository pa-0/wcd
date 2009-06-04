
README.txt                   : This file.
BUILD.dos_windows.txt        : How to build wcd for DOS and Windows.
BUILD.dos_windows.intl.txt   : How to build wcd for DOS and Windows with
                               native language support.
INSTALL.dos_windows.intl.txt : How to install wcd on DOS and Windows with
                               native language support.
INSTALL.unix.txt             : How to build and install wcd for unix.
INSTALL.unix.intl.txt        : How to build and install wcd for unix with
                               native language support.
INSTALL.os2.txt              : How to install wcd for OS/2 Warp.
INSTALL.rpm.txt              : How to install wcd using an rpm package.
wcd.txt                      : Manual, text format.
wcd.ps                       : Manual, PostScript format.
wcd.pdf                      : Manual, PDF format.
faq.txt                      : Frequently Asked Questions.
translate.txt                : How to translate wcd.
copying.txt                  : distribution license.
problems.txt                 : Known problems.
todo.txt                     : Things to do.


======================================================================
= 0 Contents                                                         =
======================================================================

 1 History
 2 Source code
 3 Copyright
 4 Acknowledgements
 5 Contact / Download new versions


======================================================================
= 1 History                                                          =
======================================================================

In 1995 I started working for Philips Research in Einhoven. I had to do all my
work on HP Unix (HP-UX). I was missing an application like Norton Change
Directory (NCD). I didn't know of any other NCD like program so in 1996 I
decided to make it myself. On the internet I found Jason Mathew's DOSDIR
package in the Simtel archives. DOSDIR was my starting point. Because DOSDIR
was portable I could make wcd at home with Borland-C 3.1 on my PC (intel 80486,
80 MHz) on DOS 6.2. I mailed the source code to work and there I build a
version for HP-UX. I made wcd for myself, but was happy to share it with
others. Some colleagues started using wcd.

In 1997, I found the DJGPP compiler and used that as my main development
environment.  Now I was able to create a 32 bit DOS version, and it supported
long directory names under Windows 95.

In Dec 1997 I put wcd on the internet. I was amazed that already 7 people
downloaded it in the first month. From then on development was driven a
lot by feedback I got via the internet. In March 1998 the GNU distribution
license was attached.

Later I installed Linux on my PC and that became my main development
platform.

Because eventually most DOS users will migrate to Windows I ported
wcd to Windows. I started with Borland C 4.0, but changed soon to
the MinGW compiler.

Until wcd 3.2.1 wcd supported many compilers, but from version 4.0.0 the main
focus will be on GNU C compiler and tools. The DOS 16 bit version cannot keep
up. It runs out of memory and GNU libraries for native language support are not
available. Also building the DOS 32 bit version with DJGPP will become a
challenge in the future.


======================================================================
= 2 Source code                                                      =
======================================================================

The 16 bit DOS version has been compiled with Borland C/C++ 4.02, but will
also compile with Borland 3. A 16 bit DOS wcd can also be build with the
Open Watcom compiler.

The 32 bit dos versions are compiled with DJGPP and do not use DOSDIR. DJGPP is
a mix of DOS/Unix compiler (both 'MSDOS' and 'unix' are defined).  DOSDIR's
dd_findfirst/dd_findnext implementation for Unix is build with opendir/readdir.
Using DJGPP's implementation of opendir/readdir to scan a disk is about a
factor 100 slower than using findfirst/findnext (with DJGPP 2.01, gcc 2.7.2).
Also using DOSDIR's dd_findfirst/dd_findnext for DOS is very slow when it is
compiled with DJGPP.  It is about a factor 35 slower than using DJGPP's
findfirst/findnext.  Probably due to a slow stat() function in dd_initstruct().
Using DOSDIR in combination with DJGPP would make scanning the disk very slow.
A 32 bit DOS wcd can also be build with the Open Watcom compiler.


The win32 console version is by default compiled with MinGW.  Win32 versions
can also be compiled with Borland, LCC, Open Watcom and Cygwin. Although
Borland 4 can compile wcd with conio interface for windows console, it doesn't
work well. The screen gets garbled after exit.

Wcd for Unix compiles with gcc and with the native system C compilers such as
HP-UX cc or SunOS cc.

Wcd can optionally have a curse-based interface.  Wcd compiles with curses,
ncurses and pdcurses. Ncurses is preferred, because of portability and it
restores the screen automatically after exit (if possible) and has better
support for resizing of the terminal.

The ncurses (new curses) library is a free software emulation of curses in
System V Release 4.0, and more. It uses terminfo format, supports pads and
color and multiple highlights and forms characters and function-key mapping,
and has all the other SYSV-curses enhancements over BSD curses.
The ncurses distribution is available via anonymous FTP at the GNU distribution
site ftp://ftp.gnu.org/pub/gnu/ncurses. It is also available at
ftp://ftp.clark.net/pub/dickey/ncurses. 

PDCurses is a port of System VR4 curses for multiple platforms.  PDCurses has
been ported to DOS, OS/2, X11, WIN32 and Flexos. A directory containing the
port-specific source files exists for each of these platforms.
Get it at http://pdcurses.sourceforge.net/

DJGPP is a project to port the GNU C/C++ compiler to DOS.
It includes a shell (bash) and many ported unix utilities.
For more information about DJGPP take a look at this
internet homepage:   http://www.delorie.com/djgpp/

Cygwin is a project to port the GNU C/C++ compiler to Windows.
It includes a shell (bash) and many ported unix utilities.
For more information about Cygwin take a look at this
internet homepage:   http://cygwin.com/

MinGW is a project to port GNU C/C++ compiler to Windows.
internet homepage: http://www.mingw.org/

LCC is a free C compiler for windows.
internet homepage: http://www.cs.virginia.edu/~lcc-win32/


======================================================================
= 3 Copyright                                                        =
======================================================================

Copyright (C) 1997-2008 Erwin Waterlander

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

.......................................................................

Data structures with dynamic memory allocation were created
with Ondrej Popp's C3PO ( Compiler Compiler Compiler Popp Ondrej ).

http://sourceforge.net/projects/c3po/

e-mail: ondrejpopp@users.sourceforge.net
.......................................................................

For the DOS-16-bit and the Unix version I use DOSDIR. Only the parts
of DOSDIR that are distributed under GNU General Public License are
used in WCD.

DOSDIR: A Portable DOS/UNIX/VMS Directory Interface

DOSDIR minimizes the OS-specific directory access code for applications
allowing programmers to concentrate on the application itself and not
on the low-level directory and file structure.  DOSDIR applications
will run on their native operating systems with the appropriate file
syntax and handling, which is expected for that platform.

Copyright (C) 1994 Jason Mathews.

        Jason Mathews
        The MITRE Corporation
   202 Burlington Road
   Bedford, MA 01730-1420

   Email: mathews@mitre.org, mathews@computer.org


DOSDIR can be download from my own homepage
http://www.xs4all.nl/~waterlan/

.......................................................................

wcd_fixpath() is a modified version of
_fixpath()  Copyright (C) 1995-1996 DJ Delorie, see COPYING.DJ for details

_fixpath() is a library function of libc for DJGPP.
See also http://www.delorie.com/djgpp/

.......................................................................

I used the regular matching algorithm, recmatch(), of Info-Zip's
unzip program.

recmatch() was written by Mark Adler.

Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
Kai Uwe Rommel and Igor Mandrichenko.

Mark Adler (original Zip author; UnZip decompression; writer of
recmatch() ) and Greg Roelofs (former UnZip maintainer/co-author)
have given permission to me to distribute recmatch() (match.c,match.h)
under the GNU General Public License conditions as long as there's
some sort of comment included that indicates it came from Info-ZIP's
UnZip/Zip and was written by Mark Adler.

   Info-ZIP's home WWW site is at:

   http://www.info-zip.org/

......................................................................

Ideas and source code of NCD (Ninux Czo Directory) have been
used in the WCD graphical interface.

Ninux Change Directory
Copyright (C) 1995 Borja Etxebarria
<borja@bips.bi.ehu.es> or <jtbecgob@s835cc.bi.ehu.es>
http://www.ibiblio.org/pub/Linux/utils/shell/ncd-0.9.8.tgz

Additions on NCD were made by Olivier Sirol (Czo)

Ninux Czo Directory
Copyright (C) 1995 Borja Etxebarria
Copyright (C) 1996 Olivier Sirol
Olivier Sirol <sirol@ecoledoc.ibp.fr>
http://www.ibiblio.org/pub/Linux/utils/file/managers/ncd1_205.tgz


......................................................................

Source code to scan Windows LAN was originally written and placed
in the Public Domain by Felix Kasza.

Copied from Felix Kasza's Win32 Samples page
at MVPS.ORG.  http://www.mvps.org/win32/

......................................................................


Source code for colours in curses interface was copied from the PDCurses
demo program 'tui'.
The 'tui' demo program was originally written by P.J. Kunst and placed
in the Public Domain.


......................................................................

Markus Kuhn's free wcwidth() implementation is used in Wcd for Windows
with UTF-8 Unicode support.
See also http://www.cl.cam.ac.uk/~mgk25/unicode.html

======================================================================
= 4 Acknowledgements                                                =
======================================================================

Thanks to the following people for giving ideas and/or
reporting/solving bugs:

Leo Sevat           (? I forgot. ew)
Paul Lippens        (? I forgot. ew)
Ondrej Popp         (relative treefiles)
Ad Vaassen          (subdirectories)
Michael Magan       (interface)
Christopher Drexler (bugfix on SGI IRIX64)
Richard Tietjen     (HOME on DOS/Windows)
Howard Schwartz     (interface: use letters, WCDHOME)
Ajit J. Thakkar     (zsh on windows)
Christian Mondrup   (man page)
Gary R. Johnson     (graphical tree mode)
Peter Gutmann       (just go mode)
Etienne Joarlette   (SunOS makefile)
Edwin Rijpkema      (zoom in graphical tree mode)
Juhapekka Tolvanen  (man page)
Andre vd Avoird     (Keep paths, -k)
Kevin M. Rosenberg  (patch for compile problems w.r.t. gcc and <stdbool.h>, man page)
Zoltan Varady       (fix compile problems on Mac OS X/Darwin)
Eric Auer           (default makefile for DOS 16 bit (compact memory model) works again)
Val Vakar           (WCDFILTER)
Jari Aalto          (man page improvements)
Jari Aalto          (option -l takes alias directly)
Jari Aalto          (in match list key z does page down.)
Jari Aalto          (Swap options -v and -V.)
Gabor Grothendieck  (UNC paths support)
Gabor Grothendieck  (automatically set PDC_RESTORE_SCREEN)
Gabor Grothendieck  (original console colours on windows)
Mark Hessling       (original console colours on windows)
Mark Hessling       (fix: scroll away problem on Windows NT/2000/XP)
Ken Zinnato         (alternative tree navigation)
Scott D Friedemann  (fix: curses interface writes uninitialised data to screen)
Richard Boss        (dump matches to stdout)
Richard Boss        (8-bit characters)
Richard Boss        (print match list at bottom of console)
Roland Messier      Shell script to compile wcd for QNX 4.25 systems.
Elbert Pol          Port to OS/2 Warp
Billy Chen          Optional colours in graphical mode (option -K)
Billy Chen          Less screen movement when navigating in graphical mode.


Thanks to the following people for using their Free software:

Ondrej Popp,                      C3PO
Jason Mathews,                    dosdir
Mark Adler,                       recmatch()
DJ Delorie                        _fixpath()
Borja Etxebarria & Olivier Sirol  Ninux Czo Directory
Felix Kasza                       WIN32 API sample WNetOpenEnum()/WNetEnumResource()
                                  MVPS.ORG http://www.mvps.org/win32/
Markus Kuhn                       wcwidth(), used in Windows version of Wcd.
                                  http://www.cl.cam.ac.uk/~mgk25/unicode.html

======================================================================
= 5 Contact / Download new versions                                 =
======================================================================

I can be contacted via E-mail:
          waterlan@xs4all.nl

Address : Zeelsterstraat 59B
          5652 EB Eindhoven
          The Netherlands

--

Latest development source code is maintained in
SourceForge Subversion repository:

svn co https://wcd.svn.sourceforge.net/svnroot/wcd wcd


The latest WCD packages, executables and sources, can be downloaded from
this internet-page:

http://www.xs4all.nl/~waterlan/

--

For each release of wcd there are two source packages
(the question marks indicate the version number):

wcd-?.?.?-src.tar.gz   : source package
   Source code in unix text.

wcd???s.zip : source package
   Source code in DOS text.


--

Wcd is also distributed by:

SourceForge:
http://sourceforge.net/projects/wcd/

the SfR Freeware/Shareware Archive
http://www.sfr-fresh.com/unix/misc/

Debian Linux
http://packages.debian.org/search?keywords=wcd

Ubuntu Linux
http://packages.ubuntu.com/

Simtel.Net:
Search: wcd
SYSTEM UTILITIES > File management and conversion utils > wcd

Ibiblio (was Metalab (was SunSite)):
http://www.ibiblio.org/pub/Linux/utils/shell/


-- 
Erwin Waterlander

