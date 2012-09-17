SRCDIR = ..\..\src

!include $(SRCDIR)\version.mk

CC   = cl.exe /nologo
LINK = link.exe /nologo
PROGRAM = wcdwin32.exe
DEFINES = /DWCD_USECURSES /DWCD_UNICODE
CURSES = pdcursesw

#!ifneq CURSES
#DEFINES += -dWCD_USECURSES -dPDC_WIDE
#!endif

CFLAGS  = $(DEFINES) /I$(SRCDIR) /I$(SRCDIR)\c3po
CFLAGS_VERSION = -DVERSION=\"$(VERSION)\" -DVERSION_DATE=\"$(VERSION_DATE)\"

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj wcwidth.obj


#!ifneq CURSES
CURSES_OBJS  = dirnode.obj colors.obj graphics.obj
CURSES_LIBS  = user32.lib advapi32.lib $(CURSES).lib
#!endif
#!ifeq UCS 1
UCS_OBJS  = matchw.obj
UCS_LIBS  = shell32.lib
#VERSIONSUFFIX = -win32-ucs
#!else
#VERSIONSUFFIX = -win32
#!endif
#HTMLEXT     = htm
#DOCFILES    = $(SRCDIR)\..\doc\wcd.txt $(SRCDIR)\..\doc\wcd.$(HTMLEXT)
#ZIPOBJ_EXTRA = bin\wcd.bat bin\wcd_win95.bat

prefix = c:\usr\local


#all: $(PROGRAM) $(DOCFILES) .SYMBOLIC
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(CURSES_OBJS) $(UCS_OBJS)
	$(LINK) /out:$@ $(LDFLAGS) $(OBJS) $(CURSES_OBJS) $(UCS_OBJS) mpr.lib $(CURSES_LIBS) $(UCS_LIBS)

!include $(SRCDIR)\vc.mak

