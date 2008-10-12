CC	= gcc
CFLAGS	= -DMSDOS -I../../src -I../../src/c3po -O3 -Wall
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
	wcddir.o

all:wcdwin32.exe

wcdwin32.exe: $(OBJS1)
	$(CC) $(OBJS1) -lmpr -Wall -o $@



%.o : ../../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../../src/c3po/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	del *.o
	del wcd.exe
