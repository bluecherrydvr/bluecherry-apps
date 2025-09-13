#!/bin/bash
set -euo pipefail

log(){ echo ">$SHELL_MSG $*"; }

# -------------------------------
# 0) Resolve env with defaults
# -------------------------------
MYSQL_ADMIN_LOGIN="${MYSQL_ADMIN_LOGIN:-root}"
MYSQL_ADMIN_PASSWORD="${MYSQL_ADMIN_PASSWORD:-root}"

DB_HOST="${BLUECHERRY_DB_HOST:-127.0.0.1}"
DB_NAME="${BLUECHERRY_DB_NAME:-bluecherry}"
DB_USER="${BLUECHERRY_DB_USER:-bluecherry}"
DB_PASS="${BLUECHERRY_DB_PASSWORD:-bluecherry}"
DB_USERHOST="${BLUECHERRY_DB_ACCESS_HOST:-%}"

# -------------------------------
# 1) Write helper configs from env
# -------------------------------
log "Writing /root/.my.cnf"
cat >/root/.my.cnf <<EOF
[client]
user=${MYSQL_ADMIN_LOGIN}
password=${MYSQL_ADMIN_PASSWORD}
[mysql]
user=${MYSQL_ADMIN_LOGIN}
password=${MYSQL_ADMIN_PASSWORD}
[mysqldump]
user=${MYSQL_ADMIN_LOGIN}
password=${MYSQL_ADMIN_PASSWORD}
[mysqldiff]
user=${MYSQL_ADMIN_LOGIN}
password=${MYSQL_ADMIN_PASSWORD}
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
        dbname   = "${DB_NAME}";
        user     = "${DB_USER}";
        password = "${DB_PASS}";
        host     = "${DB_HOST}";
        userhost = "${DB_USERHOST}";
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

# Replace deprecated '~' discard action with 'stop' (quiet warning)
sed -i -E 's/^[[:space:]]*~[[:space:]]*$/stop/' /etc/rsyslog.d/*.conf 2>/dev/null || true

# Ensure logs also go to docker logs
cat >/etc/rsyslog.d/99-stdout.conf <<'EOF'
*.*  /proc/self/fd/1
EOF

# Start rsyslog with a clean pidfile
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
  echo "WARN: No php-fpm binary found; continuing."
  return 1
}
start_php_fpm || true

# Create a compatibility symlink for configs expecting /etc/alternatives/php-fpm.sock
for s in /run/php/php8.3-fpm.sock /run/php/php-fpm.sock; do
  if [ -S "$s" ]; then
    mkdir -p /etc/alternatives
    ln -sf "$s" /etc/alternatives/php-fpm.sock
    echo "Linked $s -> /etc/alternatives/php-fpm.sock"
    break
  fi
done

log "Starting nginx"
nginx -g 'daemon off;' &
NGINX_PID=$!

# -------------------------------
# 4) DB wait + create/upgrade
# -------------------------------
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
    bc-database-upgrade || true
  fi
else
  log "DB missing → running create"
  if command -v bc-database-create >/dev/null 2>&1; then
    bc-database-create || true
  fi
fi

# -----------------------------------------
# 4.1) DB grants for MySQL 8 (quiet backup)
# -----------------------------------------
# If admin creds are available, ensure the app user has backup-friendly perms
# and uses mysql_native_password for compatibility, using your env values.
if [ -n "${MYSQL_ADMIN_LOGIN:-}" ] && [ -n "${MYSQL_ADMIN_PASSWORD:-}" ]; then
  log "Ensuring '${DB_USER}' has PROCESS/SHOW VIEW/EVENT/TRIGGER/LOCK TABLES and native auth"
  mysql -h"${DB_HOST}" -u"${MYSQL_ADMIN_LOGIN}" -p"${MYSQL_ADMIN_PASSWORD}" <<SQL || true
GRANT PROCESS, SHOW VIEW, EVENT, TRIGGER, LOCK TABLES ON *.* TO '${DB_USER}'@'${DB_USERHOST}' IDENTIFIED BY '${DB_PASS}';
FLUSH PRIVILEGES;
ALTER USER '${DB_USER}'@'${DB_USERHOST}' IDENTIFIED WITH mysql_native_password BY '${DB_PASS}';
FLUSH PRIVILEGES;
SQL
fi

# -----------------------------------------
# 4.2) Optional: motion_map one-time fixes
# -----------------------------------------
#if mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -D"${DB_NAME}" -e "SHOW TABLES LIKE 'Devices'" | grep -q Devices; then
#  echo "Fixing motion maps..."
#  mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -D"${DB_NAME}" <<'SQL' || true
#UPDATE Devices SET motion_map = REPEAT('3', 768) WHERE protocol LIKE 'IP%' AND LENGTH(motion_map) <> 768;
#UPDATE Devices SET motion_map = REPEAT('3', 192) WHERE driver = 'tw5864' AND LENGTH(motion_map) <> 192;
#UPDATE Devices SET motion_map = REPEAT('3', 396) WHERE driver LIKE 'solo6%' AND LENGTH(motion_map) <> 396;
#UPDATE Devices SET motion_map = REPEAT('3', 330) WHERE driver LIKE 'solo6%' AND LENGTH(motion_map) <> 330;
#SQL
#fi

# -------------------------------
# 5) DEBUG mode (optional)
# -------------------------------
if [ "${DEBUG:-0}" = "1" ]; then
  export LD_LIBRARY_PATH=/usr/lib/bluecherry
  exec /usr/sbin/bc-server -u bluecherry -g bluecherry -d 7
fi

# -------------------------------
# 6) Start Bluecherry server
# -------------------------------
log "Starting bc-server as bluecherry:bluecherry"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
/usr/sbin/bc-server -u bluecherry -g bluecherry &
BC_PID=$!

# -------------------------------
# 7) Graceful shutdown trap
# -------------------------------
graceful_exit() {
  echo "> Caught signal, stopping services..."
  kill -TERM "${BC_PID:-0}" 2>/dev/null || true
  pkill -TERM -x nginx 2>/dev/null || true
  if [ -f /run/php/php-fpm.pid ]; then
    kill -TERM "$(cat /run/php/php-fpm.pid)" 2>/dev/null || true
  else
    pkill -TERM -f 'php-fpm' 2>/dev/null || true
  fi
  kill -TERM "${RSYSLOG_PID:-0}" 2>/dev/null || true
  wait
  exit 0
}
trap graceful_exit TERM INT

# -------------------------------
# 8) Watchdog loop
# -------------------------------
log "All services launched. Entering watchdog loop."
while sleep 15; do
  kill -0 "${RSYSLOG_PID}" 2>/dev/null || { echo "rsyslogd exited"; exit 1; }
  pgrep -x nginx >/dev/null || { echo "nginx exited"; exit 1; }
  kill -0 "${BC_PID}" 2>/dev/null || { echo "bc-server exited"; exit 1; }
done
