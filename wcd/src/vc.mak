# Common parts of the Microsoft Visual C++ makefiles.

!if "$(ASCII_TREE)" == "1"
CFLAGS = $(CFLAGS) /DASCII_TREE
!endif
!if "$(UCS)" == "1"
CFLAGS = $(CFLAGS) /DWCD_UNICODE
!endif

stack.obj :  $(SRCDIR)\stack.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\stack.c

match.obj :  $(SRCDIR)\match.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\match.c

matchl.obj :  $(SRCDIR)\matchl.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\matchl.c

matchw.obj :  $(SRCDIR)\matchw.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\matchw.c

querycp.obj :  $(SRCDIR)\querycp.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\querycp.c

wcwidth.obj :  $(SRCDIR)\wcwidth.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\wcwidth.c

wfixpath.obj :  $(SRCDIR)\wfixpath.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\wfixpath.c

dosdir.obj :  $(SRCDIR)\dosdir.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\dosdir.c

display.obj :  $(SRCDIR)\display.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\display.c

colors.obj :  $(SRCDIR)\colors.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\colors.c

graphics.obj :  $(SRCDIR)\graphics.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\graphics.c

wcd.obj :  $(SRCDIR)\wcd.c
	$(CC) /c $(CFLAGS) $(CFLAGS_VERSION) $(SRCDIR)\wcd.c

wcddir.obj :  $(SRCDIR)\wcddir.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\wcddir.c


WcdStack.obj :  $(SRCDIR)\c3po\WcdStack.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\WcdStack.c

Text.obj :  $(SRCDIR)\c3po\Text.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\Text.c

nameset.obj :  $(SRCDIR)\c3po\nameset.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\nameset.c

Error.obj :  $(SRCDIR)\c3po\Error.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\Error.c

intset.obj :  $(SRCDIR)\c3po\intset.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\intset.c

command.obj :  $(SRCDIR)\c3po\command.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\command.c

dirnode.obj :  $(SRCDIR)\c3po\dirnode.c
	$(CC) /c $(CFLAGS) $(SRCDIR)\c3po\dirnode.c


PACKAGE     = wcd
exec_prefix = $(prefix)
bindir      = $(exec_prefix)\bin
datarootdir = $(prefix)\share
datadir     = $(datarootdir)
!ifndef docsubdir
docsubdir   = $(PACKAGE)-$(VERSION)
!endif
docdir      = $(datarootdir)\doc\$(docsubdir)


status:
	@echo "CFLAGS = $(CFLAGS)"
	@echo $(MAKEDIR)

$(prefix):
	if not exist $@ mkdir $@

$(bindir): $(prefix)
	if not exist $@ mkdir $@

$(datarootdir): $(prefix)
	if not exist $@ mkdir $@

$(datarootdir)\doc: $(datarootdir)
	if not exist $@ mkdir $@

$(docdir): $(datarootdir)\doc
	if not exist $@ mkdir $@

install: $(PROGRAM) $(DOCFILES) $(bindir) $(docdir)
	copy $(PROGRAM) $(bindir)
!if "$(PROGRAM)" == "wcdwin32.exe"
	copy ..\wcd.bat $(bindir)
	copy ..\wcd_win95.bat $(bindir)
!endif
	$(MAKE) install-doc

$(SRCDIR)\man\man1\wcd1.pod : $(SRCDIR)\man\man1\wcd1pod.in
	wcc386 -pc -ddos -dunix $** > $@

$(SRCDIR)\..\doc\wcd.txt : $(SRCDIR)\man\man1\wcd1.pod
	pod2text $** > $@

$(SRCDIR)\..\doc\wcd.$(HTMLEXT) : $(SRCDIR)\man\man1\wcd1.pod
	pod2html --title="$(PACKAGE) $(VERSION) - Wherever Change Directory" $** > $@

txt: $(SRCDIR)\..\doc\wcd.txt

html: $(SRCDIR)\..\doc\wcd.$(HTMLEXT)

doc : $(DOCFILES)

install-doc: $(docdir) $(DOCFILES)
	copy $(SRCDIR)\..\doc\faq.txt $(docdir)
	copy $(SRCDIR)\..\doc\whatsnew.txt $(docdir)
	copy $(SRCDIR)\..\doc\README.txt $(docdir)
	copy $(SRCDIR)\..\doc\problems.txt $(docdir)
	copy $(SRCDIR)\..\doc\todo.txt $(docdir)
	copy $(SRCDIR)\..\doc\INSTALL.txt $(docdir)
	copy $(SRCDIR)\..\doc\INST_DOS.txt $(docdir)
	copy $(SRCDIR)\..\doc\translat.txt $(docdir)
	copy $(SRCDIR)\..\doc\copying.txt $(docdir)
	copy $(SRCDIR)\..\doc\$(PACKAGE).txt $(docdir)
	copy $(SRCDIR)\..\doc\$(PACKAGE).$(HTMLEXT) $(docdir)

uninstall:
	-del $(bindir)\$(PROGRAM)
!if "$(PROGRAM)" == "wcdwin32.exe"
	-del $(bindir)\wcd.bat
	-del $(bindir)\wcd_win95.bat
!endif
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

DISTCMD = dist.bat

dist :
	@echo cd /d $(prefix) > $(DISTCMD)
	@echo unix2dos -k share\doc\$(docsubdir)\*.txt >> $(DISTCMD)
	@echo unix2dos -k share\doc\$(docsubdir)\*.$(HTMLEXT) >> $(DISTCMD)
	@echo zip -r $(ZIPFILE) $(ZIPOBJ) >> $(DISTCMD)
	@echo cd /d $(MAKEDIR) >> $(DISTCMD)
	@echo move $(prefix)\$(ZIPFILE) $(ZIPFILEDIR) >> $(DISTCMD)
	.\$(DISTCMD)

mostlyclean:
	-del *~
	-del *.bak
	-del *.obj
	-del *.exe
	-del $(DISTCMD)

clean: mostlyclean
	-del $(DOCFILES)

