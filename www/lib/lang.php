<?php defined('INDVR') or exit();

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

#company global
define('DVR_COMPANY_NAME', 'Bluecherry');
define('DVR_DVR', 'DVR');

#headers
define('PAGE_HEADER_MAIN', 'Administration');
define('PAGE_HEADER_VIEWER', 'Live View');
define('PAGE_HEADER_SCHED', 'Access is not allowed at this time');

define('PAGE_HEADER_LOGIN', 'Login');
define('PAGE_HEADER_SEPARATOR', ' :: ');

#main menu
define('MMENU_NEWS', 'Announcements');
define('MMENU_GS', 'General settings');
define('MMENU_STORAGE', 'Storage');
define('MMENU_USERS', 'Users');
define('MMENU_LOG', 'System Log');
define('MMENU_DEVICES', 'Devices');
define('MMENU_SCHED', 'Global schedule');
define('MMENU_LIVEVIEW', 'LiveView');
define('MMENU_STATISTICS', 'Event statistics');
define('MMENU_ACTIVE_USERS', 'Connected users');

#main
define('COUND_NOT_OPEN_PAGE', 'Page could not be loaded');
define('COUND_NOT_OPEN_PAGE_EX', 'The page requested could not be loaded at this time. Please make sure you are able to connect to the DVR server and that the server is running.');

#dsched
define('DSCED_HEADER', 'Access to live view is not allowed for this account at this time.');
define('DSCED_MSG', 'You can not use web interface at this time. If this is a mistake, please contact your system administrator.');


#die (fatal) messages
define('LANG_DIE_COULDNOTCONNECT', 'Fatal error: could not connect to the database: ');
define('LANG_DIE_COULDNOTOPENCONF', 'Fatal error: could not read configuration file.');

#general
define('BB_MAP_ARROW', '>');
define('SAVE_SETTINGS', 'Save settings');
define('SAVE_CHANGES', 'Save changes');
define('SAVE', 'Save');
define('SAVE_AS', 'Save as...');
define('CLEAR_ALL', 'Clear all');

define('CHANGES_OK', 'Changes successfully saved');
define('CHANGES_FAIL', 'Could not apply changes at this time, please try later.');

define('ENABLE_DEVICE_NOTENOUGHCAP', '<![CDATA[<b>Card does not have sufficient capacity to accomodate this device. <br/></b>Please reduce Resolution/Framerate of other deivces on this card.]]>');
define('NEW_DEV_NEFPS', 'The device was configured using generic configuration, but there is insufficient card capacity to enable it. Please reduce framerate/resolution of other devices and try again.');

#general settings
define('GLOBAL_SETTINGS', 'DVR General settings');
define('G_DISABLE_VERSION_CHECK', 'Check for available updates');
define('G_DISABLE_VERSION_CHECK_EX', 'requires server to have internet access');
define('G_DVR_NAME', 'DVR Name');
define('G_DVR_NAME_EX', 'You can name you dvr after its location or usage');
define('G_DVR_EMAIL', 'Email address');
define('G_DVR_EMAIL_EX', 'Email for notifications');
define('G_DEV_NOTES', 'Notes');
define('G_DEV_NOTES_EX', 'dvr memos');
define('G_DISABLE_IP_C_CHECK', 'Disable IP camera connectivity check');
define('G_DISABLE_IP_C_CHECK_EX', 'May improve web UI performance');
define('G_DISABLE_WEB_STATS', 'Disable Stats display in the web UI');
define('G_DISABLE_WEB_STATS_EX', 'May improve web UI performance');

#users page
define('EDIT_ACCESS_LIST', 'Camera access permissions');
define('USERS_ALL_TABLE_HEADER', 'DVR Users');
define('USERS_DETAIL_TABLE_HEADER', 'User details: ');

define('CAMERA_PERMISSIONS_SUB', 'User camera access permissions');

define('USERS_ALL_TABLE_SUBHEADER', 'Users currently registered in the DVR system.');
define('USERS_DETAIL_TABLE_SUBHEADER', 'Edit user information and access rights. ');

