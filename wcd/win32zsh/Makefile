
PREFIX = c:/usr/local
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LDFLAGS_EXTRA = -lkernel32 -luser32 -lmpr

CFLAGS = -O2 -Wall
CURSES = pdcurses
ENABLE_NLS = 1
DEFS   = -DMSDOS -DWCD_WINZSH

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES -DPDC_STATIC_BUILD
endif


ifdef ENABLE_NLS
LDFLAGS_EXTRA += -lintl -liconv
endif

ifdef WCD_UTF8
# WCD_UTF8 was used in older versions.
UCS=1
endif

ifdef UCS
ifdef CURSES
DEFS   += -DPDC_WIDE
endif
EXTRA_OBJ += wcwidth.o
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdwin32zsh.exe
VERSIONSUFFIX = -win32zsh
ZIPOBJ_EXTRA =
ifdef ENABLE_NLS
# Using MinGW gettext,iconv
ZIPOBJ_EXTRA += bin/libintl-8.dll bin/libiconv-2.dll
endif

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL="../doc/DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

