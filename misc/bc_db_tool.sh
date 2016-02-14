#!/bin/bash -e

# !!! ATTENTION !!!
# This file called from DEB and RPM postinstall scripts

### to create new bluecherry mysql db run
# bc_db_tool.sh new_db db_admin_name db_admin_pass bc_db_name db_user db_pass
### to upgrade existing db run (it will check, that current db version is lower then in installed package)
# bc_db_tool.sh upgrade_db bc_db_name db_user db_pass

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


function db_upgrade_err() {
	trap - ERR # avoid recurrence
	# Say something to user
	echo -e "An error occurred while migrating your database to the latest version. Please contact Bluecherry support here: https://bluecherry.zendesk.com/anonymous_requests/new . Provide full  uncut output from console (the above text).\n\nATTENTION! The above log contains your MySQL logins and passwords, please do not post these logs on public forums.\n" >&2
	if [[ -s $DB_BACKUP_GZ_FILE ]]
	then
		echo "Restoring DB from previously taken backup in $DB_BACKUP_GZ_FILE" >&2
		# Drop all tables
		for x in `echo "show tables" | mysql -D"$dbname" -u"$user" -p"$password"| tail -n +2`
		do
			echo "SET FOREIGN_KEY_CHECKS=0; DROP TABLE $x" | mysql -D"$dbname" -u"$user" -p"$password"
		done
		echo "SET FOREIGN_KEY_CHECKS=1" | mysql -D"$dbname" -u"$user" -p"$password" # Restore setting just in case

		# Restore from backup
		gunzip -c $DB_BACKUP_GZ_FILE | mysql -D"$dbname" -u"$user" -p"$password"
	fi
	exit 1 # Indicate failure
}

