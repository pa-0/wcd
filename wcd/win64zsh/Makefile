
PREFIX = c:/usr/local64
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LIBS_EXTRA = -lkernel32 -luser32 -lmpr

CC = x86_64-w64-mingw32-gcc
STRIP = x86_64-w64-mingw32-strip
CFLAGS = -O2 -Wall -Wextra -Wconversion -Wno-unused-parameter
CURSES = pdcursesw
ENABLE_NLS =
DEFS   = -DWCD_WINZSH

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES -DPDC_STATIC_BUILD
DEFS   += -DPDC_WIDE
EXTRA_OBJ += wcwidth.o
endif


ifdef ENABLE_NLS
LIBS_EXTRA += -lintl -liconv
endif

ifdef WCD_UTF8
# WCD_UTF8 was used in older versions.
UCS=1
endif

ifdef UNINORM
UCS=1
endif

ifdef UCS
EXTRA_OBJ += matchw.o
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdwin64zsh.exe
VERSIONSUFFIX = -win64zsh
ZIPOBJ_EXTRA =
ifdef ENABLE_NLS
# Using MinGW gettext,iconv
ZIPOBJ_EXTRA += bin/libintl-8.dll bin/libiconv-2.dll
endif

ifdef UNINORM
ifndef ENABLE_NLS
# libunistring needs libiconv
ZIPOBJ_EXTRA += bin/libiconv-2.dll
endif
ZIPOBJ_EXTRA += bin/libunistring-0.dll
endif

ifeq ($(findstring ncurses,$(CURSES)),ncurses)
ifneq ($(findstring libwinpthread,$(ZIPOBJ_EXTRA)),libwinpthread)
ZIPOBJ_EXTRA += bin/libwinpthread-1.dll
endif
endif


all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS} CC=${CC}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LIBS_EXTRA="${LIBS_EXTRA}" prefix=${prefix} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL="../doc/INST_DOS.txt ../doc/INSTALL.txt" ENABLE_NLS=${ENABLE_NLS} CC=${CC}

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} prefix=${prefix} ENABLE_NLS=${ENABLE_NLS}


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} prefix=${prefix} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM} STRIP=$(STRIP)

mostlyclean :
	${MAKE} -C ../src mostlyclean

clean :
	${MAKE} -C ../src clean

