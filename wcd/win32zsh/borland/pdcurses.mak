!include ..\..\src\version.mk

#CC_HOME = c:\Borland\Bcc55
CC_HOME = c:\bc4


CC      = BCC32 +PDCURSES.CFG
LIBPATH = $(CC_HOME)\lib;
INCLUDEPATH = ..\..\src;..\..\src\c3po;$(CC_HOME)\include

all: wcdwin32zsh.exe

OBJS = wcd.obj \
	wfixpath.obj\
	WcdStack.obj\
	Text.obj\
	stack.obj\
	nameset.obj\
	intset.obj\
	match.obj\
	error.obj\
	dosdir.obj\
	display.obj\
	dirnode.obj\
	colors.obj\
	graphics.obj\
	wcddir.obj

LIBS=pdcurses.lib

wcdwin32zsh.exe : objlist.txt $(OBJS)
	$(CC) -L$(CC_HOME)\lib -e$@ @objlist.txt $(LIBS)



WcdStack.obj : pdcurses.cfg  ..\..\src\c3po\WcdStack.c
	$(CC) -c -o$@ ..\..\src\c3po\WcdStack.c

Text.obj : pdcurses.cfg  ..\..\src\c3po\Text.c
	$(CC) -c -o$@ ..\..\src\c3po\Text.c

nameset.obj : pdcurses.cfg  ..\..\src\c3po\nameset.c
	$(CC) -c -o$@ ..\..\src\c3po\nameset.c

dirnode.obj : pdcurses.cfg  ..\..\src\c3po\dirnode.c
	$(CC) -c -o$@ ..\..\src\c3po\dirnode.c

Error.obj : pdcurses.cfg  ..\..\src\c3po\Error.c
	$(CC) -c -o$@ ..\..\src\c3po\Error.c

intset.obj : pdcurses.cfg  ..\..\src\c3po\intset.c
	$(CC) -c -o$@ ..\..\src\c3po\intset.c

stack.obj : pdcurses.cfg  ..\..\src\stack.c
	$(CC) -c -o$@ ..\..\src\stack.c

match.obj : pdcurses.cfg  ..\..\src\match.c
	$(CC) -c -o$@ ..\..\src\match.c

wfixpath.obj : pdcurses.cfg  ..\..\src\wfixpath.c
	$(CC) -c -o$@ ..\..\src\wfixpath.c

dosdir.obj : pdcurses.cfg  ..\..\src\dosdir.c
	$(CC) -c -o$@ ..\..\src\dosdir.c

display.obj : pdcurses.cfg  ..\..\src\display.c
	$(CC) -c -o$@ ..\..\src\display.c

colors.obj : pdcurses.cfg  ..\..\src\colors.c
	$(CC) -c -o$@ ..\..\src\colors.c

graphics.obj : pdcurses.cfg  ..\..\src\graphics.c
	$(CC) -c -o$@ ..\..\src\graphics.c

wcd.obj : pdcurses.cfg  ..\..\src\wcd.c
	$(CC) -c -o$@ ..\..\src\wcd.c

wcddir.obj : pdcurses.cfg  ..\..\src\wcddir.c
	$(CC) -c -o$@ ..\..\src\wcddir.c

clean:
	del *.obj
	del *.cfg
	del *.exe
	del objlist.txt

objlist.txt : 
  copy &&|
$(OBJS)
| objlist.txt

#		*Compiler Configuration File*
pdcurses.cfg: makefile
  copy &&|
-w
-WC
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS;WCD_USECURSES;WCD_WINZSH
-DVERSION="$(VERSION)"
-DVERSION_DATE="$(VERSION_DATE)"
| pdcurses.cfg
