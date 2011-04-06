Name:                   qtgpsc
License:                GPLv3
Group:                  Productivity/Scientific/Other
Summary:                A client for the gpsd GPS server, based on the Qt toolkit
Version:                @VERSION@
Release:                1
URL:                    http://navlost.eu/devel/qtgpsc/
Source:                 %{name}-%{version}.tar.gz
BuildRoot:              %{_tmppath}/%{name}-%{version}-build
BuildRequires:          cmake >= 2.6, libqt4-devel >= 4.5, libkde4-devel >= 4.3, gpsd-devel >= 2.96
%if 0%{?suse_version}  
BuildRequires:          update-desktop-files
%endif
Requires:               libqt4 >= 4.5.1, libgps20 >= 2.96

%description
qtGPSc is a simple graphical client for gpsd.  It connects to
a GPS receiver via gpsd and displays position, speed, and time,
as well as various precision indicators and satellite configuration.
It can also show the raw GPS data in JSON or NMEA format.

%prep
%setup -n %{name}-%{version}  

%build  
%cmake_kde4 -d builddir
%make_jobs
   
%install
cd builddir
%makeinstall
%if 0%{?suse_version}  
%suse_update_desktop_file -i %{name}  
%endif

%clean  
[ "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf "$RPM_BUILD_ROOT"
   
%files
%defattr(-,root,root)
%doc CHANGELOG INSTALL LICENSE README
%{_bindir}/qtgpsc
%{_datadir}/icons/hicolor/48x48/apps/qtgpsc.png
%{_datadir}/applications/qtgpsc.desktop

%changelog
