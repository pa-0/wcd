SRCDIR = ..\..\src

!include $(SRCDIR)\version.mk

CC   = cl.exe /nologo
LINK = link.exe /nologo
PROGRAM = wcdwin32.exe
DEFINES =
CURSES = pdcursesw

!if "$(CURSES)" != ""
DEFINES = $(DEFINES) /DWCD_USECURSES /DPDC_WIDE
!endif

CFLAGS  = $(DEFINES) /I$(SRCDIR) /I$(SRCDIR)\c3po
CFLAGS_VERSION = -DVERSION=\"$(VERSION)\" -DVERSION_DATE=\"$(VERSION_DATE)\"

OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj wcwidth.obj
LIBS = mpr.lib


!if "$(CURSES)" != ""
OBJS  = $(OBJS) dirnode.obj colors.obj graphics.obj
LIBS  = $(LIBS) user32.lib advapi32.lib $(CURSES).lib
!endif

!if "$(UCS)" == "1"
OBJS  = $(OBJS) matchw.obj
LIBS  = $(LIBS) shell32.lib
VERSIONSUFFIX = -win32-ucs
!else
VERSIONSUFFIX = -win32
!endif
HTMLEXT     = htm
DOCFILES    = $(SRCDIR)\..\doc\wcd.txt $(SRCDIR)\..\doc\wcd.$(HTMLEXT)
ZIPOBJ_EXTRA = bin\wcd.bat bin\wcd_win95.bat

prefix = c:\usr\local


all: $(PROGRAM) $(DOCFILES)

$(PROGRAM): $(OBJS)
	$(LINK) /out:$@ $(LDFLAGS) $(OBJS) $(LIBS)

!include $(SRCDIR)\vc.mak

