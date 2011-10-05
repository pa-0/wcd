!include ..\..\src\version.mk

CC      = wcc386
SRCDIR = ..\..\src
DEFINES = -dMSDOS
CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -w4 -e25 -zq -od -d2 -5r -bt=nt -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj langinfo.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj,langinfo.obj

TARGET = nt

all: wcdwin32.exe

wcdwin32.exe: $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
	@%append wcd.lnk library mpr.lib
	wlink name wcdwin32 d all SYS $(TARGET) op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk


stack.obj :  $(SRCDIR)\stack.c
	$(CC) $(CFLAGS) $(SRCDIR)\stack.c

match.obj :  $(SRCDIR)\match.c
	$(CC) $(CFLAGS) $(SRCDIR)\match.c

matchl.obj :  $(SRCDIR)\matchl.c
	$(CC) $(CFLAGS) $(SRCDIR)\matchl.c

querycp.obj :  $(SRCDIR)\querycp.c
	$(CC) $(CFLAGS) $(SRCDIR)\querycp.c

langinfo.obj :  $(SRCDIR)\langinfo.c
	$(CC) $(CFLAGS) $(SRCDIR)\langinfo.c

wfixpath.obj :  $(SRCDIR)\wfixpath.c
	$(CC) $(CFLAGS) $(SRCDIR)\wfixpath.c

dosdir.obj :  $(SRCDIR)\dosdir.c
	$(CC) $(CFLAGS) $(SRCDIR)\dosdir.c

display.obj :  $(SRCDIR)\display.c
	$(CC) $(CFLAGS) $(SRCDIR)\display.c

wcd.obj :  $(SRCDIR)\wcd.c
	$(CC) $(CFLAGS) $(CFLAGS_VERSION) $(SRCDIR)\wcd.c

wcddir.obj :  $(SRCDIR)\wcddir.c
	$(CC) $(CFLAGS) $(SRCDIR)\wcddir.c


WcdStack.obj :  $(SRCDIR)\c3po\WcdStack.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\WcdStack.c

Text.obj :  $(SRCDIR)\c3po\Text.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\Text.c

nameset.obj :  $(SRCDIR)\c3po\nameset.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\nameset.c

Error.obj :  $(SRCDIR)\c3po\Error.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\Error.c

intset.obj :  $(SRCDIR)\c3po\intset.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\intset.c

command.obj :  $(SRCDIR)\c3po\command.c
	$(CC) $(CFLAGS) $(SRCDIR)\c3po\command.c

clean
	-del *.obj
	-del *.map
	-del *.ilk
	-del *.sym
	-del *.exe
