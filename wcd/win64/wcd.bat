@echo off

set PDC_RESTORE_SCREEN=1
rem set WCDHOME=%HOMEDRIVE%%HOMEPATH%
rem set LANG=nl_NL
rem set LANGUAGE=nl
rem set WCDLOCALEDIR=c:/Program Files/wcd/share/locale

wcdwin64.exe %*

IF DEFINED WCDHOME (
 %WCDHOME%\wcdgo.bat
) ELSE (
  IF DEFINED HOME (
    %HOME%\wcdgo.bat
  ) ELSE (
    c:\wcdgo.bat
  )
)

