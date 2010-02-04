#
#   Copyright information
#
#	Copyright (C) 2009-2010 Erwin Waterlander
#	Copyright (C) 2009 Jari Aalto
#
#   License
#
#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License as
#	published by the Free Software Foundation; either version 2 of the
#	License, or (at your option) any later version
#
#	This program is distributed in the hope that it will be useful, but
#	WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#	General Public License for more details at
#	Visit <http://www.gnu.org/copyleft/gpl.html>.
#
#   Description
#
#	This is a GNU Makefile that uses GNU compilers, linkers and cpp. The
#	platform specific issues are determined by the various OS teets that
#	rely on the uname(1) command and directory locations.
#
#   Developer notes
#
#	The documentation default is set for DOS and Unix.
#
#       In order to generate documentation for Unix only, use:
#
#	    make CPP_FLAGS_POD="-Dunix" man
#
#       In order to generate documentation for DOS/Windows only, use:
#
#	    make CPP_FLAGS_POD="-Ddos" man
#
#       In order to clean generated POD file:
#
#           make maintainer-clean


ifneq (,)
This makefile requires GNU Make.
endif

include version.mk

CC		= gcc
STATIC		=

ENABLE_NLS = 1

PACKAGE		= wcd
EXT		= .exe
PROGRAM         = $(PACKAGE)$(EXT)
BIN		= $(PROGRAM)

PREFIX		= /usr/local
prefix		= $(PREFIX)
exec_prefix	= $(prefix)
bindir		= $(exec_prefix)/bin
datarootdir	= $(prefix)/share
datadir		= $(datarootdir)

docdir		= $(datarootdir)/doc/$(PACKAGE)-$(VERSION)
localedir	= $(datarootdir)/locale

# 1 = regular, 5 = conf, 6 = games, 8 = daemons
mandir		= $(datarootdir)/man
man1dir		= $(mandir)/man1


# On some systems (e.g. FreeBSD 4.10) GNU install is installed as `ginstall'.
INSTALL		= install
# On some systems (e.g. GNU Win32) GNU mkdir is installed as `gmkdir'.
MKDIR           = mkdir

INSTALL_BIN	= $(INSTALL) -m 755
INSTALL_DATA	= $(INSTALL) -m 644
INSTALL_SUID	= $(INSTALL) -m 4755

INSTALL_OBJS_BIN   = $(BIN)
INSTALL_OBJS_MAN1  = man/man1/wcd.1

INSTALL_OBJS_DOC_INSTALL = \
  ../doc/INSTALL.unix.txt \
  ../doc/INSTALL.rpm.txt \
  ../doc/translate.txt

INSTALL_OBJS_DOC_COPYING = \
  ../doc/copying.txt

# Documents for end users, not for developers:
INSTALL_OBJS_DOC   = \
  ../doc/faq.txt \
  ../doc/$(PACKAGE).txt \
  ../doc/$(PACKAGE).ps \
  ../doc/$(PACKAGE).pdf \
  ../doc/$(PACKAGE).html \
  ../doc/whatsnew.txt \
  ../doc/README.txt \
  ../doc/problems.txt \
  ../doc/todo.txt \
  $(INSTALL_OBJS_DOC_INSTALL) \
  $(INSTALL_OBJS_DOC_COPYING)