define('USERS_TABLE_HEADER_NAME', 'Full Name');
define('USERS_TABLE_HEADER_LOGIN', 'Login');
define('USERS_TABLE_HEADER_EMAIL', 'Email');
define('USERS_TABLE_HEADER_STATUS', 'Status');
define('USERS_LIST', 'DVR users');

define('DELETE_USER', 'Delete user');
define('DELETE_USER_SELF', 'You can not remove your own user record');
define('USER_DELETED_OK', 'User record successfully deleted');

define('USERS_NEW', 'Create New User');
define('USERS_NAME', 'Name');
define('USERS_NAME_EX', 'user\'s full name');
define('USERS_LOGIN', 'Login');
define('USERS_LOGIN_EX', 'login');
define('USERS_PASSWORD', 'Password');
define('USERS_PASSWORD_EX', 'password');
define('USERS_PHONE', 'Phone');
define('USERS_PHONE_EX', 'phone #');
define('USERS_ACCESS_SETUP', 'Admin access');
define('USERS_ACCESS_SETUP_EX', 'administrative priveleges');
define('USERS_ACCESS_REMOTE', 'Remote access');
define('USERS_ACCESS_REMOTE_EX', 'remote client access');
define('USERS_ACCESS_WEB', 'Web access');
define('USERS_ACCESS_WEB_EX', 'viewer web access');
define('USERS_ACCESS_BACKUP', 'Backup access');
define('USERS_ACCESS_BACKUP_EX', 'recorded videos');
define('USERS_EMAIL', 'Email');
define('USERS_EMAIL_EX', 'number of emails per hour');
define('EMAIL_LIMIT', 'Limit');
define('EMAIL_LIMIT_EX', 'Emails/hour (0 - unlimited)');
define('USERS_NOTES', 'User notes');
define('USERS_NOTES_EX', 'visible only to admin');


define('USERS_STATUS_ADMIN', 'Administrator');
define('USERS_STATUS_VIEWER','Viewer');
define('USERS_STATUS_DISABLE', 'Disabled');


define('USERS_STATUS_SETUP', 'Admin');

#log
define('LOG_EMPTY', 'Current log file is empty.');
define('LOG_ALL', 'Whole log');
define('LOG_COULD_NOT_OPEN', 'Could not open log file');
define('LOG_HOW_MANY', 'Bottom lines to load...');
define('LOG_FILE_DOES_NOT_EXIST', 'Log file does not exist');
define('LOG_DOWNLOAD', 'Download debugging information');

define('SCROLL_TO_BOTTOM', 'scroll to bottom');
define('SCROLL_TO_TOP', 'scroll to top');

#news
define('NEWS_HEADER', DVR_COMPANY_NAME.' Announcements');

#login
define('LOGIN_WRONG', 'Wrong login/password combination, please try again.');
define('LOGIN_OK', 'Please wait while you are redirected...');
define('NA_WEB', 'You do not have permission to use web interface');
define('NA_CLIENT', 'You do not have permission to use remote client');



#devices
define('CARD_HEADER', 'Bluecherry hardware compression');
define('CARD_CHANGE_ENCODING', 'click to change encoding');
define('LOCAL_DEVICES', 'Local devices:');
define('PORT', 'Port');
define('DEVICE_VIDEO_SOURCE', 'Video source');
define('DEVICE_VIDEO_STATUS', 'Device status');
define('DEVICE_VIDEO_STATUS_OK', 'Enabled');
define('DEVICE_VIDEO_STATUS_CHANGE_OK', 'Disable');
define('DEVICE_VIDEO_STATUS_disabled', 'Disabled');
define('DEVICE_VIDEO_STATUS_CHANGE_disabled', 'Enable');
define('DEVICE_VIDEO_NAME_notconfigured', 'Not configured');
define('DEVICE_VIDEO_STATUS_notconfigured', 'Disabled');
define('DEVICE_VIDEO_STATUS_CHANGE_notconfigured', 'Enable');
define('DEVICE_VIDEO_RESFPS', 'Device resolution/FPS');
define('DEVICE_EDIT_MMAP', 'Motion detection settings');
define('DEVICE_EDIT_VIDADJ', 'Video/audio settings');
define('DEVICE_EDIT_SCHED', 'Schedule');
define('VIDEO_ADJ', 'Video adjustments for');
define('DEVICE_UNNAMED', 'Unnamed');
define('TOTAL_DEVICES', 'Total devices');
define('ENABLE_ALL_PORTS', 'enable all ports');

