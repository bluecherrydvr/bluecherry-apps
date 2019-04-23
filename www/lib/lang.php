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
define('MMENU_PLAYBACK', 'Playback');
define('MMENU_STATISTICS', 'Event statistics');
define('MMENU_ACTIVE_USERS', 'Connected users');
define('MMENU_CLIENT_DOWNLOAD', 'Download remote client');
define('MMENU_DOCUMENTATION', 'Documentation');
define('MMENU_NOTFICATIONS', 'Email notifications');
define('MMENU_BACKUP', 'Database backup');
define('MMENU_LICENSING', 'Support ID keys');

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

define('ENABLE_DEVICE_NOTENOUGHCAP', '<b>Card does not have sufficient capacity to accomodate this device. <br/></b>Please reduce Resolution/Framerate of other deivces on this card.');
define('NEW_DEV_NEFPS', 'The device was configured using generic configuration, but there is insufficient card capacity to enable it. Please reduce framerate/resolution of other devices and try again.');

#general settings
define('GLOBAL_SETTINGS', 'DVR General settings');
define('G_DISABLE_VERSION_CHECK', 'Check for available updates');
define('G_DISABLE_VERSION_CHECK_EX', 'requires server to have internet access');
define('G_DVR_NAME', 'DVR Name');
define('G_DVR_NAME_EX', 'You can name your dvr after its location or usage');
define('G_DVR_EMAIL', 'Email address');
define('G_DVR_EMAIL_EX', 'Email for notifications');
define('G_DEV_NOTES', 'Notes');
define('G_DEV_NOTES_EX', 'dvr memos');
define('G_DISABLE_IP_C_CHECK', 'Disable IP camera connectivity check');
define('G_DISABLE_IP_C_CHECK_EX', 'May improve web UI performance');
define('G_DISABLE_WEB_STATS', 'Disable Stats display in the web UI');
define('G_DISABLE_WEB_STATS_EX', 'May improve web UI performance');
define('G_DATA_SOURCE', 'List source');
define('G_DATA_SOURCE_LOCAL', 'Local list');
define('G_DATA_SOURCE_LIVE', 'Live (You need an active internet connection)');

define('G_SMTP_TITLE', 'SMTP server configuration');
define('G_PERF_TITLE', 'Performance/connectivity');

define('G_MAX_RECORD_TIME', 'Record length');
define('G_MAX_RECORD_TIME_EX', 'Split videos into segments of');

define('G_MAX_RECORD_AGE', 'Max record storage age');
define('G_MAX_RECORD_AGE_EX', 'Let system remove all recordings older than given number of days (optional)');

define('G_VAAPI_DEVICE', 'VAAPI device');
define('G_VAAPI_DEVICE_EX', 'Render node used for VAAPI hardware acceleration of video decoding and encoding');

#mail
define('G_SMTP_SERVICE', 'Mailing service');
define('G_SMTP_EMAIL_FROM', 'SMTP From value');
define('G_SMTP_USERNAME', 'Username');
define('G_SMTP_PASSWORD', 'Password');
define('G_SMTP_HOST', 'SMTP host');
define('G_SMTP_PORT', 'SMTP port');
define('G_SMTP_SSL', 'SSL');
define('G_SMTP_SERVICE_EX', '');
define('G_SMTP_EMAIL_FROM_EX', 'When Bluecherry service sends email, this value is used as sender address ("From")');
define('G_SMTP_USERNAME_EX', '');
define('G_SMTP_PASSWORD_EX', '');
define('G_SMTP_HOST_EX', 'e.g smtp.googlemail.com');
define('G_SMTP_PORT_EX', '');
define('G_SMTP_SSL_EX', '');
define('G_SMTP_FAILED', 'Unable to send mail with this configuration.');

define('G_SMTP_DEF_MTA', 'System default MTA');
define('G_SMTP_SMTP', 'User configured SMTP');
define('G_SEND_TEST_EMAIL_TO', 'Send test email to');
define('G_SEND_TEST_EMAIL', 'Send');
define('G_TEST_EMAIL_SUBJECT', 'Test email');
define('G_TEST_EMAIL_BODY', 'Test email');
define('G_TEST_EMAIL_SENT', 'Message sent');