ifdef ENABLE_NLS
POT		= po/$(PACKAGE).pot
POFILES		= $(wildcard po/*.po)
MOFILES		= $(patsubst %.po,%.mo,$(POFILES))
endif
DOCFILES	= ../doc/$(PACKAGE).txt ../doc/$(PACKAGE).ps ../doc/$(PACKAGE).pdf ../doc/$(PACKAGE).html

CPP		= cpp
CPP_FLAGS_POD	= ALL

VERSIONSUFFIX	= -bin

ifdef UCS
ZIPFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX)-ucs.zip
TGZFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX)-ucs.tar.gz
else
ZIPFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX).zip
TGZFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX).tar.gz
endif

ZIPOBJ		= bin/$(BIN) \
		  share/man/man1/$(PACKAGE).1 \
		  share/doc/$(PACKAGE)-$(VERSION) \
		  $(ZIPOBJ_EXTRA)

INSTALL_TARGETS = install-bin install-man install-doc

ifdef ENABLE_NLS
	INSTALL_TARGETS += install-mo
	ZIPOBJ += share/locale/*/LC_MESSAGES/$(PACKAGE).mo
endif

# .......................................................... unicode ...

ifdef WCD_UTF8
	# WCD_UTF8 was used in older versions.
	UCS=1
endif

ifdef UCS
	CFLAGS_UCS = -std=gnu99
	WDEFS	   = -DWCD_UNICODE
endif

ifdef ENABLE_NLS
	NLSDEFS    = -DENABLE_NLS -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
endif

# ........................................................... curses ...

# possible values: ncurses, curses, pdcurses or <empty>
CURSES = ncurses

ifdef UCS
	CURSES  = ncursesw
endif

ifneq (,$(CURSES))
	DEFS_CURSES = -DWCD_USECURSES
	LIB_CURSES  = -l$(CURSES)
endif

ifeq ($(CURSES),ncursesw)
	EXTRA_DEFS = -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED

	# Pick right include:
	# libncurses5-dev: /usr/include/curses.h
	# libncursesw5-dev: /usr/include/ncursesw/curses.h

ifneq (,$(wildcard /usr/include/ncursesw/curses.h))
	INCFLAGS = -I/usr/include/ncursesw
endif
endif

ifeq ($(CURSES),pdcurses)
	DEFS_CURSES = -DWCD_USECURSES
	EXTRA_DEFS  = -fomit-frame-pointer -DXCURSES
	LIB_CURSES  = -lXCurses -L/usr/X11R6/lib -lXaw -lXmu -lXt -lX11 -lSM -lICE -lXext
endif

# Local installation prefix of ncurses.
#LOCAL_NCURSES	= $(HOME)
#
#ifneq (, $(wildcard $(LOCAL_NCURSES)/include/ncurses.h))
#	NCFLAG = -I$(LOCAL_NCURSES)/include
#	NLFLAG = -L$(LOCAL_NCURSES)/lib
#endif

# ......................................................... OS flags ...

OS =

ifndef OS
ifneq (, $(wildcard /boot/vmlinuz*))
	OS = linux
endif
endif

ifndef OS
ifneq (, $(wildcard /cygdrive))
	OS = cygwin
endif
endif

ifeq (cygwin,$(OS))
	LDFLAGS_EXTRA = -lintl -liconv -lmpr -Wl,--enable-auto-import
endif


ifndef OS
ifeq (FreeBSD, $(shell uname -s))
	OS = freebsd
endif
endif

ifeq (freebsd,$(OS))
	# Running under FreeBSD
	CFLAGS_OS     = -I/usr/local/include
	LDFLAGS_EXTRA = -lintl -L/usr/local/lib
endif

ifndef OS
ifneq (, $(wildcard /opt/csw))
	OS = sun
endif
endif

ifeq (sun,$(OS))
	# Running under SunOS/Solaris
	LDFLAGS_EXTRA = -lintl
endif

ifndef OS
ifeq (HP-UX, $(shell uname -s))
	OS = hpux
endif
endif

ifeq (hpux,$(OS))
	# Running under HP-UX
	EXTRA_DEFS += -Dhpux -D_HPUX_SOURCE -D_XOPEN_SOURCE_EXTENDED

	# These flags were for native HP compiler
	# CFLAGS_OS = -O -Aa
endif

ifndef OS
	OS = unknown
endif

# ............................................................ flags ...

# Statically linking of a specific library can be done by linking
# to a lib*.a library file instead a lib*.s* library file.
# To link ncurses statically (if your system links by default
# dynamically) comment the LDFLAGS line and add the 'libncurses.a' file
# (often found as /usr/lib/libncurses.a) to the OBJS1 list.

CFLAGS	= -O2 -Wall

EXTRA_CFLAGS	= -Ic3po \
		  $(CFLAGS_OS) \
		  $(CFLAGS_UCS) \
		  $(WDEFS) \
		  $(NLSDEFS) \
		  $(NCFLAG) \
		  $(INCFLAGS)

ifdef STATIC
	EXTRA_CFLAGS += -static
endif

LDFLAGS		= $(LIB_CURSES) \
	          $(LDFLAGS_EXTRA) \
		  $(NLFLAG)

DEFS		= -DUNIX $(DEFS_CURSES) $(EXTRA_DEFS)

# .......................................................... targets ...

OBJS1 = \
	wcd.o \
	match.o \
	stack.o \
	nameset.o \
	intset.o \
	Error.o \
	Text.o \
	WcdStack.o \
	dirnode.o \
	display.o \
	dosdir.o \
	wfixpath.o \
	wcddir.o \
	$(EXTRA_OBJ)

ifneq (,$(CURSES))
OBJS1 += colors.o \
	graphics.o
endif

all: $(BIN) $(DOCFILES) $(MOFILES)

status:
	@echo "-- $(PACKAGE) Makefile settings begin --"
	@echo "OS            = $(OS)"
	@echo "prefix        = $(prefix)"
	@echo "EXT           = $(EXT)"
	@echo "CPP_FLAGS_POD = $(CPP_FLAGS_POD)"
	@echo "STATIC        = $(STATIC)"
	@echo "UCS           = $(UCS)"
	@echo "ENABLE_NLS    = $(ENABLE_NLS)"
	@echo "CURSES        = $(CURSES)"
	@echo "DEFS          = $(DEFS)"
	@echo "DEFS_CURSES   = $(DEFS_CURSES)"
	@echo "EXTRA_DEFS    = $(EXTRA_DEFS)"
	@echo "LDFLAGS_EXTRA = $(LDFLAGS_EXTRA)"
	@echo "CFLAGS        = $(CFLAGS)"
	@echo "CFLAGS_OS     = $(CFLAGS_OS)"
	@echo "EXTRA_CFLAGS  = $(EXTRA_CFLAGS)"
	@echo "-- $(PACKAGE) Makefile settings end --"

$(BIN): $(OBJS1)
	$(MAKE) status
	$(CC) $(OBJS1) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(DEFS) -c $< -o $@

%.o: c3po/%.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(DEFS) -c $< -o $@

%.pdf: %.ps
	ps2pdf $< $@

%.po : $(POT)
	msgmerge -U $@ $(POT) --backup=numbered

%.mo : %.po
	msgfmt -c $< -o $@

$(POT) : wcd.c wcddir.c stack.c display.c graphics.c wfixpath.c c3po/Error.c
	xgettext -C --keyword=_ $+ -o $(POT)

man/man1/wcd.1.pod : man/man1/wcd.1.pod.in
ifeq ($(CPP_FLAGS_POD),ALL)
	sed 	-e '/#ifdef/d' \
		-e '/#endif/d' \
		-e '/#define/d' \
		-e '/^\/\*/d' \
		-e '/^\*\//d' \
		-e 's/WILDSTAR()/*/g' \
		$< > $@
else
	$(CPP) -traditional-cpp -P -fno-show-column $(CPP_FLAGS_POD) $< > $@
endif

man/man1/wcd.1 : man/man1/wcd.1.pod
	$(MAKE) -C man/man1

../doc/$(PACKAGE).txt : man/man1/wcd.1.pod
	LC_CTYPE=C pod2text $< > $@

../doc/$(PACKAGE).ps : man/man1/wcd.1
	groff -man $< -T ps > $@

../doc/$(PACKAGE).html : man/man1/wcd.1.pod
	pod2html --title="$(PACKAGE) $(VERSION) - Wherever Change Directory" $< > $@


pod: man/man1/wcd.1.pod

man: man/man1/wcd.1

html: ../doc/$(PACKAGE).html

txt: ../doc/$(PACKAGE).txt

ps: ../doc/$(PACKAGE).ps

pdf: ../doc/$(PACKAGE).pdf

mofiles: $(MOFILES)

docfiles: $(DOCFILES)

tags: $(POT)

merge: $(POFILES)

dist-zip:
	rm -f $(prefix)/$(ZIPFILE)
	cd $(prefix) ; zip -r $(ZIPFILE) $(ZIPOBJ)
	mv -f $(prefix)/$(ZIPFILE) ../..

dist-tgz:
	cd $(prefix) ; tar cvzf $(TGZFILE) $(ZIPOBJ)
	mv -f $(prefix)/$(TGZFILE) ../..

dist: dist-tgz


strip:
	@echo "-- target: strip"
	strip $(BIN)

clean:
	@echo "-- target: clean"
	-rm -f \
		$(BIN) \
		*.o \
		*/*.o \
		../*/*.bck \
		../*/*.bak \
		../*/*[#~] \
		*.\#* \
		*/*.bak \
		*/*.bck \
		*/*~ \
		*/*/*.bak \
		*/*/*~ \
		*.stackdump

distclean: clean

maintainer-clean: distclean
	@echo "-- target: maintainer-clean:"
	rm -f \
	  man/man1/wcd.1.pod \
	  po/*.mo \
	  man/man1/wcd.1 \
	  $(DOCFILES) \
	  $(MOFILES)

# 'maintainer-clean' was 'realclean' in old GNU standards.
realclean: maintainer-clean


# Old versions of install(1) don't support option -D. Use
# mkdir instead. Was seen on HP-UX 11 and DOS DJGPP.

install-mo: $(MOFILES)
	@echo "-- target: install-mo"
	$(foreach mofile, $(MOFILES), $(MKDIR) -p -m 755 $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES ;)
	$(foreach mofile, $(MOFILES), $(INSTALL) -m 644 $(mofile) $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)

install-doc: $(DOCFILES)
	@echo "-- target: install-doc"
	$(MKDIR) -p -m 755 $(DESTDIR)$(docdir)
	$(INSTALL_DATA) $(INSTALL_OBJS_DOC) $(DESTDIR)$(docdir)

install-man: $(INSTALL_OBJS_MAN1)
	@echo "-- target: install-man"
	$(MKDIR) -p -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL_DATA) $(INSTALL_OBJS_MAN1) $(DESTDIR)$(man1dir)

install-bin: $(BIN)
	@echo "-- target: install-bin"
	$(MKDIR) -p -m 755 $(DESTDIR)$(bindir)
	$(INSTALL_BIN) $(INSTALL_OBJS_BIN) $(DESTDIR)$(bindir)

install: $(INSTALL_TARGETS)

uninstall:
	@echo "-- target: uninstall"
	-rm -f $(DESTDIR)$(bindir)/$(BIN)
ifdef ENABLE_NLS
	$(foreach mofile, $(MOFILES), rm -f $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
endif
	-rm -f $(DESTDIR)$(mandir)/man1/$(PACKAGE).1
	-rm -rf $(DESTDIR)$(docdir)

.PHONY: status uninstall install install-doc dist dist-zip dist-tgz
.PHONY: man pod ps txt html pdf

# End of file