function apply_db_patches() {
	pushd /usr/share/bluecherry/mysql-upgrade
	# Note that entries must be numeric, and they are traversed in numeric order, ascending
	# Entries are directories, see below
	for x in `ls | sort -n`
	do
		if [[ $x -gt $DB_VERSION ]]
		then
			pushd $x
				# We execute each *.sh script, in lexicographical order
				for y in *.sh
				do
					# Such shell scenraio can make some checks and execute some SQL statements.
					# DB credentials are exported by env vars $dbname, $user, $password for them
					./$y
				done
			popd
			DB_VERSION=$x # our DB_VERSION is updated
		fi
	done
	popd

	# A check
	if [[ "`cat /usr/share/bluecherry/installed_db_version`" != "$DB_VERSION" ]]
	then
		false # Fail the installation
	fi

	# Save new DB_VERSION
	echo "UPDATE GlobalSettings SET value='$DB_VERSION' WHERE parameter = 'G_DB_VERSION'" | mysql -D"$dbname" -u"$user" -p"$password"
}
function print_usage
{
    echo usage: to create new database
    echo        bc_db_tool.sh new_db db_admin_name db_admin_pass bc_db_name db_user db_pass
    echo to upgrade existing database
    echo        bc_db_tool.sh upgrade_db bc_db_name db_user db_pass
    echo example:
    echo        bc_db_tool.sh new_db root '""' bluecherry bluecherry \'@some_cryptic_password_#*%\'
}
function mysql_wrapper
{
# !!! DO NOT PASS -p or --password to mysql_wrapper !!!
#
# if defined nonempty MYSQL_ADMIN_PASSWORD - use it
# else use empty password or ~/.my.cnf
if [[ "$MYSQL_ADMIN_PASSWORD" ]]
then
        mysql --password="$MYSQL_ADMIN_PASSWORD" "$@"
else
        mysql "$@"
fi
}
function check_mysql_admin
{
    MYSQL_ADMIN_LOGIN="$1"
    MYSQL_ADMIN_PASSWORD="$2"
    echo "Testing MySQL admin credentials correctness..."
    if ! echo "show databases" | mysql_wrapper -u"$MYSQL_ADMIN_LOGIN" &>/dev/null
    then
        echo -e "\n\n\tProvided MySQL admin credentials are incorrect\n"
	echo -e "Please create ~$(whoami)/.my.cnf with right password like this\n"
	echo    "[client]"
	echo    "user=root"
	echo -e "password=\"pass\"\n"
	echo    "[mysql]"
	echo    "user=root"
	echo -e "password=\"pass\"\n"
	echo    "[mysqldump]"
	echo    "user=root"
	echo -e "password=\"pass\"\n"
	echo    "[mysqldiff]"
	echo    "user=root"
	echo    "password=\"pass\""
        exit 1
    fi
}
function check_db_exists
{
    MYSQL_ADMIN_LOGIN="$1"
    MYSQL_ADMIN_PASSWORD="$2"
    dbname=$3
    echo "Testing whether database already exists..."
    if echo "show databases" | mysql_wrapper -u"$MYSQL_ADMIN_LOGIN" -D"$dbname" &>/dev/null
    then
        echo -e "\n\n\tDatabase '$dbname' already exists, but /etc/bluecherry.conf is not found. Aborting installation. In order to proceed installation, please do one of following things:\n" \
           "\t * Restore /etc/bluecherry.conf config file if it was removed, template is in /usr/share/bluecherry/bluecherry.conf.in;\n"
	if [[ IN_DEB ]]
	then
           echo -e "\t * Accomplish removal of previous setup by 'apt-get remove --purge bluecherry';\n"
	fi
        echo -e "\t * Remove the database '$dbname' manually.\n\n" 
        exit 1
    fi
}
function create_db
{
    MYSQL_ADMIN_LOGIN="$1"
    MYSQL_ADMIN_PASSWORD="$2"
    dbname=$3
    user=$4 
    password=$5
    # Actually create DB and tables
    if [[ "$MYSQL_ADMIN_PASSWORD" ]]
    then
	echo "DROP DATABASE IF EXISTS $dbname; CREATE DATABASE $dbname" | mysql -u"$MYSQL_ADMIN_LOGIN" --password="$MYSQL_ADMIN_PASSWORD"
        echo "GRANT ALL ON ${dbname}.* to ${user}@'localhost' IDENTIFIED BY '$password'" | mysql -u"$MYSQL_ADMIN_LOGIN" --password="$MYSQL_ADMIN_PASSWORD"
    else
	echo "DROP DATABASE IF EXISTS $dbname; CREATE DATABASE $dbname" | mysql -u"$MYSQL_ADMIN_LOGIN"
	echo "GRANT ALL ON ${dbname}.* to ${user}@'localhost' IDENTIFIED BY '$password'" | mysql -u"$MYSQL_ADMIN_LOGIN"
    fi
    mysql -u"$user" --password="$password" -D"$dbname" < /usr/share/bluecherry/schema_mysql.sql
    # Save actual DB version
    DB_VERSION=`cat /usr/share/bluecherry/installed_db_version`
    echo "INSERT INTO GlobalSettings (parameter, value) VALUES ('G_DB_VERSION', '$DB_VERSION')" | mysql -u"$user" --password="$password" -D"$dbname"
    # Put initial data into DB
    mysql -u"$user" --password="$password" -D"$dbname" < /usr/share/bluecherry/initial_data_mysql.sql
}
function new_db
{
    # new_db db_admin_name db_admin_pass bc_db_name db_user db_pass
    echo new_db "$@"
    MYSQL_ADMIN_LOGIN="$1"
    MYSQL_ADMIN_PASSWORD="$2"
    dbname=$3
    user=$4
    password=$5

    check_mysql_admin "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD"
    check_db_exists "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$dbname"
    create_db "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$dbname" "$user" "$password"
}
function upgrade_db
{
    # This branch will not use initial_data_mysql.sql,
    # so DB patching script must be added to mysql-upgrade dir
    
    trap db_upgrade_err ERR # will restore DB from backup and terminate the execution with failure retcode
    DB_BACKUP_GZ_FILE=$(mktemp ~bluecherry/bc_db_backup.XXXXXXXXXX.sql.gz)
    
    . /usr/share/bluecherry/load_db_creds.sh
    export dbname user password
    
    # Get DB version
    export DB_VERSION=`echo "SELECT value from GlobalSettings WHERE parameter = 'G_DB_VERSION'" | mysql -D"$dbname" -u"$user" -p"$password" | tail -n 1`

    if [[ "$DB_VERSION" != "`cat /usr/share/bluecherry/installed_db_version`" ]]
    then
	
	# Notify user through debconf non-blocking dialog that backup is taken
	echo "Going to updgrade Bluecherry DB. Taking a backup into $DB_BACKUP_GZ_FILE just in case" >&2
	# Backup the DB
	mysqldump "$dbname" -u"$user" -p"$password" | gzip -c > $DB_BACKUP_GZ_FILE
	
	if [[ "$DB_VERSION" == "" ]]
	then
	    # If no version stored (pre-2.2 was installed):
	    # Rename all tables to ${table}_bkp
	    for x in `echo "show tables" | mysql -D"$dbname" -u"$user" -p"$password" | tail -n +2`
	    do
	        echo "RENAME TABLE $x TO ${x}_bkp" | mysql -D"$dbname" -u"$user" -p"$password"
	    done
	    # Create tables from the reference DB schema
	    mysql -u"$user" -p"$password" -D"$dbname" < /usr/share/bluecherry/schema_mysql_2020000.sql
	    # Migrate data from all tables
	    # Devices.rtsp_rtp_prefer_tcp, added in 2.1.7-7. Workaround for twofold incoherences:
	    # 1) Failure of dbconfig-common to apply a DB patch to add it
	    # 2) Different fields order in fresh installation and patched one
	    # Try to add, ignore result because this may fail if field is already here
	    echo "ALTER TABLE Devices_bkp ADD COLUMN rtsp_rtp_prefer_tcp boolean NOT NULL DEFAULT 1 AFTER rtsp_password" | mysql -D"$dbname" -u"$user" -p"$password" || true
	    # Try to move it to be after rtsp_password for the case it was added in the end
	    echo "ALTER TABLE Devices_bkp MODIFY COLUMN rtsp_rtp_prefer_tcp boolean NOT NULL DEFAULT 1 AFTER rtsp_password" | mysql -D"$dbname" -u"$user" -p"$password"
	    # 2.1.7-1 DB patch
	    echo "ALTER TABLE Storage_bkp DROP COLUMN record_time" | mysql -D"$dbname" -u"$user" -p"$password" || true
	    # Drop "?tcp" and "?udp" postfixes from RTSP URLs in Devices
	    echo "UPDATE Devices_bkp SET device = SUBSTRING(device, 1, LENGTH(device) - 4) WHERE SUBSTRING(device, -4) = '?udp' OR SUBSTRING(device, -4) = '?tcp'" | mysql -D"$dbname" -u"$user" -p"$password"
	    # Now table contents copying should work
	    # All other tables columns modifications (besides of constraints) are from 2.0.0 days, those versions are not used as of present days (May 2014)
	    # preinst checks that version is not less than 2.0.3
	    for x_bkp in `echo "show tables" | mysql -D"$dbname" -u"$user" -p"$password" | tail -n +2 | grep _bkp`
	    do
                x=`echo $x_bkp | sed 's/_bkp//'`
		echo "SET FOREIGN_KEY_CHECKS=0; INSERT INTO $x SELECT * FROM $x_bkp" | mysql -D"$dbname" -u"$user" -p"$password"
		# Drop all *_bkp tables
		echo "SET FOREIGN_KEY_CHECKS=0; DROP TABLE $x_bkp" | mysql -D"$dbname" -u"$user" -p"$password"
		echo "SET FOREIGN_KEY_CHECKS=1" | mysql -D"$dbname" -u"$user" -p"$password" # Restore setting just in case
	    done
	    export DB_VERSION=2020000 # to assist apply_db_patches() logics
	    apply_db_patches
	else
	    # If there is a DB version stored (2.2 and newer was installed):
	    apply_db_patches  # uses $DB_VERSION value, exported
	fi # Whether there was non-empty DB_VERSION
	
	if [[ "$DB_VERSION" != "`cat /usr/share/bluecherry/installed_db_version`" ]]
	then
	    echo "DB_VERISON $DB_VERSION after applying DB patches differ from `cat /usr/share/bluecherry/installed_db_version` in /usr/share/bluecherry/installed_db_version" >&2
	    false
	fi
	echo "DELETE FROM GlobalSettings WHERE parameter = 'G_DB_VERSION'" | mysql -u"$user" -p"$password" -D"$dbname"
	echo "INSERT INTO GlobalSettings (parameter, value) VALUES ('G_DB_VERSION', '$DB_VERSION')" | mysql -u"$user" -p"$password" -D"$dbname"
    fi # Whether old and new DB version match
}

MODE="$1"

case "$MODE" in
new_db)
	if [[ $# -lt 6 ]]
	then
		print_usage
		exit 1
	fi
	new_db "$2" "$3" "$4" "$5" "$6"
	;;
upgrade_db)
	upgrade_db
	;;
*)
	print_usage
	exit 1
esac
exit 0
