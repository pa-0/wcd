
PREFIX = c:/usr/local
MKDIR  = mkdir
LDFLAGS_EXTRA = -lkernel32 -luser32 -lmpr

GCCFLAGS = -O3 -Wall
CURSES = pdcurses
ENABLE_NLS = 1
DEFS   = -DMSDOS

ifdef CURSES
LIB_CURSES = -l${CURSES}
DEFS   += -DWCD_USECURSES -DPDC_STATIC_BUILD -DPDC_WIDE
endif

ifdef ENABLE_NLS
LDFLAGS_EXTRA += -lintl -liconv
EXTRA_OBJ += langinfo.o
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

BINDIR    = ${PREFIX}/bin
PROGRAM = wcdwin32.exe
VERSIONSUFFIX = -win32
ZIPOBJ_EXTRA = bin/wcd.bat bin/wcd_win95.bat
# Using GnuWin32 gettext,iconv
ZIPOBJ_EXTRA += bin/libintl3.dll bin/libiconv2.dll
# Using MinGW gettext,iconv
#ZIPOBJ_EXTRA += bin/libintl-8.dll bin/libiconv-2.dll

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}" ENABLE_NLS=${ENABLE_NLS}

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} GCCFLAGS="${GCCFLAGS}" CURSES=${CURSES} LIB_CURSES=${LIB_CURSES} DEFS="${DEFS}" LDFLAGS_EXTRA="${LDFLAGS_EXTRA}" PREFIX=${PREFIX} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}" INSTALL_OBJS_DOC_INSTALL=../doc/INSTALL.dos_windows.txt ENABLE_NLS=${ENABLE_NLS}
	install -m 755 wcd.bat ${BINDIR}/wcd.bat
	install -m 755 wcd_win95.bat ${BINDIR}/wcd_win95.bat

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX} ENABLE_NLS=${ENABLE_NLS}
	-rm -f ${BINDIR}/wcd.bat
	-rm -f ${BINDIR}/wcd_win95.bat


dist :
	${MAKE} -C ../src dist-zip ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX} ENABLE_NLS=${ENABLE_NLS}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

