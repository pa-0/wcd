CC	= gcc
CFLAGS	= -DUNIX -DWCD_USECURSES -I../src -I../src/c3po -O3 -Wall
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

all:wcd.exe

wcd.exe: $(OBJS1)
	$(CC) $(OBJS1) -lcurses -lmpr -Wall -o $@


%.o : ../src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : ../src/c3po/%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm *.o
	rm wcd.exe
