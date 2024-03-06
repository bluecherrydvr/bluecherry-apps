#!/usr/bin/env bash
set -euo pipefail # strict mode

if [[ $SUDO_USER == "bluecherry" ]]
then
        echo "You're installing Bluecherry on a system that already has an existing bluecherry user. Please remove that user first as it will be automatically created."
        exit
fi

if [[ $(getent passwd "$UID" | cut -d: -f1) == "bluecherry" ]]
then
    echo "You're installing Bluecherry on a system that already has an existing bluecherry user. Please remove that user first as it will be automatically created."
exit
fi 

# Ubuntu 18.04
bionic_install()
{
    apt update
    apt -y install gpg python3-distutils wget
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
    apt -y install bluecherry
    systemctl restart bluecherry
}

# Ubuntu 20.04
focal_install()
{
    apt-get update
# Update 1/5/23 Resolve dependancy problems on Ubuntu 20.04 desktop
#    apt -y install gpg python3-pip
#    pip install pyopenssl==22.0.0
    apt -y install python3-distutils gnupg2 wget
#    wget https://bootstrap.pypa.io/get-pip.py | python3 get-pip.py -
    wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/get-pip.py 
    python3 /tmp/get-pip.py
#    pip install pyopenssl --upgrade
    pip3 install pyOpenSSL --upgrade
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
#    apt -y install mariadb-server-10.3 
    apt -y install bluecherry
    systemctl restart bluecherry
}

# Ubuntu 20.10
groovy_install()
{
    apt -y install gpg wget
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
    apt -y install mariadb-server bluecherry
    systemctl restart bluecherry
}

# Ubuntu 21.04
hirsute_install()
{
    apt -y install gpg wget
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
    apt -y install mariadb-server bluecherry
    systemctl restart bluecherry
}

# Ubuntu 22.04
# Also used for Mint 21.1 Vera, based on Ubuntu 22.04 Jammy
jammy_install()
{
    apt-get update
    apt -y install gpg software-properties-common wget
#    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | sudo tee /etc/apt/trusted.gpg.d/bluecherry.asc
    VERSION_CODENAME=jammy # don't say "vera" for Linux Mint at this point
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    add-apt-repository ppa:ondrej/php -y
    apt -y update
    apt -y install php7.4-fpm php7.4-sqlite3 php7.4-curl php7.4-mysql php7.4-gd php-mail php-mail-mime php-mysql php7.4-fpm php7.4-mysql
#    apt -y install mariadb-server-10.3
    apt -y install bluecherry
    systemctl restart bluecherry
}

centos_7_install()
{   
    setenforce 0
    sed -i 's/^SELINUX=.*/SELINUX=permissive/g' /etc/selinux/config
    curl -s https://dl.bluecherrydvr.com/yum.repos.d/bluecherry-centos7.repo -o /etc/yum.repos.d/bluecherry-centos7.repo 
    yum -y update
    yum -y install epel-release
    yum -y install dpkg wget epel-release httpd libbsd mariadb mariadb-server mkvtoolnix mkvtoolnix-gui mod_ssl nmap opencv-core php php-mysqlnd php-pdo php-pear-Mail php-pear-Mail-Mime policycoreutils-python sysstat v4l-utils
    firewall-cmd --zone=public --add-port=7001/tcp --permanent
    firewall-cmd --zone=public --add-port=7002/tcp --permanent
    firewall-cmd --reload
    yum -y install bluecherry
    systemctl restart bluecherry
}

# Debian 10
buster_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo wget
    apt-get -y install python3-pip
    pip3 install --user --upgrade pip
    wget -q https://repo.mysql.com/RPM-GPG-KEY-mysql-2023 -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt-get -y update
    apt-get -y install mysql-server bluecherry
}

# Debian 11
bullseye_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo sudo python3-distutils wget
    wget -q https://repo.mysql.com/RPM-GPG-KEY-mysql-2023 -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME"-unstable.list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt-get -y update
    apt-get -y install mariadb-server bluecherry
#   apt-get install mariadb-server
}

check_distro()
{
    if [[ -e /etc/lsb-release ]]
    then
        . /etc/lsb-release
    fi

    if [[ -e /etc/os-release ]] 
    then
        . /etc/os-release
    fi
    
    if [[ -e /etc/os-release ]] && [[ $ID == "centos" ]]
    then
        DISTRO=${ID}_${VERSION_ID}
    else

        if [[ -e /etc/lsb-release ]]
        then
            DISTRO=$DISTRIB_CODENAME
        else
            DISTRO=`echo $VERSION | sed -e 's/^.*[(]//' -e 's/[)]//'`
        fi
    fi
    export DISTRO
    echo Distro: $DISTRO
}

check_distro
if   [[ "$ID" == "ubuntu" && "$VERSION_ID" == "18.04" && "$VERSION_CODENAME" == "bionic"   ]]; then bionic_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "20.10" && "$VERSION_CODENAME" == "groovy"   ]]; then groovy_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "20.04" && "$VERSION_CODENAME" == "focal"    ]]; then focal_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "22.04" && "$VERSION_CODENAME" == "jammy"    ]]; then jammy_install;
elif [[ "$ID" == "debian" && "$VERSION_ID" == "10"    && "$VERSION_CODENAME" == "buster"   ]]; then buster_install;
elif [[ "$ID" == "debian" && "$VERSION_ID" == "11"    && "$VERSION_CODENAME" == "bullseye" ]]; then bullseye_install;
elif [[ "$ID" == "mint"   && "$VERSION_ID" == "21.1"  && "$VERSION_CODENAME" == "vera"     ]]; then jammy_install; # Mint 21.1 Vera, based on Ubuntu 22.04 Jammy
else
    echo "Currently we only support Ubuntu 18.04 (Bionic), Ubuntu 20.04 (Focal), Ubuntu 22.04 (Jammy) and Debian 10 (Buster), Linux Mint 21.1 (Vera) for unstable testing"
fi
