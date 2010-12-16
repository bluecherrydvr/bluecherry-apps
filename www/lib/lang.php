<?php //defined('INDVR') or exit();

#company global
define(DVR_COMPANY_NAME, 'Bluecherry');
define(DVR_DVR, 'DVR');

#headers
define(PAGE_HEADER_MAIN, 'Administration');
define(PAGE_HEADER_VIEWER, 'Live View');
define(PAGE_HEADER_SCHED, 'Access is not allowed at this time');

define(PAGE_HEADER_LOGIN, 'Login');
define(PAGE_HEADER_SEPARATOR, ' :: ');

#main menu
define(MMENU_NEWS, 'Announcements');
define(MMENU_GS, 'General settings');
define(MMENU_STORAGE, 'Storage');
define(MMENU_USERS, 'Users');
define(MMENU_LOG, 'System Log');
define(MMENU_DEVICES, 'Devices');
define(MMENU_SCHED, 'Recording schedule');
define(MMENU_LIVEVIEW, 'LiveView');
define(MMENU_ACTIVE_USERS, 'Connected users');
#main
define(COUND_NOT_OPEN_PAGE, 'Page could not be loaded');
define(COUND_NOT_OPEN_PAGE_EX, 'The page requested could not be loaded at this time. Please make sure you are able to connect to the DVR server and that the server is running.');

#dsched
define(DSCED_HEADER, 'Access to live view is not allowed for this account at this time.');
define(DSCED_MSG, 'You can not use web interface at this time. If this is a mistake, please contact your system administrator.');


#die (fatal) messages
define(LANG_DIE_COULDNOTCONNECT, 'Fatal error: could not connect to the database: ');
define(LANG_DIE_COULDNOTOPENCONF, 'Fatal error: could not read configuration file.');

#general
define(BB_MAP_ARROW, '>');
define(SAVE_SETTINGS, 'Save settings');
define(SAVE_CHANGES, 'Save changes');

define(CHANGES_OK, 'Changes successfully saved');
define(CHANGES_FAIL, 'Could not apply changes at this time, please try later.');

define(ENABLE_DEVICE_NOTENOUGHCAP, '<![CDATA[<b>Card does not have sufficient capacity to accomodate this device. <br/></b>Please reduce Resolution/Framerate of other deivces on this card.]]>');
define(NEW_DEV_NEFPS, 'The device was configured using generic configuration, but there is insufficient card capacity to enable it. Please reduce framerate/resolution of other devices and try again.');

#general settings
define(GLOBAL_SETTINGS, 'DVR General settings');
define(G_DVR_NAME, 'DVR Name');
define(G_DVR_NAME_EX, 'You can name you dvr after its location or usage');
define(G_DVR_EMAIL, 'Email address');
define(G_DVR_EMAIL_EX, 'Email for notifications');
define(G_DEV_NOTES, 'Notes');
define(G_DEV_NOTES_EX, 'dvr memos');

#users page
define(USERS_ALL_TABLE_HEADER, 'DVR Users');
define(USERS_DETAIL_TABLE_HEADER, 'User details: ');

define(USERS_ALL_TABLE_SUBHEADER, 'Users currently registered in the DVR system.');
define(USERS_DETAIL_TABLE_SUBHEADER, 'Edit user information and access rights. ');

define(USERS_TABLE_HEADER_NAME, 'Full Name');
define(USERS_TABLE_HEADER_LOGIN, 'Login');
define(USERS_TABLE_HEADER_EMAIL, 'Email');
define(USERS_TABLE_HEADER_STATUS, 'Status');
define(USERS_LIST, 'DVR users');

define(DELETE_USER, 'Delete user');
define(DELETE_USER_SELF, 'You can not remove your own user record');
define(USER_DELETED_OK, 'User record successfully deleted');

define(USERS_NEW, 'Create New User');
define(USERS_NAME, 'Name');
define(USERS_NAME_EX, 'user\'s full name');
define(USERS_LOGIN, 'Login');
define(USERS_LOGIN_EX, 'login');
define(USERS_PASSWORD, 'Password');
define(USERS_PASSWORD_EX, 'password');
define(USERS_PHONE, 'Phone');
define(USERS_PHONE_EX, 'phone #');
define(USERS_ACCESS_SETUP, 'Admin access');
define(USERS_ACCESS_SETUP_EX, 'administrative priveleges');
define(USERS_ACCESS_REMOTE, 'Remote access');
define(USERS_ACCESS_REMOTE_EX, 'remote client access');
define(USERS_ACCESS_WEB, 'Web access');
define(USERS_ACCESS_WEB_EX, 'viewer web access');
define(USERS_ACCESS_BACKUP, 'Backup access');
define(USERS_ACCESS_BACKUP_EX, 'recorded videos');
define(USERS_EMAIL, 'Email');
define(USERS_EMAIL_EX, 'email');
define(USERS_NOTES, 'User notes');
define(USERS_NOTES_EX, 'visible only to admin');


define(USERS_STATUS_ADMIN, 'Administrator');
define(USERS_STATUS_VIEWER,'Viewer');
define(USERS_STATUS_DISABLE, 'Disabled');


define(USERS_STATUS_SETUP, 'Admin');
define(USERS_STATUS_VIEWER, 'Viewer');

