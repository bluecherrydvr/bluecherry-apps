#!/bin/bash

set -e

echo "> Update MySQL's my.cnf from environment variables passed in from docker"
echo "> Writing /root/.my.cnf"
{
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

echo "> Update bluecherry server's bluecherry.conf from environment variables passed in from docker"
echo "> Writing /etc/bluecherry.conf"
{
  echo "# Bluecherry configuration file"; \
  echo "# Used to be sure we don't use configurations not suitable for us";\
  echo "version = \"1.0\";"; \
  echo "bluecherry:"; \
  echo "{"; \
  echo "    db:"; \
  echo "    {"; \
  echo "        # 0 = sqlite, 1 = pgsql, 2 = mysql"; \
  echo "        type = 2;"; \
  echo "        dbname = \"$BLUECHERRY_DB_NAME\";"; \
  echo "        user = \"$BLUECHERRY_DB_USER\";"; \
  echo "        password = \"$BLUECHERRY_DB_PASSWORD\";"; \
  echo "        host = \"$BLUECHERRY_DB_HOST\";"; \
  echo "        userhost = \"$BLUECHERRY_DB_ACCESS_HOST\";"; \
  echo "    };"; \
  echo "};"; \
} > /etc/bluecherry.conf

echo "> chown bluecherry:bluecherry /var/lib/bluecherry/recordings"
chown bluecherry:bluecherry /var/lib/bluecherry/recordings
chown -R bluecherry:bluecherry /var/lib/bluecherry/.local/share/data/

# The bluecherry container's Dockerfile sets rsyslog to route the bluecherry
# server's main log file to STDOUT for process #1, which then gets picked up
# by docker (so its messages get routed out through docker logs, etc.), but
# the location permissions have to be reset on every start of the container:
chmod 777 /proc/self/fd/1

sleep 15

license_key=$(mysql bluecherry -e "SELECT license FROM Licenses;" | tail -n +2 | awk '{print $1}')
echo "License key: $license_key"

deactivate_license() {
    echo "Deactivating license..."
    /usr/lib/bluecherry/licensecmd bc_v3_license_DeactivateLicense $license_key
}

on_exit() {
    echo "Container is shutting down..."
    # Place your command to run before exit here
#    deactivate_license $license_key
    /usr/lib/bluecherry/licensecmd bc_v3_license_DeactivateLicense $license_key
    exit 0
}

# Hack to fix race condition where rsyslog starts too soon and throws errors
# https://github.com/bluecherrydvr/bluecherry-docker/issues/26

# sleep for 5 for good measure
sleep 5

echo "> /usr/sbin/rsyslogd"

# rm rsyslog.pid to prevent respawning
rm -f /run/rsyslogd.pid
/usr/sbin/rsyslogd
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to start rsyslog: $status"
  exit $status
fi

exec "$@"

/etc/init.d/php7.4-fpm start

echo "> /usr/sbin/nginx"
#source /etc/apache2/envvars
/usr/sbin/nginx
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to start nginx web server: $status"
  exit $status
fi

trap 'on_exit' SIGTERM SIGINT

echo "> /usr/sbin/bc-server -u bluecherry -g bluecherry"
export LD_LIBRARY_PATH=/usr/lib/bluecherry
/usr/sbin/bc-server -u bluecherry -g bluecherry
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to start bluecherry bc-server: $status"
  exit $status
fi


# Naive check runs checks once a minute to see if either of the processes exited.
# This illustrates part of the heavy lifting you need to do if you want to run
# more than one service in a container. The container exits with an error
# if it detects that any of the processes has exited.
# Otherwise it loops forever, waking up every 15 seconds

while sleep 15; do
   if ! pgrep -x rsyslog > /dev/null; then
        echo "rsyslog has exited."
        break
    fi
    if ! pgrep -x nginx > /dev/null; then
        echo "nginx has exited."
        break
    fi
    if ! pgrep -x bc-server > /dev/null; then
        echo "bc-server has exited."
        break
    fi
done

# Deactivate license on docker stop.
on_exit
