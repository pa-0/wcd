
PREFIX = c:/usr

CFLAGS = -O3 -Wall

# PDcurses
CURSES = pdcurses
LCURSES = pdcurses.a

# Ncurses
#CURSES = ncurses
#LCURSES = -lncurses -Zomf

LFLAGS_EXTRA = -lintl -liconv
DEFS   = -DMSDOS -DWCD_USECURSES
EXTRA_OBJ=langinfo.o
BINDIR    = ${PREFIX}/bin
PROGRAM = wcdos2.exe
VERSIONSUFFIX = -os2

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LCURSES=${LCURSES} DEFS="${DEFS}" LFLAGS_EXTRA="${LFLAGS_EXTRA}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LCURSES=${LCURSES} DEFS="${DEFS}" LFLAGS_EXTRA="${LFLAGS_EXTRA}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"
	install -m 755 wcd.cmd ${BINDIR}/wcd.cmd

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}
	-rm -f ${BINDIR}/wcd.cmd


zippackage :
	${MAKE} -C ../src zippackage ZIPOBJ_EXTRA=bin/wcd.cmd PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

