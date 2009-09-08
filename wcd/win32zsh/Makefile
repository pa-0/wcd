
PREFIX = c:/usr/local
MKDIR  = mkdir

GCCFLAGS = -O3 -Wall
CURSES = pdcurses
LIB_CURSES = -l${CURSES}
LDFLAGS_EXTRA = -lintl -liconv -lkernel32 -luser32 -lmpr
ifdef WCD_UTF8
# WCD_UTF8 was used in older versions.
UCS=1
endif
ifdef UCS 
DEFS   = -DMSDOS -DWCD_USECURSES -DWCD_WINZSH -DPDC_STATIC_BUILD -DPDC_WIDE
EXTRA_OBJ=wcwidth.o langinfo.o
else
DEFS   = -DMSDOS -DWCD_USECURSES -DWCD_WINZSH -DPDC_STATIC_BUILD
EXTRA_OBJ=langinfo.o
endif
BINDIR    = ${PREFIX}/bin
PROGRAM = wcdwin32zsh.exe
VERSIONSUFFIX = -win32zsh
ZIPOBJ_EXTRA = bin/libintl3.dll bin/libiconv2.dll

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}"

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


zippackage :
	${MAKE} -C ../src zippackage ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

