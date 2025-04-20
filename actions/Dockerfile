# set a base image with environment to build from
FROM ubuntu:20.04 AS baseos

ARG BLUECHERRY_GIT_BRANCH_TAG=master

# ---------------------------------------------------------------------------
# Build the base OS with some development libs and tools
FROM baseos AS os_dev_environment
ENV DEBIAN_FRONTEND=noninteractive

COPY ./scripts/policy-rc.d /usr/sbin/policy-rc.d
RUN chmod +x /usr/sbin/policy-rc.d

WORKDIR /root

RUN apt-get update && apt-get install --no-install-recommends -y \
    git sudo openssl ca-certificates wget gnupg gnupg2 gnupg1 \
    ssl-cert nmap curl sysstat iproute2 \
    autoconf automake libtool build-essential gcc g++ \
    debhelper ccache bison flex texinfo yasm cmake libcurl4-openssl-dev \
    libbsd-dev libopencv-dev libudev-dev libva-dev \
    linux-image-generic linux-headers-generic \
    libmysqlclient-dev rsyslog

# ---------------------------------------------------------------------------
FROM os_dev_environment as bluecherry_base_environment
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

RUN git clone --recurse-submodules --progress http://github.com/bluecherrydvr/bluecherry-apps.git \
 && cd bluecherry-apps \
 && git checkout $BLUECHERRY_GIT_BRANCH_TAG \
 && sed -i '/#define PRODUCT_VERSION/s/"$/-docker"/' server/v3license_processor.h

RUN apt-get install --no-install-recommends -y \
    libbsd0 libc6 libgcc1 libssl1.1 libstdc++6 libudev1 \
    zlib1g ucf mkvtoolnix v4l-utils vainfo i965-va-driver \
    php-mail php-mail-mime php-net-smtp php-gd php-curl \
    php-mysql php-sqlite3 \
    apache2 libapache2-mod-php mysql-client sqlite3

# ---------------------------------------------------------------------------
FROM bluecherry_base_environment as bluecherry_build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

COPY depends/onvif_tool bluecherry-apps/utils/onvif_tool

RUN cd bluecherry-apps && ./scripts/build_pkg_native.sh

# ---------------------------------------------------------------------------
FROM bluecherry_build as bluecherry_build_cleaned
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

RUN rm -rf /usr/src/linux-headers-* \
 && rm -rf .ccache \
 && rm -rf bluecherry-apps/.git \
 && rm -rf bluecherry-apps/misc/libav \
 && rm -rf bluecherry-apps/misc/libconfig \
 && rm -rf bluecherry-apps/misc/pugixml

# ---------------------------------------------------------------------------
FROM baseos as bluecherry_install
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

COPY --from=bluecherry_build_cleaned /root/bluecherry-apps/releases/bluecherry_*.deb /root/bluecherry-apps/releases/
COPY ./scripts/policy-rc.d /usr/sbin/policy-rc.d
RUN chmod +x /usr/sbin/policy-rc.d

ARG MYSQL_ADMIN_LOGIN=root
ARG MYSQL_ADMIN_PASSWORD=root
ARG BLUECHERRY_DB_USER=bluecherry
ARG BLUECHERRY_DB_HOST=172.17.0.1
ARG BLUECHERRY_DB_PASSWORD=bluecherry
ARG BLUECHERRY_DB_NAME=bluecherry
ARG BLUECHERRY_DB_ACCESS_HOST=%
ARG BLUECHERRY_LINUX_GROUP_NAME=bluecherry
ARG BLUECHERRY_LINUX_GROUP_ID=1000
ARG BLUECHERRY_LINUX_USER_NAME=bluecherry
ARG BLUECHERRY_LINUX_USER_ID=1000

RUN apt-get update && apt-get install -y \
    rsyslog nmap curl sysstat iproute2 \
    openssl ca-certificates ssl-cert gnupg gnupg2 gnupg1 sudo mysql-client python3-pip wget curl nano cron

RUN echo "[client]\nuser=$MYSQL_ADMIN_LOGIN\npassword=$MYSQL_ADMIN_PASSWORD\n[mysqldump]\nuser=$MYSQL_ADMIN_LOGIN\npassword=$MYSQL_ADMIN_PASSWORD" > /root/.my.cnf

# Debconf pre-seed values
RUN { \
    echo bluecherry bluecherry/mysql_admin_login string $MYSQL_ADMIN_LOGIN; \
    echo bluecherry bluecherry/mysql_admin_password password $MYSQL_ADMIN_PASSWORD; \
    echo bluecherry bluecherry/db_host string $BLUECHERRY_DB_HOST; \
    echo bluecherry bluecherry/db_userhost string $BLUECHERRY_DB_ACCESS_HOST; \
    echo bluecherry bluecherry/db_name string $BLUECHERRY_DB_NAME; \
    echo bluecherry bluecherry/db_user string $BLUECHERRY_DB_USER; \
    echo bluecherry bluecherry/db_password password $BLUECHERRY_DB_PASSWORD; \
} | debconf-set-selections \
 && export host=$BLUECHERRY_DB_HOST \
 && dpkg -i --unpack ./bluecherry-apps/releases/bluecherry_*.deb \
 && sed -i 's|update-alternatives --install /run/php/php-fpm.sock php-fpm.sock .*$|true|' /var/lib/dpkg/info/bluecherry.postinst \
 && grep 'update-alternatives' /var/lib/dpkg/info/bluecherry.postinst || echo "Patch successful - line removed" \
 && dpkg --configure bluecherry


# Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /root/.my.cnf

# Setup users, dirs, and entrypoints
RUN sed -i '/imklog/s/^/#/' /etc/rsyslog.conf && \
    groupadd -g $BLUECHERRY_LINUX_GROUP_ID $BLUECHERRY_LINUX_GROUP_NAME || true && \
    useradd -m -u $BLUECHERRY_LINUX_USER_ID -g $BLUECHERRY_LINUX_GROUP_NAME -G audio,video -c "Bluecherry DVR" -d /var/lib/bluecherry $BLUECHERRY_LINUX_USER_NAME || true

RUN mkdir -p /recordings && chown bluecherry:bluecherry /recordings && chmod ug=rwx,o=rx /recordings

EXPOSE 7001/tcp 7002/tcp

COPY entrypoint.sh /entrypoint.sh
COPY bc-database-create.sh /bin/bc-database-create
COPY bc-database-upgrade.sh /bin/bc-database-upgrade
COPY bc-rsyslog.conf /etc/rsyslog.d/10-bluecherry.conf

RUN chmod +x /entrypoint.sh /bin/bc-database-create /bin/bc-database-upgrade

RUN rm -f /etc/nginx/sites-enabled/default && \
    chown -R bluecherry:bluecherry /var/lib/bluecherry

CMD ["/entrypoint.sh"]
