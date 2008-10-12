# makefile for MINGW32
CC_HOME=c:\mingw

CC	= gcc
CFLAGS	= -DMSDOS -DWCD_USECURSES -DPDC_STATIC_BUILD -I../../src -I../../src/c3po -O3 -Wall
OBJS1	= \
	nameset.o \
	intset.o \
	Error.o \
	Text.o \
	WcdStack.o \
	dirnode.o \
	wcd.o \
	dosdir.o \
	match.o \
	stack.o \
	display.o \
	wfixpath.o \
	colors.o \
	graphics.o \
	wcddir.o

all:wcdwin32.exe

wcdwin32.exe: $(OBJS1)
	gcc -L$(CC_HOME)\lib $(OBJS1) -lpdcurses -lkernel32 -luser32 -lmpr -Wall -o $@



%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../../src/c3po/%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	del *.o
	del wcd.exe
