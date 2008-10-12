
PREFIX = c:/djgpp

CFLAGS = -Ic3po -O3 -Wall
LFLAGS = -lpdcurses -lintl -liconv
DEFS   = -DWCD_DOSBASH -DWCD_USECURSES
PROGRAM = wcd.exe
VERSIONSUFFIX = -dos32bsh

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


zippackage :
	${MAKE} -C ../src zippackage PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

