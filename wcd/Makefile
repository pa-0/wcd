#
#	Copyright (C) 2009-2011 Erwin Waterlander
#	Copyright (C) 2009 Jari Aalto
#
#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License as
#	published by the Free Software Foundation; either version 2 of the
#	License, or (at your option) any later version
#
#	This program is distributed in the hope that it will be useful, but
#	WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#	General Public License for more details.
#
#	Visit <http://www.gnu.org/copyleft/gpl.html>


# This makefile is intended only for Unix/Linux.
# See also documentation in directory `doc'.

ifneq (,)
This makefile requires GNU Make.
endif

OBJDIR	= src

all:
	$(MAKE) -C $(OBJDIR)

clean:
	$(MAKE) -C $(OBJDIR) clean

distclean:
	$(MAKE) -C $(OBJDIR) distclean

maintainer-clean:
	$(MAKE) -C $(OBJDIR) maintainer-clean

realclean: maintainer-clean

install:
	$(MAKE) -C $(OBJDIR) install

uninstall:
	$(MAKE) -C $(OBJDIR) uninstall

.PHONY: clean distclean realclean install

# End of file
