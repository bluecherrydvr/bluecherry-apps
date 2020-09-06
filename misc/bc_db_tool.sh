#!/bin/bash -e

# !!! ATTENTION !!!
# This file called from DEB and RPM postinstall scripts

### to create new bluecherry mysql db run
# bc_db_tool.sh new_db db_admin_name db_admin_pass bc_db_name db_user db_pass
### to upgrade existing db run (it will check, that current db version is lower then in installed package)
# bc_db_tool.sh upgrade_db bc_db_name db_user db_pass [db_host]

set -x # trace
if [[ $(cat /etc/os-release | grep "^ID=" | grep centos) ]]
then
    IN_RPM="1"
else
    IN_DEB="1"
fi

function db_upgrade_err() {
	trap - ERR # avoid recurrence
	echo Failed upgrade DB
	exit 1 # Indicate failure
}

function apply_db_patches() {
	pushd /usr/share/bluecherry/mysql-upgrade
	# Note that entries must be numeric, and they are traversed in numeric order, ascending
	# Entries are directories, see below
	export dbname user password host
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
	echo "UPDATE GlobalSettings SET value='$DB_VERSION' WHERE parameter = 'G_DB_VERSION'" \
		| mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
}
function print_usage
{
    echo usage: to create new database
    echo        bc_db_tool.sh new_db db_admin_name db_admin_pass bc_db_name db_user db_pass db_host db_userhost
    echo to upgrade existing database
    echo        bc_db_tool.sh upgrade_db bc_db_name db_user db_pass [db_host]
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
    if ! echo "show databases" | mysql_wrapper -h"${host}" -u"$MYSQL_ADMIN_LOGIN" &>/dev/null
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
	dbhost=$3
    dbname=$4

    echo "Testing whether database already exists..."
    if echo "show databases" | mysql_wrapper -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN" -D"$dbname" &>/dev/null
    then
        echo -e "\n\n\tDatabase '$dbname' already exists, but /etc/bluecherry.conf is not found."
        echo -e "\tAborting installation. In order to proceed installation, please do one of following things:\n" \
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
	dbhost=$3
    dbname=$4
    user=$5 
    password=$6
	userhost=$7

    # Actually create DB and tables
    if [[ "$MYSQL_ADMIN_PASSWORD" ]]
    then
	echo "DROP DATABASE IF EXISTS $dbname; CREATE DATABASE $dbname" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN" --password="$MYSQL_ADMIN_PASSWORD"
	echo "CREATE USER ${user}@'${userhost}' IDENTIFIED BY '$password'" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN" --password="$MYSQL_ADMIN_PASSWORD"
	echo "GRANT ALL ON ${dbname}.* to ${user}@'${userhost}'" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN" --password="$MYSQL_ADMIN_PASSWORD"
    else
	echo "DROP DATABASE IF EXISTS $dbname; CREATE DATABASE $dbname" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN"
	echo "CREATE USER ${user}@'${userhost}' IDENTIFIED BY '$password'" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN"
	echo "GRANT ALL ON ${dbname}.* to ${user}@'${userhost}'" | mysql -h"$dbhost" -u"$MYSQL_ADMIN_LOGIN"
    fi
    mysql -h"$dbhost" -u"$user" --password="$password" -D"$dbname" < /usr/share/bluecherry/schema_mysql.sql
    # Save actual DB version
    DB_VERSION=`cat /usr/share/bluecherry/installed_db_version`
    echo "INSERT INTO GlobalSettings (parameter, value) VALUES ('G_DB_VERSION', '$DB_VERSION')" | mysql -h"$dbhost" -u"$user" --password="$password" -D"$dbname"
    # Put initial data into DB
    mysql -h"$dbhost" -u"$user" --password="$password" -D"$dbname" < /usr/share/bluecherry/initial_data_mysql.sql
}
function new_db
{
    # new_db db_admin_name db_admin_pass bc_db_name db_user db_pass db_host db_userhost
    echo new_db "$@"
    MYSQL_ADMIN_LOGIN="$1"
    MYSQL_ADMIN_PASSWORD="$2"
    dbname=$3
    user=$4
    password=$5
	host=$6
	userhost=$7

    check_mysql_admin "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD"
    check_db_exists "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$host" "$dbname"
    create_db "$MYSQL_ADMIN_LOGIN" "$MYSQL_ADMIN_PASSWORD" "$host" "$dbname" "$user" "$password" "$userhost"
}
function check_mysql_connect
{
    dbname="$1"
    user="$2"
    password="$3"
    host="$4"
    echo 'exit' | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
}

function fix_motion_maps
{
# fix motion maps for versions 2.7.9 and earlier
    dbname="$1"
    user="$2"
    password="$3"
    host="$4"

    echo "Fixing motion maps..."

    #IP cams - motion map of 32x24 = 768
    echo "update Devices set motion_map = repeat('3', 768) where protocol like 'IP%' and length(motion_map) <> 768;" | mysql -h "$host" -u"$user" -p"$password" -D"$dbname"

    #tw5864 cards 16x12 = 192
    echo "update Devices set motion_map = repeat('3', 192) where driver = 'tw5864' and length(motion_map) <> 192;" | mysql -h "$host" -u"$user" -p"$password" -D"$dbname"

    #solo6x10 PAL 22x18 = 396
    echo "update Devices set motion_map = repeat('3', 396) where driver like 'solo6%' and length(motion_map) <> 396;" | mysql -h "$host" -u"$user" -p"$password" -D"$dbname"
    #solo6x10 NTSC 22x15 = 330
    echo "update Devices set motion_map = repeat('3', 330) where driver like 'solo6%' and length(motion_map) <> 330;" | mysql -h "$host" -u"$user" -p"$password" -D"$dbname"
}

