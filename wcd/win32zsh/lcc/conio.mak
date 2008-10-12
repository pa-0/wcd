# Wedit Makefile for project wcd
CC_HOME=c:\lcc
SRCDIR=..\..\src
CFLAGS=-I$(CC_HOME)\include -I"$(SRCDIR)" -I"$(SRCDIR)\c3po" -DMSDOS -DWCD_USECONIO -DWCD_WINZSH -g2 
CC=$(CC_HOME)\bin\lcc.exe
LINKER=$(CC_HOME)\bin\lcclnk.exe
OBJS=\
	wfixpath.obj \
	wcd.obj \
	stack.obj \
	match.obj \
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
	wcddir.obj

LIBS=mpr.lib TCCONIO.LIB

wcdwin32.exe:	$(OBJS) Makefile
	$(LINKER) -s -subsystem console -o wcdwin32.exe $(OBJS) $(LIBS)

# Build WFIXPATH.C
WFIXPATH_C=\
	$(SRCDIR)\tailor.h\

wfixpath.obj: $(WFIXPATH_C) $(SRCDIR)\wfixpath.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wfixpath.c

# Build WCDDIR.C
WCDDIR_C=\
	$(SRCDIR)\wcddir.h\

wcddir.obj: $(WCDDIR_C) $(SRCDIR)\wcddir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wcddir.c

# Build WCD.C
WCD_C=\
	$(SRCDIR)\dosdir.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\match.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\c3po\nameset.h\
	$(SRCDIR)\c3po\WcdStack.h\
	$(SRCDIR)\c3po\dirnode.h\
	$(SRCDIR)\wcd.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\stack.h\
	$(SRCDIR)\display.h\
	$(SRCDIR)\wfixpath.h\
	$(SRCDIR)\graphics.h\

wcd.obj: $(WCD_C) $(SRCDIR)\wcd.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wcd.c

# Build STACK.C
STACK_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\nameset.h\
	$(SRCDIR)\display.h\
	$(SRCDIR)\dosdir.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\c3po\WcdStack.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\wcd.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\stack.h\

stack.obj: $(STACK_C) $(SRCDIR)\stack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\stack.c

# Build MATCH.C
MATCH_C=\
	$(SRCDIR)\match.h\
	$(SRCDIR)\tailor.h\

match.obj: $(MATCH_C) $(SRCDIR)\match.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\match.c

# Build COLORS.C
COLORS_C=\
	$(SRCDIR)\colors.h\

colors.obj: $(COLORS_C) $(SRCDIR)\colors.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\colors.c

# Build GRAPHICS.C
GRAPHICS_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\nameset.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\dirnode.h\
	$(SRCDIR)\wcd.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\dosdir.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\display.h\
	$(SRCDIR)\wfixpath.h\
	$(SRCDIR)\match.h\
	$(SRCDIR)\tailor.h\

graphics.obj: $(GRAPHICS_C) $(SRCDIR)\graphics.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\graphics.c

# Build DOSDIR.C
DOSDIR_C=\
	$(SRCDIR)\dosdir.h\
	$(SRCDIR)\tailor.h\
	$(SRCDIR)\match.h\
	$(SRCDIR)\tailor.h\

dosdir.obj: $(DOSDIR_C) $(SRCDIR)\dosdir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dosdir.c

# Build DISPLAY.C
DISPLAY_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\nameset.h\
	$(SRCDIR)\display.h\
	$(SRCDIR)\wcd.h\
	$(SRCDIR)\tailor.h\

display.obj: $(DISPLAY_C) $(SRCDIR)\display.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\display.c

# Build WCDSTACK.C
WCDSTACK_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\c3po\WcdStack.h\

wcdstack.obj: $(WCDSTACK_C) $(SRCDIR)\c3po\wcdstack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\wcdstack.c

# Build TEXT.C
TEXT_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\nameset.h\
	$(SRCDIR)\c3po\intset.h\

text.obj: $(TEXT_C) $(SRCDIR)\c3po\text.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\text.c

# Build NAMESET.C
NAMESET_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\c3po\nameset.h\

nameset.obj: $(NAMESET_C) $(SRCDIR)\c3po\nameset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\nameset.c

# Build INTSET.C
INTSET_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\intset.h\

intset.obj: $(INTSET_C) $(SRCDIR)\c3po\intset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\intset.c

# Build ERROR.C
ERROR_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\

error.obj: $(ERROR_C) $(SRCDIR)\c3po\error.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\error.c

# Build DIRNODE.C
DIRNODE_C=\
	$(SRCDIR)\c3po\std_macr.h\
	$(SRCDIR)\c3po\structur.h\
	$(SRCDIR)\c3po\Error.h\
	$(SRCDIR)\c3po\Text.h\
	$(SRCDIR)\c3po\dirnode.h\

dirnode.obj: $(DIRNODE_C) $(SRCDIR)\c3po\dirnode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\dirnode.c

link:
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\wcdwin32.exe $(OBJS) $(LIBS)

