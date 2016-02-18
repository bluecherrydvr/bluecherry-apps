Name:           bluecherry-rpm-repo
Version:        7
Release:        1
Summary:        Bluecherry Packages for CentOS 7 Linux repository configuration
Group:          System Environment/Base
License:        Proprietary-BC
URL:            http://www.bluecherrydvr.com/
Source0:        bluecherry.repo
BuildArch:      noarch

%description
This package contains the Bluecherry DVR Packages repository for Enterprise Linux (EPEL)

%prep

%build

%install
install -dm 755 $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d
install -pm 644 %{SOURCE0} $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d

%clean

%files
%config(noreplace) /etc/yum.repos.d/*

%changelog
* Thu Feb 18 2016 Alex Syrnikov <masterspline@gmx.com> 7-1
- initial release
