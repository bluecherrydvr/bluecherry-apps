<?php defined('INDVR') or exit();

#db
define('VAR_CONF_PATH', "/etc/bluecherry.conf");

define('VAR_ROOT_PATH', '/');

define('VAR_LOG_PATH', '/var/log/bluecherry.log');

define('VAR_NEWS_XML', 'http://forums.bluecherrydvr.com/index.php?app=core&module=global&section=rss&type=forums&id=2');

define('VAR_SESSION_NAME', 'bluecherrydvr');

define('JS_RELOAD', '<script>window.location = "/";</script>');

define('VAR_PATH_TO_CURRENT_VERSION', 'http://www.bluecherrydvr.com/version');
define('VAR_PATH_TO_INSTALLED_VERSION', '/usr/share/bluecherry/version');

#local card props
$local_device_fps = array(30, 15, 10, 7.5, 5, 3, 2);
$resolutions['NTSC'] = array('704x480', '352x240');
$resolutions['PAL']  = array('704x576', '352x288');

#anaglog ptz properties
$ptz['PROTOCOLS'] = array('PELCO');
$ptz['BAUD']= array(1200, 2400, 4800, 9600, 19200, 38400);
$ptz['BIT'] = array(8, 7);
$ptz['STOP_BIT']= array(1, 2);
$ptz['PARITY'] = array('None', 'Even', 'Odd');

?>
