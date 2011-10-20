
PREFIX = c:/usr/local
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LDFLAGS_EXTRA = -lkernel32 -luser32 -lmpr

CC = x86_64-w64-mingw32-gcc
STRIP = x86_64-w64-mingw32-strip
CFLAGS = -O2 -Wall -Wextra -Wconversion -Wno-unused-parameter
CURSES = pdcurses
ENABLE_NLS =
DEFS   = -DMSDOS -DWCD_WINPWRSH

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

ifdef UNINORM
UCS=1
endif

ifdef UCS
ifdef CURSES
DEFS   += -DPDC_WIDE
CURSES = pdcursesw
endif
EXTRA_OBJ += matchw.o wcwidth.o
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdwin64psh.exe
VERSIONSUFFIX = -win64psh
ZIPOBJ_EXTRA =
# Using MinGW gettext,iconv
ifdef ENABLE_NLS
ZIPOBJ_EXTRA += bin/libintl-8.dll bin/libiconv-2.dll
endif
ifdef UNINORM
ZIPOBJ_EXTRA += bin/libunistring-0.dll
endif

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS} CC=$(CC)

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL="../doc/DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS} CC=$(CC)

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM} STRIP=$(STRIP)

clean :
	${MAKE} -C ../src clean

