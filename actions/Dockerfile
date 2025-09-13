# actions/Dockerfile
FROM ubuntu:24.04 AS base
ENV DEBIAN_FRONTEND=noninteractive

# Minimal runtime + tools needed to repack a .deb cleanly
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl wget gnupg openssl \
    rsyslog iproute2 sysstat nano cron \
    mysql-client \
    xz-utils dpkg-dev \
 && rm -rf /var/lib/apt/lists/*

FROM base AS app
WORKDIR /root

# ---- Build-time args (set from CI) ----
ARG INSTALL_METHOD=deb
ARG BLUECHERRY_DEB_URL=""
ARG BLUECHERRY_DEB_SHA256=""

# ---- DB defaults (for preseeding only; real DB comes from runtime env) ----
ARG MYSQL_ADMIN_LOGIN=root
ARG MYSQL_ADMIN_PASSWORD=root
ARG BLUECHERRY_DB_USER=bluecherry
ARG BLUECHERRY_DB_HOST=172.17.0.1
ARG BLUECHERRY_DB_PASSWORD=bluecherry
ARG BLUECHERRY_DB_NAME=bluecherry
ARG BLUECHERRY_DB_ACCESS_HOST=%

# ---- Runtime user ----
ARG BLUECHERRY_LINUX_GROUP_NAME=bluecherry
ARG BLUECHERRY_LINUX_GROUP_ID=1000
ARG BLUECHERRY_LINUX_USER_NAME=bluecherry
ARG BLUECHERRY_LINUX_USER_ID=1000

# ---- Bring in helper scripts ----
COPY entrypoint.sh /entrypoint.sh
COPY bc-database-create.sh /bin/bc-database-create
COPY bc-database-upgrade.sh /bin/bc-database-upgrade
COPY bc-rsyslog.conf /etc/rsyslog.d/10-bluecherry.conf

# Helper MySQL rc file for installer stage; removed later
RUN printf "[client]\nuser=%s\npassword=%s\n[mysqldump]\nuser=%s\npassword=%s\n" \
  "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" > /root/.my.cnf

# Preseed debconf answers
RUN { \
  echo bluecherry bluecherry/mysql_admin_login string $MYSQL_ADMIN_LOGIN; \
  echo bluecherry bluecherry/mysql_admin_password password $MYSQL_ADMIN_PASSWORD; \
  echo bluecherry bluecherry/db_host string $BLUECHERRY_DB_HOST; \
  echo bluecherry bluecherry/db_userhost string $BLUECHERRY_DB_ACCESS_HOST; \
  echo bluecherry bluecherry/db_name string $BLUECHERRY_DB_NAME; \
  echo bluecherry bluecherry/db_user string $BLUECHERRY_DB_USER; \
  echo bluecherry bluecherry/db_password password $BLUECHERRY_DB_PASSWORD; \
} | debconf-set-selections

# Fail if INSTALL_METHOD=deb but no URL provided
RUN if [ "$INSTALL_METHOD" = "deb" ] && [ -z "$BLUECHERRY_DEB_URL" ]; then \
      echo "ERROR: INSTALL_METHOD=deb but BLUECHERRY_DEB_URL is empty"; exit 2; \
    fi

# Fetch & patch .deb
RUN if [ "$INSTALL_METHOD" = "deb" ] && [ -n "$BLUECHERRY_DEB_URL" ]; then \
      set -eux; \
      echo "Fetching .deb from: $BLUECHERRY_DEB_URL"; \
      mkdir -p /root/releases; \
      curl -fsSL "$BLUECHERRY_DEB_URL" -o /root/releases/bluecherry.deb; \
      if [ -n "$BLUECHERRY_DEB_SHA256" ]; then \
        echo "$BLUECHERRY_DEB_SHA256  /root/releases/bluecherry.deb" | sha256sum -c -; \
      fi; \
      mkdir -p /tmp/deb-patch && cd /tmp/deb-patch; \
      dpkg-deb -x /root/releases/bluecherry.deb .; \
      dpkg-deb -e /root/releases/bluecherry.deb DEBIAN/; \
      for s in postinst preinst prerm postrm; do \
        if [ -f "DEBIAN/$s" ]; then \
          printf '#!/bin/sh\nexit 0\n' > "DEBIAN/$s"; \
          chmod 0755 "DEBIAN/$s"; \
        fi; \
      done; \
      dpkg-deb -b . /root/releases/bluecherry.deb; \
      cd /root && rm -rf /tmp/deb-patch; \
    fi

# Disable kernel logging in rsyslog
RUN sed -i '/imklog/s/^/#/' /etc/rsyslog.conf || true

# Required dirs
RUN set -eux; \
    install -d -m 0750 -o root -g root /etc/sudoers.d; \
    printf '#!/bin/sh\nexit 101\n' > /usr/sbin/policy-rc.d; chmod +x /usr/sbin/policy-rc.d

# Install the patched package
RUN set -eux; \
    DEB_PATH="/root/releases/bluecherry.deb"; \
    test -f "$DEB_PATH"; \
    apt-get update; \
    apt-get install -y --no-install-recommends "$DEB_PATH"; \
    apt-get clean; rm -rf /var/lib/apt/lists/*

# Runtime user & recordings dir
RUN set -eux; \
    if ! getent group "$BLUECHERRY_LINUX_GROUP_NAME" >/dev/null; then \
      groupadd -f "$BLUECHERRY_LINUX_GROUP_NAME"; \
    fi; \
    if ! id -u "$BLUECHERRY_LINUX_USER_NAME" >/dev/null 2>&1; then \
      useradd -m -d /var/lib/bluecherry -g "$BLUECHERRY_LINUX_GROUP_NAME" \
        -G audio,video -s /bin/bash "$BLUECHERRY_LINUX_USER_NAME"; \
    fi; \
    install -d -m 0775 /recordings; \
    chown "$BLUECHERRY_LINUX_USER_NAME":"$BLUECHERRY_LINUX_GROUP_NAME" /recordings || true

# Cleanup installer-time secrets
RUN rm -f /root/.my.cnf /etc/bluecherry.conf || true

RUN chmod +x /entrypoint.sh /bin/bc-database-create /bin/bc-database-upgrade
EXPOSE 7001/tcp 7002/tcp

# ✅ Updated healthcheck
HEALTHCHECK --interval=30s --timeout=5s --start-period=60s --retries=10 \
  CMD pgrep -f '/usr/sbin/bc-server' >/dev/null || exit 1

CMD ["/entrypoint.sh"]