#users page
define('EDIT_ACCESS_LIST', 'Camera access permissions');
define('USERS_ALL_TABLE_HEADER', 'DVR Users');
define('USERS_DETAIL_TABLE_HEADER', 'User details: ');

define('CAMERA_PERMISSIONS_SUB', 'User camera access permissions');
define('RESTRICT_ALL', 'Restrict all');
define('ALLOW_ALL', 'Allow all');
define('CAMERA_PERMISSIONS_LIST_TITLE', 'click on camera names to allow/restict access');

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
define('USERS_PASSWORD_CURRENT', 'Current password');
define('USERS_PASSWORD_NEW', 'New password');
define('USERS_PASSWORD_WRONG', 'Wrong current password.');
define('USERS_PHONE', 'Phone');
define('USERS_PHONE_EX', 'phone #');
define('USERS_ACCESS_SETUP', 'Admin access');
define('USERS_ACCESS_SETUP_EX', 'administrative priveleges');
define('USERS_ACCESS_REMOTE', 'Remote access');
define('USERS_ACCESS_REMOTE_EX', 'remote client access');
define('USERS_ACCESS_WEB', 'Web access');
define('USERS_ACCESS_WEB_EX', 'viewer web access');
define('USERS_ACCESS_BACKUP', 'Archived video');
define('USERS_ACCESS_BACKUP_EX', 'allow access to previous events');
define('USERS_EMAIL', 'Emails');
define('USERS_EMAIL_EX', 'for email notifications');
define('EMAIL_LIMIT', 'Limit');
define('EMAIL_LIMIT_EX', 'Emails/hour (0 - unlimited)');
define('USERS_NOTES', 'User notes');
define('USERS_NOTES_EX', 'visible only to admin');
define('USERS_CHANGEPASSWORD', 'Change password on next login');
define('USERS_CHANGEPASSWORD_EX', 'User is asked to change password');

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

define('WARN_DEFAULT_PASSWORD', 'You have not changed the default password on this system. For security we highly recommend <a id="updateAdminPassword" href="#">updating your password</a>.');

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
define('DEVICE_EDIT_ONCAM', 'On-camera configuration'); 
define('VIDEO_ADJ', 'Video adjustments for');
define('DEVICE_UNNAMED', 'Unnamed');
define('TOTAL_DEVICES', 'Total devices');
define('ENABLE_ALL_PORTS', 'enable all ports');

define('VA_HEADER', 'Video adjustments');
define('VA_BRIGHTNESS', 'Brightness');
define('VA_HUE', 'Hue');
define('VA_AUDIO', 'Audio gain');
define('VA_AUDIO_ENABLE', 'Enable audio');
define('VA_SATURATION', 'Saturation');
define('VA_CONTRAST', 'Contrast');
define('VA_VIDEO_QUALITY', 'Video Quality');
define('VA_SETDEFAULT', 'Reset values to default');

define('DEVICE_ENCODING_UPDATED', 'Video standard setting changed. <hr /> <b>For the change to take effect you need to restart the server.</b><br>');
define('DB_FAIL_TRY_LATER', 'Could not apply changes. Please make sure you are able to connect to the DVR server and that the server is running.');

define('MMAP_HEADER', 'Motion detection settings');
define('MMAP_SELECTOR_TITLE', 'Use these selectors to choose sensitivity level to fill in the grid below');
define('BUFFER_TITLE', 'Choose how many second to record before and after motion events');
define('PRE_REC_BUFFER', 'Pre-event buffer');
define('POST_REC_BUFFER', 'Post-event buffer');
define('MOTION_ALGORITHM_TITLE', 'Motion detection algorithm');
define('MOTION_ALGORITHM', 'Algorithm');
define('MOTION_DEFAULT', 'Default');
define('MOTION_EXPERIMENTAL', 'Experimental');

