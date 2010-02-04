
PREFIX = c:/djgpp
prefix = $(PREFIX)
MKDIR  = mkdir

CFLAGS = -O3 -Wall
CURSES = pdcurses
ENABLE_NLS = 1

ifdef CONIO
CURSES=
DEFS   += -DWCD_USECONIO
endif

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   = -DWCD_USECURSES
endif

ifdef ENABLE_NLS
LDFLAGS_EXTRA = -lintl -liconv
EXTRA_OBJ=langinfo.o
endif

PROGRAM = wcd.exe
VERSIONSUFFIX = -dos32
ZIPOBJ_EXTRA = bin/cwsdpmi.exe

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL=../doc/INSTALL.dos_windows.txt ENABLE_NLS=${ENABLE_NLS}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

