CC_HOME = c:\Borland\Bcc55
#CC_HOME = c:\bc4


CC      = BCC32
DEFINES = -DMSDOS;WCD_USECURSES
CFLAGS  = $(DEFINES) -w -WC -I..\..\src -I..\..\src\c3po -I$(CC_HOME)\include
LFLAGS  = -L$(CC_HOME)\lib

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
	colors.obj\
	graphics.obj\
	wcddir.obj

LIBS=pdcurses.lib

wcdwin32.exe : objlist.txt $(OBJS)
	$(CC) -L$(CC_HOME)\lib -e$@ @objlist.txt $(LIBS)



WcdStack.obj :  ..\..\src\c3po\WcdStack.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\WcdStack.c

Text.obj :  ..\..\src\c3po\Text.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\Text.c

nameset.obj :  ..\..\src\c3po\nameset.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\nameset.c

dirnode.obj :  ..\..\src\c3po\dirnode.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\dirnode.c

Error.obj :  ..\..\src\c3po\Error.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\Error.c

intset.obj :  ..\..\src\c3po\intset.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\c3po\intset.c

stack.obj :  ..\..\src\stack.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\stack.c

match.obj :  ..\..\src\match.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\match.c

wfixpath.obj :  ..\..\src\wfixpath.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\wfixpath.c

dosdir.obj :  ..\..\src\dosdir.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\dosdir.c

display.obj :  ..\..\src\display.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\display.c

colors.obj :  ..\..\src\colors.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\colors.c

graphics.obj :  ..\..\src\graphics.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\graphics.c

wcd.obj :  ..\..\src\wcd.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\wcd.c

wcddir.obj :  ..\..\src\wcddir.c
	$(CC) -c $(CFLAGS) -o$@ ..\..\src\wcddir.c

objlist.txt : 
  copy &&|
$(OBJS)
| objlist.txt

