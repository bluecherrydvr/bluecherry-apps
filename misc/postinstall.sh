#!/bin/bash -e

# !!! ATTENTION !!!
# This file is common for debian amd centos (called at package postinstall stage)

set -x # trace
if [[ $(cat /etc/os-release | grep "^ID=" | grep centos) ]]
then 
    IN_RPM="1"
else
    IN_DEB="1"
fi

if [[ $IN_DEB ]]
then
	. /usr/share/debconf/confmodule
fi

function start_apache
{
	if [[ $IN_DEB ]]
	then

		# Start back the Apache, just in case we have jumped into here from the middle of upgrade procedure
		if [ ! -z `which service` ]; then
			service apache2 start || true
		else
			invoke-rc.d apache2 start || true
		fi
	fi
	if [[ $IN_RPM ]]
	then
		systemctl start httpd.service
	fi
}
function stop_apache
{
	if [[ $IN_DEB ]]
	then
		if [ ! -z `which service` ]
		then
			service apache2 stop || true
		else
			invoke-rc.d apache2 stop || true
		fi
	else
		systemctl stop httpd.service
	fi
}

# 1 - rpm install, 2 - rpm upgrade
case "$1" in
	configure|reconfigure|1|2)
		if [[ $IN_DEB ]]
		then
			ucfr bluecherry /etc/bluecherry.conf
		fi

		# Fixup some permissions
		echo -e "\n\n\tVerifying permissions of existing files, please wait\n\n"
		if ! [[ -d /var/lib/bluecherry ]]
		then
			mkdir -p /var/lib/bluecherry
		fi	
		chown -R bluecherry:bluecherry /var/lib/bluecherry
		chmod -R 770 /var/lib/bluecherry
		if [[ $IN_DEB ]]
		then
			chown -R bluecherry:bluecherry /var/run/bluecherry
			chmod -R 750 /var/run/bluecherry
		fi

		# Create the logfile, because rsyslog might not be able to
		touch /var/log/bluecherry.log

		if id syslog > /dev/null 2>&1; then
			chown syslog:bluecherry /var/log/bluecherry.log
		else
			chown root:bluecherry /var/log/bluecherry.log
		fi
		chmod 640 /var/log/bluecherry.log

		if [[ $IN_DEB ]]
		then
			# Apache modules and sites
			a2enmod ssl
			a2enmod php5
			a2enmod rewrite
		fi

		if [[ $IN_DEB ]]
		then
			# Remove a file prefiously tracked as conffile from old place
			rm /etc/apache2/sites-{enabled,available}/bluecherry || true

			a2ensite bluecherry.conf
		else
			install -d /etc/httpd/sites-enabled
			if [[ -e /etc/httpd/sites-enabled/bluecherry.conf ]]
			then
				rm /etc/httpd/sites-enabled/bluecherry.conf
			fi
			ln -s /etc/httpd/sites-available/bluecherry.conf /etc/httpd/sites-enabled/bluecherry.conf
			grep -q -E '^[[:space:]]*IncludeOptional .*sites-enabled.*' /etc/httpd/conf/httpd.conf || \
				echo 'IncludeOptional sites-enabled/*.conf' >> /etc/httpd/conf/httpd.conf
			if [[ -e /etc/ssl/certs/ssl-cert-snakeoil.pem ]]
			then
				rm /etc/ssl/certs/ssl-cert-snakeoil.pem
			fi
			ln -s /etc/pki/tls/certs/localhost.crt /etc/ssl/certs/ssl-cert-snakeoil.pem
			install -d /etc/ssl/private
			if [[ -e /etc/ssl/private/ssl-cert-snakeoil.key ]]
			then
				rm /etc/ssl/private/ssl-cert-snakeoil.key
			fi
			ln -s /etc/pki/tls/private/localhost.key /etc/ssl/private/ssl-cert-snakeoil.key
		fi
		
		if [[ $IN_DEB ]]
		then
			# Remove php ini (tracked previously as conffile) from old place
			rm /etc/php5/conf.d/bluecherry.ini || true

			if [ ! -z `which service` ]; then
				service rsyslog restart || true
			else
				invoke-rc.d rsyslog restart || true
			fi
		fi
		
		stop_apache

		if [ ! -e /var/lib/.bcins ]; then
			date +'%s' > /var/lib/.bcins
		fi

		if [ -x /sbin/apparmor_parser ]; then
			: #apparmor_parser -r < /etc/apparmor.d/usr.sbin.bc-server || true
		fi

		# Manage DB
		if ! [[ -s /etc/bluecherry.conf ]]
		then
			if [[ $IN_DEB ]]
			then
				# 0-bytes config - fresh installation
				# Create DB

				# Remove cached answers
				db_fset bluecherry/mysql_admin_login    seen false || true
				db_fset bluecherry/mysql_admin_password seen false || true
				db_fset bluecherry/db_name              seen false || true
				db_fset bluecherry/db_user              seen false || true
				db_fset bluecherry/db_password          seen false || true

				# Ask questions
				db_input high bluecherry/mysql_admin_login  || true
				db_input high bluecherry/mysql_admin_password  || true
				db_go  || true

				db_get bluecherry/mysql_admin_login  || true
				export MYSQL_ADMIN_LOGIN="$RET"
				db_get bluecherry/mysql_admin_password  || true
				export MYSQL_ADMIN_PASSWORD="$RET"

				db_input high bluecherry/db_name  || true
				db_go  || true

				db_get bluecherry/db_name  || true
				export dbname="$RET"
				
				/usr/share/bluecherry/bc_db_tool.sh "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" \
					bc_db_name db_user db_pass
				
				db_input high bluecherry/db_user  || true
				db_input high bluecherry/db_password  || true
				db_go  || true

				db_get bluecherry/db_user  || true
				export user="$RET"
				db_get bluecherry/db_password  || true
				export password="$RET"
			else
				export MYSQL_ADMIN_LOGIN="root"
				export MYSQL_ADMIN_PASSWORD=""
				export dbname="bluecherry"
				export user="bluecherry"
				password=$(dd bs=10 count=1 if=/dev/urandom status=none| md5sum - | awk '{print $1}')
				if ! [[ $password ]]
				then
					password="bluecherry"
				fi
				export password
			fi

			/usr/share/bluecherry/bc_db_tool.sh new_db "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" \
				"$dbname" "$user" "$password" || exit 1

			# Generate config
			if [[ $IN_DEB ]]
			then
				dbhost="localhost"
			else
				dbhost="127.0.0.1"
			fi
			cat /usr/share/bluecherry/bluecherry.conf.in | sed -e "s/_DBC_DBNAME_/${dbname}/"  -e "s/_DBC_DBUSER_/${user}/"  -e "s/_DBC_DBPASS_/${password}/" -e "s/_DBC_HOST_/${dbhost}/" > /etc/bluecherry.conf
			chown root:bluecherry /etc/bluecherry.conf
			chmod 640 /etc/bluecherry.conf
		else
			# Upgrade
			/usr/share/bluecherry/bc_db_tool.sh upgrade_db

		fi # Whether there was an upgrade or fresh install
		
		. /usr/share/bluecherry/load_db_creds.sh
		export dbname user password
		/usr/share/bluecherry/compare.sh >&2
		# trap - ERR # FIXME: restore dumped database if compare failes

		mkdir -p /usr/share/bluecherry/sqlite
		if [[ $IN_DEB ]]
		then
			chown www-data /usr/share/bluecherry/sqlite  # to allow creation of lock file for sqlite db
		else
			chown apache /usr/share/bluecherry/sqlite
		fi
		cp /usr/share/bluecherry/cameras_shipped.db /usr/share/bluecherry/sqlite/cameras.db

		# End of DB management

		# Reenable our site in Apache
		start_apache
		;;
esac
