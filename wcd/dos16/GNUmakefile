include ../src/version.mk

PREFIX = c:/usr/local
prefix = $(PREFIX)
exec_prefix = $(prefix)
INSTALL=install
MKDIR=mkdir

PACKAGE = wcd
PROGRAM = ${PACKAGE}.exe
bindir    = ${exec_prefix}/bin
datarootdir = $(prefix)/share
mandir    = ${datarootdir}/man
docsubdir = ${PACKAGE}-${VERSION_SHORT}
docdir    = ${datarootdir}/doc/${docsubdir}

ifdef CONIO
# binary conio
VERSIONSUFFIX = bc
else
# binary
VERSIONSUFFIX = b
endif
ZIPFILE = ${PACKAGE}${VERSION_SHORT}${VERSIONSUFFIX}.zip


all :
	@echo "targets: install uninstall dist"

install :
	${MAKE} -C ../src docfiles
	${MKDIR} -p -m 755 ${DESTDIR}${bindir}
	${MKDIR} -p -m 755 ${DESTDIR}${mandir}/man1
	${MKDIR} -p -m 755 ${DESTDIR}${docdir}
	${INSTALL} -m 755 ${PROGRAM} ${DESTDIR}${bindir}/${PROGRAM}
	${INSTALL} -m 644 ../src/man/man1/${PACKAGE}.1 ${DESTDIR}${mandir}/man1/${PACKAGE}.1
	${INSTALL} -m 644 ../doc/copying.txt "${DESTDIR}${docdir}/copying.txt"
	${INSTALL} -m 644 ../doc/faq.txt ${DESTDIR}${docdir}/faq.txt
	${INSTALL} -m 644 ../doc/wcd.txt ${DESTDIR}${docdir}/wcd.txt
	${INSTALL} -m 644 ../doc/wcd.pdf ${DESTDIR}${docdir}/wcd.pdf
	${INSTALL} -m 644 ../doc/whatsnew.txt ${DESTDIR}${docdir}/whatsnew.txt
	${INSTALL} -m 644 ../doc/INSTALL.dos_windows.txt ${DESTDIR}${docdir}/INSTALL.txt
	${INSTALL} -m 644 ../doc/README.txt ${DESTDIR}${docdir}/README.txt
	${INSTALL} -m 644 ../doc/problems.txt ${DESTDIR}${docdir}/problems.txt
	${INSTALL} -m 644 ../doc/todo.txt ${DESTDIR}${docdir}/todo.txt

uninstall :
	-rm -f ${DESTDIR}${bindir}/${PROGRAM}
	-rm -f ${DESTDIR}${mandir}/man1/${PACKAGE}.1
	-rm -rf ${DESTDIR}${docdir}


ZIPOBJ = bin/${PROGRAM} \
	share/man/man1/${PACKAGE}.1 \
	share/doc/${docsubdir}


dist :
	cd ${prefix} ; zip -r ${ZIPFILE} ${ZIPOBJ}
	mv ${prefix}/${ZIPFILE} ../..

