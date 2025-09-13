#!/bin/bash
set -euo pipefail

# -------------------------------
# 0) Helper: echo with prefix
# -------------------------------
log() { echo ">" "$@"; }

# -------------------------------
# 1) Write helper configs from env
# -------------------------------
log "Writing /root/.my.cnf"
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

log "Writing /etc/bluecherry.conf"
cat >/etc/bluecherry.conf <<EOF
# Bluecherry configuration file
version = "1.0";
bluecherry:
{
    db:
    {
        # 0 = sqlite, 1 = pgsql, 2 = mysql
        type = 2;
        dbname   = "${BLUECHERRY_DB_NAME:-bluecherry}";
        user     = "${BLUECHERRY_DB_USER:-bluecherry}";
        password = "${BLUECHERRY_DB_PASSWORD:-bluecherry}";
        host     = "${BLUECHERRY_DB_HOST:-127.0.0.1}";
        userhost = "${BLUECHERRY_DB_ACCESS_HOST:-%}";
    };
};
EOF

# Runtime dirs & recordings
log "Fixing permissions on /var/lib/bluecherry/recordings"
mkdir -p /var/lib/bluecherry/recordings /var/run/bluecherry
chown -R bluecherry:bluecherry /var/lib/bluecherry /var/run/bluecherry || true
chmod ug+rwx /var/lib/bluecherry/recordings || true

# Route app logs to STDOUT if needed
chmod 777 /proc/self/fd/1 || true

# -------------------------------
# 2) Container-safe rsyslog setup
# -------------------------------
# Precreate target log and relax perms so rsyslog can write without chown
mkdir -p /var/log
touch /var/log/bluecherry.log
chmod 666 /var/log/bluecherry.log || true

# Strip owner/group directives from shipped config to prevent chown errors
RSYS_CFG="/etc/rsyslog.d/10-bluecherry.conf"
if [ -f "$RSYS_CFG" ]; then
  sed -i -E \
    -e 's/^[[:space:]]*\$?FileOwner.*$//g' \
    -e 's/^[[:space:]]*\$?FileGroup.*$//g' \
    -e 's/(owner|group)=\"?[A-Za-z0-9_.-]+\"?//g' \
    "$RSYS_CFG" || true
fi

# Ensure logs also go to docker logs
cat >/etc/rsyslog.d/99-stdout.conf <<'EOF'
*.*  /proc/self/fd/1
EOF

# Start rsyslog in foreground (backgrounded by us) with a clean pidfile
log "Starting rsyslogd"
rm -f /run/rsyslogd.pid /var/run/rsyslogd.pid 2>/dev/null || true
/usr/sbin/rsyslogd -n &
RSYSLOG_PID=$!

# -------------------------------
# 3) PHP-FPM (auto-detect) + nginx
# -------------------------------
log "Starting php-fpm"
mkdir -p /run/php
start_php_fpm() {
  for v in 8.3 8.2 8.1 7.4; do
    if command -v "php-fpm${v}" >/dev/null 2>&1; then
      log ">> php-fpm${v} -F"
      "php-fpm${v}" -F &
      echo $! > /run/php/php-fpm.pid
      return 0
    fi
  done
  if command -v php-fpm >/dev/null 2>&1; then
    log ">> php-fpm -F"
    php-fpm -F &
    echo $! > /run/php/php-fpm.pid
    return 0
  fi
  echo "WARN: No php-fpm binary found; continuing."
  return 1
}
start_php_fpm || true

log "Starting nginx"
nginx -g 'daemon off;' &
NGINX_PID=$!

# -------------------------------
# 4) DB wait + create/upgrade
# -------------------------------
DB_HOST="${BLUECHERRY_DB_HOST:-127.0.0.1}"
DB_NAME="${BLUECHERRY_DB_NAME:-bluecherry}"
DB_USER="${BLUECHERRY_DB_USER:-bluecherry}"
DB_PASS="${BLUECHERRY_DB_PASSWORD:-bluecherry}"

if [ "${WAIT_FOR_DB:-1}" = "1" ]; then
  log "Waiting for MySQL at ${DB_HOST}..."
  for i in {1..60}; do
    if mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -e "SELECT 1" >/dev/null 2>&1; then
      log "DB is up."
      break
    fi
    sleep 2
  done
fi

log "Checking if DB '${DB_NAME}' exists…"
if mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -e "SHOW DATABASES LIKE '${DB_NAME}';" 2>/dev/null | grep -q "${DB_NAME}"; then
  log "DB exists → running upgrade"
  if command -v bc-database-upgrade >/dev/null 2>&1; then
    # may warn about PROCESS privilege if non-admin; that's OK
    bc-database-upgrade || true
  fi
else
  log "DB missing → running create"
  if command -v bc-database-create >/dev/null 2>&1; then
    bc-database-create || true
  fi
fi

# -------------------------------
# 5) Start Bluecherry server
# -------------------------------
log "Starting bc-server as bluecherry:bluecherry"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
/usr/sbin/bc-server -u bluecherry -g bluecherry &
BC_PID=$!

# -------------------------------
# 6) Watchdog loop
# -------------------------------
log "All services launched. Entering watchdog loop."
while sleep 15; do
  # rsyslogd must be running
  if ! kill -0 "${RSYSLOG_PID}" 2>/dev/null; then
    echo "rsyslogd exited"
    exit 1
  fi
  # nginx must be running (check master process name)
  if ! pgrep -x nginx >/dev/null; then
    echo "nginx exited"
    exit 1
  fi
  # bc-server must be running
  if ! kill -0 "${BC_PID}" 2>/dev/null; then
    echo "bc-server exited"
    exit 1
  fi
done
