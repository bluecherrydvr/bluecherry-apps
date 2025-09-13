# -------------------------------
# 4) Start bc-server
# -------------------------------
log "> Starting bc-server"

export LD_LIBRARY_PATH=/usr/lib/bluecherry

# Build args for bc-server
BC_ARGS=(-u "${BLUECHERRY_LINUX_USER_NAME:-bluecherry}" -g "${BLUECHERRY_LINUX_GROUP_NAME:-bluecherry}")

# If DEBUG=1 → run with debug logging, stay attached
if [ "${DEBUG:-0}" = "1" ]; then
  log "Running bc-server in DEBUG mode (foreground, log level debug)"
  exec /usr/sbin/bc-server "${BC_ARGS[@]}" -l d
fi

# Otherwise → normal mode, stay in foreground (no -d flag)
# Watchdog will keep container alive, Docker will handle restart policy.
log "Running bc-server in normal mode (foreground)"
/usr/sbin/bc-server "${BC_ARGS[@]}" &
BC_PID=$!
