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

function start_nginx
{
	if [[ $IN_DEB ]]
	then

		# Start nginx, just in case we have jumped into here from the middle of upgrade procedure
		if [ ! -z `which service` ]; then
			service nginx start || true
		else
			invoke-rc.d nginx start || true
		fi
	fi
	if [[ $IN_RPM ]]
	then
		systemctl start nginx
	fi
}
function stop_nginx
{
	if [[ $IN_DEB ]]
	then
		if [ ! -z `which service` ]
		then
			service nginx stop || true
		else
			invoke-rc.d nginx stop || true
		fi
	else
		systemctl stop nginx
	fi
}

function install_pip
{
	# If already installed - nothing to do
	if which pip3 >/dev/null; then
		return
	fi

	source /etc/os-release
	if [[ "$ID" == ubuntu ]] && [[ "$VERSION_CODENAME" == bionic ]]; then
        	wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/pip/3.6/get-pip.py
		python3 /tmp/get-pip.py
	elif [[ "$ID" == ubuntu ]] && [[ "$VERSION_CODENAME" == focal ]]; then
        	wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/pip/3.8/get-pip.py
		python3 /tmp/get-pip.py
	else
        	wget --output-document=/tmp/get-pip.py https://bootstrap.pypa.io/get-pip.py 
		python3 /tmp/get-pip.py
	fi
}

function install_certbot
{
	source /etc/os-release
	if [[ "$ID" == ubuntu ]] && [[ "$VERSION_CODENAME" == noble ]]; then
		echo 'For Ubuntu 24.04 (noble) we rely on packaged certbot'
		return
	elif [[ "$ID" == ubuntu ]] && [[ "$VERSION_CODENAME" == mantic ]]; then
		echo 'For Ubuntu 23.10 (mantic) we rely on packaged certbot'
		return
	elif [[ "$ID" == ubuntu ]] && [[ "$VERSION_CODENAME" == oracular ]]; then
		echo 'For Ubuntu 24.10 (oracular) we rely on packaged certbot'
		return
	elif [[ "$ID" == debian ]] && [[ "$VERSION_CODENAME" == bookworm ]]; then
		echo 'For Debian 12 (bookworm) we rely on packaged certbot'
		return
	fi

	export PATH=/usr/local/bin:"$PATH"

	# Ubuntu 23+, Debian 12 make this step fail. Fall back to system package.
	install_pip || return 0

	# Install pip3 dependencies
	pip3 install --user --upgrade setuptools_rust certbot certbot-dns-subdomain-provider
	pip3 install --user --upgrade pip || true
	pip3 install --user --upgrade cryptography
	pip3 install pyopenssl --upgrade
}

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
		PREV_INSTALLED_VERSION=$2
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
		su bluecherry -c "chmod -R 770 /var/lib/bluecherry"
		if [[ $IN_DEB ]]
		then
			mkdir -p /var/run/bluecherry
			chown -R bluecherry:bluecherry /var/run/bluecherry
			su bluecherry -c "chmod -R 750 /var/run/bluecherry"
		fi

		# Allow update_subdomain_certs.sh script to run as www-data
		chown root:www-data /usr/share/bluecherry/scripts/update_subdomain_certs.sh
		chmod 550 /usr/share/bluecherry/scripts/update_subdomain_certs.sh
		install --mode ug=r,o= /usr/share/bluecherry/sudoers.d/www-data /etc/sudoers.d/www-data

		mkdir -p /usr/share/bluecherry/ssl
		cp /etc/ssl/certs/ssl-cert-snakeoil.pem /usr/share/bluecherry/ssl/bluecherry-snakeoil.pem
		cp /etc/ssl/private/ssl-cert-snakeoil.key /usr/share/bluecherry/ssl/bluecherry-snakeoil.key
		chown bluecherry:bluecherry /usr/share/bluecherry/ssl/bluecherry-snakeoil.pem
		chown bluecherry:bluecherry /usr/share/bluecherry/ssl/bluecherry-snakeoil.key

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
# Don't remove bluecherry config file in case of custom SSL certificates			
#			rm /etc/apache2/sites-{enabled,available}/bluecherry || true

			if test -f "/etc/apache2/sites-enabled/bluecherry.conf"; then
				rm /etc/apache2/sites-enabled/bluecherry.conf
			fi

			# Backup nginx configuration file in case of Bad Things (tm)
			mkdir -p /usr/share/bluecherry/backups/nginx/
			tar -czvf /usr/share/bluecherry/backups/nginx/nginx_$(date +'%F_%H-%M-%S').tar.gz /etc/nginx/
		fi

