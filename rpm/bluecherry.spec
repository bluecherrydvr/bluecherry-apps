Name:           bluecherry
Version:	2.5.23
Release:        1
Epoch:		1
Summary:        Bluecherry DVR Server
Group:          Applications/Multimedia
License:        Proprietary
URL:            http://www.bluecherrydvr.com/
Source:         %{name}-%{version}.tar.gz
Prefix:         %{_prefix}
BuildRequires:	git,make,rpm-build,gcc-c++,ccache,autoconf,automake,libtool,bison,flex
BuildRequires:	texinfo,php-devel,yasm,cmake,libbsd-devel,chrpath
BuildRequires:  mariadb-devel,opencv-devel,systemd-devel,sudo
BuildRequires:  systemd
Requires:	httpd,mod_ssl,php,php-pdo,php-pear-Mail,php-pear-Mail-Mime,mkvtoolnix,dpkg
Requires:	mariadb,mariadb-server,sysstat,opencv-core,epel-release,libbsd,policycoreutils-python

%description
This package contains the server components for the Bluecherry DVR system.

%prep
sudo rm -rf %{_builddir}/%{name}
%autosetup -n %{name} -c %{name}
echo "#define GIT_REVISION \"`git describe --dirty --always --long` `git describe --all`\"" > server/version.h

%build
git submodule foreach --recursive "git clean -dxf && git reset --hard"
git clean -dxf && git reset --hard
echo "#define GIT_REVISION \"`git describe --dirty --always --long` `git describe --all`\"" > server/version.h

sudo rm -rf %{buildroot}
make clean
sudo rm -rf /usr/lib/bluecherry
sudo make

%install
sudo chown -R $USER %{_builddir}/%{name}
%make_install
%{_builddir}/%{name}/scripts/build_helper/post_make_install.sh rpm "%{_builddir}/%{name}" "%{buildroot}" %{epoch}:%{version}-%{release}

mkdir -p %{buildroot}/etc/logrotate.d
cp %{_builddir}/%{name}/rpm/bluecherry.logrotate %{buildroot}/etc/logrotate.d/bluecherry
mkdir -p %{buildroot}/etc/php.d
mv %{buildroot}/etc/php5/apache2/conf.d/bluecherry_apache2.ini %{buildroot}/etc/php.d/bluecherry.ini
rm -r %{buildroot}/etc/php5
chmod 755 %{buildroot}/usr/lib/libbluecherry.so.0
chmod 755 %{buildroot}/usr/lib64/php/modules/bluecherry.so
install -m644 -D %{_builddir}/%{name}/rpm/bc-server.service %{buildroot}/%{_unitdir}/bc-server.service

%files
%config(noreplace) %{_sysconfdir}/httpd/sites-available/bluecherry.conf
%config(noreplace) %{_sysconfdir}/php.d/bluecherry.ini
%config(noreplace) %{_sysconfdir}/cron.d/bluecherry
%config(noreplace) %{_sysconfdir}/logrotate.d/bluecherry
%config(noreplace) %{_sysconfdir}/monit/conf.d/bluecherry
%config(noreplace) %{_sysconfdir}/rsyslog.d/10-bluecherry.conf
%{_sbindir}/*
%{_libdir}/*
/usr/lib/libbluecherry.so.0
/usr/lib/bluecherry/*
%{_datadir}/%{name}/*
%{_unitdir}/*

%pre
set -x
if [[ $(getenforce) == "Enforcing" ]]
then
    echo Configuring selinux. Please wait.
    semanage port -m -t http_port_t -p tcp 7001 # for selinux
    setsebool -P httpd_can_network_connect 1
fi
systemctl enable mariadb.service
systemctl start mariadb.service
INSTALL="1"
UPGRADE="2"
if [[ "$1" == "$UPGRADE" ]] && [[ -s /etc/bluecherry.conf ]]
then
	eval $(sed '/\(dbname\|user\|password\)/!d;s/ *= */=/'";s/\"/'/g" /etc/bluecherry.conf)
	DB_VERSION=$(cat /usr/share/bluecherry/installed_db_version)
        OLD_DB_VERSION=`echo "SELECT value from GlobalSettings WHERE parameter = 'G_DB_VERSION'" | mysql -D"$dbname" -u"$user" -p"$password" | tail -n 1`
        NEW_DB_VERSION=$DB_VERSION
        if [[ "$OLD_DB_VERSION" -gt "$NEW_DB_VERSION" ]]
        then
                echo "DOWNGRADE ACROSS DATABASE VERSIONS IS NOT SUPPORTED"
                echo "Something goes wrong. Please 'rpm -e bluecherry' "
		echo "then drop database and '/etc/bluecherry.conf' and make clean install"
		echo "ALL your data will be lost!!!"
		echo "Or ask support..."
                exit 1
        fi
fi

case "$1" in
1|2)
    if ! id bluecherry > /dev/null 2>&1; then
	groupadd -r -f bluecherry
	useradd -c "Bluecherry DVR" -d /var/lib/bluecherry \
                -g bluecherry -G audio,video -r -m bluecherry
    else
        # just to be sure we have such group, if user was created manually
        groupadd -r -f bluecherry || true
        usermod -c "Bluecherry DVR" -d /var/lib/bluecherry \
                -g bluecherry -G audio,video bluecherry
    fi
    usermod -a -G video,audio,bluecherry,dialout apache || true
    ;;
esac
if [[ "$1" == "$UPGRADE" ]]
then
    systemctl stop bc-server.service 
fi

%post
/sbin/ldconfig
bash -x %{_datadir}/%{name}/postinstall.sh "$@"

%posttrans
systemctl enable httpd.service
systemctl enable bc-server.service
systemctl daemon-reload
systemctl stop  bc-server.service
systemctl start bc-server.service

%preun
set -x
if [[ $1 == 0 ]] # uninstall, not upgrade
then
    systemctl stop bc-server.service
    rm /etc/httpd/sites-enabled/bluecherry.conf
    systemctl reload httpd.service
fi

%postun
/sbin/ldconfig
