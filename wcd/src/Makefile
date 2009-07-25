include version.mk

CC	= gcc

#PREFIX=${HOME}
PREFIX=/usr/local
#PREFIX=/usr

MKDIR=mkdir
INSTALL=install

# if ncurses is not on a default place edit the two following
# lines and uncomment them. Then run make.

#NCFLAG = -I/home/waterlan/include  # Place where (n)curses.h resides.
#NLFLAG = -L/home/waterlan/lib      # Place where libncurses.* resides.

# Statically linking of a specific library can be done by linking
# to a lib*.a library file instead a lib*.s* library file.
# To link ncurses statically (if your system links by default
# dynamically) comment the LFLAGS line and add the 'libncurses.a' file
# (often found as /usr/lib/libncurses.a) to the OBJS1 list.

# Flags and defs for wide character support.
ifdef WCD_UTF8
# WCD_UTF8 was used in older versions.
UCS=1
endif
ifdef UCS 
WCFLAGS   = -std=gnu99
WDEFS     = -DWCD_UNICODE
LFLAGS    = -lncursesw $(NLFLAG)
else
LFLAGS    = -lncurses $(NLFLAG)
endif

CFLAGS    = -O2 -Wall -Ic3po $(NCFLAG)
DEFS      = -DUNIX -DWCD_USECURSES
NLSDEFS   = -DENABLE_NLS -DLOCALEDIR=\"${LOCALEDIR}\" -DPACKAGE=\"${PACKAGE}\"

PACKAGE = wcd
PROGRAM = ${PACKAGE}.exe
BINDIR    = ${PREFIX}/bin
LOCALEDIR = ${PREFIX}/share/locale
MANDIR    = ${PREFIX}/share/man
DOCDIR    = ${PREFIX}/share/doc/${PACKAGE}-${VERSION}

# install and uninstall destination folders.
DESTBINDIR    = ${DESTDIR}${BINDIR}
DESTLOCALEDIR = ${DESTDIR}${LOCALEDIR}
DESTMANDIR    = ${DESTDIR}${MANDIR}
DESTDOCDIR    = ${DESTDIR}${DOCDIR}


