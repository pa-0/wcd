SRCDIR = ..\..\src

!include $(SRCDIR)\version.mk

CC      = wcc386
PROGRAM = wcdos2.exe
DEFINES =
CURSES = pdcurses

!ifneq CURSES
DEFINES += -dWCD_USECURSES
!endif

CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -i$(%WATCOM)\h\os2 -w4 -e25 -zq -od -d2 -6r -bt=os2 -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj


!ifneq CURSES
OBJS  += dirnode.obj colors.obj graphics.obj
LOBJS +=,dirnode.obj,colors.obj,graphics.obj
!endif
VERSIONSUFFIX = -os2
HTMLEXT     = htm
DOCFILES    = $(SRCDIR)\man\man1\wcd.txt $(SRCDIR)\man\man1\wcd.$(HTMLEXT)
ZIPOBJ_EXTRA=bin\wcd.cmd

prefix = c:\usr

TARGET = os2

all: $(PROGRAM) $(DOCFILES) .SYMBOLIC

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
!ifneq CURSES
	@%append wcd.lnk library $(CURSES).lib
!endif
	wlink name wcdos2 d all SYS os2v2 op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include $(SRCDIR)\watcom.mif

