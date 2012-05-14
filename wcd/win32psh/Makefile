
PREFIX = c:/usr/local
prefix = $(PREFIX)
exec_prefix = $(prefix)
MKDIR  = mkdir
LDFLAGS_EXTRA = -lkernel32 -luser32 -lmpr

CFLAGS = -O2 -Wall -Wextra -Wconversion -Wno-unused-parameter
CURSES = pdcursesw
ENABLE_NLS = 1
DEFS   = -DMSDOS -DWCD_WINPWRSH

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES -DPDC_STATIC_BUILD
DEFS   += -DPDC_WIDE
EXTRA_OBJ += wcwidth.o
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
EXTRA_OBJ += matchw.o
endif

bindir    = ${exec_prefix}/bin
PROGRAM = wcdwin32psh.exe
VERSIONSUFFIX = -win32psh
ZIPOBJ_EXTRA =
ifdef ENABLE_NLS
# Using GnuWin32 gettext,iconv
#ZIPOBJ_EXTRA += bin/libintl3.dll bin/libiconv2.dll
# Using MinGW gettext,iconv
ZIPOBJ_EXTRA += bin/libintl-8.dll bin/libiconv-2.dll
endif
ifdef UNINORM
ZIPOBJ_EXTRA += bin/libunistring-0.dll
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

mostlyclean :
	${MAKE} -C ../src mostlyclean

clean :
	${MAKE} -C ../src clean

