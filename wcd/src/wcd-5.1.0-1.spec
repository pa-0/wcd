Summary: chdir for DOS and Unix
Name: wcd
Version: 5.1.0
Release: 1
License: GPL
Group: Applications/File
Source: wcd-5.1.0-src.tar.gz
URL: http://www.xs4all.nl/~waterlan/
Packager: Erwin Waterlander <waterlan@xs4all.nl>

%description
Wcd.   Directory changer for DOS and Unix.  Another Norton
Change Directory (NCD) clone.

Wcd is a program to change directory fast. It  saves  time
typing at the keyboard. One needs to type only a part of a
directory name and wcd will jump to it.  Wcd  has  a  fast
selection  method  in  case of multiple matches and allows
aliasing and banning of directories. Wcd also  includes  a
full-screen interactive directory tree browser with  speed
search.

%prep
%setup

%build
make -C src PREFIX=/usr UCS=1

%install
make -C src install DESTDIR=${RPM_BUILD_ROOT} PREFIX=/usr

%post
if ! /bin/grep "function wcd" /etc/bashrc > /dev/null ; then
 echo "function wcd"          >> /etc/bashrc
 echo "{"                       >> /etc/bashrc
 echo "   /usr/bin/wcd.exe \$*" >> /etc/bashrc
 echo "   . \$HOME/bin/wcd.go"  >> /etc/bashrc
 echo "}"                       >> /etc/bashrc ; 
fi

%files
/usr/bin/wcd.exe
/usr/share/man/man1/wcd.*
/usr/share/locale/*/LC_MESSAGES/wcd.mo

%doc doc/README.txt doc/wcd.txt doc/wcd.ps doc/wcd.pdf doc/wcd.html doc/INSTALL.unix.txt doc/INSTALL.rpm.txt doc/copying.txt doc/faq.txt doc/problems.txt doc/whatsnew.txt doc/translate.txt

