!include ..\..\src\version.mk

CC      = wcc386
SRCDIR = ..\..\src
PROGRAM = wcdos2.exe
DEFINES = -dWCD_USECURSES
CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -i$(%WATCOM)\h\os2 -w4 -e25 -zq -od -d2 -6r -bt=os2 -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj dirnode.obj colors.obj graphics.obj matchl.obj querycp.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,dirnode.obj,colors.obj,graphics.obj,matchl.obj,querycp.obj

TARGET = os2

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
	@%append wcd.lnk library pdcurses.lib
	wlink name wcdos2 d all SYS os2v2 op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include ..\..\src\watcom.mif

