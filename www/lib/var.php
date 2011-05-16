<?php //defined('INDVR') or exit();

#db
define('VAR_CONF_PATH', "/etc/bluecherry.conf");

define('VAR_ROOT_PATH', '/');

define('VAR_LOG_PATH', '/var/log/bluecherry.log');

define('VAR_NEWS_XML', 'http://forums.bluecherrydvr.com/index.php?app=core&module=global&section=rss&type=forums&id=2');

define('VAR_SESSION_NAME', 'bluecherrydvr');

define('JS_RELOAD', '<script>window.location = "/";</script>');

define('VAR_PATH_TO_CURRENT_VERSION', 'http://www.bluecherrydvr.com/version');
define('VAR_PATH_TO_INSTALLED_VERSION', '/usr/share/bluecherry/version');
define('DB_WRITABLE', '/var/run/bluecherry/db-writable');
#local card props
$GLOBALS['buffer'] = array(
	'post' => array(1,2,3,4,5,10,15,20),
	'pre' => array('Currently not supported')
);

$GLOBALS['local_device_fps'] = array(30, 15, 10, 7.5, 5, 3, 2);
$GLOBALS['capacity'] = array(
	'solo6010' => 480,
	'solo6110' => 600,
	'solo6x10' => 480 #backward compatibility
);

$GLOBALS['resolutions'] = array(
	'NTSC' => array('704x480', '352x240'),
	'PAL'  => array('704x576', '352x288')
);

#anaglog ptz properties
$GLOBALS['ptz'] = array(
	'PROTOCOLS' => array('PELCO'),
	'BAUD' => array(1200, 2400, 4800, 9600, 19200, 38400),
	'BIT' =>  array(8, 7),
	'STOP_BIT' => array(1, 2),
	'PARITY' => array('None', 'Even', 'Odd')
);

$GLOBALS['storage_limits'] = array(100, 99.99, 99.98, 99.97, 99.96, 99.95, 99.94, 99.93, 99.92, 99.91, 99.9, 99.8, 99.7, 99.6, 99.5, 99.5, 99.4, 99.3, 99.2, 99.1, 99, 98, 96, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50);

?>
