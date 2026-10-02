#!/usr/bin/env bash
set -euo pipefail

log() { echo "$(date -Iseconds) $*"; }

# -------------------------------
# 1) Config files
# -------------------------------
log "> Writing /root/.my.cnf"
cat >/root/.my.cnf <<EOF
[client]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
[mysqldump]
user=${MYSQL_ADMIN_LOGIN:-root}
password=${MYSQL_ADMIN_PASSWORD:-root}
EOF
chmod 600 /root/.my.cnf || true

log "> Writing /etc/bluecherry.conf"
cat >/etc/bluecherry.conf <<EOF
# Bluecherry configuration file
version = "1.0";
bluecherry:
{
    db:
    {
        type = 2;  # 0=sqlite, 1=pgsql, 2=mysql
        dbname   = "${BLUECHERRY_DB_NAME:-bluecherry}";
        user     = "${BLUECHERRY_DB_USER:-bluecherry}";
        password = "${BLUECHERRY_DB_PASSWORD:-bluecherry}";
        host     = "${BLUECHERRY_DB_HOST:-bc-mysql}";
        userhost = "${BLUECHERRY_DB_ACCESS_HOST:-%}";
    };
};
EOF

# -------------------------------
# 2) Permissions
# -------------------------------
REC_DIR=/var/lib/bluecherry/recordings
BC_USER="${BLUECHERRY_LINUX_USER_NAME:-bluecherry}"
BC_GROUP="${BLUECHERRY_LINUX_GROUP_NAME:-bluecherry}"

install -d -m 0775 "$REC_DIR"
if ! chown "${BC_USER}:${BC_GROUP}" "$REC_DIR"; then
  log "WARN: chown ${BC_USER}:${BC_GROUP} $REC_DIR failed; check the volume mount"
fi
if ! chmod ug+rwx "$REC_DIR"; then
  log "WARN: chmod ug+rwx $REC_DIR failed"
fi

# Ensure www-data is in the bluecherry group so PHP can read recordings
if ! usermod -a -G "$BC_GROUP" www-data; then
  log "WARN: adding www-data to group $BC_GROUP failed"
fi
log "> Recordings dir: $(stat -c '%U:%G %a' "$REC_DIR" 2>/dev/null || echo unknown)"

# Self-check: the web user must traverse and read recordings, otherwise
# Storage settings, playback and downloads fail with confusing errors.
if command -v su >/dev/null 2>&1; then
  BECOME_WWW=(su -s /bin/sh www-data -c)
elif command -v runuser >/dev/null 2>&1; then
  BECOME_WWW=(runuser -u www-data -- sh -c)
else
  BECOME_WWW=()
fi
if [ "${#BECOME_WWW[@]}" -gt 0 ]; then
  if ! "${BECOME_WWW[@]}" "test -r '$REC_DIR' && test -x '$REC_DIR'" >/dev/null 2>&1; then
    log "WARN: www-data cannot read $REC_DIR (debug: id www-data; ls -ld $REC_DIR)"
  fi
else
  log "WARN: neither su nor runuser available; skipping www-data readability check"
fi

# Route container logs to stdout safely
chmod 777 /proc/self/fd/1 || true

# -------------------------------
# 3) Ancillary services (warn-only)
# -------------------------------
log "> Starting rsyslogd"
/usr/sbin/rsyslogd || log "WARN: rsyslogd failed (continuing)"

# PHP-FPM (try 8.3 → 8.2 → 8.1), daemonized (no -F)
PHPFPM_STARTED=0
for v in 8.3 8.2 8.1; do
  if command -v "php-fpm${v}" >/dev/null 2>&1; then
    log "> Starting php-fpm${v}"
    if "php-fpm${v}"; then
      sock="/run/php/php${v}-fpm.sock"
      [ -S "$sock" ] && ln -sf "$sock" /etc/alternatives/php-fpm.sock || true
      PHPFPM_STARTED=1
    else
      log "WARN: php-fpm${v} failed (continuing)"
    fi
    break
  fi
done
[ "$PHPFPM_STARTED" -eq 1 ] || log "WARN: no php-fpm found; UI may not work"

