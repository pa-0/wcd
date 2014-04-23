# pod2man.mk -- Makefile portion to convert *.pod files to manual pages
#
#   Copyright information
#
#	Copyright (C) 2008-2009 Jari Aalto
#	Copyright (C) 2010-2014 Erwin Waterlander
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
# 	Convert *.pod files to manual pages.

ifneq (,)
    This makefile requires GNU Make.
endif

.PRECIOUS: %.pod

# This variable *must* be set when calling
PACKAGE		?= wcd

# Optional variables to set
MANSECT		?= 1
PODCENTER	?= $$(date "+%Y-%m-%d")

# Directories
MANSRC		=
MANDEST		= $(MANSRC)

MANPOD		= $(MANSRC)$(PACKAGE).pod
MANPAGE		= $(MANDEST)$(PACKAGE).$(MANSECT)

POD2MAN		= pod2man
POD2MAN_FLAGS	=

POFILES = $(wildcard ../*/man1/wcd.po)
PODFILES = $(patsubst %.po,%.pod,$(POFILES))
MAN_OBJECTS = wcd.1 $(patsubst %.pod,%.1,$(PODFILES))

all: $(MAN_OBJECTS) wcd-man.pot


wcd-man.pot : wcd.pod
	po4a-updatepo -f pod -m $< -p $@

%.po : wcd.pod
	po4a-updatepo -f pod -m $< -p $@ --msgmerge-opt --backup=numbered

# Create pod files from po.
# Fix problem that =encoding is before =pod command.
%.pod : %.po
	po4a-translate -f pod -m wcd.pod -p $< > $@ -k 30
	perl -ni.bak -e 'print unless /=encoding UTF-8/' $@
	perl -pli.bak -e 's/=pod/=pod\n\n=encoding UTF-8/' $@

# An empty recipe for wcd.pod to break circular dependency.
wcd.pod : ;

%.1 : %.pod
	# make target - create manual page from a *.pod page
	podchecker $<
	$(POD2MAN) $(POD2MAN_FLAGS) \
                --utf8 \
		--center="$(PODCENTER)" \
		--name="$(PACKAGE)" \
		--section="$(MANSECT)" \
		$< \
	| perl -p -e 's/[Pp]erl v[0-9.]+/$(PACKAGE)/;' \
	  > $@ && \
	rm -f pod*.tmp



clean:
	rm -f $(MAN_OBJECTS)
	rm -f wcd-man.pot
	rm -f $(PODFILES)

# End of of Makefile part
