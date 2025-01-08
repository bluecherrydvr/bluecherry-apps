# set a base image with environment to build from
FROM ubuntu:20.04 AS baseos

ARG BLUECHERRY_GIT_BRANCH_TAG=master

# ---------------------------------------------------------------------------
# Build the base OS with some development libs and tools
FROM baseos AS os_dev_environment
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

RUN apt-get update

RUN apt-get install --no-install-recommends -y \
            git sudo openssl ca-certificates wget gnupg gnupg2 gnupg1 \
            ssl-cert nmap curl sysstat iproute2 \
            autoconf automake libtool build-essential gcc g++ libcurl4-openssl-dev \
            debhelper ccache bison flex texinfo yasm cmake

RUN apt-get install --no-install-recommends -y \
            libbsd-dev libopencv-dev libudev-dev libva-dev \
            linux-image-generic linux-headers-generic \
            libmysqlclient-dev rsyslog

ENV PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig
RUN ls -ltrc /usr/include/x86_64-linux-gnu/curl/curl.h            
            
# ---------------------------------------------------------------------------
FROM os_dev_environment as bluecherry_base_environment
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

# RUN git clone --recurse-submodules --progress --depth 1 \
 RUN git clone --recurse-submodules --progress http://github.com/bluecherrydvr/bluecherry-apps.git \
 && cd bluecherry-apps \
 && git checkout $BLUECHERRY_GIT_BRANCH_TAG \
 && sed -i '/#define PRODUCT_VERSION/s/"$/-docker"/' server/v3license_processor.h


 
RUN apt-get --no-install-recommends -y install \
        libbsd0 libc6 libgcc1 libssl1.1 libstdc++6 libudev1 \
        zlib1g ucf mkvtoolnix v4l-utils vainfo i965-va-driver

RUN apt-get --no-install-recommends -y install \
        php-mail php-mail-mime php-net-smtp php-gd php-curl \
        php-mysql php-sqlite3 \
        apache2 libapache2-mod-php mysql-client sqlite3

# ---------------------------------------------------------------------------
# Build the bluecherry app and dependencies. This is done in a separate
# image because there are many ways it can fail and then we save time
# by being able to reuse prior containers leading up to this build.
FROM bluecherry_base_environment as bluecherry_build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

COPY depends/onvif_tool bluecherry-apps/utils/onvif_tool

RUN cd bluecherry-apps \
 && ./scripts/build_pkg_native.sh


# ---------------------------------------------------------------------------
FROM bluecherry_build as bluecherry_build_cleaned
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

RUN rm -rf /usr/src/linux-headers-*

RUN rm -rf .ccache \
 && rm -rf bluecherry-apps/.git \
 && rm -rf bluecherry-apps/misc/libav \
 && rm -rf bluecherry-apps/misc/libconfig \
 && rm -rf bluecherry-apps/misc/pugixml


# ---------------------------------------------------------------------------
# Install the bluecherry app and dependencies
FROM baseos as bluecherry_install
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root

COPY --from=bluecherry_build_cleaned \
    /root/bluecherry-apps/releases/bluecherry_*.deb \
    /root/bluecherry-apps/releases/ 

RUN ls -l /root/bluecherry-apps/releases/

# This step is needed if/when building a new bluecherry docker container
# that will connect to an existing bluecherry database. In this case, the
# bluecherry installer will see the existing database, and it needs the
# /etc/bluecherry.conf file to tell it that it is okay to connect to (and
# modify) that database
#
#COPY bluecherry.conf /etc/bluecherry.conf

ARG MYSQL_ADMIN_LOGIN=root
ARG MYSQL_ADMIN_PASSWORD=root

# Specific database credentials used by bluecherry server
ARG BLUECHERRY_DB_USER=bluecherry
ARG BLUECHERRY_DB_HOST=172.17.0.1
ARG BLUECHERRY_DB_PASSWORD=bluecherry
ARG BLUECHERRY_DB_NAME=bluecherry
ARG BLUECHERRY_DB_ACCESS_HOST=%

# User and Group info used for running bluecherry server processes
ARG BLUECHERRY_LINUX_GROUP_NAME=bluecherry
ARG BLUECHERRY_LINUX_GROUP_ID=1000
ARG BLUECHERRY_LINUX_USER_NAME=bluecherry
ARG BLUECHERRY_LINUX_USER_ID=1000

RUN apt-get update \
 && apt-get install -y \
        rsyslog nmap curl sysstat iproute2 \
        openssl ca-certificates ssl-cert gnupg gnupg2 gnupg1 sudo mysql-client python3-pip wget curl nano cron

# Allow service commands during installation
RUN echo "#!/bin/sh\nexit 0" > /usr/sbin/policy-rc.d && chmod +x /usr/sbin/policy-rc.d