define('VA_HEADER', 'Video adjustments');
define('VA_BRIGHTNESS', 'Brightness');
define('VA_HUE', 'Hue');
define('VA_AUDIO', 'Audio gain');
define('VA_SATURATION', 'Staruration');
define('VA_CONTRAST', 'Contrast');
define('VA_SETDEFAULT', 'Reset values to default');

define('DEVICE_ENCODING_UPDATED', '<![CDATA[Video standard setting changed. <hr /> <b>For the change to take effect you need to restart the server.</b><br> If you are changing from NTSC to PAL please follow the instructions on <a href="http://help.bluecherrydvr.com/spaces/manuals/manuals/version2/lessons/22732-Enabling-video-ports">this page</a>]]>');
define('DB_FAIL_TRY_LATER', 'Could not apply changes. Please make sure you are able to connect to the DVR server and that the server is running.');

define('MMAP_HEADER', 'Motion detection settings');
define('PRE_REC_BUFFER', 'Pre-event buffer');
define('POST_REC_BUFFER', 'Post-event buffer');
define('SCHED_HEADER','Device recording schedule');
define('ALL_DEVICES', 'All devices');
define('EDITING_MMAP', 'Motion detection settings for camera:');
define('EDITING_SCHED', 'Recording schedule for camera:');
define('EDITING_GLOBAL_SCHED', 'Global schedule');
define('GLOBAL_SCHED_OG', 'Use device-specific schedule for this camera.');
define('SIGNAL_TYPE_NOT_CONFD', 'Configure at least one device first');
define('SCHED_GLOBAL', 'global');
define('SCHED_SPEC', 'device specific');

define('HIDE_IMG', 'Show/hide image');

define('AVAILABLE_DEVICES', 'Available video sources:');
define('LAYOUT_CONTROL', 'Select layout:');
define('NONE_SELECTED', 'None selected');
define('CLEAR_LAYOUT', 'Clear layout');
define('BACK_TO_ADMIN', 'Return to admin panel');

define('NO_NEWS', 'There are no news right now.');

define('NO_CARDS', 'No valid local video devices were detected.');

/*user page errors*/
define('NO_USERNAME', 'Login field can not be empty.');
define('NO_EMAIL', 'Email field can not be empty.');
define('NO_PASS', 'Password field can not be empty.');
define('CANT_REMOVE_ADMIN', 'You can not remove your own admin priveleges.');
define('USER_CREATED', 'New user record created.');
define('USER_DELETED', 'User record deleted.');


#stats
define('STATS_HEARDER', 'Server statistics:');
define('STATS_CPU', 'CPU usage:');
define('STATS_MEM', 'Memory usage:');
define('STATS_UPT', 'Server uptime:');

#messages general
define('USER_KICKED', 'Your session was terminated by an Administrator.');
define('USER_NACCESS', 'You need administrator priveleges to access this page. Please <a href="/">login again</a>.');
define('USER_RELOGIN', 'Your session has expired, please <a htef="/">relogin</a>. ');
define('NO_CONNECTION', 'Cound not connect to the server.<br /><br />Please make sure that:<br />-server httpd is running<br />-you are online');

#active users 
define('ACTIVE_USERS_HEADER', 'Connected users');
define('ACTIVE_USERS_SUBHEADER', 'Users currently using this DVR server');
define('AU_IP', 'IP address');
define('AU_KICK', 'Kick (IP)');
define('AU_BAN', 'Ban user');
define('AU_CANT_EOS', 'You can not terminate your own session.');
define('AU_KICKED', 'User session terminated');
define('AU_CANT_SB', 'You can not ban yourself');
define('AU_BANNED', 'User is banned.');
define('AU_INFO', 'Kicking user will end session from that IP address, ban user for 5 minutes and require re-login. Banning user will remove all access priveleges (can be restored on user page).');
define('RCLIENT', 'Remote Client');
define('WEB', 'Web');
define('AC_ACCESS', '');
define('CLICK_HERE_TO_UPDATE', 'Click here to update');

