
PREFIX = c:/usr
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LDFLAGS_EXTRA =

CFLAGS = -O2 -Wall

# PDcurses
#CURSES = pdcurses
# Ncurses
CURSES = ncurses
ENABLE_NLS = 1
DEFS   = -DMSDOS

ifeq ($(CURSES),pdcurses)
LIB_CURSES = pdcurses.a
DEFS   += -DWCD_USECURSES
endif

ifeq ($(CURSES),ncurses)
LIB_CURSES = -lncurses -Zomf
DEFS   += -DWCD_USECURSES
endif

ifdef ENABLE_NLS
LDFLAGS_EXTRA += -lintl -liconv
EXTRA_OBJ += langinfo.o
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdos2.exe
VERSIONSUFFIX = -os2
ZIPOBJ_EXTRA=bin/wcd.cmd

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES="${LIB_CURSES}" DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES="${LIB_CURSES}" DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL="../doc/OS2.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS}
	install -m 755 wcd.cmd $(DESTDIR)${bindir}/wcd.cmd

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}
	-rm -f $(DESTDIR)${bindir}/wcd.cmd


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

