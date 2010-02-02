
PREFIX = /usr
LDFLAGS_EXTRA = -lmpr -Wl,--enable-auto-import

CFLAGS = -O3 -Wall
CURSES = ncurses
#CURSES = pdcurses
ENABLE_NLS = 1
DEFS   = -DUNIX

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES
endif

ifdef ENABLE_NLS
LDFLAGS_EXTRA += -lintl -liconv
endif

ifdef UCS
ifeq ($(CURSES),ncurses)
CURSES = ncursesw
endif
endif

BINDIR    = ${PREFIX}/bin
PROGRAM = wcd.exe
VERSIONSUFFIX = -cygwin

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} INSTALL_OBJS_DOC_INSTALL=../doc/INSTALL.dos_windows.txt ENABLE_NLS=${ENABLE_NLS}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX} ENABLE_NLS=${ENABLE_NLS}


dist-zip :
	${MAKE} -C ../src dist-zip PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

dist-tgz :
	${MAKE} -C ../src dist-tgz PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

dist : dist-tgz

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

docfiles :
	${MAKE} -C ../src docfiles

mofiles :
	${MAKE} -C ../src mofiles

clean :
	${MAKE} -C ../src clean

