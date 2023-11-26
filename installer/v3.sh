#!/usr/bin/env bash

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
    echo $DISTRO
}

trusty_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-trusty.list https://unstable.bluecherrydvr.com/sources.list.d/bluecherry-trusty-unstable.list
    apt -y update
    apt -y install bluecherry
    service bluecherry restart
}

bionic_install()
{
    apt update
    apt -y install gpg python3-distutils
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --no-check-certificate --output-document=/etc/apt/sources.list.d/bluecherry-bionic.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-bionic-unstable.list
    apt -y update
    apt -y install bluecherry
    systemctl restart bluecherry
}

xenial_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-xenial.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-xenial-unstable.list
    apt -y update
    apt -y install bluecherry
    systemctl restart bluecherry
}

# 20.04

focal_install()
{
    apt-get update
# Update 1/5/23 Resolve dependancy problems on Ubuntu 20.04 desktop
#    apt -y install gpg python3-pip
#    pip install pyopenssl==22.0.0
    apt -y install python3-distutils gnupg2
#    wget https://bootstrap.pypa.io/get-pip.py | python3 get-pip.py -
    wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/get-pip.py 
    python3 /tmp/get-pip.py
#    pip install pyopenssl --upgrade
    pip3 install pyOpenSSL --upgrade
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-focal.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-focal-unstable.list
    apt -y update
#    apt -y install mariadb-server-10.3 
    apt -y install bluecherry
    systemctl restart bluecherry
}

# 20.10

groovy_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-groovy.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-groovy-unstable.list
    apt -y update
    apt -y install mariadb-server bluecherry
    systemctl restart bluecherry
}

# 21.04

hirsute_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-hirsute.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-hirsute-unstable.list
    apt -y update
    apt -y install mariadb-server bluecherry
    systemctl restart bluecherry
}

# 22.04

jammy_install()
{
    apt-get update
    apt -y install gpg software-properties-common
#    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | sudo tee /etc/apt/trusted.gpg.d/bluecherry.asc
    wget --output-document=/etc/apt/sources.list.d/bluecherry-jammy.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-jammy-unstable.list
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

jessie_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-jessie.list https:/unstablel.bluecherrydvr.com/sources.list.d/bluecherry-jessie-unstable.list
    apt-get -y update
    apt-get -y install bluecherry
}

wheezy_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-wheezy.list https://unstable.bluecherrydvr.com/sources.list.d/bluecherry-wheezy-unstable.list
    apt-get -y update
    apt-get -y install bluecherry
}

stretch_install()
{
    apt -y install gpg
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    wget --output-document=/etc/apt/sources.list.d/bluecherry-stretch.list https://unstable.bluecherrydvr.com/sources.list.d/bluecherry-stretch-unstable.list
    apt-get -y update
    apt-get -y install bluecherry
}

buster_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo
    apt-get -y install python3-pip
    pip3 install --user --upgrade pip
    wget -q https://repo.mysql.com/RPM-GPG-KEY-mysql-2022 -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
#    wget -q http://repo.mysql.com/RPM-GPG-KEY-mysql -O- | apt-key add -
    wget --no-check-certificate --output-document=/etc/apt/sources.list.d/bluecherry-buster.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-buster-unstable.list
    apt-get -y update
    apt-get -y install mysql-server bluecherry
}

bullseye_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo sudo python3-distutils
#    wget -q http://repo.mysql.com/RPM-GPG-KEY-mysql -O- | apt-key add -
    wget -q https://repo.mysql.com/RPM-GPG-KEY-mysql-2022 -O- | apt-key add -
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
#    wget -q http://repo.mysql.com/RPM-GPG-KEY-mysql -O- | apt-key add -
    wget --no-check-certificate --output-document=/etc/apt/sources.list.d/bluecherry-buster.list https://dl.bluecherrydvr.com/sources.list.d/bluecherry-bullseye-unstable.list
    apt-get -y update
    apt-get -y install mariadb-server bluecherry
#   apt-get install mariadb-server
}



if   [ $(check_distro) == "bionic" ]; then
    bionic_install
elif [ $(check_distro) == "buster" ]; then
    buster_install
elif [ $(check_distro) == "focal" ]; then
    focal_install
elif [ $(check_distro) == "jammy" ]; then
    jammy_install
elif [ $(check_distro) == "vera" ]; then
    jammy_install
elif [ $(check_distro) == "groovy" ]; then
    groovy_install
elif [ $(check_distro) == "hirsute" ]; then
    hirsute_install
elif [ $(check_distro) == "bullseye" ]; then
    bullseye_install
#elif [ $(check_distro) == "centos_7" ]; then
#    centos_7_install
else
    echo "Currently we only support Ubuntu 18.04 (Bionic), Ubuntu 20.04 (Focal), Ubuntu 22.04 (Jammy) and Debian 10 (Buster), Linux Mint 21.1 (Vera) for unstable testing"
fi
