# GNU Makefile for project wcd
# Use gnuwin32-make or mingw32-make.

include ../../src/version.mk

CC_HOME=c:/lcc
CC_HOME_I=c:\\lcc
SRCDIR=..\\..\\src
CC=$(CC_HOME)/bin/lcc.exe
LINKER=$(CC_HOME)/bin/lcclnk.exe

ifdef CURSES
	DEFS_CURSES = -DWCD_USECURSES
	LIB_CURSES  = pdcurses.lib
endif

ifdef CONIO
	DEFS_CONIO = -DWCD_USECONIO
	LIB_CONIO  = tcconio.lib
endif


CFLAGS=-I$(CC_HOME_I)\include -I"$(SRCDIR)" -I"$(SRCDIR)\c3po" -DMSDOS $(DEFS_CURSES) $(DEFS_CONIO) -g2
CFLAGS_VERSION	= -DVERSION=\"$(VERSION)\" -DVERSION_DATE=\"$(VERSION_DATE)\"

OBJS=\
	wfixpath.obj \
	wcd.obj \
	stack.obj \
	match.obj \
	matchl.obj \
	colors.obj \
	graphics.obj \
	dosdir.obj \
	display.obj \
	wcdstack.obj \
	text.obj \
	nameset.obj \
	intset.obj \
	error.obj \
	dirnode.obj \
	wcddir.obj \
	querycp.obj

LIBS=mpr.lib $(LIB_CURSES) $(LIB_CONIO)

wcdwin32.exe:	$(OBJS) GNUmakefile
	$(LINKER) -s -subsystem console -o wcdwin32.exe $(OBJS) $(LIBS)

# Build WFIXPATH.C
WFIXPATH_C=\
	$(SRCDIR)\\tailor.h\

wfixpath.obj: $(WFIXPATH_C) $(SRCDIR)\\wfixpath.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\wfixpath.c

# Build WCDDIR.C
WCDDIR_C=\
	$(SRCDIR)\\wcddir.h\

wcddir.obj: $(WCDDIR_C) $(SRCDIR)\\wcddir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\wcddir.c

# Build WCD.C
WCD_C=\
	$(SRCDIR)\\dosdir.h\
	$(SRCDIR)\\wcddir.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\match.h\
	$(SRCDIR)\\matchl.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\c3po\\nameset.h\
	$(SRCDIR)\\c3po\\WcdStack.h\
	$(SRCDIR)\\c3po\\dirnode.h\
	$(SRCDIR)\\wcd.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\stack.h\
	$(SRCDIR)\\display.h\
	$(SRCDIR)\\wfixpath.h\
	$(SRCDIR)\\colors.h\
	$(SRCDIR)\\graphics.h\
	$(SRCDIR)\\querycp.h\

wcd.obj: $(WCD_C) $(SRCDIR)\\wcd.c
	$(CC) -c $(CFLAGS) $(CFLAGS_VERSION) $(SRCDIR)\\wcd.c

# Build STACK.C
STACK_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\nameset.h\
	$(SRCDIR)\\display.h\
	$(SRCDIR)\\dosdir.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\c3po\\WcdStack.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\wcd.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\stack.h\

stack.obj: $(STACK_C) $(SRCDIR)\\stack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\stack.c

# Build MATCH.C
MATCH_C=\
	$(SRCDIR)\\match.h\
	$(SRCDIR)\\tailor.h\

match.obj: $(MATCH_C) $(SRCDIR)\\match.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\match.c

# Build MATCHL.C
MATCHL_C=\
	$(SRCDIR)\\matchl.h\
	$(SRCDIR)\\tailor.h\

matchl.obj: $(MATCHL_C) $(SRCDIR)\\matchl.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\matchl.c

# Build QUERYCP.C
QUERYCP_C=\
	$(SRCDIR)\\querycp.h\
	$(SRCDIR)\\tailor.h\

querycp.obj: $(QUERYCP_C) $(SRCDIR)\\querycp.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\querycp.c

# Build COLORS.C
COLORS_C=\
	$(SRCDIR)\\colors.h\

colors.obj: $(COLORS_C) $(SRCDIR)\\colors.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\colors.c

# Build GRAPHICS.C
GRAPHICS_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\nameset.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\dirnode.h\
	$(SRCDIR)\\wcd.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\dosdir.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\display.h\
	$(SRCDIR)\\wfixpath.h\
	$(SRCDIR)\\match.h\
	$(SRCDIR)\\tailor.h\

graphics.obj: $(GRAPHICS_C) $(SRCDIR)\\graphics.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\graphics.c

# Build DOSDIR.C
DOSDIR_C=\
	$(SRCDIR)\\dosdir.h\
	$(SRCDIR)\\tailor.h\
	$(SRCDIR)\\match.h\
	$(SRCDIR)\\tailor.h\

dosdir.obj: $(DOSDIR_C) $(SRCDIR)\\dosdir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\dosdir.c

# Build DISPLAY.C
DISPLAY_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\nameset.h\
	$(SRCDIR)\\display.h\
	$(SRCDIR)\\wcd.h\
	$(SRCDIR)\\tailor.h\

display.obj: $(DISPLAY_C) $(SRCDIR)\\display.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\display.c

# Build WCDSTACK.C
WCDSTACK_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\c3po\\WcdStack.h\

wcdstack.obj: $(WCDSTACK_C) $(SRCDIR)\\c3po\\wcdstack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\wcdstack.c

# Build TEXT.C
TEXT_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\nameset.h\
	$(SRCDIR)\\c3po\\intset.h\

text.obj: $(TEXT_C) $(SRCDIR)\\c3po\\text.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\text.c

# Build NAMESET.C
NAMESET_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\c3po\\nameset.h\

nameset.obj: $(NAMESET_C) $(SRCDIR)\\c3po\\nameset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\nameset.c

# Build INTSET.C
INTSET_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\intset.h\

intset.obj: $(INTSET_C) $(SRCDIR)\\c3po\\intset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\intset.c

# Build ERROR.C
ERROR_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\

error.obj: $(ERROR_C) $(SRCDIR)\\c3po\\error.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\error.c

# Build DIRNODE.C
DIRNODE_C=\
	$(SRCDIR)\\c3po\\std_macr.h\
	$(SRCDIR)\\c3po\\structur.h\
	$(SRCDIR)\\c3po\\Error.h\
	$(SRCDIR)\\c3po\\Text.h\
	$(SRCDIR)\\c3po\\dirnode.h\

dirnode.obj: $(DIRNODE_C) $(SRCDIR)\\c3po\\dirnode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\\c3po\\dirnode.c

link:
	$(LINKER)  -subsystem console -o $(SRCDIR)\\lcc/wcdwin32.exe $(OBJS) $(LIBS)

clean:
	-rm *.obj
	-rm *.exe
