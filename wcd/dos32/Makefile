
PREFIX = c:/djgpp
MKDIR  = mkdir

GCCFLAGS = -O3 -Wall
CURSES = pdcurses
LIB_CURSES = -l${CURSES}
LDFLAGS_EXTRA = -lintl -liconv
DEFS   = -DWCD_USECURSES
EXTRA_OBJ=langinfo.o
PROGRAM = wcd.exe
VERSIONSUFFIX = -dos32
ZIPOBJ_EXTRA = bin/cwsdpmi.exe

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