define('AIP_HEADER', 'Add an IP Camera');
define('AIP_NEW_LIST_AVAILABLE', 'Updates ip camera list is available.');
define('AIP_LIST_UPDATED_SUCCESSFULLY', 'Camera list updates sussessfully.');
define('AIP_NEW_LIST_AVAIL', 'New ip camera tables are available.');
define('AIP_NEW_LIST_MYSQL_E', 'Could not update the database, please check Apache logs for details');
define('AIP_CAN_NOT_GET_NEW_LIST', 'Could not load the new tables from the server. Please try later');
define('AIP_SUBHEADER', '');
define('AIP_CAMERA_NAME', 'Camera name');
define('AIP_CHOOSE_MANUF', 'Please choose manufacturer');
define('AIP_CHOOSE_MODEL', 'Please choose model');
define('AIP_CHOOSE_FPSRES', 'Please choose fps/resolution');
define('AIP_CH_MAN_FIRST', 'Choose manufacturer first');
define('AIP_CH_MOD_FIRST', 'Choose model first');
define('AIP_IP_ADDR_MJPEG', 'MJPEG IP Address');
define('AIP_PORT_MJPEG', 'MJPEG Port');
define('AIP_IP_ADDR', 'Camera IP or Host');
define('AIP_CHOOSE_RES', 'Please choose framerate/resolution');
define('AIP_PORT', 'Port');
define('AIP_USER', 'Username');
define('AIP_PASS', 'Password');
define('AIP_MJPATH', 'Path to MJPEG');
define('AIP_RTSP', 'Path to RTSP');
define('AIP_ADD', 'Add camera');
define('AIP_ALREADY_EXISTS', '<![CDATA[Camera with ip <b>%IP%</b> already exists in the list of the devices.]]>');
define('AIP_NEEDIP', 'Please enter ip address of the camera');
define('AIP_NEEDPORT', 'Please enter port number');
define('AIP_NEEDMJPEG', 'Please enter path to MJPEG');
define('AIP_RTSPPATH', 'Please enter path to RTSP');
define('AIP_ADVANCED_SETTINGS', 'advanced settings');
define('AIP_CAMADDED', 'IP camera has been successfully added');


define('DEVICE_EDIT_DETAILS', 'Properties');
define('DELETE_CAM', 'Delete');
define('PROPS_HEADER', 'IP Camera properties');
define('IPCAM_WIKI_LINK', 'Please visit this <a href="http://improve.bluecherrydvr.com/projects/v2tracker/wiki/Ipcameras">WIKI</a> for information on supported "Generic" cameras, along with RTSP path and MJPEG path');
define('SERVER_NOT_RUNNING', 'Server process stopped.');
define('SERVER_RUNNING', 'Server process running');
define('NOT_UP_TO_DATE', 'New version available.');
define('UP_TO_DATE', 'Version');
define('NOT_UP_TO_DATE_LONG', 'New version of the server was released. We strongly recommend to update.');
define('NOT_UP_TO_DATE_LINK', 'For release notes and update instructions, please visit our <a href="http://forums.bluecherrydvr.com/index.php?/forum/27-announcements/">forums</a>.');
define('WANT_TO_LEARN_MORE', 'Learn more...');
define('INSTALLED', 'Installed');
define('CURR', 'Current');
define('SETTINGS', 'Settings');
define('DELTE_CAM_CONF', 'Are you sure you want to delete this camera (ID:');

#layouts
define('NO_SAVED_LAYOUTS', 'No saved layouts');
define('CHOOSE_LAYOUT', 'Layout to load...');
define('LAYOUTS', 'Layouts');

#login
define('PLEASE_LOGIN', 'You need');

define('USERNAME_EXISTS', 'User with this username already exists.');

#PTZ
define('DEVICE_EDIT_PTZ', 'Edit PTZ settings');

define('PTZ_SET_HEADER', 'PTZ settings for camera: ');
define('PTZ_PROTOCOL', 'Protocol:');
define('PTZ_SET_SUBHEADER', 'Basic PTZ configuration');
define('PTZ_CONTOL_PATH', 'Control path:');
define('PTZ_ADDR', 'Camera address:');
define('PTZ_BAUD', 'Baud rate:');
define('PTZ_BIT', 'Bit:');
define('PTZ_STOP_BIT', 'Stop bit:');
define('PTZ_PARITY', 'Parity:');

