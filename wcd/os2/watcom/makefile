!include ..\..\src\version.mk

CC      = wcc386
SRCDIR = ..\..\src
DEFINES =
CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -i$(%WATCOM)\h\os2 -w4 -e25 -zq -od -d2 -6r -bt=os2 -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj

TARGET = os2

all: wcdos2.exe

wcdos2.exe: $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
	wlink name wcdos2 d all SYS os2v2 op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk


stack.obj :  $(SRCDIR)\stack.c
	$(CC) $(CFLAGS) $(SRCDIR)\stack.c

match.obj :  $(SRCDIR)\match.c
	$(CC) $(CFLAGS) $(SRCDIR)\match.c

matchl.obj :  $(SRCDIR)\matchl.c
	$(CC) $(CFLAGS) $(SRCDIR)\matchl.c

querycp.obj :  $(SRCDIR)\querycp.c
	$(CC) $(CFLAGS) $(SRCDIR)\querycp.c

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
	-del *.ilk
	-del *.map
	-del *.sym
	-del *.exe
