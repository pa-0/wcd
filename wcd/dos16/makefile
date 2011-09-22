!include ..\src\version.mk

CC      = bcc +WCD.CFG
LIBPATH = C:\BC4\LIB;
INCLUDEPATH = ..\SRC;..\SRC\C3PO;C:\BC4\INCLUDE
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj matchl.obj querycp.obj

all: wcd.exe

wcd.exe: $(OBJS)
	#bcc -mc $(OBJS) noehc.lib
	bcc -mc @objlist.txt

# remove noehc.lib if you are using Borland C version prior
# to 4.0. noeh?.lib, where ? stands for the memory model,
# removes the C++ exception handling from the startup-code
# which is included by default since Borland 4.0 and
# higher. Including noeh?.lib reduces the executable size
# significant, provided your source is C only.


#.c.obj: wcd.cfg
#	$(CC) -c $<

stack.obj : wcd.cfg  ..\src\stack.c
	$(CC) -c -o$@ ..\src\stack.c

match.obj : wcd.cfg  ..\src\match.c
	$(CC) -c -o$@ ..\src\match.c

matchl.obj : wcd.cfg  ..\src\matchl.c
	$(CC) -c -o$@ ..\src\matchl.c

querycp.obj : wcd.cfg  ..\src\querycp.c
	$(CC) -c -o$@ ..\src\querycp.c

wfixpath.obj : wcd.cfg  ..\src\wfixpath.c
	$(CC) -c -o$@ ..\src\wfixpath.c

dosdir.obj : wcd.cfg  ..\src\dosdir.c
	$(CC) -c -o$@ ..\src\dosdir.c

display.obj : wcd.cfg  ..\src\display.c
	$(CC) -c -o$@ ..\src\display.c

wcd.obj : wcd.cfg  ..\src\wcd.c
	$(CC) -c -o$@ ..\src\wcd.c

wcddir.obj : wcd.cfg  ..\src\wcddir.c
	$(CC) -c -o$@ ..\src\wcddir.c


WcdStack.obj : wcd.cfg  ..\src\c3po\WcdStack.c
	$(CC) -c -o$@ ..\src\c3po\WcdStack.c

Text.obj : wcd.cfg  ..\src\c3po\Text.c
	$(CC) -c -o$@ ..\src\c3po\Text.c

nameset.obj : wcd.cfg  ..\src\c3po\nameset.c
	$(CC) -c -o$@ ..\src\c3po\nameset.c

Error.obj : wcd.cfg  ..\src\c3po\Error.c
	$(CC) -c -o$@ ..\src\c3po\Error.c

intset.obj : wcd.cfg  ..\src\c3po\intset.c
	$(CC) -c -o$@ ..\src\c3po\intset.c

command.obj : wcd.cfg  ..\src\c3po\command.c
	$(CC) -c -o$@ ..\src\c3po\command.c

strip :
	tdstrip wcd.exe

clean:
	del *.obj
	del *.cfg
	del wcd.exe

#		*Compiler Configuration File*
wcd.cfg: makefile
  copy &&|
-Z
-O
-w
-mc
-1
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DDOSWILD;WCD_USECONIO
-DVERSION="$(VERSION)"
-DVERSION_DATE="$(VERSION_DATE)"
| wcd.cfg
