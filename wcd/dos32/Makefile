
PREFIX = c:/djgpp

CFLAGS = -Ic3po -O3 -Wall
LFLAGS = -lpdcurses -lintl -liconv
DEFS   = -DWCD_USECURSES
EXTRA_OBJ=langinfo.o
PROGRAM = wcd.exe
VERSIONSUFFIX = -dos32

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


zippackage :
	${MAKE} -C ../src zippackage ZIPOBJ_EXTRA=bin/cwsdpmi.exe PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