# Ensure noninteractive mode
ENV DEBIAN_FRONTEND=noninteractive

RUN { \
        echo "[client]";                        \
        echo "user=$MYSQL_ADMIN_LOGIN";         \
        echo "password=$MYSQL_ADMIN_PASSWORD";  \
        echo "[mysql]";                         \
        echo "user=$MYSQL_ADMIN_LOGIN";         \
        echo "password=$MYSQL_ADMIN_PASSWORD";  \
        echo "[mysqldump]";                     \
        echo "user=$MYSQL_ADMIN_LOGIN";         \
        echo "password=$MYSQL_ADMIN_PASSWORD";  \
        echo "[mysqldiff]";                     \
        echo "user=$MYSQL_ADMIN_LOGIN";         \
        echo "password=$MYSQL_ADMIN_PASSWORD";  \
    } > /root/.my.cnf

# NOTE: The line "export host=$BLUECHERRY_DB_HOST" ... This is required 
# due to a weird global check of this env var by the "check_mysql_admin"
# function in /usr/share/bluecherry/bc_db_tool.sh ... which doesn't accept
# the db host as an argument like most of the other functions in that file.
# --- The Specific problem line is:
# if ! echo "show databases" | mysql_wrapper -h"${host}" -u"$MYSQL_ADMIN_LOGIN" &>/dev/null
#
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
 && apt install -y --no-install-recommends ./bluecherry-apps/releases/bluecherry_*.deb

# Cleanup tasks
RUN apt-get clean \
# && rm -f bluecherry-apps/releases/bluecherry_*.deb \
 && rm -rf /var/lib/apt/lists/*

# Remove these files -- we needed them to build the docker image, since the 
# bluecherry installer scripts interact with the database. However, once the
# image is created, we expect it to receive all of the settings/credentials
# from environment variables passed in by docker or docker-compose.
RUN rm -f /root/.my.cnf \
 && rm -f /etc/bluecherry.conf

RUN rm /usr/sbin/policy-rc.d


# When running rsyslog in a container, we need to disable imklog
# since the in-container process won't be allowed access to it.
RUN sed -i '/imklog/s/^/#/' /etc/rsyslog.conf

RUN /usr/sbin/groupadd -rf \
    --gid=$BLUECHERRY_LINUX_GROUP_ID \
    $BLUECHERRY_LINUX_GROUP_NAME \
 && /usr/sbin/useradd -rm \
    --comment "Bluecherry DVR" \
    --home-dir=/var/lib/bluecherry \
    --gid=$BLUECHERRY_LINUX_GROUP_NAME \
    --groups=audio,video \
    --uid=$BLUECHERRY_LINUX_USER_ID \
    $BLUECHERRY_LINUX_USER_NAME \
 || echo "bluecherry user already exists"

RUN mkdir /recordings \
 && chown bluecherry:bluecherry /recordings \
 && chmod ug=rwx,o=rx /recordings

EXPOSE 7001/tcp 7002/tcp 

# This is the main script that runs as process ID 1 in the docker container
COPY entrypoint.sh /entrypoint.sh

# These scripts are wrappers used to manage the bluecherry database. They are
# necessary because the bluecherry installer usually sets up the database, but
# with a pre-built docker image the installer isn't run (so these actions have
# to be done manually as needed from the docker container... example usage 
# from the docker host looks like:
#
# --- CREATE:    sudo docker-compose run bluecherry bc-database-create
# --- UPGRADE:   sudo docker-compose run bluecherry bc-database-upgrade
COPY bc-database-create.sh /bin/bc-database-create
COPY bc-database-upgrade.sh /bin/bc-database-upgrade

# This copies in a modified rsyslog config, which tells rsyslog to route
# bluecherry logs to both /var/log/bluecherry.log (within the container) and
# also to the STDOUT of container process with PID 1, which then allows the
# logs to be received by the docker engine (and read via `docker logs` , etc.)
COPY bc-rsyslog.conf /etc/rsyslog.d/10-bluecherry.conf

# Make the previously copied scripts executable
RUN chmod +x /entrypoint.sh \
 && chmod +x /bin/bc-database-create \
 && chmod +x /bin/bc-database-upgrade

# Delete the default nginx config, we don't need it.
RUN rm /etc/nginx/sites-enabled/default

RUN chown bluecherry.bluecherry -R /var/lib/bluecherry
#CMD rm -f /var/run/rsyslogd.pid
#CMD ["/usr/sbin/rsyslogd", "-n", "-f", "/etc/rsyslog.conf"]
#CMD service rsyslog start
CMD /usr/sbin/php-fpm7.4 -D
CMD ["/usr/sbin/nginx", "-g", "daemon off;"]
CMD "/entrypoint.sh"
