SRCDIR = ..\..\src

!include $(SRCDIR)\version.mk

CC      = wcc386
PROGRAM = wcdbash.exe
DEFINES = -dWCD_DOSBASH
CURSES = pdcurses

!ifneq CURSES
DEFINES += -dWCD_USECURSES
!endif

CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -w4 -e25 -zq -od -d2 -5r -bt=dos -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj


!ifneq CURSES
OBJS  += dirnode.obj colors.obj graphics.obj
LOBJS +=,dirnode.obj,colors.obj,graphics.obj
!endif
docsubdir = wcd-$(VERSION_SHORT)
VERSIONSUFFIX = bh
ZIPFILE = wcd$(VERSION_SHORT)$(VERSIONSUFFIX).zip
HTMLEXT     = htm
DOCFILES    = $(SRCDIR)\..\doc\wcd.txt $(SRCDIR)\..\doc\wcd.$(HTMLEXT)
ZIPOBJ_EXTRA = bin\cwstub.exe
#ZIPOBJ_EXTRA = bin\dos4gw.exe

prefix = c:\dos32

TARGET = causeway
#TARGET = dos4g

all: $(PROGRAM) $(DOCFILES) .SYMBOLIC

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
!ifneq CURSES
	@%append wcd.lnk library $(CURSES).lib
!endif
	wlink name wcdbash d all SYS $(TARGET) op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include $(SRCDIR)\watcom.mif

