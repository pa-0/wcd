!include ..\..\src\version.mk

CC      = wcc386
SRCDIR = ..\..\src
PROGRAM = wcdwin32.exe
DEFINES = -dWCD_USECURSES -dWCD_UNICODE -dPDC_WIDE
CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -w4 -e25 -zq -od -d2 -5r -bt=nt -mf -za99
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj dirnode.obj colors.obj graphics.obj matchw.obj querycp.obj wcwidth.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,dirnode.obj,colors.obj,graphics.obj,matchw.obj,querycp.obj,wcwidth.obj

TARGET = nt

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
	@%append wcd.lnk library mpr.lib
	@%append wcd.lnk library pdcursesw.lib
	wlink name wcdwin32 d all SYS $(TARGET) op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include ..\..\src\watcom.mif

