#!/usr/bin/env bash
set -euo pipefail # strict mode

if ! [[ "$(whoami)" == root ]]; then
    echo "Must be root to install Bluecherry." >&2
    exit
fi

if getent passwd bluecherry; then
    echo "You're installing Bluecherry on a system that already has an existing bluecherry user. Please remove that user first as it will be automatically created."
    exit
fi 

# Ubuntu 20.04
focal_install()
{
    apt-get update
    apt -y install python3-distutils gnupg2 wget
    wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/pip/3.8/get-pip.py
	python3 /tmp/get-pip.py
    pip3 install pyOpenSSL --upgrade
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME".list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
    apt -y install bluecherry
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
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME".list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    echo "deb [trusted=yes] https://ppa.launchpadcontent.net/ondrej/php/ubuntu/ jammy main" > /etc/apt/sources.list.d/ondrej-ubuntu-php-jammy.list
    apt -y update
    apt -y install php7.4-fpm php7.4-sqlite3 php7.4-curl php7.4-mysql php7.4-gd php-mail php-mail-mime php-mysql php7.4-fpm php7.4-mysql
    apt -y install bluecherry
    systemctl restart bluecherry
}

# Ubuntu 23.10
mantic_install()
{
    # Differences from jammy:
    # Don't add ppa:ondrej/php, it fails for Mantic.
    # Don't explicitly state which php packages to install
    apt-get update
    apt -y install gpg software-properties-common wget
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | sudo tee /etc/apt/trusted.gpg.d/bluecherry.asc
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME".list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt -y update
    apt -y install bluecherry
    systemctl restart bluecherry
}

# Ubuntu 24.04
# Ubuntu 24.10
noble_install()
{
    mantic_install
}

# Debian 10
buster_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo wget
    apt-get -y install python3-pip
    pip3 install --user --upgrade pip
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME".list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt-get -y update
    apt-get -y install default-mysql-server bluecherry
}

# Debian 11
bullseye_install()
{
    apt-get -y update
    apt-get -y install gnupg sudo sudo python3-distutils wget
    wget -q https://dl.bluecherrydvr.com/key/bluecherry.asc -O- | apt-key add -
    : "${SRCLIST_URL:=https://dl.bluecherrydvr.com/sources.list.d/bluecherry-"$VERSION_CODENAME".list}"
    wget --output-document=/etc/apt/sources.list.d/bluecherry-"$VERSION_CODENAME".list "$SRCLIST_URL"
    apt-get -y update
    apt-get -y install mariadb-server bluecherry
}

# Debian 12
bookworm_install()
{
    bullseye_install
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
if   [[ "$ID" == "ubuntu" && "$VERSION_ID" == "23.10" && "$VERSION_CODENAME" == "mantic"   ]]; then
    echo "This distribution release has reached its End Of Life and is not supported anymore. All users should upgrade."
    exit 1
fi

if   [[ "$ID" == "ubuntu" && "$VERSION_ID" == "20.04" && "$VERSION_CODENAME" == "focal"    ]]; then focal_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "22.04" && "$VERSION_CODENAME" == "jammy"    ]]; then jammy_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "24.04" && "$VERSION_CODENAME" == "noble"    ]]; then noble_install;
elif [[ "$ID" == "ubuntu" && "$VERSION_ID" == "24.10" && "$VERSION_CODENAME" == "oracular" ]]; then noble_install;
elif [[ "$ID" == "debian" && "$VERSION_ID" == "10"    && "$VERSION_CODENAME" == "buster"   ]]; then buster_install;
elif [[ "$ID" == "debian" && "$VERSION_ID" == "11"    && "$VERSION_CODENAME" == "bullseye" ]]; then bullseye_install;
elif [[ "$ID" == "debian" && "$VERSION_ID" == "12"    && "$VERSION_CODENAME" == "bookworm" ]]; then bookworm_install;
elif [[ "$ID" == "linuxmint" && "$VERSION_ID" == "21.1" && "$VERSION_CODENAME" == "vera"     ]]; then jammy_install; # based on Ubuntu 22.04 Jammy
elif [[ "$ID" == "linuxmint" && "$VERSION_ID" == "21.2" && "$VERSION_CODENAME" == "victoria" ]]; then jammy_install; # based on Ubuntu 22.04 Jammy
elif [[ "$ID" == "linuxmint" && "$VERSION_ID" == "21.3" && "$VERSION_CODENAME" == "virginia" ]]; then jammy_install; # based on Ubuntu 22.04 Jammy
elif [[ "$ID" == "linuxmint" && "$VERSION_ID" == "22" && "$VERSION_CODENAME" == "wilma" ]]; then noble_install; # based on Ubuntu 24.04 Noble
elif [[ "$ID" == "linuxmint" && "$VERSION_ID" == "22.1" && "$VERSION_CODENAME" == "xia" ]]; then noble_install; # based on Ubuntu 24.04 Noble
else
    echo "Currently we only support up to date Ubuntu, Debian and Mint Linux distributions."
    exit 1
fi
