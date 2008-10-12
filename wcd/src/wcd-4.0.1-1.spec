Summary: chdir for DOS and Unix
Name: wcd
Version: 4.0.1
Release: 1
Copyright: GPL
Group: Applications/File
Source: wcd-4.0.1-src.tar.gz
URL: http://www.xs4all.nl/~waterlan/
Packager: Erwin Waterlander <waterlan@xs4all.nl>

%description
Wcd.   Directory changer for DOS and Unix.  Another Norton
Change Directory (NCD) clone with more features.

Wcd is a program to change directory fast. It  saves  time
typing at the keyboard. One needs to type only a part of a
directory name and wcd will jump to it.  Wcd  has  a  fast
selection  method  in  case of multiple matches and allows
aliasing and banning of directories. Wcd also  includes  a
full-screen   interactive  directory  browser  with  speed
search.

%prep
%setup

%build
cd src ; make PREFIX=/usr

%install
cd src ; make install PREFIX=/usr

%post
if ! /bin/grep "function wcd" /etc/bashrc > /dev/null ; then
 echo "function wcd"          >> /etc/bashrc
 echo "{"                       >> /etc/bashrc
 echo "   /usr/bin/wcd.exe \$*" >> /etc/bashrc
 echo "   . \$HOME/bin/wcd.go"  >> /etc/bashrc
 echo "}"                       >> /etc/bashrc ; 
fi

%files
%doc doc/README.txt doc/wcd.txt doc/wcd.ps doc/wcd.pdf doc/INSTALL.unix.intl.txt doc/INSTALL.rpm.txt doc/copying.txt doc/faq.txt doc/problems.txt doc/whatsnew.txt doc/translate.txt
/usr/bin/wcd.exe
/usr/share/man/man1/wcd.1