#log
define(LOG_EMPTY, 'Current log file is empty.');
define(LOG_ALL, 'Whole log');
define(LOG_COULD_NOT_OPEN, 'Could not open log file');
define(LOG_HOW_MANY, 'Bottom lines to load...');
define(LOG_FILE_DOES_NOT_EXIST, 'Log file does not exist');

define(SCROLL_TO_BOTTOM, 'scroll to bottom');
define(SCROLL_TO_TOP, 'scroll to top');

#news
define(NEWS_HEADER, DVR_COMPANY_NAME.' Announcements');

#login
define(LOGIN_WRONG, 'Wrong login/password combination, please try again.');
define(LOGIN_OK, 'Please wait while you are redirected...');

#devices
define(CARD_HEADER, 'Bluecherry hardware compression');
define(CARD_CHANGE_ENCODING, 'click to change encoding');
define(LOCAL_DEVICES, 'Local devices:');
define(PORT, 'Port');
define(DEVICE_VIDEO_SOURCE, 'Video source');
define(DEVICE_VIDEO_STATUS, 'Device status');
define(DEVICE_VIDEO_STATUS_OK, 'Enabled');
define(DEVICE_VIDEO_STATUS_CHANGE_OK, 'Disable');
define(DEVICE_VIDEO_STATUS_disabled, 'Disabled');
define(DEVICE_VIDEO_STATUS_CHANGE_disabled, 'Enable');
define(DEVICE_VIDEO_NAME_notconfigured, 'Not configured');
define(DEVICE_VIDEO_STATUS_notconfigured, 'Disabled');
define(DEVICE_VIDEO_STATUS_CHANGE_notconfigured, 'Enable');
define(DEVICE_VIDEO_RESFPS, 'Device resolution/FPS');
define(DEVICE_EDIT_MMAP, 'Motion detection settings');
define(DEVICE_EDIT_VIDADJ, 'Video/Audio adjustment settings');
define(DEVICE_EDIT_SCHED, 'Schedule');
define(VIDEO_ADJ, 'Video adjustments for');
define(DEVICE_UNNAMED, 'Unnamed');
define(TOTAL_DEVICES, 'Total devices: ');

define(VA_HEADER, 'Video adjustments');
define(VA_BRIGHTNESS, 'Brightness');
define(VA_HUE, 'Hue');
define(VA_AUDIO, 'Audio gain');
define(VA_SATURATION, 'Staruration');
define(VA_CONTRAST, 'Contrast');
define(VA_SETDEFAULT, 'Reset values to default');

define(DEVICE_ENCODING_UPDATED, 'Video standard changed.');
define(DB_FAIL_TRY_LATER, 'Could not apply changes. Please make sure you are able to connect to the DVR server and that the server is running.');

define(MMAP_HEADER, 'Motion detection settings');
define(SCHED_HEADER,'Device recording schedule');
define(ALL_DEVICES, 'All devices');
define(EDITING_MMAP, 'Motion detection settings for camera:');
define(EDITING_SCHED, 'Recording schedule for camera:');
define(EDITING_GLOBAL_SCHED, 'Global schedule');
define(GLOBAL_SCHED_OG, 'Use device-specific schedule for this camera.');
define(SIGNAL_TYPE_NOT_CONFD, 'Configure at least one device first');

define(AVAILABLE_DEVICES, 'Available video sources:');
define(LAYOUT_CONTROL, 'Select layout:');
define(BACK_TO_ADMIN, 'Return to admin panel');

define(NO_NEWS, 'There are no news right now.');

define(NO_CARDS, 'No bluecherry cards were detected. Please make sure the card is in the computer and run <b>sudo bc-detect</b>');

/*user page errors*/
define(NO_USERNAME, 'Login field can not be empty.');
define(NO_EMAIL, 'Email field can not be empty.');
define(NO_PASS, 'Password field can not be empty.');
define(CANT_REMOVE_ADMIN, 'You can not remove your own admin priveleges.');
define(USER_CREATED, 'New user record created.');
define(USER_DELETED, 'User record deleted.');


#stats
define(STATS_HEARDER, 'Server statistics:');
define(STATS_CPU, 'CPU usage:');
define(STATS_MEM, 'Memory usage:');
define(STATS_UPT, 'Server uptime:');

#messages general
define(USER_KICKED, 'Your session was terminated by an Administrator.');
define(USER_NACCESS, 'You need administrator priveleges to access this page. Please <a href="/">login again</a>.');
define(USER_RELOGIN, 'Your session has expired, please <a htef="/">relogin</a>. ');

#active users 
define(ACTIVE_USERS_HEADER, 'Connected users');
define(ACTIVE_USERS_SUBHEADER, 'Users currently using this DVR server');
define(AU_IP, 'IP address');
define(AU_KICK, 'Kick (IP)');
define(AU_BAN, 'Ban user');
define(AU_CANT_EOS, 'You can not terminate your own session.');
define(AU_KICKED, 'User session terminated');
define(AU_CANT_SB, 'You can not ban yourself');
define(AU_BANNED, 'User is banned.');
define(AU_INFO, 'Kicking user will end session from that IP address, ban user for 5 minutes and require re-login. Banning user will remove all access priveleges (can be restored on user page).');
define(RCLIENT, 'Remote Client');
define(WEB, 'Web');
define(AC_ACCESS, '');

define(AIP_HEADER, 'Add an IP Camera');
define(AIP_SUBHEADER, '');

#login
define(PLEASE_LOGIN, 'You need');
?>
