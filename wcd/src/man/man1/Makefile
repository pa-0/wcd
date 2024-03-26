# pod2man.mk -- Makefile portion to convert *.pod files to manual pages
#
#   Copyright information
#
#       Copyright (C) 2008-2009 Jari Aalto
#       Copyright (C) 2010-2024 Erwin Waterlander
#
#   License
#
#       This program is free software; you can redistribute it and/or
#       modify it under the terms of the GNU General Public License as
#       published by the Free Software Foundation; either version 2 of the
#       License, or (at your option) any later version
#
#       This program is distributed in the hope that it will be useful, but
#       WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#       General Public License for more details at
#       Visit <http://www.gnu.org/copyleft/gpl.html>.
#
#   Description
#
#       Convert *.pod files to manual pages.

ifneq (,)
    This makefile requires GNU Make.
endif

include ../../version.mk

.PRECIOUS: %.pod

.PHONY: pods

# This variable *must* be set when calling
PACKAGE         ?= wcd

# Optional variables to set
MANSECT         ?= 1
PODCENTER       ?= $(VERSION_DATE)

POD2MAN         = pod2man
POD2MAN_FLAGS   =

POFILES = $(wildcard ../../po-man/*.po)
PODFILES = $(patsubst ../../po-man/%.po,../%/man1/wcd.pod,$(POFILES))
MAN_OBJECTS = wcd.1 $(patsubst %.pod,%.1,$(PODFILES))

MKDIR = mkdir

all: pods $(MAN_OBJECTS)


# Create pod files from po.
pods:
	touch ../../po-man/wcd-man.pot
	po4a --wrap-po no -k 70 --msgmerge-opt --no-wrap po4a.cfg
	# Fix problem that =encoding is before =pod command.
	#perl -ni.bak -e 'print unless /=encoding UTF-8/' $@
	#perl -pli.bak -e 's/=pod/=pod\n\n=encoding UTF-8/' $@

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
	rm -f ../../po-man/wcd-man.pot
	rm -f $(PODFILES)
