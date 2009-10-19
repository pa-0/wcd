# Makefile for Borland C++
.AUTODEPEND

#		*Translator Definitions*
CC = bcc +WCD.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BC4\LIB;
INCLUDEPATH = ..\SRC;..\SRC\C3PO;C:\BC4\INCLUDE


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
 wcddir.obj

#		*Explicit Rules*
wcd.exe: wcd.cfg $(EXE_dependencies)
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
wcddir.obj
wcd
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
display.obj: wcd.cfg ..\src\display.c 
	$(CC) -c ..\src\display.c

dosdir.obj: wcd.cfg ..\src\dosdir.c 
	$(CC) -c ..\src\dosdir.c

match.obj: wcd.cfg ..\src\match.c 
	$(CC) -c ..\src\match.c

stack.obj: wcd.cfg ..\src\stack.c 
	$(CC) -c ..\src\stack.c

wcd.obj: wcd.cfg ..\src\wcd.c 
	$(CC) -c ..\src\wcd.c

wfixpath.obj: wcd.cfg ..\src\wfixpath.c 
	$(CC) -c ..\src\wfixpath.c

wcddir.obj: wcd.cfg ..\src\wcddir.c 
	$(CC) -c ..\src\wcddir.c

command.obj: wcd.cfg ..\src\c3po\command.c 
	$(CC) -c ..\src\c3po\command.c

error.obj: wcd.cfg ..\src\c3po\error.c 
	$(CC) -c ..\src\c3po\error.c

intset.obj: wcd.cfg ..\src\c3po\intset.c 
	$(CC) -c ..\src\c3po\intset.c

nameset.obj: wcd.cfg ..\src\c3po\nameset.c 
	$(CC) -c ..\src\c3po\nameset.c

text.obj: wcd.cfg ..\src\c3po\text.c 
	$(CC) -c ..\src\c3po\text.c

wcdstack.obj: wcd.cfg ..\src\c3po\wcdstack.c 
	$(CC) -c ..\src\c3po\wcdstack.c

#		*Compiler Configuration File*
wcd.cfg: wcd.mak
  copy &&|
-ml
-v
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DDOSWILD;WCD_USECONIO
| wcd.cfg


