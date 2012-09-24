Summary: Chdir for DOS and Unix
Name: wcd
Version: 5.2.2
Release: 4%{?dist}
License: GPLv2 
Group: Applications/File
Source: http://waterlan.home.xs4all.nl/%{name}-%{version}-src.tar.gz
URL: http://waterlan.home.xs4all.nl/
BuildRequires: gettext
BuildRequires: perl
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
%config /etc/profile.d/wcd.*

%doc doc/README.txt doc/wcd.txt doc/wcd.htm doc/INSTALL.txt doc/UNIX.txt doc/RPM.txt doc/copying.txt doc/faq.txt doc/problems.txt doc/whatsnew.txt doc/translat.txt
%{_mandir}/man1/wcd.*


%changelog
* Mon Sep 24 2012 Erwin Waterlander <waterlan@xs4all.nl> - 5.2.2-4
- Summary starts with capital letter C.
- Config files marked with config.
- Removed clean section (needed only if supporting EPEL5).
- Moved man-pages under doc.

* Sun Sep 23 2012 Erwin Waterlander <waterlan@xs4all.nl> - 5.2.2-3
- Increment release version.

* Sun Sep 23 2012 Erwin Waterlander <waterlan@xs4all.nl> - 5.2.2-2
- Removed tag Packager.
- Removed Buildrequires sed.
- Changed License tag from GPL to GPLv2

* Sun Sep 23 2012 Erwin Waterlander <waterlan@xs4all.nl> - 5.2.2-1
- Initial version for Fedora.

