# Makefile for Borland C++
# PDCurses is installed in C:\CURSES
# pdcurses.lib has been copied to $(LIBPATH)

.AUTODEPEND

!include ..\src\version.mk

#		*Translator Definitions*
CC = bcc +PDCURSES.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BC4\LIB;
INCLUDEPATH = ..\SRC;..\SRC\C3PO;C:\BC4\INCLUDE;C:\CURSES


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 display.obj \
 dosdir.obj \
 match.obj \
 stack.obj \
 wcd.obj \
 wfixpath.obj \
 command.obj \
 error.obj \
 intset.obj \
 nameset.obj \
 text.obj \
 wcdstack.obj \
 dirnode.obj \
 colors.obj \
 graphics.obj \
 wcddir.obj

#		*Explicit Rules*
wcd.exe: pdcurses.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
display.obj+
dosdir.obj+
match.obj+
stack.obj+
wcd.obj+
wfixpath.obj+
command.obj+
error.obj+
intset.obj+
nameset.obj+
text.obj+
wcdstack.obj+
dirnode.obj+
colors.obj+
graphics.obj+
wcddir.obj
wcd
		# no map file
emu.lib+
mathl.lib+
cl.lib+
pdcurses.lib
|


#		*Individual File Dependencies*
display.obj: pdcurses.cfg ..\src\display.c 
	$(CC) -c ..\src\display.c

dosdir.obj: pdcurses.cfg ..\src\dosdir.c 
	$(CC) -c ..\src\dosdir.c

match.obj: pdcurses.cfg ..\src\match.c 
	$(CC) -c ..\src\match.c

stack.obj: pdcurses.cfg ..\src\stack.c 
	$(CC) -c ..\src\stack.c

colors.obj: pdcurses.cfg ..\src\colors.c 
	$(CC) -c ..\src\colors.c

graphics.obj: pdcurses.cfg ..\src\graphics.c 
	$(CC) -c ..\src\graphics.c

wcd.obj: pdcurses.cfg ..\src\wcd.c 
	$(CC) -c ..\src\wcd.c

wfixpath.obj: pdcurses.cfg ..\src\wfixpath.c 
	$(CC) -c ..\src\wfixpath.c

wcddir.obj: pdcurses.cfg ..\src\wcddir.c 
	$(CC) -c ..\src\wcddir.c

command.obj: pdcurses.cfg ..\src\c3po\command.c 
	$(CC) -c ..\src\c3po\command.c

error.obj: pdcurses.cfg ..\src\c3po\error.c 
	$(CC) -c ..\src\c3po\error.c

intset.obj: pdcurses.cfg ..\src\c3po\intset.c 
	$(CC) -c ..\src\c3po\intset.c

nameset.obj: pdcurses.cfg ..\src\c3po\nameset.c 
	$(CC) -c ..\src\c3po\nameset.c

text.obj: pdcurses.cfg ..\src\c3po\text.c 
	$(CC) -c ..\src\c3po\text.c

wcdstack.obj: pdcurses.cfg ..\src\c3po\wcdstack.c 
	$(CC) -c ..\src\c3po\wcdstack.c

dirnode.obj: pdcurses.cfg ..\src\c3po\dirnode.c 
	$(CC) -c ..\src\c3po\dirnode.c

clean:
	del *.obj
	del *.cfg
	del wcd.exe

#		*Compiler Configuration File*
pdcurses.cfg: pdcurses.mak
  copy &&|
-ml
-v
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DDOSWILD;WCD_USECURSES
-DVERSION="$(VERSION)"
-DVERSION_DATE="$(VERSION_DATE)"
| pdcurses.cfg


