#
#   Copyright information
#
#	Copyright (C) 2009-2014 Erwin Waterlander
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
#       In order to clean generated POD files and manuals:
#
#           make maintainer-clean
#
#	Set additional flags for the build with variables CFLAGS_USER,
#	DEFS_USER and LDFLAGS_USER.

ifneq (,)
This makefile requires GNU Make.
endif

include version.mk

.PHONY: status uninstall install install-doc dist dist-zip dist-tgz
.PHONY: man txt html ps pdf mofiles tags merge


.PRECIOUS: %.1 %.pod

CC		?= gcc
STATIC		=
STRIP		= strip

ENABLE_NLS = 1

PACKAGE		= wcd
EXT		= .exe
PROGRAM         = $(PACKAGE)$(EXT)
BIN		= $(PROGRAM)

PREFIX		= /usr
prefix		= $(PREFIX)
exec_prefix	= $(prefix)
bindir		= $(exec_prefix)/bin
datarootdir	= $(prefix)/share
datadir		= $(datarootdir)

docsubdir	= $(PACKAGE)-$(VERSION)
docdir		= $(datarootdir)/doc/$(docsubdir)
localedir	= $(datarootdir)/locale
sysconfdir      = /etc

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

HTMLEXT = htm

INSTALL_OBJS_DOC_INSTALL = \
  ../doc/INSTALL.txt \
  ../doc/UNIX.txt \
  ../doc/RPM.txt

INSTALL_OBJS_DOC_COPYING = \
  ../doc/copying.txt

# Documents for end users, not for developers:
INSTALL_OBJS_DOC   = \
  ../doc/faq.txt \
  ../doc/whatsnew.txt \
  ../doc/README.txt \
  ../doc/problems.txt \
  ../doc/todo.txt \
  $(INSTALL_OBJS_DOC_INSTALL) \
  $(INSTALL_OBJS_DOC_COPYING) \
  man/man1/$(PACKAGE).txt \
  man/man1/$(PACKAGE).$(HTMLEXT)

ifdef ENABLE_NLS
POT		= po/$(PACKAGE).pot
POFILES		= $(wildcard po/*.po)
MOFILES		= $(patsubst %.po,%.mo,$(POFILES))
NLSSUFFIX	= -nls
endif
DOCFILES	= man/man1/$(PACKAGE).txt man/man1/$(PACKAGE).$(HTMLEXT)

MANPOTFILE      = man/man1/wcd-man.pot
MANPOFILES      = $(wildcard man/*/man1/wcd.po)
PODFILES	= $(patsubst %.po,%.pod,$(MANPOFILES))
PODFILES_ALL	= man/man1/wcd.pod $(PODFILES)
MANFILES	= $(patsubst %.pod,%.1,$(PODFILES))
TXTFILES	= $(patsubst %.pod,%.txt,$(PODFILES_ALL))
HTMLFILES	= $(patsubst %.pod,%.$(HTMLEXT),$(PODFILES_ALL))
PSFILES 	= $(patsubst %.pod,%.ps,$(PODFILES_ALL))
PDFFILES	= $(patsubst %.pod,%.pdf,$(PODFILES_ALL))

CPP		= cpp
CFLAGS_VERSION	= -DVERSION=\"$(VERSION)\" -DVERSION_DATE=\"$(VERSION_DATE)\"
DEF_UNIX        = -DUNIX
EXTRA_DEFS      =

VERSIONSUFFIX	= -bin

# Large File Support (LFS)
LFS             = 1
DEBUG           = 0

# Enble Unicode for Cygwin
ifeq ($(findstring CYGWIN,$(shell uname)),CYGWIN)
UCS = 1
UNINORM = 1
endif

ZIPOBJ		= bin/$(BIN) \
		  share/man/man1/$(PACKAGE).1 \
		  share/doc/$(docsubdir) \
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
ifdef UNINORM
	UCS = 1
	CFLAGS_UCS = -std=gnu99
	WDEFS	   = -DWCD_UNICODE -DWCD_UNINORM
	LIB_UNISTRING  = -lunistring
