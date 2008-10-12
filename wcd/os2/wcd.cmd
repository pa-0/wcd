@echo off

set PDC_RESTORE_SCREEN=1
rem set LANG=nl_NL
rem set WCDLOCALEDIR=c:/Program Files/wcd/share/locale

wcdos2.exe %1 %2 %3 %4 %5 %6 %7 %8 %9

IF NOT "%WCDHOME%" == "" GOTO WCDHOME

IF NOT "%HOME%" == "" GOTO HOME

c:\wcdgo.cmd
GOTO END

:WCDHOME
%WCDHOME%\wcdgo.cmd
GOTO END

:HOME
%HOME%\wcdgo.cmd
GOTO END

:END

