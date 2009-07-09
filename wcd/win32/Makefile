
PREFIX = c:/usr/local
MKDIR  = mkdir

CFLAGS = -Ic3po -O3 -Wall
LFLAGS = -lpdcurses -lintl -liconv -lkernel32 -luser32 -lmpr
ifdef WCD_UTF8 
DEFS   = -DMSDOS -DWCD_USECURSES -DPDC_STATIC_BUILD -DPDC_WIDE
EXTRA_OBJ=wcwidth.o langinfo.o
else
DEFS   = -DMSDOS -DWCD_USECURSES -DPDC_STATIC_BUILD
endif
BINDIR    = ${PREFIX}/bin
PROGRAM = wcdwin32.exe
VERSIONSUFFIX = -win32
ZIPOBJ_EXTRA = bin/wcd.bat bin/wcd_win95.bat bin/libintl3.dll bin/libiconv2.dll

all :
	${MAKE} -C ../src all PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX} EXTRA_OBJ="${EXTRA_OBJ}"

install :
	${MAKE} -C ../src install PROGRAM=${PROGRAM} CFLAGS="${CFLAGS}" DEFS="${DEFS}" LFLAGS="${LFLAGS}" PREFIX=${PREFIX} MKDIR=${MKDIR} EXTRA_OBJ="${EXTRA_OBJ}"
	install -m 755 wcd.bat ${BINDIR}/wcd.bat
	install -m 755 wcd_win95.bat ${BINDIR}/wcd_win95.bat

uninstall :
	${MAKE} -C ../src uninstall PROGRAM=${PROGRAM} PREFIX=${PREFIX}
	-rm -f ${BINDIR}/wcd.bat
	-rm -f ${BINDIR}/wcd_win95.bat


zippackage :
	${MAKE} -C ../src zippackage ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" PROGRAM=${PROGRAM} PREFIX=${PREFIX} VERSIONSUFFIX=${VERSIONSUFFIX}

strip:
	${MAKE} -C ../src strip PROGRAM=${PROGRAM}

clean :
	${MAKE} -C ../src clean