define('SCHED_HEADER','Device recording schedule');
define('SCHED_SELECTOR_TITLE', 'Use these selectors to fill in the schedule below');
define('SCHED_OFF', 'Off');
define('SCHED_CONT', 'Continuous');
define('SCHED_MOTION', 'Motion');
define('SCHED_SENSOR', 'Sensor');
define('SCHED_TRIGGER', 'Trigger');
define('SCHED_FILLALL', 'Fill');


define('ALL_DEVICES', 'All devices');
define('EDITING_MMAP', 'Motion detection settings for camera:');
define('EDITING_SCHED', 'Recording schedule for camera:');
define('EDITING_GLOBAL_SCHED', 'Global schedule');
define('GLOBAL_SCHED_OG', 'Use device-specific schedule for this camera.');
define('SIGNAL_TYPE_NOT_CONFD', 'Configure at least one device first');
define('SCHED_GLOBAL', 'global');
define('SCHED_SPEC', 'device specific');
define('SCHED_SEL_TYPE', 'Please select use select the type of recording and click on desired day/hour to change recording type for that period.');

define('HIDE_IMG', 'Show/hide image');

define('AVAILABLE_DEVICES', 'Available video sources:');
define('LAYOUT_CONTROL', 'Select layout:');
define('NONE_SELECTED', 'None selected');
define('CLEAR_LAYOUT', 'Clear layout');
define('BACK_TO_ADMIN', 'Return to admin panel');

define('NO_NEWS', 'There are no news right now.');

define('NO_CARDS', 'There are no supported video capture cards detected on this system.');

define('NO_DEVICE_NAME', 'Name field can not be empty.');
define('EXIST_NAME', 'This name already exists.');

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
define('AU_KICK', 'End session (bans user for 5 minutes)');
define('AU_BAN', 'Ban user');
define('AU_CANT_EOS', 'You can not terminate your own session.');
define('AU_KICKED', 'User session terminated');
define('AU_CANT_SB', 'You can not ban yourself');
define('AU_BANNED', 'User is banned.');
define('AU_INFO', 'Kicking user will end session from that IP address, ban user for 5 minutes and require re-login. Banning user will remove all access priveleges (can be restored on user page).');
define('RCLIENT', 'Remote Client');
define('WEB', 'Web');
define('AC_ACCESS', '');
define('CLICK_HERE_TO_UPDATE', '[Yes]');

define('AIP_HEADER', 'Add an IP Camera');
define('DISCOVER_IP_CAMERAS', 'Discover IP Cameras');
define('AIP_NEW_LIST_AVAILABLE', 'It appears that a newer version of our supported camera list is available. Would you like to upgrade to this version?');
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
define('AIP_PORT', 'RTSP Port');
define('AIP_USER', 'Username');
define('AIP_PASS', 'Password');
define('AIP_MJPATH', 'Path to MJPEG');
define('AIP_RTSP', 'Path to RTSP');
define('AIP_ADD', 'Add camera');
define('AIP_ALREADY_EXISTS', 'Camera with these settings already exists in the list of the devices.');
define('AIP_NAME_ALREADY_EXISTS', 'Camera with this name already exists. Names must be unique.');
define('AIP_NEEDIP', 'Please enter ip address of the camera');
define('AIP_NEEDPORT', 'Please enter RTSP port number');
define('AIP_NEEDMJPEGPORT', 'Please enter MJPEG port number');
define('AIP_NEEDMJPEG', 'Please enter path to MJPEG');
define('AIP_NEEDRTSP', 'Please enter path to RTSP');
define('AIP_ADVANCED_SETTINGS', 'advanced settings');
define('AIP_CAMADDED', 'IP camera has been successfully added');
define('AIP_PROTOCOL', 'Video stream source');
define('AIP_ONVIF_PORT', 'ONVIF port');
define('AIP_CHECK_ONVIF_PORT', 'Check');
define('AIP_CHECK_ONVIF_SUCCESS', 'Successfull');
define('AIP_CHECK_ONVIF_ERROR', 'Unsuccessful');

