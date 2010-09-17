!include ..\..\src\version.mk

#CC_HOME = c:\Borland\Bcc55
CC_HOME = c:\bc4
#CC_HOME = c:\bc5


CC      = BCC32 +WCD.CFG
LIBPATH = $(CC_HOME)\lib;
INCLUDEPATH = ..\..\src;..\..\src\c3po;$(CC_HOME)\include

all: wcdwin32.exe

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
	wcddir.obj



wcdwin32.exe : objlist.txt $(OBJS)
	$(CC) -L$(CC_HOME)\lib -e$@ @objlist.txt



WcdStack.obj : wcd.cfg  ..\..\src\c3po\WcdStack.c
	$(CC) -c -o$@ ..\..\src\c3po\WcdStack.c

Text.obj : wcd.cfg  ..\..\src\c3po\Text.c
	$(CC) -c -o$@ ..\..\src\c3po\Text.c

nameset.obj : wcd.cfg  ..\..\src\c3po\nameset.c
	$(CC) -c -o$@ ..\..\src\c3po\nameset.c

dirnode.obj : wcd.cfg  ..\..\src\c3po\dirnode.c
	$(CC) -c -o$@ ..\..\src\c3po\dirnode.c

Error.obj : wcd.cfg  ..\..\src\c3po\Error.c
	$(CC) -c -o$@ ..\..\src\c3po\Error.c

intset.obj : wcd.cfg  ..\..\src\c3po\intset.c
	$(CC) -c -o$@ ..\..\src\c3po\intset.c

stack.obj : wcd.cfg  ..\..\src\stack.c
	$(CC) -c -o$@ ..\..\src\stack.c

match.obj : wcd.cfg  ..\..\src\match.c
	$(CC) -c -o$@ ..\..\src\match.c

wfixpath.obj : wcd.cfg  ..\..\src\wfixpath.c
	$(CC) -c -o$@ ..\..\src\wfixpath.c

dosdir.obj : wcd.cfg  ..\..\src\dosdir.c
	$(CC) -c -o$@ ..\..\src\dosdir.c

display.obj : wcd.cfg  ..\..\src\display.c
	$(CC) -c -o$@ ..\..\src\display.c

wcd.obj : wcd.cfg  ..\..\src\wcd.c
	$(CC) -c -o$@ ..\..\src\wcd.c

wcddir.obj : wcd.cfg  ..\..\src\wcddir.c
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
wcd.cfg: makefile
  copy &&|
-w
-WC
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS;WCD_WINZSH
-DVERSION="$(VERSION)"
-DVERSION_DATE="$(VERSION_DATE)"
| wcd.cfg
