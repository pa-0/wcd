Summary: chdir for DOS and Unix
Name: wcd
Version: 5.2.2
Release: 1%{?dist}
License: GPL
Group: Applications/File
Source: http://waterlan.home.xs4all.nl/%{name}-%{version}-src.tar.gz
URL: http://waterlan.home.xs4all.nl/
Packager: Erwin Waterlander <waterlan@xs4all.nl>
BuildRequires: gettext
BuildRequires: perl
BuildRequires: sed
BuildRequires: ncurses-devel
BuildRequires: libunistring-devel

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
%setup -q

%build
make -C src %{?_smp_mflags} prefix=%{_prefix} UCS=1 UNINORM=1

%install
make -C src install DESTDIR=${RPM_BUILD_ROOT} prefix=%{_prefix}
make -C src install-profile DESTDIR=${RPM_BUILD_ROOT} prefix=%{_prefix}

%find_lang %{name}

%files -f %{name}.lang
%{_bindir}/wcd.exe
%{_mandir}/man1/wcd.*
/etc/profile.d/wcd.*

%doc doc/README.txt doc/wcd.txt doc/wcd.htm doc/INSTALL.txt doc/UNIX.txt doc/RPM.txt doc/copying.txt doc/faq.txt doc/problems.txt doc/whatsnew.txt doc/translat.txt

%clean
rm -rf $RPM_BUILD_ROOT

%changelog

