include version.mk

CC	= gcc

#PREFIX=${HOME}
PREFIX=/usr/local
#PREFIX=/usr

MKDIR=mkdir

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
WCFLAGS   = -std=gnu99
WDEFS     = -DWCD_UTF8
endif

CFLAGS    = -O -Wall -Ic3po $(NCFLAG)
DEFS      = -DUNIX -DWCD_USECURSES
NLSDEFS   = -DENABLE_NLS -DLOCALEDIR=\"${LOCALEDIR}\" -DPACKAGE=\"${PACKAGE}\"
LFLAGS    = -lncurses $(NLFLAG)

PACKAGE = wcd
PROGRAM = ${PACKAGE}.exe
BINDIR    = ${PREFIX}/bin
LOCALEDIR = ${PREFIX}/share/locale
MANDIR    = ${PREFIX}/share/man
DOCDIR    = ${PREFIX}/share/doc/${PACKAGE}-${VERSION}

POT = po/${PACKAGE}.pot
POFILES = $(wildcard po/*.po)
MOFILES = $(patsubst %.po,%.mo,$(POFILES))
DOCFILES = ../doc/${PACKAGE}.txt ../doc/${PACKAGE}.ps ../doc/${PACKAGE}.pdf

VERSIONSUFFIX = -bin
ZIPFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}.zip
TGZFILE = ${PACKAGE}-${VERSION}${VERSIONSUFFIX}.tar.gz

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
	wcddir.o

all: ${PROGRAM} ${MOFILES} ${DOCFILES}

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
	${MKDIR} -p ${BINDIR}
	install -m 755 ${PROGRAM} ${BINDIR}/${PROGRAM}
	$(foreach mofile, ${MOFILES}, ${MKDIR} -p ${LOCALEDIR}/$(basename $(notdir ${mofile}))/LC_MESSAGES)
	$(foreach mofile, ${MOFILES}, install -m 644 ${mofile} ${LOCALEDIR}/$(basename $(notdir ${mofile}))/LC_MESSAGES/${PACKAGE}.mo)
	${MKDIR} -p ${MANDIR}/man1
	install -m 644 man/man1/${PACKAGE}.1 ${MANDIR}/man1/${PACKAGE}.1
	${MKDIR} -p ${DOCDIR}
	install -m 644 ../doc/copying.txt "${DOCDIR}/copying.txt"
	install -m 644 ../doc/faq.txt ${DOCDIR}/faq.txt
	install -m 644 ../doc/wcd.txt ${DOCDIR}/wcd.txt
	install -m 644 ../doc/wcd.ps ${DOCDIR}/wcd.ps
	install -m 644 ../doc/wcd.pdf ${DOCDIR}/wcd.pdf
	install -m 644 ../doc/whatsnew.txt ${DOCDIR}/whatsnew.txt
	install -m 644 ../doc/INSTALL.dos_windows.intl.txt ${DOCDIR}/INSTALL.dos_windows.intl.txt
	install -m 644 ../doc/INSTALL.unix.intl.txt ${DOCDIR}/INSTALL.unix.intl.txt
	install -m 644 ../doc/INSTALL.os2.txt ${DOCDIR}/INSTALL.os2.txt
	install -m 644 ../doc/INSTALL.rpm.txt ${DOCDIR}/INSTALL.rpm.txt
	install -m 644 ../doc/README.txt ${DOCDIR}/README.txt
	install -m 644 ../doc/problems.txt ${DOCDIR}/problems.txt
	install -m 644 ../doc/translate.txt ${DOCDIR}/translate.txt

uninstall :
	-rm -f ${BINDIR}/${PROGRAM}
	$(foreach mofile, ${MOFILES}, rm -f ${LOCALEDIR}/$(basename $(notdir ${mofile}))/LC_MESSAGES/${PACKAGE}.mo)
	-rm -f ${MANDIR}/man1/${PACKAGE}.1
	-rm -rf ${DOCDIR}

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

