CC	= gcc
CFLAGS	= -DWCD_USECURSES -I../src -I../src/c3po -O3 -Wall
OBJS1	= \
	nameset.o \
	intset.o \
	Error.o \
	Text.o \
	WcdStack.o \
	dirnode.o \
	wcd.o \
	match.o \
	stack.o \
	display.o \
	wfixpath.o \
	colors.o \
	graphics.o \
	wcddir.o

all:wcd.exe

wcd.exe: $(OBJS1)
	$(CC) $(OBJS1) -lpdcurses -Wall -o $@


%.o : ../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../src/c3po/%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	del *.o
	del ..\c3po\*.o
	del ..\dos16\*.o
	del wcd.exe