#ip cam lib
define('COUND_NOT_CONNECT', 'Could not connect to the camera to perform the setup, please do so manually');

define('IPCAM_MODEL', 'Camera model');
define('IPCAM_DEBUG_LEVEL', 'Turn camera debugging on:');

define('DEVICE_EDIT_DETAILS', 'Properties');
define('DELETE_CAM', 'Delete');
define('PROPS_HEADER', 'IP Camera properties');
define('IPCAM_WIKI_LINK', 'Is your IP camera not in the list?  We maintain a larger list of MJPEG and RTSP paths <a href="http://www.bluecherrydvr.com/2012/01/technical-information-list-of-mjpeg-and-rtsp-paths-for-network-cameras/">here</a>.  If you run into problems, please <a href="https://www.hipchat.com/gDERje4QK">join us on HipChat</a> (https://www.hipchat.com/gDERje4QK) and we\'ll see if we can add support for it.');
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

#reencode
define('REENCODE_HEADER', 'Livestream reencoding configuration');
define('REENCODE_SUBHEADER', 'Livestream reencoding settings for camera');
define('REENCODE_ENABLED', 'Enable livestream reencoding');
define('REENCODE_BITRATE', 'Output bitrate');
define('REENCODE_RESOLUTION', 'Output resolution');
define('DEVICE_EDIT_REENCODE', 'Livestream reencoding settings');

#PTZ
define('DEVICE_EDIT_PTZ', 'PTZ settings');

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
define('LOGOUT', 'logout');
define('WRITE_FAILED', 'Failed to write to storage folder at: ');

