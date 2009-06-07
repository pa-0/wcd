include ../src/version.mk

PREFIX = c:/usr/local
INSTALL=install

PACKAGE = wcd
PROGRAM = ${PACKAGE}.exe
BINDIR    = ${PREFIX}/bin
MANDIR    = ${PREFIX}/share/man
DOCDIR    = ${PREFIX}/share/doc/${PACKAGE}-${VERSION_SHORT}

# install and uninstall destination folders.
DESTBINDIR    = ${DESTDIR}${BINDIR}
DESTMANDIR    = ${DESTDIR}${MANDIR}
DESTDOCDIR    = ${DESTDIR}${DOCDIR}
ifdef CONIO
# binary conio
VERSIONSUFFIX = bc
else
# binary
VERSIONSUFFIX = b
endif
ZIPFILE = ${PACKAGE}${VERSION_SHORT}${VERSIONSUFFIX}.zip


all :
	@echo "targets: install uninstall zippackage"

install :
	${MAKE} -C ../src docfiles
	${INSTALL} -D -m 755 ${PROGRAM} ${DESTBINDIR}/${PROGRAM}
	${INSTALL} -D -m 644 ../src/man/man1/${PACKAGE}.1 ${DESTMANDIR}/man1/${PACKAGE}.1
	${INSTALL} -D -m 644 ../doc/copying.txt "${DESTDOCDIR}/copying.txt"
	${INSTALL} -D -m 644 ../doc/faq.txt ${DESTDOCDIR}/faq.txt
	${INSTALL} -D -m 644 ../doc/wcd.txt ${DESTDOCDIR}/wcd.txt
	${INSTALL} -D -m 644 ../doc/wcd.pdf ${DESTDOCDIR}/wcd.pdf
	${INSTALL} -D -m 644 ../doc/whatsnew.txt ${DESTDOCDIR}/whatsnew.txt
	${INSTALL} -D -m 644 ../doc/INSTALL.dos_windows.intl.txt ${DESTDOCDIR}/INSTALL.txt
	${INSTALL} -D -m 644 ../doc/README.txt ${DESTDOCDIR}/README.txt
	${INSTALL} -D -m 644 ../doc/problems.txt ${DESTDOCDIR}/problems.txt
	${INSTALL} -D -m 644 ../doc/todo.txt ${DESTDOCDIR}/todo.txt

uninstall :
	-rm -f ${DESTBINDIR}/${PROGRAM}
	-rm -f ${DESTMANDIR}/man1/${PACKAGE}.1
	-rm -rf ${DESTDOCDIR}


ZIPOBJ = bin/${PROGRAM} \
	share/man/man1/${PACKAGE}.1 \
	share/doc/${PACKAGE}-${VERSION_SHORT}


zippackage :
	cd ${PREFIX} ; zip -r ${ZIPFILE} ${ZIPOBJ}
	mv ${PREFIX}/${ZIPFILE} ../..

