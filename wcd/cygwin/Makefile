
PREFIX = /usr

CFLAGS = -Ic3po -O3 -Wall
ifdef WCD_UTF8
LFLAGS = -lncursesw -lintl -liconv -lmpr
else
LFLAGS = -lcurses -lintl -liconv -lmpr
endif
DEFS   = -DUNIX -DWCD_USECURSES
BINDIR    = ${PREFIX}/bin
PROGRAM = wcd.exe
VERSIONSUFFIX = -cygwin

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX}

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

