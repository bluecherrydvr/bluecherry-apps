#!/bin/bash -e

# !!! ATTENTION !!!
# This file is common for debian and centos (called at package postinstall stage)

set -x # trace

for x in /etc/*-release
do
	if [[ ! -d $x ]]
	then
		source $x
	fi
done

if [[ $(cat /etc/os-release | grep "^ID=" | grep centos) ]]
then 
    IN_RPM="1"
else
    IN_DEB="1"
fi

function restore_mysql_backup
{
    # restore_mysql_backup dbname user password host
    dbname="$1"
    user="$2"
    password="$3"
    host="${4:-localhost}"
    
    echo -e "An error occurred while migrating your database to the latest version. Please contact Bluecherry support here: support@bluecherrydvr.com . Provide full  uncut output from console (the above text).\n\nATTENTION! The above log contains your MySQL logins and passwords, please do not post these logs on public forums.\n" >&2
    if [[ -s $DB_BACKUP_GZ_FILE ]]
    then
        echo "Restoring DB from previously taken backup in $DB_BACKUP_GZ_FILE" >&2
        # Drop all tables
        for x in `echo "show tables" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"| tail -n +2`
        do
            echo "SET FOREIGN_KEY_CHECKS=0; DROP TABLE $x" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
        done
        echo "SET FOREIGN_KEY_CHECKS=1" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" # Restore setting just in case
        # Restore from backup
        gunzip -c $DB_BACKUP_GZ_FILE | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
    fi
}
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
		
		# Use the correct user for rsyslog under debian
		if [[ $(cat /etc/os-release | grep "^ID=" | grep debian) ]]
		then
			sed -i 's/ syslog / root /' /etc/logrotate.d/bluecherry
		fi

		if [[ $IN_DEB ]]
		then
			# Apache modules and sites
			a2enmod ssl
			a2enmod rewrite
			if [[ "$UBUNTU_CODENAME" == 'xenial' || "$VERSION" == "9 (stretch)" ]]
			then
				a2enmod php7.0
			elif [[ "$UBUNTU_CODENAME" == 'bionic' ]]
			then
				a2enmod php7.2
			elif [[ "$VERSION" == "10 (buster)" ]]
			then
				a2enmod php7.3
			else
				a2enmod php5
			fi
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
		else
			systemctl restart rsyslog.service
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
				db_fset bluecherry/db_host				seen false || true
				db_fset bluecherry/db_userhost		    seen false || true

				# Ask questions
				db_input high bluecherry/mysql_admin_login  || true
				db_input high bluecherry/mysql_admin_password  || true
				db_go  || true

				db_get bluecherry/mysql_admin_login  || true
				export MYSQL_ADMIN_LOGIN="$RET"
				db_get bluecherry/mysql_admin_password  || true
				export MYSQL_ADMIN_PASSWORD="$RET"

				db_input high bluecherry/db_host  || true
				db_input high bluecherry/db_name  || true
				db_go  || true
				
				db_get bluecherry/db_host || true
				export host="$RET"
				db_get bluecherry/db_name  || true
				export dbname="$RET"
				
				db_input high bluecherry/db_user  || true
				db_input high bluecherry/db_password  || true
				db_input high bluecherry/db_userhost || true
				db_go  || true

				db_get bluecherry/db_user  || true
				export user="$RET"
				db_get bluecherry/db_password  || true
				export password="$RET"
				db_get bluecherry/db_userhost || true
				export userhost="$RET"
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
				"$dbname" "$user" "$password" "$host" "$userhost" || exit 1

			# Generate config
			if [[ $IN_DEB ]]
			then
				dbhost="${host:-localhost}"
			else
				dbhost="${host:-127.0.0.1}"
			fi
			cat /usr/share/bluecherry/bluecherry.conf.in | sed -e "s/_DBC_DBNAME_/${dbname}/"  -e "s/_DBC_DBUSER_/${user}/"  -e "s/_DBC_DBPASS_/${password}/" -e "s/_DBC_HOST_/${dbhost}/" -e "s/_DBC_USERHOST_/${userhost}/"> /etc/bluecherry.conf
			chown root:bluecherry /etc/bluecherry.conf
			chmod 640 /etc/bluecherry.conf
		else
			# Upgrade
			. /usr/share/bluecherry/load_db_creds.sh

			if ! echo 'exit' | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
			then
				echo "Can't connect mysql with credentials in /etc/bluecherry.conf"
				echo "Please remove config or fix config"
				exit 1
			fi

			DB_BACKUP_GZ_FILE=$(mktemp ~bluecherry/bc_db_backup.XXXXXXXXXX.sql.gz)
			echo "Going to updgrade Bluecherry DB. Taking a backup into $DB_BACKUP_GZ_FILE just in case" >&2
			# Backup the DB
			mysqldump -h "$host" "$dbname" -u"$user" -p"$password" | gzip -c > $DB_BACKUP_GZ_FILE

			if ! /usr/share/bluecherry/bc_db_tool.sh upgrade_db "$dbname" "$user" "$password" "$host"
			then
				echo Failed upgrade database, restoring backup
				restore_mysql_backup "$dbname" "$user" "$password" "$host"
				exit 1
			fi

		fi # Whether there was an upgrade or fresh install
		
		if ! /usr/share/bluecherry/compare.sh >&2
		then
			echo "Loaded database scheme missmatch. Please load it manualy or restore from backup"
			if [[ -s "$DB_BACKUP_GZ_FILE" ]]
			then
				restore_mysql_backup "$dbname" "$user" "$password" "$host"
			fi
			exit 1
		fi
		# database successfully upgraded
		if [[ -f "$DB_BACKUP_GZ_FILE" ]]
		then
	                rm -f "$DB_BACKUP_GZ_FILE"
	        fi

		mkdir -p /usr/share/bluecherry/sqlite
		if [[ $IN_DEB ]]
		then
			chown www-data /usr/share/bluecherry/sqlite  # to allow creation of lock file for sqlite db
		else
			chown apache /usr/share/bluecherry/sqlite
		fi
		cp /usr/share/bluecherry/cameras_shipped.db /usr/share/bluecherry/sqlite/cameras.db

		# End of DB management

		# Clean up files created by solo6010-dkms
		if [[ $IN_DEB ]]
                then
			if [[ -e /etc/modprobe.d/solo6x10.conf ]]
			then
				if [[ $(grep "blacklist solo6x10" /etc/modprobe.d/solo6x10.conf) ]]
				then
					rm /etc/modprobe.d/solo6x10.conf
				fi
			fi

			# no idea which package created this file but let's remove it anyway
			if [[ -e /etc/modprobe.d/blacklist-solo6x10.conf ]]
			then
				if [[ $(grep "blacklist solo6x10" /etc/modprobe.d/blacklist-solo6x10.conf) ]]
				then
					rm /etc/modprobe.d/blacklist-solo6x10.conf
				fi
			fi

		fi

		if [[ $IN_DEB ]]
		then
			if [[ -e /etc/modules && ! $(grep "inserted by bluecherry package" /etc/modules) ]]
			then
				echo "Adding kernel modules for Bluecherry hardware capture cards to /etc/modules"
				echo "#automatically inserted by bluecherry package" >> /etc/modules
				echo "solo6x10" >> /etc/modules
				echo "tw5864" >> /etc/modules
			fi
		fi

		# Reenable our site in Apache
		start_apache
		;;
esac
