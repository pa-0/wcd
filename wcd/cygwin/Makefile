
PREFIX = /usr

GCCFLAGS = -O3 -Wall
ifdef UCS
CURSES = ncursesw
else
CURSES = ncurses
endif
LIB_CURSES = -l${CURSES}
LDFLAGS_EXTRA = -lintl -liconv -lmpr -Wl,--enable-auto-import
DEFS   = -DUNIX -DWCD_USECURSES
BINDIR    = ${PREFIX}/bin
PROGRAM = wcd.exe
VERSIONSUFFIX = -cygwin

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} INSTALL_OBJS_DOC_INSTALL=../doc/INSTALL.dos_windows.txt

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


dist-zip :
	${MAKE} -C ../src dist-zip PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

dist-tgz :
	${MAKE} -C ../src dist-tgz PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

dist : dist-tgz

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

docfiles :
	${MAKE} -C ../src docfiles

mofiles :
	${MAKE} -C ../src mofiles

clean :
	${MAKE} -C ../src clean