endif

ifdef UCS
UCSSUFFIX = -ucs
EXTRA_OBJ += matchw.o
endif

ZIPFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX)$(NLSSUFFIX)$(UCSSUFFIX).zip
TGZFILE = $(PACKAGE)-$(VERSION)$(VERSIONSUFFIX)$(NLSSUFFIX)$(UCSSUFFIX).tar.gz

ifdef ENABLE_NLS
	NLSDEFS    = -DENABLE_NLS -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
endif

# ........................................................... curses ...

# possible values: ncurses, curses, pdcurses, pdcursesw or <empty>

ifdef UCS
	CURSES = ncursesw
else
	CURSES = ncurses
endif

ifdef CURSES
ifdef UCS
EXTRA_OBJ += wcwidth.o
endif
endif

ifneq (,$(CURSES))
	DEFS_CURSES = -DWCD_USECURSES
	LIB_CURSES  = -l$(CURSES)
endif


INCPREFIX=/usr
ifeq ($(findstring MINGW,$(shell uname)),MINGW)
ifeq ($(shell gcc -dumpmachine),i686-w64-mingw32)
INCPREFIX=/mingw32
CFLAGS_COMPILER = -DWCD_COMPILER=MINGW32_W64
else
ifeq ($(shell gcc -dumpmachine),x86_64-w64-mingw32)
INCPREFIX=/mingw64
else
INCPREFIX=/mingw
endif
endif
endif

# Each ncurses variant has its own include directory
# ncurses (normal)/ncursesw (wide char)/ncursest (threads)
ifeq ($(findstring ncurses,$(CURSES)),ncurses)
ifneq ($(wildcard $(INCPREFIX)/include/$(CURSES)/curses.h),)
	INCFLAGS = -I$(INCPREFIX)/include/$(CURSES) -I$(INCPREFIX)/include
endif
ifeq ($(NCURSES_DEBUG), 1)
	LIB_CURSES  = -l$(CURSES)_g
endif
else
	INCFLAGS = -I$(INCPREFIX)/include
endif

ifeq ($(CURSES),ncursesw)
	EXTRA_DEFS += -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED
endif

ifeq ($(CURSES),pdcursesw)
	EXTRA_DEFS += -DPDC_STATIC_BUILD -DPDC_WIDE
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
ifeq (Linux, $(shell uname -s))
	OS = linux
endif
endif

ifndef OS
ifeq ($(findstring CYGWIN,$(shell uname)),CYGWIN)
	OS = cygwin
endif
endif

ifeq (cygwin,$(OS))
ifdef ENABLE_NLS
	LIBS_EXTRA    = -lintl -liconv -lmpr
else
	LIBS_EXTRA    = -lmpr
endif
	LDFLAGS_EXTRA = -Wl,--enable-auto-import
	INSTALL_TARGETS += install-profile
	docsubdir	= $(PACKAGE)
	VERSIONSUFFIX	= -cygwin
endif

ifndef OS
ifneq ($(DJGPP),)
	DEF_UNIX =
	OS = msdos
endif
endif

ifndef OS
ifeq ($(shell uname),OS/2)
	DEF_UNIX =
	OS = os/2
endif
endif

ifndef OS
ifeq ($(findstring MINGW32,$(shell uname)),MINGW32)
	CC = gcc
	DEF_UNIX =
	OS = mingw32
endif
endif

ifndef OS
ifeq ($(findstring MSYS,$(shell uname)),MSYS)
	CC = gcc
	DEF_UNIX =
	OS = msys
	PROGRAM = wcdmsys.exe
	LIBS_EXTRA  = -lkernel32 -luser32 -lmpr
	VERSIONSUFFIX	= -msys
ifdef ENABLE_NLS
	LIBS_EXTRA += -lintl
endif
endif
endif

