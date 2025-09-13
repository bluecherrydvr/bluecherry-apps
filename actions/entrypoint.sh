#!/bin/bash
set -euo pipefail

# -------------------------------
# 1) Write helper configs from env
# -------------------------------
echo "> Writing /root/.my.cnf"
cat >/root/.my.cnf <<EOF
[client]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
[mysql]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
[mysqldump]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
[mysqldiff]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
EOF

echo "> Writing /etc/bluecherry.conf"
cat >/etc/bluecherry.conf <<EOF
# Bluecherry configuration file
# Used to be sure we don't use configurations not suitable for us
version = "1.0";
bluecherry:
{
    db:
    {
        # 0 = sqlite, 1 = pgsql, 2 = mysql
        type = 2;
        dbname = "${BLUECHERRY_DB_NAME:-bluecherry}";
        user = "${BLUECHERRY_DB_USER:-bluecherry}";
        password = "${BLUECHERRY_DB_PASSWORD:-bluecherry}";
        host = "${BLUECHERRY_DB_HOST:-127.0.0.1}";
        userhost = "${BLUECHERRY_DB_ACCESS_HOST:-%}";
    };
};
EOF

echo "> Fixing permissions on /var/lib/bluecherry/recordings"
mkdir -p /var/lib/bluecherry/recordings
chown bluecherry:bluecherry /var/lib/bluecherry/recordings || true
chmod ug+rwx /var/lib/bluecherry/recordings || true

# Route app logs to STDOUT (as your rsyslog config expects)
chmod 777 /proc/self/fd/1 || true

# ------------------------------------
# 2) Start services in container style
# ------------------------------------
echo "> Starting rsyslogd"
# Run in foreground but background the process (so we can keep a watch loop)
# -n = no fork
/usr/sbin/rsyslogd -n &
RSYSLOG_PID=$!

# PHP-FPM (auto-detect version; Ubuntu 24.04 ships 8.3)
echo "> Starting php-fpm"
mkdir -p /run/php
start_php_fpm() {
  for v in 8.3 8.2 8.1 7.4; do
    if command -v "php-fpm${v}" >/dev/null 2>&1; then
      echo ">> php-fpm${v} -F"
      "php-fpm${v}" -F &
      echo $! > /run/php/php-fpm.pid
      return 0
    fi
  done
  if command -v php-fpm >/dev/null 2>&1; then
    echo ">> php-fpm -F"
    php-fpm -F &
    echo $! > /run/php/php-fpm.pid
    return 0
  fi
  echo "WARN: No php-fpm binary found; continuing without it."
  return 1
}
start_php_fpm || true

# NGINX (we use nginx only; no apache)
/usr/sbin/nginx -g 'daemon off;' &
NGINX_PID=$!

# ------------------------------------------------
# 3) (Optional) Wait for DB and run schema actions
# ------------------------------------------------
if [ "${WAIT_FOR_DB:-1}" = "1" ]; then
  echo "> Waiting for MySQL at ${BLUECHERRY_DB_HOST:-127.0.0.1}..."
  for i in {1..60}; do
    if mysql -h"${BLUECHERRY_DB_HOST:-127.0.0.1}" -u"${BLUECHERRY_DB_USER:-bluecherry}" -p"${BLUECHERRY_DB_PASSWORD:-bluecherry}" -e "SELECT 1" >/dev/null 2>&1; then
      echo "> DB is up."
      break
    fi
    sleep 2
  done
fi

# Run create/upgrade if available; ignore errors if already initialized
if command -v bc-database-create >/dev/null 2>&1; then
  echo "> bc-database-create (ignore errors if already created)"
  bc-database-create || true
fi
if command -v bc-database-upgrade >/dev/null 2>&1; then
  echo "> bc-database-upgrade (ignore errors if already up-to-date)"
  bc-database-upgrade || true
fi

# --------------------------------
# 4) Start Bluecherry server
# --------------------------------
echo "> Starting bc-server as bluecherry:bluecherry"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
/usr/sbin/bc-server -u bluecherry -g bluecherry &
BC_PID=$!

# --------------------------------
# 5) Lightweight process watchdog
# --------------------------------
echo "> All services launched. Entering watchdog loop."
while sleep 15; do
  kill -0 "${RSYSLOG_PID}" 2>/dev/null || { echo "rsyslogd exited"; exit 1; }
  # nginx may respawn masters; check by name rather than fixed PID
  pgrep -x nginx >/dev/null || { echo "nginx exited"; exit 1; }
  kill -0 "${BC_PID}" 2>/dev/null || { echo "bc-server exited"; exit 1; }
done
