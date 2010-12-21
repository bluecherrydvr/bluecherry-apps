<?php //defined('INDVR') or exit();

#db
define(VAR_CONF_PATH, "/etc/bluecherry.conf");

define(VAR_ROOT_PATH, '/');

define(VAR_LOG_PATH, '/var/log/bluecherry.log');

define(VAR_NEWS_XML, 'http://forums.bluecherrydvr.com/index.php?app=core&module=global&section=rss&type=forums&id=2');

define(VAR_SESSION_NAME, 'bluecherrydvr');


#local card props
$local_device_fps = array(30, 15, 10, 5, 3, 2);
$resolutions['NTSC'] = array('704x480', '352x240');
$resolutions['PAL']  = array('704x576', '352x288');

?>
