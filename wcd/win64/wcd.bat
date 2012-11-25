@echo off

set PDC_RESTORE_SCREEN=1
rem set WCDHOME=%HOMEDRIVE%%HOMEPATH%
rem set LANG=nl_NL
rem set LANGUAGE=nl
rem set WCDLOCALEDIR=c:/Program Files/wcd/share/locale
rem set WCDSCAN=c:;d:

rem Define WCDHOME when WCDHOME and HOME are both not defined,
rem because on Windows Vista and up users are not allowed to
rem create files in the root directory of the system partition.
IF NOT DEFINED WCDHOME (
  IF NOT DEFINED HOME (
    set WCDHOME=%HOMEDRIVE%%HOMEPATH%
  )
)

wcdwin64.exe %*

IF DEFINED WCDHOME (
 "%WCDHOME%\wcdgo.bat"
) ELSE (
  IF DEFINED HOME (
    "%HOME%\wcdgo.bat"
  ) ELSE (
    c:\wcdgo.bat
  )
)

