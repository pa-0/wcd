Summary: chdir for DOS and Unix
Name: wcd
Version: 5.1.5
Release: 1
License: GPL
Group: Applications/File
Source: wcd-5.1.5-src.tar.gz
URL: http://www.xs4all.nl/~waterlan/
Packager: Erwin Waterlander <waterlan@xs4all.nl>

%description
Wcd.   Directory changer for DOS and Unix.  Another Norton
Change Directory (NCD) clone.

Wcd is a command-line program to change directory fast. It
saves time typing at the keyboard.  One needs to type only
a part of a directory  name and wcd  will jump to it.  Wcd
has a fast selection  method  in  case of multiple matches
and allows aliasing and  banning of directories.  Wcd also
includes a full-screen interactive  directory tree browser
with speed search.

%prep
%setup

%build
make -C src prefix=/usr UCS=1

%install
make -C src install DESTDIR=${RPM_BUILD_ROOT} prefix=/usr
make -C src install-profile DESTDIR=${RPM_BUILD_ROOT} prefix=/usr

%post

%files
/usr/bin/wcd.exe
/usr/share/man/man1/wcd.*
/usr/share/locale/*/LC_MESSAGES/wcd.mo
/etc/profile.d/wcd.*

%doc doc/README.txt doc/wcd.txt doc/wcd.htm doc/INSTALL.txt doc/UNIX.txt doc/RPM.txt doc/copying.txt doc/faq.txt doc/problems.txt doc/whatsnew.txt doc/translat.txt

