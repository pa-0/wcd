
PREFIX = c:/usr
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LDFLAGS_EXTRA = -Zhigh-mem -Zomf -Zargs-resp

CFLAGS = -O2 -Wall

# PDcurses
#CURSES = pdcurses
# Ncurses
CURSES = ncurses
ENABLE_NLS =
DEFS   =

ifeq ($(CURSES),pdcurses)
LIB_CURSES = pdcurses.a
DEFS   += -DWCD_USECURSES
endif

ifeq ($(CURSES),ncurses)
LIB_CURSES = -lncurses -ltinfo
DEFS   += -DWCD_USECURSES
endif

LIBS_EXTRA =
ifdef ENABLE_NLS
LIBS_EXTRA += -lintl
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdos2.exe
VERSIONSUFFIX = -os2
ZIPOBJ_EXTRA=bin/wcd.cmd

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES="${LIB_CURSES}" DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} ENABLE_NLS=${ENABLE_NLS} OS=os/2

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES="${LIB_CURSES}" DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} INSTALL_OBJS_DOC_INSTALL="../doc/INST_DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS} OS=os/2
	install -m 755 wcd.cmd $(DESTDIR)${bindir}/wcd.cmd

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}
	-rm -f $(DESTDIR)${bindir}/wcd.cmd


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

mostlyclean :
	${MAKE} -C ../src mostlyclean

clean :
	${MAKE} -C ../src clean

