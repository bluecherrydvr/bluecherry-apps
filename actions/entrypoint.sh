# Logging (make sure file exists and is writable)
mkdir -p /var/log
touch /var/log/bluecherry.log
chmod 666 /var/log/bluecherry.log || true

# Runtime dirs for bluecherry
mkdir -p /var/run/bluecherry /var/lib/bluecherry/recordings
chown -R bluecherry:bluecherry /var/run/bluecherry /var/lib/bluecherry || true
chmod ug+rwx /var/lib/bluecherry/recordings || true

# -----------------------------
# Start rsyslog (container-safe)
# -----------------------------
echo "> Starting rsyslogd"
rm -f /run/rsyslogd.pid /var/run/rsyslogd.pid 2>/dev/null || true
/usr/sbin/rsyslogd -n &
RSYSLOG_PID=$!

# -----------------------------
# PHP-FPM (auto-detect version)
# -----------------------------
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
  echo "WARN: No php-fpm binary found; continuing."
  return 1
}
start_php_fpm || true

# -----------
# Start nginx
# -----------
echo "> Starting nginx"
nginx -g 'daemon off;' &
NGINX_PID=$!

# ------------------------------------------
# DB wait + conditional create/upgrade logic
# ------------------------------------------
DB_HOST="${BLUECHERRY_DB_HOST:-127.0.0.1}"
DB_NAME="${BLUECHERRY_DB_NAME:-bluecherry}"
DB_USER="${BLUECHERRY_DB_USER:-bluecherry}"
DB_PASS="${BLUECHERRY_DB_PASSWORD:-bluecherry}"

# Optionally wait for DB
if [ "${WAIT_FOR_DB:-1}" = "1" ]; then
  echo "> Waiting for MySQL at ${DB_HOST}..."
  for i in {1..60}; do
    if mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -e "SELECT 1" >/dev/null 2>&1; then
      echo "> DB is up."
      break
    fi
    sleep 2
  done
fi

# Decide whether to create or upgrade
echo "> Checking if DB '${DB_NAME}' exists..."
if mysql -h"${DB_HOST}" -u"${DB_USER}" -p"${DB_PASS}" -e "SHOW DATABASES LIKE '${DB_NAME}';" 2>/dev/null | grep -q "${DB_NAME}"; then
  echo "> DB exists -> skipping create; running upgrade"
  if command -v bc-database-upgrade >/dev/null 2>&1; then
    # Note: upgrade may try to mysqldump with PROCESS priv; harmless warning if not root.
    bc-database-upgrade || true
  fi
else
  echo "> DB missing -> running create"
  if command -v bc-database-create >/dev/null 2>&1; then
    bc-database-create || true
  fi
fi

# --------------------------
# Start Bluecherry in fg/bg
# --------------------------
echo "> Starting bc-server as bluecherry:bluecherry"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
/usr/sbin/bc-server -u bluecherry -g bluecherry &
BC_PID=$!

# --------------------------
# Watchdog
# --------------------------
echo "> All services launched. Entering watchdog loop."
while sleep 15; do
  kill -0 "${RSYSLOG_PID}" 2>/dev/null || { echo "rsyslogd exited"; exit 1; }
  pgrep -x nginx >/dev/null || { echo "nginx exited"; exit 1; }
  kill -0 "${BC_PID}" 2>/dev/null || { echo "bc-server exited"; exit 1; }
done