# nginx (optional)
if command -v nginx >/dev/null 2>&1; then
  log "> Starting nginx"
  nginx || log "WARN: nginx failed (continuing)"
else
  log "WARN: nginx not installed; UI may be unreachable"
fi

# -------------------------------
# 4) Database wait + create/upgrade
# -------------------------------
DB_HOST="${BLUECHERRY_DB_HOST:-bc-mysql}"
DB_NAME="${BLUECHERRY_DB_NAME:-bluecherry}"
DB_USER="${BLUECHERRY_DB_USER:-bluecherry}"
DB_PASS="${BLUECHERRY_DB_PASSWORD:-bluecherry}"

log "> Waiting for MySQL at $DB_HOST..."
for i in {1..60}; do
  mysql -h "$DB_HOST" -e "SELECT 1" >/dev/null 2>&1 && break
  sleep 1
done
log "> DB is up."

# Create or upgrade
if mysql -h "$DB_HOST" -e "USE \`$DB_NAME\`" >/dev/null 2>&1; then
  log "> DB exists → upgrading"
  /bin/bc-database-upgrade "$DB_NAME" "$DB_USER" "$DB_PASS" "$DB_HOST" || { log "ERROR: DB upgrade failed"; exit 1; }
else
  log "> DB missing → creating"
  /bin/bc-database-create "$DB_NAME" "$DB_USER" "$DB_PASS" "$DB_HOST" || { log "ERROR: DB create failed"; exit 1; }
fi

# Ensure required privileges (MySQL 8-safe); warn if it fails
log "> Ensuring '${DB_USER}' has PROCESS/SHOW VIEW/EVENT/TRIGGER/LOCK TABLES and native auth"
{
  mysql -h "$DB_HOST" -uroot -p"${MYSQL_ADMIN_PASSWORD:-root}" -e \
    "GRANT PROCESS, SHOW VIEW, EVENT, TRIGGER, LOCK TABLES ON *.* TO '${DB_USER}'@'${BLUECHERRY_DB_ACCESS_HOST:-%}';"
  mysql -h "$DB_HOST" -uroot -p"${MYSQL_ADMIN_PASSWORD:-root}" -e \
    "ALTER USER '${DB_USER}'@'${BLUECHERRY_DB_ACCESS_HOST:-%}' IDENTIFIED WITH mysql_native_password BY '${DB_PASS}';"
  mysql -h "$DB_HOST" -uroot -p"${MYSQL_ADMIN_PASSWORD:-root}" -e "FLUSH PRIVILEGES;"
} || log 'WARN: grant/alter failed; continuing'

# -------------------------------
# 5) Start bc-server  (IMPORTANT: use -s so it does NOT background)
# -------------------------------
log "> Starting bc-server"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
BC_ARGS=(-u "$BC_USER" -g "$BC_GROUP" -s)

if [ "${DEBUG:-0}" = "1" ]; then
  log "Running bc-server in DEBUG mode (foreground, log level debug)"
  exec /usr/sbin/bc-server "${BC_ARGS[@]}" -l d
fi

# Normal mode: keep script control, watchdog monitors actual bc-server PID
/usr/sbin/bc-server "${BC_ARGS[@]}" &
BC_PID=$!

# -------------------------------
# 6) Watchdog loop (fail only if bc-server dies)
# -------------------------------
log "All services launched. Entering watchdog loop."

# Allow disabling watchdog for deep debugging
if [ "${DISABLE_WATCHDOG:-0}" = "1" ]; then
  log "Watchdog disabled (DISABLE_WATCHDOG=1)."
  tail -f /dev/null
fi

SLEEP="${WATCHDOG_INTERVAL:-5}"
while sleep "$SLEEP"; do
  if ! kill -0 "${BC_PID:-0}" 2>/dev/null; then
    wait "${BC_PID:-0}" || true
    EXIT=$?
    log "bc-server exited (code=$EXIT). Exiting container."
    exit 1
  fi
  # Ancillary warn-only
  pgrep -x nginx    >/dev/null || log "WARN: nginx not running (transient?)"
  pgrep -x rsyslogd >/dev/null || log "WARN: rsyslogd not running (transient?)"
done
