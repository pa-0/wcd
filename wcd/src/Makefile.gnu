#
#   Copyright information
#
#	Copyright (C) 1997-2009 Erwin Waterlander
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

ifneq (,)
This makefile requires GNU Make.
endif

ifeq (gcc,$(notdir $(shell which gcc)))
CC	= gcc
else
# gcc not available, use native cc.
CC	= cc
endif
EXT	= .exe

PACKAGE		= wcd
DESTDIR		=
prefix		= /usr/local
exec_prefix	= $(prefix)
man_prefix	= $(prefix)/share
mandir		= $(man_prefix)/man
bindir		= $(exec_prefix)/bin
sharedir	= $(prefix)/share

BINDIR		= $(DESTDIR)$(bindir)
DOCDIR		= $(DESTDIR)$(sharedir/doc
SHAREDIR	= $(DESTDIR)$(prefix)/share/$(PACKAGE)
LIBDIR		= $(DESTDIR)$(prefix)/lib/$(PACKAGE)
SBINDIR		= $(DESTDIR)$(exec_prefix)/sbin
ETCDIR		= $(DESTDIR)/etc/$(PACKAGE)

# 1 = regular, 5 = conf, 6 = games, 8 = daemons
MANDIR		= $(DESTDIR)$(mandir)
MANDIR1		= $(MANDIR)/man1
MANDIR5		= $(MANDIR)/man5
MANDIR6		= $(MANDIR)/man6
MANDIR8		= $(MANDIR)/man8

INSTALL_OBJS_BIN   = $(PACKAGE)$(EXT)
INSTALL_OBJS_MAN1  = man/man1/*.1

INSTALL		= install
INSTALL_BIN	= $(INSTALL) -m 755
INSTALL_DATA	= $(INSTALL) -m 644
INSTALL_SUID	= $(INSTALL) -m 4755

# Local installation prefix of ncurses.
LOCAL_NCURSES	= ${HOME}

ifneq (, $(wildcard ${LOCAL_NCURSES}/include/ncurses.h))
	NCFLAG = -I${LOCAL_NCURSES}/include
	NLFLAG = -L${LOCAL_NCURSES}/lib
endif

ifneq (, $(wildcard /opt/csw))
	# Running under SunOS/Solaris
	EXTRA_DEFS = -D_XOPEN_SOURCE_EXTENDED -DSYSV
endif


ifeq (HP-UX, $(shell uname -s))
	# Running under HP-UX
	EXTRA_DEFS = -Dhpux -D_HPUX_SOURCE -D_XOPEN_SOURCE_EXTENDED
ifeq ($(CC), cc)
	CFLAGS_OS = -O -Aa
endif
endif


CURSES = enable

ifneq (,$(CURSES))
	LCURSES		= -lncurses
	DEFS_CURSES	= -DWCD_USECURSES

ifeq (, $(wildcard /usr/lib/libncurses.a))
	# No system installation of ncurses found.
	LCURSES		= -lcurses
endif

ifneq (, $(wildcard ${LOCAL_NCURSES}/include/ncurses.h))
	# A local installation of ncurses found.
	LCURSES		= -lncurses
endif

endif

# Statically linking of a specific library can be done by linking
# to a lib*.a library file instead a lib*.s* library file.
# To link ncurses statically (if your system links by default
# dynamically) comment the LFLAGS line and add the 'libncurses.a' file
# (often found as /usr/lib/libncurses.a) to the OBJS1 list.

GCCFLAGS	= -O -Wall 
ifeq ($(CC), gcc)
CFLAGS		= -Ic3po $(GCCFLAGS) $(NCFLAG)
else
CFLAGS		= -Ic3po $(CFLAGS_OS) $(NCFLAG)
endif

LFLAGS		= $(LCURSES) $(NLFLAG)

DEFS		= -DUNIX $(DEFS_CURSES) $(EXTRA_DEFS)

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
	colors.o \
	graphics.o \
	wcddir.o

all:$(PACKAGE)$(EXT)

$(PACKAGE)$(EXT): $(OBJS1)
	$(CC) $(OBJS1) $(LFLAGS) -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

%.o : c3po/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@


clean:
	/bin/rm -f \
		$(PACKAGE)$(EXT) \
		*.o \
		*/*.o \
		*.bck \
		*.bak \
		*[#~] \
		*.\#* \
		*.stackdump

distclean: clean

realclean: clean

install-etc:
	# install-etc: Nothing yet
	# $(INSTALL_BIN) -d $(ETCDIR)
	# $(INSTALL_BIN)    $(INSTALL_OBJS_ETC) $(ETCDIR)

# Old (non-GNU) versions of `install' don't support option -d. Use mkdir instead. 
# Was seen on HP-UX 11.

install-man:
	# install-man
	#$(INSTALL_BIN) -d $(MANDIR1)
	mkdir -p $(MANDIR1)
	$(INSTALL_DATA) $(INSTALL_OBJS_MAN1) $(MANDIR1)

install-bin:
	# install-bin
	#$(INSTALL_BIN) -d $(BINDIR)
	mkdir -p $(BINDIR)
	$(INSTALL_BIN) -s $(INSTALL_OBJS_BIN) $(BINDIR)

install: all install-bin install-man

# End of file
