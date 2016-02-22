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

# The install target doesn't work for me in the DJGPP environment. Therefore
# I run the install and dist targets in MSYS. The OS variable must be forced to msdos,
# otherwise extra targets will get build in MSYS.

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} INSTALL_OBJS_DOC_INSTALL="../doc/INST_DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} ZIPFILE=${ZIPFILE} ENABLE_NLS=${ENABLE_NLS} docsubdir=$(docsubdir) OS=msdos

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

mostlyclean :
	${MAKE} -C ../src mostlyclean

clean :
	${MAKE} -C ../src clean