#mjpeg
define('MJPEG_DISABLED', 'This device was disabled.');
define('MJPEG_DEVICE_NOT_FOUND', 'Device with ID %ID% was not found.');
define('MJPEG_NO_PERMISSION', 'You do not have permission to access camera ID %ID%.');
#storage 
define('STRAGE_HEADER', 'Manage storage locations');
define('ADD_LOCATION', 'Add location');
define('LOCATION', 'Folder:');
define('STORAGE_INFO_MESSAGE', 'Please note that if you add a new storage location, you need to make sure that: <br /> - folder exists <br /> - folder is empty <br /> - folder belongs to user bluecherry, group bluecherry.');
define('DIR_DOES_NOT_EXIST_OR_NOT_READABLE', 'Server could not open the specified directory "<b>%PATH%</b>", please make sure it exists and the server can connect to it by running these commands in the terminal: <br /><br />
		To create directory run: "sudo mkdir -p %PATH%" <br />
		To set permissions run: "sudo chmod 770 %PATH%; sudo chown bluecherry:bluecherry -R %PATH%".
	');
define('DIR_NOT_READABLE', 'Specified directory exists, but is not readable, please make sure that the server has permissions to access it by running these commands in the terminal:<br /><br />
		To set permissions run: "sudo chmod 770 %PATH%; sudo chown bluecherry:bluecherry -R %PATH%".
	');
define('DIR_NOT_EMPTY', 'Specified directory is not empty, all contents will be deleted after it is added.');
define('DIR_OK', 'Specified directory exists and is writable. Click "save" to add this location.');


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
define('IPP_NEW', 'Create a custom set of PTZ commands');
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
define('IPP_PRESET_PROTOCOL', 'Protocol/Interface');
define('IPP_PRESET_PORT', 'Control port');
define('IPP_PRESET_PORT_EX', 'Specify port, if non-standard port is used');
define('IPP_DISABLE_PTZ', 'Disable PTZ for this camera');

#statistics
define('STS_SUBHEADER', 'distribution of events by type and date range');
define('STS_HEADER', 'Event statistics');
define('STS_START_DATE', 'Start date');
define('STS_END_DATE', 'End Date');
define('STS_PR_GRP', 'Primary grouping');
define('STS_SC_GRP', 'Secondary grouping');
define('STS_ET_ALL', 'All events');
define('STS_ET_M', 'Motion');
define('STS_ET_C', 'Continuous');
define('STS_SUBMIT', 'Get statistics');
define('STS_TOTAL_EVENTS', 'Total <b>%TYPE%</b> events for the period:');
define('STS_PICKER_HEADER', 'Choose date range and event types');
define('STS_PERCENTAGE_OF_TOTAL', 'Percent of total');
define('STS_NUM_EVENTS', 'Number of events');

#notifications
define('NTF_HEADER', 'Email notifications');
define('NTF_EXISTING', 'Current rules');
define('NTF_SUBHEADER', 'set up email notification rules');
define('NTF_ADD_RULE', 'Add a rule');
define('NTF_EDIT_RULE', 'Save changes');
define('NTF_ADD_RULE_TITLE', 'Add a new rule for notifications');
define('NTF_ADD_RULE_DATETIME', 'Select days and time for notifications');
define('NTF_DAY', 'Day of the week');
define('NTF_START_TIME', 'Start time');
define('NTF_END_TIME', 'End time');
define('NTF_CAMERAS', 'choose cameras');
define('NTF_USERS', 'users to notify');
define('NTF_LIMIT', 'Maximum notification limit');
define('NTF_LIMIT_LABEL', 'Limit of emails sent in the per hour [0 for unlimited]');
define('NTF_NO_RULES', 'There are currently no notification rules');
define('NTF_EXISTING_RULES', 'Notification rules currently in place');

define('NTF_E_USERS', 'You must select at least one user');
define('NTF_E_DAYS', 'You must select at least one day of the week');
define('NTF_E_TIME', 'Start time must be before end time');
define('NTF_E_CAMS', 'You must select at least one camera');

define('NTF_TEMPLATE', 'Send an email to <b>%USERS%</b> <br />on <b>%DAYS%</b> <br />between <b>%STIME%</b> and <b>%ETIME%</b> <br />for events on <b>%CAMERAS%</b>.');
define('NTF_T_TO', 'Send email to');
define('NTF_T_DOW', 'on days');
define('NTF_T_TIME', 'between hours');
define('NTD_T_CAMS', 'for events on');
define('NTD_T_LIM', 'emails per hour');


define('DW_M', 'Monday');
define('DW_T', 'Tuesday');
define('DW_W', 'Wednesday');
define('DW_R', 'Thursday');
define('DW_F', 'Friday');
define('DW_S', 'Saturday');
define('DW_U', 'Sunday');

#backup/restore
define('BACKUP_HEADER', 'Backup and restore your database');
define('BACKUP_R_CONF', 'Restoring your database will permanently remove all current data. We sttongly suggest performing current backup before using restore.');
define('BACKUP_SUBHEADER', 'save a backup of the database or restore from a saved backup');
define('BACKUP_B_TITLE', 'Backup the database');
define('BACKUP_PWD', 'Please enter the password for the database user <b>%USERS%</b> to for the <b>%DBNAME%</b> database:');
define('BACKUP_B_OPT', 'Additional options');
define('BACKUP_B_OPT_NE', 'Do not backup events data');
define('BACKUP_B_OPT_ND', 'Do not backup device(s) configuration');
define('BACKUP_B_OPT_NU', 'Do not backup user records');
define('BACKUP_R_TITLE', 'Restore the database');
define('BACKUP_R_WRONG_FILETYPE', 'File is expected to be a compressed tar archive. Please upload a proper backup file.');
define('BACKUP_R_NOINFO', 'Could not read the backup information file. This may mean that the archive is corrupted or contains invalid data. Proceeding may cause loss of data.');
define('BACKUP_R_LABEL', 'Please choose a previously saved backup:');
define('BACKUP_R_INFO', 'Backup files uploaded. <br /> <b>Backup date:</b> %DATE% <br /> <b>Event data:</b> %E% <br /><b>Device data:</b> %D%<br /><b>User data:</b> %U%');
define('BACKUP_FAILED', 'Could not perform the backup. Is the database password correct?');
define('BACKUP_R_FAILED', 'Could not restore the database. Mysql returned the following error:<br />');
define('BACKUP_R_SUCCESS', 'The database successfully restored. Please reload the page.');
define('U_BACKUP', 'Backup');
define('U_UPLOAD', 'Upload');
define('U_RESTORE', 'Restore');
define('U_CANCEL', 'Cancel');
define('U_INCLUDED', 'Included');
define('U_NOTINCLUDED', 'Not included');
define('BACKUP_R_WARN', 'Please note that all tables in the backup will be irreversibly overwritten once restore is complete. Please enter the database password to continue.');
define('BACKUP_DOWNLOAD', 'Download backup file');
define('BACKUP_B_SUCCESS', 'Created');

#licensing
define('L_ADDCODE', 'Add a support ID code');
define('L_ADD', 'Add');
define('L_PORTS', 'Ports');
define('L_CONFIRMCODE', 'Confirm a support ID code');
define('L_CONFIRMCODE_MSG', 'Unfortunately we could not confirm the code automatically. Please check your internet connection and try again, or <a href="mailto:support@bluecherrydvr.com">contact</a> Bluecherry to obtain authorization code using this unique id: ');
define('L_INVALID_LICENSE', 'The code you entered does not appear to be valid. Please make sure that it is typed in or copied exactly as provided. If you continue to experience this problem, please <a href="mailto:support@bluecherrydvr.com">contact</a> Bluecherry.');
define('L_INVALID_LICENSE_EXISTS', 'The code you entered is already in use on this system.');
define('L_INVALID_CONFIRMATION', 'Confirmation code you entered is invalid. Please make sure that it is typed in or copied exactly as provided. If you continue to experience this problem, please <a href="mailto:support@bluecherrydvr.com">contact</a> Bluecherry.');
define('L_LICENSE_ADDED', 'The code was successfully activated.');
define('L_CURRENT',  'Active  codes');
define('L_CONFIRM', 'Confirm');
define('L_CONFIRM_DELETE', 'Are you sure you want to remove this code?');
define('L_NO_LICENSES', 'You have no active support ID keys');
define('L_AUTO_RESP_1', 'This code does not exist.');
define('L_AUTO_RESP_2', 'This key was already used on a different machine. If you recently made changes to your computer configuration, please <a href="mailto:support@bluecherrydvr.com">contact Bluecherry</a> to reset your license.');
define('L_AUTO_RESP_3', 'Sorry, you made too many unsuccessful authorization attempts per day. Please <a href="mailto:support@bluecherrydvr.com">contact Bluecherry</a> or try again later.');
define('L_AUTO_RESP_4', 'Sorry, you need to contact Bluecherry via phone or email to obtain authorization.');
define('L_AUTO_RESP_5', 'Sorry, an error was encountered while authorizing your license. Please try again later or <a href="mailto:support@bluecherrydvr.com">contact Bluecherry</a>.');


#Cam specific
define('ACTI_STREAMING_SET_3', 'Your ACTi camera has been automatically configured for RTP over UDP streaming.');
define('ACTI_STREAMING_N_SET_3', 'Your ACTi camera could not be automatically configured for RTP over UDP streaming. Please do so manually.');

define('VIVOTEC_SETRAMING_SET', 'Your Vivotec camera has been automatically configured to use MJPEG codec on the second encoder.');
define('VIVOTEC_SETRAMING_N_SET', 'Your Vivotec camera could not be automatically configured to use MJPEG codec on the second encoder. Please do so manually.');

define('L_DELETE', 'delete');
define('L_EDIT', 'edit');
define('L_NONE', 'none');
define('U_LOADING', 'Loading...');
define('L_ENABLE', 'enable');
define('L_DISABLE', 'disable');

define('BETA_LICENSE_WARNING', 'You are using the community version of our software, for support options please click here.<br><a href=https://www.bluecherrydvr.com/licenses>www.bluecherrydvr.com/licenses</a><br>');

#ip cam props
define('IPCAM_TCPUDP_LEVEL', 'Transport');

#playback
define('PLAYBACK_FILE_NOT_FOUND', 'MKV file was not found.');

?>