ifndef OS
ifeq (FreeBSD, $(shell uname -s))
	OS = freebsd
endif
endif

ifeq (freebsd,$(OS))
	# Running under FreeBSD
ifdef ENABLE_NLS
	CFLAGS_OS     = -I/usr/local/include
	LDFLAGS_EXTRA = -L/usr/local/lib
	LIBS_EXTRA    = -lintl
endif
endif

ifeq (Darwin, $(shell uname -s))
	OS = Darwin
ifdef ENABLE_NLS
	CFLAGS_OS     = -I/usr/local/include
	LDFLAGS_EXTRA = -L/usr/local/lib
	LIBS_EXTRA    = -lintl
endif
endif


ifndef OS
ifneq (, $(wildcard /opt/csw))
	OS = sun
endif
endif

ifeq (sun,$(OS))
	# Running under SunOS/Solaris
	LIBS_EXTRA = -lintl
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
	OS = $(shell uname -s)
endif

# ............................................................ flags ...

# Statically linking of a specific library can be done by linking
# to a lib*.a library file instead a lib*.s* library file.
# To link ncurses statically (if your system links by default
# dynamically) comment the LDFLAGS line and add the 'libncurses.a' file
# (often found as /usr/lib/libncurses.a) to the OBJS1 list.

CFLAGS_USER	=
CFLAGS	?= -O2
CFLAGS  += -Wall -Wextra -Wno-unused-parameter -Wconversion $(RPM_OPT_FLAGS) $(CPPFLAGS) $(CFLAGS_USER)

EXTRA_CFLAGS	= -Ic3po \
		  $(CFLAGS_VERSION) \
		  $(CFLAGS_OS) \
		  $(CFLAGS_UCS) \
		  $(CFLAGS_COMPILER) \
		  $(WDEFS) \
		  $(NLSDEFS) \
		  $(NCFLAG) \
		  $(INCFLAGS) \
		  -DDEBUG=$(DEBUG)

ifeq ($(DEBUG), 1)
	EXTRA_CFLAGS += -g
endif

ifdef STATIC
	LDFLAG_STATIC = -static
endif

ifdef LFS
	EXTRA_CFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
endif

ifdef ASCII_TREE
	EXTRA_CFLAGS += -DASCII_TREE
endif

LDFLAGS_USER   =
LDFLAGS		= $(RPM_OPT_FLAGS) \
	          $(LDFLAGS_EXTRA) \
		  $(NLFLAG) \
		  $(LDFLAG_STATIC) \
		  $(LDFLAGS_USER)

LIBS = $(LIB_CURSES) \
       $(LIB_UNISTRING) \
       $(LIBS_EXTRA)

DEFS_USER   =
DEFS		= $(DEF_UNIX) $(DEFS_CURSES) $(EXTRA_DEFS) $(DEFS_USER)

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
	matchl.o \
	querycp.o \
	$(EXTRA_OBJ)

ifneq (,$(CURSES))
OBJS1 += colors.o \
	graphics.o
endif

all: $(BIN) $(DOCFILES) $(MOFILES) $(INSTALL_OBJS_MAN1) $(MANFILES) $(MANPOTFILE)

status:
	@echo "-- $(PACKAGE) Makefile settings begin --"
	@echo "OS            = $(OS)"
	@echo "prefix        = $(prefix)"
	@echo "EXT           = $(EXT)"
	@echo "UNINORM       = $(UNINORM)"
	@echo "STATIC        = $(STATIC)"
	@echo "UCS           = $(UCS)"
	@echo "ENABLE_NLS    = $(ENABLE_NLS)"
	@echo "CURSES        = $(CURSES)"
	@echo "DEFS          = $(DEFS)"
	@echo "DEFS_CURSES   = $(DEFS_CURSES)"
	@echo "EXTRA_DEFS    = $(EXTRA_DEFS)"
	@echo "LDFLAGS       = $(LDFLAGS)"
	@echo "LDFLAGS_EXTRA = $(LDFLAGS_EXTRA)"
	@echo "LIBS          = $(LIBS)"
	@echo "CFLAGS        = $(CFLAGS)"
	@echo "CFLAGS_OS     = $(CFLAGS_OS)"
	@echo "EXTRA_CFLAGS  = $(EXTRA_CFLAGS)"
	@echo "DEBUG         = $(DEBUG)"
	@echo "-- $(PACKAGE) Makefile settings end --"

