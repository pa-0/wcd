CC      = bcc
DEFINES = -DDOSWILD;WCD_USECONIO
CFLAGS  = $(DEFINES) -I../src -I../src/c3po -Z -O -w -mc -1
OBJS    = wcd.obj match.obj stack.obj nameset.obj error.obj text.obj WcdStack.obj display.obj dosdir.obj wfixpath.obj intset.obj wcddir.obj command.obj

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


#.c.obj:
#	$(CC) -c $(CFLAGS) $<

stack.obj :  ..\src\stack.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\stack.c

match.obj :  ..\src\match.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\match.c

wfixpath.obj :  ..\src\wfixpath.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\wfixpath.c

dosdir.obj :  ..\src\dosdir.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\dosdir.c

display.obj :  ..\src\display.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\display.c

wcd.obj :  ..\src\wcd.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\wcd.c

wcddir.obj :  ..\src\wcddir.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\wcddir.c


WcdStack.obj :  ..\src\c3po\WcdStack.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\WcdStack.c

Text.obj :  ..\src\c3po\Text.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\Text.c

nameset.obj :  ..\src\c3po\nameset.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\nameset.c

Error.obj :  ..\src\c3po\Error.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\Error.c

intset.obj :  ..\src\c3po\intset.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\intset.c

command.obj :  ..\src\c3po\command.c
	$(CC) -c $(CFLAGS) -o$@ ..\src\c3po\command.c

clean:
	del *.obj
