
PREFIX = /usr

GCCFLAGS = -O3 -Wall
CURSES = ncurses
LIB_CURSES = -l${CURSES}
LDFLAGS_EXTRA = -lintl -liconv -lmpr
DEFS   = -DUNIX -DWCD_USECURSES
BINDIR    = ${PREFIX}/bin
PROGRAM = wcd.exe
VERSIONSUFFIX = -cygwin

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


zippackage :
	${MAKE} -C ../src zippackage PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

tgzpackage :
	${MAKE} -C ../src tgzpackage PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

docfiles :
	${MAKE} -C ../src docfiles

mofiles :
	${MAKE} -C ../src mofiles

clean :
	${MAKE} -C ../src clean

