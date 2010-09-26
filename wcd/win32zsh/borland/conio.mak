!include ..\..\src\version.mk

#CC_HOME = c:\Borland\Bcc55
CC_HOME = c:\bc4


CC      = BCC32 +CONIO.CFG
LIBPATH = $(CC_HOME)\lib;
INCLUDEPATH = ..\..\src;..\..\src\c3po;$(CC_HOME)\include

all: wcdwin32zsh.exe

OBJS = wcd.obj \
	wfixpath.obj\
	wcdstack.obj\
	text.obj\
	stack.obj\
	nameset.obj\
	intset.obj\
	match.obj\
	error.obj\
	dosdir.obj\
	display.obj\
	dirnode.obj\
	wcddir.obj



wcdwin32zsh.exe : objlist.txt $(OBJS)
	$(CC) -L$(CC_HOME)\lib -e$@ @objlist.txt



WcdStack.obj : conio.cfg  ..\..\src\c3po\WcdStack.c
	$(CC) -c -o$@ ..\..\src\c3po\WcdStack.c

Text.obj : conio.cfg  ..\..\src\c3po\Text.c
	$(CC) -c -o$@ ..\..\src\c3po\Text.c

nameset.obj : conio.cfg  ..\..\src\c3po\nameset.c
	$(CC) -c -o$@ ..\..\src\c3po\nameset.c

Error.obj : conio.cfg  ..\..\src\c3po\Error.c
	$(CC) -c -o$@ ..\..\src\c3po\Error.c

intset.obj : conio.cfg  ..\..\src\c3po\intset.c
	$(CC) -c -o$@ ..\..\src\c3po\intset.c

dirnode.obj : conio.cfg  ..\..\src\c3po\dirnode.c
	$(CC) -c -o$@ ..\..\src\c3po\dirnode.c

stack.obj : conio.cfg  ..\..\src\stack.c
	$(CC) -c -o$@ ..\..\src\stack.c

match.obj : conio.cfg  ..\..\src\match.c
	$(CC) -c -o$@ ..\..\src\match.c

wfixpath.obj : conio.cfg  ..\..\src\wfixpath.c
	$(CC) -c -o$@ ..\..\src\wfixpath.c

dosdir.obj : conio.cfg  ..\..\src\dosdir.c
	$(CC) -c -o$@ ..\..\src\dosdir.c

display.obj : conio.cfg  ..\..\src\display.c
	$(CC) -c -o$@ ..\..\src\display.c

wcd.obj : conio.cfg  ..\..\src\wcd.c
	$(CC) -c -o$@ ..\..\src\wcd.c

wcddir.obj : conio.cfg  ..\..\src\wcddir.c
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
conio.cfg: makefile
  copy &&|
-w
-WC
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS;WCD_USECONIO;WCD_WINZSH
-DVERSION="$(VERSION)"
-DVERSION_DATE="$(VERSION_DATE)"
| conio.cfg
