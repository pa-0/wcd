SRCDIR = ..\src

!include $(SRCDIR)\version.mk

CC      = wcc
PROGRAM = wcd.exe
DEFINES =
CURSES = pdcurses

!ifneq CURSES
DEFINES += -dWCD_USECURSES
!endif

CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -w4 -e25 -zq -od -d2 -bt=dos -ml
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj


!ifneq CURSES
OBJS  += dirnode.obj colors.obj graphics.obj
LOBJS +=,dirnode.obj,colors.obj,graphics.obj
!endif
docsubdir = wcd
VERSIONSUFFIX = b
ZIPFILE = $(PACKAGE)$(VERSION_SHORT)$(VERSIONSUFFIX).zip
ZIPFILEDIR = ..\..
HTMLEXT     = htm
DOCFILES    = $(SRCDIR)\man\man1\wcd.txt $(SRCDIR)\man\man1\wcd.$(HTMLEXT)

prefix = c:\dos

TARGET = dos

all: $(PROGRAM) $(DOCFILES) .SYMBOLIC

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
!ifneq CURSES
	@%append wcd.lnk library $(CURSES).lib
!endif
	wlink name wcd d all SYS $(TARGET) op m op st=32k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include $(SRCDIR)\watcom.mif

