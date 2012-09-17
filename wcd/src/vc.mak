# Common parts of the Microsoft Visual C++ makefiles.

#!ifdef ASCII_TREE
#CFLAGS += -dASCII_TREE
#!endif
#!ifeq UCS 1
#CFLAGS += -dWCD_UNICODE -za99
#!endif

stack.obj :  $(SRCDIR)\stack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\stack.c

match.obj :  $(SRCDIR)\match.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\match.c

matchl.obj :  $(SRCDIR)\matchl.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\matchl.c

matchw.obj :  $(SRCDIR)\matchw.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\matchw.c

querycp.obj :  $(SRCDIR)\querycp.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\querycp.c

wcwidth.obj :  $(SRCDIR)\wcwidth.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wcwidth.c

wfixpath.obj :  $(SRCDIR)\wfixpath.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wfixpath.c

dosdir.obj :  $(SRCDIR)\dosdir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dosdir.c

display.obj :  $(SRCDIR)\display.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\display.c

colors.obj :  $(SRCDIR)\colors.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\colors.c

graphics.obj :  $(SRCDIR)\graphics.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\graphics.c

wcd.obj :  $(SRCDIR)\wcd.c
	$(CC) -c $(CFLAGS) $(CFLAGS_VERSION) $(SRCDIR)\wcd.c

wcddir.obj :  $(SRCDIR)\wcddir.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wcddir.c


WcdStack.obj :  $(SRCDIR)\c3po\WcdStack.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\WcdStack.c

Text.obj :  $(SRCDIR)\c3po\Text.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\Text.c

nameset.obj :  $(SRCDIR)\c3po\nameset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\nameset.c

Error.obj :  $(SRCDIR)\c3po\Error.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\Error.c

intset.obj :  $(SRCDIR)\c3po\intset.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\intset.c

command.obj :  $(SRCDIR)\c3po\command.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\command.c

dirnode.obj :  $(SRCDIR)\c3po\dirnode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\c3po\dirnode.c


PACKAGE     = wcd
exec_prefix = $(prefix)
bindir      = $(exec_prefix)\bin
datarootdir = $(prefix)\share
datadir     = $(datarootdir)
!ifndef docsubdir
docsubdir   = $(PACKAGE)-$(VERSION)
!endif
docdir      = $(datarootdir)\doc\$(docsubdir)


$(prefix): .EXISTSONLY
	mkdir $@

$(bindir): $(prefix) .EXISTSONLY
	mkdir $@

$(datarootdir): $(prefix) .EXISTSONLY
	mkdir $@

$(datarootdir)\doc: $(datarootdir) .EXISTSONLY
	mkdir $@

$(docdir): $(datarootdir)\doc .EXISTSONLY
	mkdir $@

install: $(PROGRAM) $(DOCFILES) $(bindir) $(docdir)
	copy $(PROGRAM) $(bindir)
#!ifeq PROGRAM wcdwin32.exe
#	copy ..\wcd.bat $(bindir)
#	copy ..\wcd_win95.bat $(bindir)
#!endif
#!ifeq PROGRAM wcdos2.exe
#	copy ..\wcd.cmd $(bindir)
#!endif
	copy $(SRCDIR)\..\doc\*.txt $(docdir)
	copy $(SRCDIR)\..\doc\*.$(HTMLEXT) $(docdir)

$(SRCDIR)\man\man1\wcd1.pod : $(SRCDIR)\man\man1\wcd1pod.in
	wcc386 -pc -ddos -dunix $< > $@

$(SRCDIR)\..\doc\wcd.txt : $(SRCDIR)\man\man1\wcd1.pod
	pod2text $< > $@

$(SRCDIR)\..\doc\wcd.$(HTMLEXT) : $(SRCDIR)\man\man1\wcd1.pod
	pod2html --title="$(PACKAGE) $(VERSION) - Wherever Change Directory" $< > $@

txt: $(SRCDIR)\..\doc\wcd.txt

html: $(SRCDIR)\..\doc\wcd.$(HTMLEXT)

doc : $(DOCFILES)

install-doc: $(docdir) $(DOCFILES)
	copy $(SRCDIR)\..\doc\*.txt $(docdir)
	copy $(SRCDIR)\..\doc\*.$(HTMLEXT) $(docdir)

uninstall:
	-del $(bindir)\$(PROGRAM)
#!ifeq PROGRAM wcdwin32.exe
#	-del $(bindir)\wcd.bat
#	-del $(bindir)\wcd_win95.bat
#!endif
#!ifeq PROGRAM wcdos2.exe
#	-del $(bindir)\wcd.cmd
#!endif
	-rmdir /s /q $(docdir)

!ifndef VERSIONSUFFIX
VERSIONSUFFIX	= -bin
!endif

!ifndef ZIPFILE
ZIPFILE = $(PACKAGE)$(VERSION)$(VERSIONSUFFIX).zip
!endif
!ifndef ZIPFILEDIR
ZIPFILEDIR = ..\..\..
!endif
ZIPOBJ = bin\$(PROGRAM) share\doc\$(docsubdir) $(ZIPOBJ_EXTRA)

#CURDISK = $+ $(%cdrive): $-
#CURDIR = $+ $(%cwd) $-

!ifdef __OS2__
DISTCMD = dist.cmd
!else
DISTCMD = dist.bat
!endif

dist :
	@%create $(DISTCMD)
	@%append $(DISTCMD) set PREFIX=$(prefix)
	@%append $(DISTCMD) set PREFIXDISK=%PREFIX:~0,2%
	@%append $(DISTCMD) %PREFIXDISK%
	@%append $(DISTCMD) cd $(prefix)
	@%append $(DISTCMD) unix2dos -k share\doc\$(docsubdir)\*.txt
	@%append $(DISTCMD) unix2dos -k share\doc\$(docsubdir)\*.$(HTMLEXT)
	@%append $(DISTCMD) zip -r $(ZIPFILE) $(ZIPOBJ)
	@%append $(DISTCMD) $(CURDISK)
	@%append $(DISTCMD) cd $(CURDIR)
	@%append $(DISTCMD) move $(prefix)\$(ZIPFILE) $(ZIPFILEDIR)
	.\$(DISTCMD)

mostlyclean:
	-del *~
	-del *.bak
	-del *.obj
	-del *.exe

clean: mostlyclean