$(BIN): $(OBJS1)
	$(MAKE) status
	$(CC) $(OBJS1) $(LDFLAGS) $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(DEFS) -c $< -o $@

%.o: c3po/%.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(DEFS) -c $< -o $@

%.pdf: %.ps
	ps2pdf $< $@

$(MANPOTFILE) : man/man1/wcd.pod
	$(MAKE) -C man/man1 $(notdir $@)

#  WARNING: Backward-incompatibility since GNU make 3.82.
#  The pattern-specific variables and pattern rules are now applied in the
#  shortest stem first order instead of the definition order (variables
#  and rules with the same stem length are still applied in the definition
#  order).
#  In order to stay compatible with GNU make < 3.82 we put the rule with
#  the shortest stem first.

po/%.po : $(POT)
	msgmerge -U $@ $(POT) --backup=numbered
	# change timestamp in case .po file was not updated.
	touch $@

%.po : man/man1/wcd.pod
	$(MAKE) -C man/man1 $(subst man/,../,$@)

%.pod : %.po
	$(MAKE) -C man/man1 $(subst man/,../,$@)

# Empty recipe to break circular dependency.
man/man1/wcd.pod : ;

%.1 : %.pod
	$(MAKE) -C man/man1 $(subst man/,../,$@)

%.txt : %.pod
	pod2text $< > $@

%.ps : %.1
	groff -man $< -T ps > $@

%.$(HTMLEXT) : %.pod
	pod2html --title="$(PACKAGE) $(VERSION) - Wherever Change Directory" $< > $@


man: man/man1/wcd.1 $(MANFILES) $(MANPOTFILE)

html: $(HTMLFILES)

txt: $(TXTFILES)

ps: $(PSFILES)

pdf: $(PDFFILES)

mofiles: $(MOFILES)

doc: $(DOCFILES)

tags: $(POT)

merge: $(POFILES)

# Get new po files from the Translation Project.
getpo:
	rsync -Lrtvz  translationproject.org::tp/latest/wcd/  po/incoming/

getpoman:
	rsync -Lrtvz  translationproject.org::tp/latest/wcd-man/  man/incoming/

%.mo : %.po
	msgfmt -c $< -o $@

$(POT) : wcd.c wcddir.c stack.c display.c graphics.c wfixpath.c c3po/Error.c
	xgettext -C --keyword=_ $+ -o $(POT)

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
	$(STRIP) $(BIN)