function upgrade_db
{
    # upgrade_db bc_db_name db_user db_pass [host]
    echo upgrade_db "$@"
    dbname="$1"
    user="$2"
    password="$3"
    host="${4}"

    trap db_upgrade_err ERR # will restore DB from backup and terminate the execution with failure retcode
    check_mysql_connect "$dbname" "$user" "$password" "$host" 
    
    # Get DB version
    DB_VERSION=`echo "SELECT value from GlobalSettings WHERE parameter = 'G_DB_VERSION'" \
	    | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" | tail -n 1`
    PACKAGE_DB_VERSION=`cat /usr/share/bluecherry/installed_db_version`

    if [[ "$DB_VERSION" != "$PACKAGE_DB_VERSION" ]]
    then
	echo package db version in base \""$DB_VERSION"\" != \""$PACKAGE_DB_VERSION"\" '(in packege db version)'
	echo Upgradeing Database...
	
	if [[ "$DB_VERSION" == "" ]]
	then
	    # If no version stored (pre-2.2 was installed):
	    # Rename all tables to ${table}_bkp
	    for x in `echo "show tables" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2`
	    do
	        echo "RENAME TABLE $x TO ${x}_bkp" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
	    done
	    # Create tables from the reference DB schema
	    mysql -h "$host" -u"$user" -p"$password" -D"$dbname" < /usr/share/bluecherry/schema_mysql_2020000.sql
	    # Migrate data from all tables
	    # Devices.rtsp_rtp_prefer_tcp, added in 2.1.7-7. Workaround for twofold incoherences:
	    # 1) Failure of dbconfig-common to apply a DB patch to add it
	    # 2) Different fields order in fresh installation and patched one
	    # Try to add, ignore result because this may fail if field is already here
	    echo "ALTER TABLE Devices_bkp ADD COLUMN rtsp_rtp_prefer_tcp boolean NOT NULL DEFAULT 1 AFTER rtsp_password" \
		    | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" || true
	    # Try to move it to be after rtsp_password for the case it was added in the end
	    echo "ALTER TABLE Devices_bkp MODIFY COLUMN rtsp_rtp_prefer_tcp boolean NOT NULL DEFAULT 1 AFTER rtsp_password" \
		    | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
	    # 2.1.7-1 DB patch
	    echo "ALTER TABLE Storage_bkp DROP COLUMN record_time" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" || true
	    # Drop "?tcp" and "?udp" postfixes from RTSP URLs in Devices
	    echo "UPDATE Devices_bkp SET device = SUBSTRING(device, 1, LENGTH(device) - 4) WHERE SUBSTRING(device, -4) = '?udp' OR SUBSTRING(device, -4) = '?tcp'" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
	    # Now table contents copying should work
	    # All other tables columns modifications (besides of constraints) are from 2.0.0 days, those versions are not used as of present days (May 2014)
	    # preinst checks that version is not less than 2.0.3
	    for x_bkp in `echo "show tables" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" | tail -n +2 | grep _bkp`
	    do
                x=`echo $x_bkp | sed 's/_bkp//'`
		echo "SET FOREIGN_KEY_CHECKS=0; INSERT INTO $x SELECT * FROM $x_bkp" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
		# Drop all *_bkp tables
		echo "SET FOREIGN_KEY_CHECKS=0; DROP TABLE $x_bkp" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password"
		echo "SET FOREIGN_KEY_CHECKS=1" | mysql -h "$host" -D"$dbname" -u"$user" -p"$password" # Restore setting just in case
	    done
	    DB_VERSION=2020000 # to assist apply_db_patches() logics
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

	echo "DELETE FROM GlobalSettings WHERE parameter = 'G_DB_VERSION'" | mysql -h "$host" -u"$user" -p"$password" -D"$dbname"
	echo "INSERT INTO GlobalSettings (parameter, value) VALUES ('G_DB_VERSION', '$DB_VERSION')" \
		| mysql -h "$host" -u"$user" -p"$password" -D"$dbname"
    fi # Whether old and new DB version match

    fix_motion_maps "$dbname" "$user" "$password" "$host"
}

MODE="$1"

case "$MODE" in
new_db)
	if [[ $# -lt 6 ]]
	then
		print_usage
		exit 1
	fi
	new_db "$2" "$3" "$4" "$5" "$6" "$7" "$8"
	;;
upgrade_db)
	if [[ $# -lt 4 ]]
        then
                print_usage
                exit 1
        fi
	upgrade_db "$2" "$3" "$4" "$5"
	;;
*)
	print_usage
	exit 1
esac
exit 0
