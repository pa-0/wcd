
PREFIX = c:/usr/local
MKDIR  = mkdir

CFLAGS = -Ic3po -O3 -Wall
LFLAGS = -lpdcurses -lintl -liconv -lkernel32 -luser32 -lmpr
DEFS   = -DMSDOS -DWCD_USECURSES -DWCD_WINZSH -DPDC_STATIC_BUILD
BINDIR    = ${PREFIX}/bin
PROGRAM = wcdwin32zsh.exe
VERSIONSUFFIX = -win32zsh
ZIPOBJ_EXTRA = bin/libintl3.dll bin/libiconv2.dll

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX} MKDIR=${MKDIR}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}


zippackage :
	${MAKE} -C ../src zippackage ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

