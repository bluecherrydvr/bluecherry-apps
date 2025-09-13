# Dockerfile
FROM ubuntu:24.04 AS base
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl wget gnupg openssl rsyslog iproute2 sysstat nano cron sudo mysql-client \
    && rm -rf /var/lib/apt/lists/*

FROM base AS app
WORKDIR /root

# Build-time args
ARG INSTALL_METHOD=deb
ARG BLUECHERRY_DEB_URL=""
ARG BLUECHERRY_DEB_SHA256=""

# DB defaults (overridden at runtime)
ARG MYSQL_ADMIN_LOGIN=root
ARG MYSQL_ADMIN_PASSWORD=root
ARG BLUECHERRY_DB_USER=bluecherry
ARG BLUECHERRY_DB_HOST=172.17.0.1
ARG BLUECHERRY_DB_PASSWORD=bluecherry
ARG BLUECHERRY_DB_NAME=bluecherry
ARG BLUECHERRY_DB_ACCESS_HOST=%

# Linux user for runtime
ARG BLUECHERRY_LINUX_GROUP_NAME=bluecherry
ARG BLUECHERRY_LINUX_GROUP_ID=1000
ARG BLUECHERRY_LINUX_USER_NAME=bluecherry
ARG BLUECHERRY_LINUX_USER_ID=1000

# Scripts we already have
COPY entrypoint.sh /entrypoint.sh
COPY bc-database-create.sh /bin/bc-database-create
COPY bc-database-upgrade.sh /bin/bc-database-upgrade
COPY bc-rsyslog.conf /etc/rsyslog.d/10-bluecherry.conf

# Helper MySQL rc file for installer stage; removed later
RUN printf "[client]\nuser=%s\npassword=%s\n[mysqldump]\nuser=%s\npassword=%s\n" \
  "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" > /root/.my.cnf

# Preseed debconf answers for non-interactive install
RUN { \
  echo bluecherry bluecherry/mysql_admin_login string $MYSQL_ADMIN_LOGIN; \
  echo bluecherry bluecherry/mysql_admin_password password $MYSQL_ADMIN_PASSWORD; \
  echo bluecherry bluecherry/db_host string $BLUECHERRY_DB_HOST; \
  echo bluecherry bluecherry/db_userhost string $BLUECHERRY_DB_ACCESS_HOST; \
  echo bluecherry bluecherry/db_name string $BLUECHERRY_DB_NAME; \
  echo bluecherry bluecherry/db_user string $BLUECHERRY_DB_USER; \
  echo bluecherry bluecherry/db_password password $BLUECHERRY_DB_PASSWORD; \
} | debconf-set-selections

# Download .deb when using deb method, verify checksum if provided
RUN if [ "$INSTALL_METHOD" = "deb" ] && [ -n "$BLUECHERRY_DEB_URL" ]; then \
      mkdir -p /root/releases && \
      curl -fsSL "$BLUECHERRY_DEB_URL" -o /root/releases/bluecherry.deb; \
      if [ -n "$BLUECHERRY_DEB_SHA256" ]; then \
        echo "$BLUECHERRY_DEB_SHA256  /root/releases/bluecherry.deb" | sha256sum -c -; \
      fi; \
      # Patch postinst to skip DB creation during Docker build \
      mkdir /tmp/deb-patch && cd /tmp/deb-patch && \
      dpkg-deb -x /root/releases/bluecherry.deb . && \
      dpkg-deb -e /root/releases/bluecherry.deb DEBIAN/ && \
      sed -i '/bc_db_tool.sh new_db/ s/^/# Docker build skip: /' ./usr/share/bluecherry/postinstall.sh || true && \
      dpkg-deb -b . /root/releases/bluecherry.deb && \
      cd .. && rm -rf /tmp/deb-patch; \
    fi

# Disable imklog in rsyslog (no kernel log access inside containers)
RUN sed -i '/imklog/s/^/#/' /etc/rsyslog.conf || true

# Some postinsts expect $host; export for install step
ENV host=$BLUECHERRY_DB_HOST

# Install the deb (downloaded above)
RUN set -eux; \
    DEB_PATH="/root/releases/bluecherry.deb"; \
    test -f "$DEB_PATH"; \
    # Avoid php-fpm alternatives issues if present:
    sed -i 's|update-alternatives --install /run/php/php-fpm.sock php-fpm.sock .*|true|' /usr/share/bluecherry/postinst || true; \
    apt-get update; \
    apt-get install -y --no-install-recommends "$DEB_PATH"; \
    apt-get clean; rm -rf /var/lib/apt/lists/*

# Create runtime user & recordings dir
RUN groupadd -g $BLUECHERRY_LINUX_GROUP_ID -f $BLUECHERRY_LINUX_GROUP_NAME && \
    useradd -m -d /var/lib/bluecherry -u $BLUECHERRY_LINUX_USER_ID -g $BLUECHERRY_LINUX_GROUP_NAME \
      -G audio,video -s /bin/bash $BLUECHERRY_LINUX_USER_NAME || true && \
    mkdir -p /recordings && chown bluecherry:bluecherry /recordings && chmod 775 /recordings

# Cleanup secrets
RUN rm -f /root/.my.cnf /etc/bluecherry.conf || true

RUN chmod +x /entrypoint.sh /bin/bc-database-create /bin/bc-database-upgrade

EXPOSE 7001/tcp 7002/tcp

HEALTHCHECK --interval=30s --timeout=5s --retries=10 CMD \
  bash -lc 'pgrep -x bc-server >/dev/null || (journalctl -u bluecherry --no-pager | tail -n 50; exit 1)'

CMD ["/entrypoint.sh"]