# Clean this up for centos...if we ever decide to support CentOS in the future....

#		else
#			install -d /etc/httpd/sites-enabled
#			if [[ -e /etc/httpd/sites-enabled/bluecherry.conf ]]
#			then
#				rm /etc/httpd/sites-enabled/bluecherry.conf
#			fi
#			ln -s /etc/httpd/sites-available/bluecherry.conf /etc/httpd/sites-enabled/bluecherry.conf
#			grep -q -E '^[[:space:]]*IncludeOptional .*sites-enabled.*' /etc/httpd/conf/httpd.conf || \
#				echo 'IncludeOptional sites-enabled/*.conf' >> /etc/httpd/conf/httpd.conf
#		fi
		
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
		stop_nginx

# Can be removed in v3...no longer needed

#		if [ ! -e /var/lib/.bcins ]; then
#			date +'%s' > /var/lib/.bcins
#		fi

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

			BC_GENERAL_VERSION=$(echo "SELECT value from GlobalSettings WHERE parameter = 'G_DVR_NAME'" \
				| mysql -h "$host" -D"$dbname" -u"$user" -p"$password" | tail -n 1 | sed -e "s/Bluecherry DVR v//")

			if [ $BC_GENERAL_VERSION == 2 ];
			then
				db_fset bluecherry/major_upgrade    seen false || true
				db_input high bluecherry/major_upgrade  || true
				db_go  || true

				db_get bluecherry/major_upgrade || true
				export answer="$RET"

				if [ $answer != "y" ] && [ $answer != "Y" ]; then
					exit 1
				fi

				# Backup the DB
				BACKUP_DB_FILE=/tmp/bluecherry_backup.sql.gz
				echo "Backup database: $BACKUP_DB_FILE"
				mysqldump -h "$host" "$dbname" -u"$user" -p"$password" | gzip -c > BACKUP_DB_FILE

				echo "Dropping Licenses"
				/usr/share/bluecherry/drop_licenses.sh "$dbname" "$user" "$password" "$host"
			else
				DB_BACKUP_GZ_FILE=$(mktemp ~bluecherry/bc_db_backup.XXXXXXXXXX.sql.gz)
				echo "Going to upgrade Bluecherry DB. Taking a backup into $DB_BACKUP_GZ_FILE just in case" >&2
				# Backup the DB
				mysqldump -h "$host" "$dbname" -u"$user" -p"$password" | gzip -c > $DB_BACKUP_GZ_FILE
			fi

			if ! /usr/share/bluecherry/bc_db_tool.sh upgrade_db "$dbname" "$user" "$password" "$host"
			then
				echo Failed upgrade database, restoring backup
				restore_mysql_backup "$dbname" "$user" "$password" "$host"
				exit 1
			fi

		fi # Whether there was an upgrade or fresh install

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
			chown www-data /usr/share/bluecherry/sqlite
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
		
		# If subdomain.conf exists don't use snakeoil
#		mkdir -p /usr/share/bluecherry/nginx-includes
		if test -f "/usr/share/bluecherry/nginx-includes/subdomain.conf"; then
		sed -i 's/snakeoil.conf/subdomain.conf/g' /etc/nginx/sites-enabled/bluecherry.conf
		fi

		# Debian 10 lacks version-agnostic link for php-fpm.sock. Add it in a compatible way.
		if ! [[ -L /run/php/php-fpm.sock ]]; then
			update-alternatives --install /run/php/php-fpm.sock php-fpm.sock /run/php/php*.*-fpm.sock 0
		fi

		install_certbot
		
		nginx -t 2>/dev/null > /dev/null
		if [[ $? == 0 ]]; then
			# Reenable our site in nginx
			start_nginx
		else
			echo "Nginx configuration failure"
		fi

		;;
esac