POT = po/${PACKAGE}.pot
POFILES = $(wildcard po/*.po)
MOFILES = $(patsubst %.po,%.mo,$(POFILES))
DOCFILES = ../doc/${PACKAGE}.txt ../doc/${PACKAGE}.ps ../doc/${PACKAGE}.pdf

VERSIONSUFFIX = -bin
ifdef UCS 
ZIPFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}-ucs.zip
TGZFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}-ucs.tar.gz
else
ZIPFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}.zip
TGZFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}.tar.gz
endif

OBJS1	= \
	nameset.o \
	intset.o \
	Error.o \
	Text.o \
	WcdStack.o \
	dirnode.o \
	wcd.o \
	match.o \
	stack.o \
	display.o \
	dosdir.o \
	wfixpath.o \
	colors.o \
	graphics.o \
	wcddir.o \
	${EXTRA_OBJ}

#all: ${PROGRAM} ${MOFILES} ${DOCFILES}
all: ${PROGRAM}

${PROGRAM}: $(OBJS1)
	$(CC) $(OBJS1) $(LFLAGS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $(WCFLAGS) $(DEFS) $(WDEFS) $(NLSDEFS) $< -o $@

%.o : c3po/%.c
	$(CC) -c $(CFLAGS) $(WCFLAGS) $(DEFS) $(WDEFS) $(NLSDEFS) $< -o $@


${POT} : wcd.c wcddir.c stack.c display.c graphics.c wfixpath.c c3po/Error.c
	xgettext -C --keyword=_ $+ -o ${POT}

tags : ${POT}

merge : ${POFILES}

%.po : ${POT}
	msgmerge -U $@ ${POT} --backup=numbered

%.mo : %.po
	msgfmt -c $< -o $@

mofiles : ${MOFILES}

docfiles : ${DOCFILES}

../doc/${PACKAGE}.txt : man/man1/wcd.1
	man -M ./man wcd | col -b > $@

../doc/${PACKAGE}.ps : man/man1/wcd.1
	man -M ./man -t wcd > $@

%.pdf : %.ps
	ps2pdf $< $@

install : ${PROGRAM} ${MOFILES} ${DOCFILES}
	${INSTALL} -D -m 755 ${PROGRAM} ${DESTBINDIR}/${PROGRAM}
	$(foreach mofile, ${MOFILES}, ${INSTALL} -D -m 644 ${mofile} ${DESTLOCALEDIR}/$(basename $(notdir ${mofile}))/LC_MESSAGES/${PACKAGE}.mo ;)
	${INSTALL} -D -m 644 man/man1/${PACKAGE}.1 ${DESTMANDIR}/man1/${PACKAGE}.1
	${INSTALL} -D -m 644 ../doc/copying.txt "${DESTDOCDIR}/copying.txt"
	${INSTALL} -D -m 644 ../doc/faq.txt ${DESTDOCDIR}/faq.txt
	${INSTALL} -D -m 644 ../doc/wcd.txt ${DESTDOCDIR}/wcd.txt
	${INSTALL} -D -m 644 ../doc/wcd.ps ${DESTDOCDIR}/wcd.ps
	${INSTALL} -D -m 644 ../doc/wcd.pdf ${DESTDOCDIR}/wcd.pdf
	${INSTALL} -D -m 644 ../doc/whatsnew.txt ${DESTDOCDIR}/whatsnew.txt
	${INSTALL} -D -m 644 ../doc/INSTALL.dos_windows.intl.txt ${DESTDOCDIR}/INSTALL.dos_windows.intl.txt
	${INSTALL} -D -m 644 ../doc/INSTALL.unix.intl.txt ${DESTDOCDIR}/INSTALL.unix.intl.txt
	${INSTALL} -D -m 644 ../doc/INSTALL.os2.txt ${DESTDOCDIR}/INSTALL.os2.txt
	${INSTALL} -D -m 644 ../doc/INSTALL.rpm.txt ${DESTDOCDIR}/INSTALL.rpm.txt
	${INSTALL} -D -m 644 ../doc/README.txt ${DESTDOCDIR}/README.txt
	${INSTALL} -D -m 644 ../doc/problems.txt ${DESTDOCDIR}/problems.txt
	${INSTALL} -D -m 644 ../doc/translate.txt ${DESTDOCDIR}/translate.txt
	${INSTALL} -D -m 644 ../doc/todo.txt ${DESTDOCDIR}/todo.txt

uninstall :
	-rm -f ${DESTBINDIR}/${PROGRAM}
	$(foreach mofile, ${MOFILES}, rm -f ${DESTLOCALEDIR}/$(basename $(notdir ${mofile}))/LC_MESSAGES/${PACKAGE}.mo ;)
	-rm -f ${DESTMANDIR}/man1/${PACKAGE}.1
	-rm -rf ${DESTDOCDIR}

ZIPOBJ = bin/${PROGRAM} \
	share/locale/*/LC_MESSAGES/${PACKAGE}.mo \
	share/man/man1/${PACKAGE}.1 \
	share/doc/${PACKAGE}-${VERSION} \
	${ZIPOBJ_EXTRA}


zippackage :
	cd ${PREFIX} ; zip -r ${ZIPFILE} ${ZIPOBJ}
	mv ${PREFIX}/${ZIPFILE} ../..

tgzpackage :
	cd ${PREFIX} ; tar cvzf ${TGZFILE} ${ZIPOBJ}
	mv ${PREFIX}/${TGZFILE} ../..

strip:
	strip ${PROGRAM}

clean:
	-rm -f *.o
	-rm -f *.exe
	-rm -f ../*/*.bak
	-rm -f ../*/*~
	-rm -f ../*/*.bck
	-rm -f */*.bak
	-rm -f */*~
	-rm -f */*.bck
	-rm -f */*/*.bak
	-rm -f */*/*~