###
define('QUERY_FAILED', 'Could not execute query. Please check <i>/var/log/apache2/error.log</i> for details.');
define('LOGOUT', 'Logout');
define('WRITE_FAILED', 'Failed to write to storage folder at: ');

#mjpeg
define('MJPEG_DISABLED', 'This device was disabled.');
define('MJPEG_DEVICE_NOT_FOUND', 'Device with ID %ID% was not found.');
define('MJPEG_NO_PERMISSION', 'You do not have permission to access camera ID %ID%.');
#storage 
define('STRAGE_HEADER', 'Manage storage locations');
define('ADD_LOCATION', 'Add location');
define('LOCATION', 'Folder:');

define('ON_CARD', ' on Card ');
define('MAP_PRESET', 'New preset...');

#logs
define('LOG_APACHE', 'Apache web server log');
define('LOG_PHP', 'PHP Error log');
define('LOG_BC', 'Bluecherry log');

#ipcam statuses
define('IP_ACCESS_STATUS_F', 'Could not connect to the <b>%TYPE%</b>. Please verify that the camera is turned on and the IP address is correct.');
define('IP_ACCESS_STATUS_404', 'Incorrect <b>%TYPE%</b> path, please check camera manual for correct paths.');
define('IP_ACCESS_STATUS_401', 'Incorrect login or password.');
define('IP_ACCESS_STATUS_303', 'Could not resolve the hostname.');
define('IP_ACCESS_NCONFIG', 'Could not connect to the camera. Please verify hostname/ip address.');

#ip ptz presets
define('IPP_NEW', 'New preset');
define('IPP_DELETE', 'Are you sure you want to delete preset "');
define('IPP_HEADER', 'Select PTZ preset for');
define('IPP_EDIT', 'Edit PTZ preset');
define('IPP_EDIT_HELP', 'Please fill in the preset commands according to the manufacturers specifications. <br /><br />The following varialbes are accepted: <br /> 
							<b>%USERNAME%</b> - Username to access the camera <br />
							<b>%PASSWORD%</b> - Password to access the camera <br />
							<b>%ID%</b> - ID for the command, e.g. position preset ID');
define('IPP_PRESET_NAME', 'Preset name');
define('IPP_PRESET_NAME_EX', 'preset name');


define('IPP_PRESET_RIGHT', 'Right');
define('IPP_PRESET_LEFT', 'Left');
define('IPP_PRESET_UP', 'Up');
define('IPP_PRESET_DOWN', 'Down');
define('IPP_PRESET_UPRIGHT', 'Up-right');
define('IPP_PRESET_UPLEFT', 'Up-left');
define('IPP_PRESET_DOWNRIGHT', 'Down-right');
define('IPP_PRESET_DOWNLEFT', 'Down-left');
define('IPP_PRESET_TIGHT', 'Tight');
define('IPP_PRESET_TIGHT_EX', 'zoom in');
define('IPP_PRESET_WIDE', 'Wide');
define('IPP_PRESET_WIDE_EX', 'zoom out');
define('IPP_PRESET_FOCUSIN', 'Focus in');
define('IPP_PRESET_FOCUSOUT', 'Focus out');
define('IPP_PRESET_PRESET_SAVE', 'Save preset');
define('IPP_PRESET_PRESET_GO', 'Go to preset');
define('IPP_PRESET_STOP', 'Stop');
define('IPP_PRESET_STOP_EX', 'stop moving');
define('IPP_PRESET_NEEDS_STOP', 'Needs stop');
define('IPP_PRESET_NEEDS_STOP_EX', 'whether camera needs a stop command');
define('IPP_PRESET_HTTP_AUTH', 'Use http auth');
define('IPP_PRESET_HTTP_AUTH_EX', 'use http auth or GET parameters');
define('IPP_PRESET_PORT', 'Control port');
define('IPP_PRESET_PORT_EX', 'if camera uses port!=80');
?>
