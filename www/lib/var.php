<?php defined('INDVR') or exit();

/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#db
define('VAR_CONF_PATH', '/etc/bluecherry.conf');

define('VAR_ROOT_PATH', '/');

define('VAR_LOG_PATH', '/var/log/bluecherry.log');
define('VAR_WWW_LOG_PATH', '/var/log/apache2/bluecherry-error.log');

define('VAR_NEWS_XML', 'http://www.bluecherrydvr.com/feed/');

define('VAR_SESSION_NAME', 'bluecherrydvr');

define('JS_RELOAD', '<script>window.location = "/";</script>');

#versions
define('VAR_PATH_TO_CURRENT_VERSION', 'http://versioncheck.bluecherrydvr.com/server-version');
define('VAR_PATH_TO_INSTALLED_VERSION', '/usr/share/bluecherry/version');

define('VAR_PATH_TO_IPCAMLIST_VERSION', 'http://versioncheck.bluecherrydvr.com/ipcamtableversion');
define('VAR_PATH_TO_IPCAMLIST_IPCAM', 'http://versioncheck.bluecherrydvr.com/ipcam.csv');
define('VAR_PATH_TO_IPCAMLIST_DRIVER', 'http://versioncheck.bluecherrydvr.com/ipcamdriver.csv');
define('VAR_PATH_TO_IPCAMLIST_PTZ', 'http://versioncheck.bluecherrydvr.com/ipcamptz.csv');

define('VAR_TARRED_LOGS_TMP_LOCATION', '/tmp/bc-logs.tar'); #before logs are downloaded they will be compressed to this file
define('VAR_MYSQLDUMP_TMP_LOCATION', '/tmp/bluecherry-backup.tgz');

define('VAR_PATH_TO_IPCAMLIST_UPDATE', 'http://versioncheck.bluecherrydvr.com/ipcamlistupdate.php');
define('VAR_IPCAMLIST_UPDATE_TOKEN', 'Hiuhg3TnbJl1676T');

define('RM_CLIENT_DOWNLOAD', 'http://www.bluecherrydvr.com/downloads');

define('VAR_LICENSE_AUTH', 'http://keycheck.bluecherrydvr.com');

$varpub = VarPub::get();
$varpub_globals = Array();
#local card props
$GLOBALS['buffer'] = array(
	'post' => array(0,1,2,3,4,5,10,15,20),
	'pre' => array(0,1,2,3,4,6,8,10)
);
$varpub_globals['buffer'] = $GLOBALS['buffer'];


$GLOBALS['local_device_fps'] = array(30, 15, 10, 7.5, 5, 3, 2);
$varpub_globals['local_device_fps'] = $GLOBALS['local_device_fps'];
$GLOBALS['capacity'] = array(
	'tw5864'   => 9999999,
	'generic'  => 9999999,
	'solo6010' => 480,
	'solo6110' => 600,
	'solo6x10' => 480 #backward compatibility
);
$varpub_globals['capacity'] = $GLOBALS['capacity'];

$GLOBALS['resolutions'] = array(
	'NTSC' => array('704x480', '352x240'),
	'PAL'  => array('704x576', '352x288')
);
$varpub_globals['resolutions'] = $GLOBALS['resolutions'];

#anaglog ptz properties
$GLOBALS['ptz'] = array(
	'PROTOCOLS' => array('PELCO'),
	'DEFAULT_PATH' => '/dev/ttyS0',
	'BAUD' => array(1200, 2400, 4800, 9600, 19200, 38400),
	'BIT' =>  array(8, 7),
	'STOP_BIT' => array(1, 2),
	'PARITY' => array('None', 'Even', 'Odd')
);
$varpub_globals['ptz'] = $GLOBALS['ptz'];

$GLOBALS['cts_record_durations'] = array(
	'900' => '15 minutes' , 
	'1800' => '30 minutes', 
	'3600' => '1 hour', 
	'7200' => '2 hours', 
	'10800' => '3 hours', 
	'14400' => '4 hours',
	'21600' => '6 hours'
);
$varpub_globals['cts_record_durations'] = $GLOBALS['cts_record_durations'];

#reencode bitrates
$GLOBALS['reencode_bitrates'] = array(
	'64000' => '64 kbps',
	'128000' => '128 kbps',
	'256000' => '256 kbps',
	'512000' => '512 kbps',
	'1000000' => '1 mbps'
);
$varpub_globals['reencode_bitrates'] = $GLOBALS['reencode_bitrates'];
#reencode resolutions
$GLOBALS['reencode_resolutions'] = array(
	'160x120',
	'320x240',
	'352x240',
	'352x288',
	'640x480'
);
$varpub_globals['reencode_resolutions'] = $GLOBALS['reencode_resolutions'];

$varpub->globals = $varpub_globals;
