# Dockerfile
FROM ubuntu:24.04 AS base
ENV DEBIAN_FRONTEND=noninteractive
# Keep runtime tools minimal; include dpkg tooling for .deb repack
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl wget gnupg openssl \
    rsyslog iproute2 sysstat nano cron \
    mysql-client \
    xz-utils dpkg-dev \
 && rm -rf /var/lib/apt/lists/*

FROM base AS app
WORKDIR /root

# ---- Build-time args ----
ARG INSTALL_METHOD=deb
ARG BLUECHERRY_DEB_URL=""
ARG BLUECHERRY_DEB_SHA256=""

# ---- DB defaults (used only to preseed package; runtime uses env/compose) ----
ARG MYSQL_ADMIN_LOGIN=root
ARG MYSQL_ADMIN_PASSWORD=root
ARG BLUECHERRY_DB_USER=bluecherry
ARG BLUECHERRY_DB_HOST=172.17.0.1
ARG BLUECHERRY_DB_PASSWORD=bluecherry
ARG BLUECHERRY_DB_NAME=bluecherry
ARG BLUECHERRY_DB_ACCESS_HOST=%

# ---- Linux user for runtime ----
ARG BLUECHERRY_LINUX_GROUP_NAME=bluecherry
ARG BLUECHERRY_LINUX_GROUP_ID=1000
ARG BLUECHERRY_LINUX_USER_NAME=bluecherry
ARG BLUECHERRY_LINUX_USER_ID=1000

# ---- Bring in your helper scripts ----
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

# ---- Fetch and (optionally) verify the .deb, then patch to skip DB actions at build time ----
RUN if [ "$INSTALL_METHOD" = "deb" ] && [ -n "$BLUECHERRY_DEB_URL" ]; then \
      set -eux; \
      mkdir -p /root/releases; \
      curl -fsSL "$BLUECHERRY_DEB_URL" -o /root/releases/bluecherry.deb; \
      if [ -n "$BLUECHERRY_DEB_SHA256" ]; then \
        echo "$BLUECHERRY_DEB_SHA256  /root/releases/bluecherry.deb" | sha256sum -c -; \
      fi; \
      # Repack the .deb with a patched postinstall to avoid DB creation at build
      mkdir -p /tmp/deb-patch && cd /tmp/deb-patch; \
      dpkg-deb -x /root/releases/bluecherry.deb .; \
      dpkg-deb -e /root/releases/bluecherry.deb DEBIAN/; \
      # Comment out the DB creation/init line if present
      if [ -f ./usr/share/bluecherry/postinstall.sh ]; then \
        sed -i '/bc_db_tool.sh[[:space:]]\+new_db/ s/^/# Docker build skip: /' ./usr/share/bluecherry/postinstall.sh || true; \
      fi; \
      dpkg-deb -b . /root/releases/bluecherry.deb; \
      cd /root && rm -rf /tmp/deb-patch; \
    fi

# Disable imklog in rsyslog (no kernel log access inside containers)
RUN sed -i '/imklog/s/^/#/' /etc/rsyslog.conf || true

# Some installers read $host for DB connectivity checks; set benign default
ENV host=$BLUECHERRY_DB_HOST

# ---- Install the (patched) deb into the image ----
RUN set -eux; \
    DEB_PATH="/root/releases/bluecherry.deb"; \
    test -f "$DEB_PATH"; \
    # Avoid php-fpm alternatives issue if present in package scripts
    sed -i 's|update-alternatives --install /run/php/php-fpm.sock php-fpm.sock .*|true|' /usr/share/bluecherry/postinst || true; \
    apt-get update; \
    apt-get install -y --no-install-recommends "$DEB_PATH"; \
    apt-get clean; rm -rf /var/lib/apt/lists/*

# ---- Runtime user & recordings dir ----
RUN groupadd -g $BLUECHERRY_LINUX_GROUP_ID -f $BLUECHERRY_LINUX_GROUP_NAME && \
    useradd -m -d /var/lib/bluecherry -u $BLUECHERRY_LINUX_USER_ID -g $BLUECHERRY_LINUX_GROUP_NAME \
      -G audio,video -s /bin/bash $BLUECHERRY_LINUX_USER_NAME || true && \
    mkdir -p /recordings && chown bluecherry:bluecherry /recordings && chmod 775 /recordings

# ---- Cleanup installer-time secrets ----
RUN rm -f /root/.my.cnf /etc/bluecherry.conf || true

# ---- Permissions & ports ----
RUN chmod +x /entrypoint.sh /bin/bc-database-create /bin/bc-database-upgrade
EXPOSE 7001/tcp 7002/tcp

# Simpler healthcheck: confirm server process is alive
HEALTHCHECK --interval=30s --timeout=5s --retries=10 CMD pgrep -x bc-server >/dev/null || exit 1

CMD ["/entrypoint.sh"]