mostlyclean:
	@echo "-- target: mostlyclean"
	-rm -f \
		$(BIN) \
		*.exe \
		*.o \
		*.tmp \
		*/*.o \
		po/*.mo \
		../*/*.bck \
		../*/*.bak \
		../*/*[#~] \
		*.\#* \
		*/*.bak \
		*/*.bck \
		*/*~ \
		*/*/*.bak \
		*/*/*~ \
		*/*/*/*.bak \
		*/*/*/*~ \
		*.stackdump

clean: mostlyclean
	@echo "-- target: clean"
	rm -f man/man1/*.ps
	rm -f man/man1/*.pdf
	rm -f man/*/man1/*.ps
	rm -f man/*/man1/*.pdf

distclean: clean
	@echo "-- target: distclean"

maintainer-clean: distclean
	@echo "-- target: maintainer-clean:"
	rm -f man/man1/*.1
	rm -f man/man1/*.txt
	rm -f man/man1/*.$(HTMLEXT)
	rm -f man/man1/dos2unix-man.pot
	rm -f man/*/man1/*.1
	rm -f man/*/man1/*.txt
	rm -f man/*/man1/*.pod
	rm -f man/*/man1/*.$(HTMLEXT)

# 'maintainer-clean' was 'realclean' in old GNU standards.
realclean: maintainer-clean

# Install shell function (sh) and alias (csh).
# If DOTWCD=1, the shell scripts define WCDHOME=$HOME/.wcd
install-profile:
	@echo "-- target: install-profile"
	mkdir -p $(DESTDIR)$(sysconfdir)/profile.d
ifdef DOTWCD
	sed -e "s#BINDIR#$(bindir)#" -e "s/PROGRAM/$(BIN)/" -e 's/##//' etc/profile.d/wcd.sh > $(DESTDIR)$(sysconfdir)/profile.d/wcd.sh
	sed -e "s#BINDIR#$(bindir)#" -e "s/PROGRAM/$(BIN)/" -e 's/##//' etc/profile.d/wcd.csh > $(DESTDIR)$(sysconfdir)/profile.d/wcd.csh
else
	sed -e "s#BINDIR#$(bindir)#" -e "s/PROGRAM/$(BIN)/" etc/profile.d/wcd.sh > $(DESTDIR)$(sysconfdir)/profile.d/wcd.sh
	sed -e "s#BINDIR#$(bindir)#" -e "s/PROGRAM/$(BIN)/" etc/profile.d/wcd.csh > $(DESTDIR)$(sysconfdir)/profile.d/wcd.csh
endif

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
	# Install translated manuals when they have been generated.
	$(foreach txtfile, $(wildcard man/*/man1/*.txt), $(MKDIR) -p -m 755 $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(txtfile),)) ;)
	$(foreach txtfile, $(wildcard man/*/man1/*.txt), $(INSTALL) -m 644 $(txtfile) $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(txtfile),)) ;)
	$(foreach htmlfile, $(wildcard man/*/man1/*.$(HTMLEXT)), $(MKDIR) -p -m 755 $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(htmlfile),)) ;)
	$(foreach htmlfile, $(wildcard man/*/man1/*.$(HTMLEXT)), $(INSTALL) -m 644 $(htmlfile) $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(htmlfile),)) ;)
	$(foreach pdffile, $(wildcard man/*/man1/*.pdf), $(MKDIR) -p -m 755 $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(pdffile),)) ;)
	$(foreach pdffile, $(wildcard man/*/man1/*.pdf), $(INSTALL) -m 644 $(pdffile) $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(pdffile),)) ;)
	$(foreach pdffile, $(wildcard man/man1/*.pdf), $(INSTALL) -m 644 $(pdffile) $(DESTDIR)$(docdir) ;)
	$(foreach psfile, $(wildcard man/*/man1/*.ps), $(MKDIR) -p -m 755 $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(psfile),)) ;)
	$(foreach psfile, $(wildcard man/*/man1/*.ps), $(INSTALL) -m 644 $(psfile) $(DESTDIR)$(docdir)/$(word 2,$(subst /, ,$(psfile),)) ;)
	$(foreach psfile, $(wildcard man/man1/*.ps), $(INSTALL) -m 644 $(psfile) $(DESTDIR)$(docdir) ;)

install-man: $(INSTALL_OBJS_MAN1) $(MANFILES) $(MANPOTFILE)
	@echo "-- target: install-man"
	$(MKDIR) -p -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL_DATA) $(INSTALL_OBJS_MAN1) $(DESTDIR)$(man1dir)
	$(foreach manfile, $(MANFILES), $(MKDIR) -p -m 755 $(DESTDIR)$(datarootdir)/$(dir $(manfile)) ;)
	$(foreach manfile, $(MANFILES), $(INSTALL_DATA) -m 644 $(manfile) $(DESTDIR)$(datarootdir)/$(dir $(manfile)) ;)

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

# End of file
