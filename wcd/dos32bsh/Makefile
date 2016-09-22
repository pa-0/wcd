include ../src/version.mk

PREFIX = c:/dos32
prefix = $(PREFIX)
MKDIR  = mkdir

CFLAGS = -O2 -Wall -Wextra -Wconversion
CURSES = pdcurses
ENABLE_NLS =
DEFS   = -DWCD_DOSBASH

ifdef CONIO
CURSES=
DEFS   += -DWCD_USECONIO
endif

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES
endif

PROGRAM = wcdbash.exe
VERSIONSUFFIX = bh
ZIPFILE = wcd${VERSION_SHORT}${VERSIONSUFFIX}.zip
docsubdir = wcd-${VERSION_SHORT}

ifdef ENABLE_NLS
LIBS_EXTRA = -lintl -liconv
ZIPFILE = wcd${VERSION_SHORT}bh-nls.zip
endif

# The install and dist targets can be run in MSYS. The OS variable must be
# forced to msdos, otherwise extra targets will get build in MSYS.

# On DOS we need to set SHELL to sh.exe or bash.exe, otherwise targets may fail
# (targets install and dist fail certainly). SHELL can't be overridden in this
# make level. It sticks to command.com (at least with DJGPP 2.03 make 3.79.1).
# SHELL has to be set in a parent process, so we pass it to the sub make instances.
D2U_MAKESHELL=$(shell which sh)


all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos SHELL=$(D2U_MAKESHELL)

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} INSTALL_OBJS_DOC_INSTALL="../doc/INST_DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos SHELL=$(D2U_MAKESHELL)

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos SHELL=$(D2U_MAKESHELL)


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} ZIPFILE=${ZIPFILE} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos SHELL=$(D2U_MAKESHELL)

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM} SHELL=$(D2U_MAKESHELL)

mostlyclean :
	${MAKE} -C ../src mostlyclean SHELL=$(D2U_MAKESHELL)

clean :
	${MAKE} -C ../src clean SHELL=$(D2U_MAKESHELL)

