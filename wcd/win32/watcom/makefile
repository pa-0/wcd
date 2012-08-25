SRCDIR = ..\..\src

!include $(SRCDIR)\version.mk

CC      = wcc386
PROGRAM = wcdwin32.exe
DEFINES =
CURSES = pdcursesw

!ifneq CURSES
DEFINES += -dWCD_USECURSES -dPDC_WIDE
!endif

!ifeq UCS 1
DEFINES += -dWCD_UNICODE
!endif

CFLAGS  = $(DEFINES) -i=$(SRCDIR) -i=$(SRCDIR)\c3po -w4 -e25 -zq -od -d2 -5r -bt=nt -mf
CFLAGS_VERSION = -DVERSION="$(VERSION)" -DVERSION_DATE="$(VERSION_DATE)"

!ifeq UCS 1
CFLAGS += -za99
!endif

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj wcwidth.obj
LOBJS   = wcd.obj,match.obj,stack.obj,nameset.obj,error.obj,text.obj,WcdStack.obj,display.obj,dosdir.obj,wfixpath.obj,intset.obj,wcddir.obj,command.obj,matchl.obj,querycp.obj,wcwidth.obj


!ifneq CURSES
OBJS  += dirnode.obj colors.obj graphics.obj
LOBJS +=,dirnode.obj,colors.obj,graphics.obj
!endif
!ifeq UCS 1
OBJS  += matchw.obj
LOBJS +=,matchw.obj
VERSIONSUFFIX = -win32-ucs
!else
VERSIONSUFFIX = -win32
!endif
HTMLEXT     = htm
DOCFILES    = $(SRCDIR)\..\doc\wcd.txt $(SRCDIR)\..\doc\wcd.$(HTMLEXT)

prefix = c:\usr\local

TARGET = nt

all: $(PROGRAM) $(DOCFILES) .SYMBOLIC

$(PROGRAM): $(OBJS)
	@%create wcd.lnk
	@%append wcd.lnk FIL $(LOBJS)
	@%append wcd.lnk library mpr.lib
!ifneq CURSES
	@%append wcd.lnk library $(CURSES).lib
!endif
	wlink name wcdwin32 d all SYS $(TARGET) op inc op m op st=64k op maxe=25 op q op symf @wcd.lnk
	del wcd.lnk

!include $(SRCDIR)\watcom.mif

