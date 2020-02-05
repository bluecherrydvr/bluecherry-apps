INSERT INTO Users (`username`, `password`, `salt`, `name`, `email`, `phone`, `notes`, `access_setup`, `access_remote`, `access_web`, `access_backup`, `access_relay`) VALUES ('Admin', 'b22dec1d6cfa580962f3a3796a5dc6b3', '1234', 'Bluecherry Admin', NULL, NULL, NULL, 1, 1, 1, 1, 1);

INSERT INTO GlobalSettings VALUES ('G_DVR_NAME', 'Bluecherry DVR v3');
INSERT INTO GlobalSettings VALUES ('G_DVR_EMAIL', 'root@localhost');
INSERT INTO GlobalSettings VALUES ('G_DEV_SCED', 'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC');
INSERT INTO GlobalSettings VALUES ('G_DEV_NOTES', '');
INSERT INTO GlobalSettings VALUES ('G_IPCAMLIST_VERSION', '1.0');
INSERT INTO GlobalSettings VALUES ('G_LAST_SOFTWARE_VERSION_CHECK', 0);
INSERT INTO GlobalSettings VALUES ('G_DISABLE_VERSION_CHECK', '0');
INSERT INTO GlobalSettings VALUES ('G_DISABLE_IP_C_CHECK', '1');
INSERT INTO GlobalSettings VALUES ('G_DISABLE_WEB_STATS', '0');

INSERT INTO GlobalSettings VALUES ('G_SMTP_USERNAME', NULL);
INSERT INTO GlobalSettings VALUES ('G_SMTP_PASSWORD', NULL);
INSERT INTO GlobalSettings VALUES ('G_SMTP_HOST', NULL);
INSERT INTO GlobalSettings VALUES ('G_SMTP_PORT', '25');
INSERT INTO GlobalSettings VALUES ('G_SMTP_SSL', 'none');
INSERT INTO GlobalSettings VALUES ('G_SMTP_FAIL', NULL);
INSERT INTO GlobalSettings VALUES ('G_SMTP_SERVICE', 'default');
INSERT INTO GlobalSettings VALUES ('G_VAAPI_DEVICE', 'Autodetect');

INSERT INTO Storage VALUES (1, '/var/lib/bluecherry/recordings', 95.00, 90.00);

INSERT INTO EventLevels VALUES ('info');
INSERT INTO EventLevels VALUES ('warn');
INSERT INTO EventLevels VALUES ('alrm');
INSERT INTO EventLevels VALUES ('crit');

INSERT INTO EventTypesCam VALUES ('motion');
INSERT INTO EventTypesCam VALUES ('continuous');
INSERT INTO EventTypesCam VALUES ('not found');
INSERT INTO EventTypesCam VALUES ('video signal loss');
INSERT INTO EventTypesCam VALUES ('audio signal loss');
INSERT INTO EventTypesCam VALUES ('user');

INSERT INTO EventTypesSys VALUES ('disk-space');
INSERT INTO EventTypesSys VALUES ('crash');
INSERT INTO EventTypesSys VALUES ('boot');
INSERT INTO EventTypesSys VALUES ('shutdown');
INSERT INTO EventTypesSys VALUES ('reboot');
INSERT INTO EventTypesSys VALUES ('power-outage');

INSERT INTO TagNames (name) VALUES ('suspicious');
INSERT INTO TagNames (name) VALUES ('burglary');
INSERT INTO TagNames (name) VALUES ('theft');
INSERT INTO TagNames (name) VALUES ('shoplifting');
INSERT INTO TagNames (name) VALUES ('customer');
INSERT INTO TagNames (name) VALUES ('employee');
INSERT INTO TagNames (name) VALUES ('person');
INSERT INTO TagNames (name) VALUES ('incident');
INSERT INTO TagNames (name) VALUES ('vandalism');

INSERT INTO `ipPtzCommandPresets` (`id`, `name`, `mright`, `mleft`, `up`, `down`, `up_right`, `up_left`, `down_right`, `down_left`, `wide`, `tight`, `focus_in`, `focus_out`, `preset_save`, `preset_go`, `stop`, `needs_stop`, `http_auth`, `custom`, `port`, `protocol`) VALUES
(1, 'Pixord', '/SetPTZ.cgi?Dir=Right', '/SetPTZ.cgi?Dir=Left', '/SetPTZ.cgi?Dir=Up', '/SetPTZ.cgi?Dir=Down', '', '', '', '', '/SetPTZ.cgi?Dir=ZoomOut', '/SetPTZ.cgi?Dir=ZoomIn', '/SetPTZ.cgi?Dir=FocusNear', '/SetPTZ.cgi?Dir=FocusFar', '', '', '', 0, 1, 0, '80', 'http'),
(2, 'ACTi', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=RIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=LEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UP', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWN', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UPRIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UPLEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWNRIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWNLEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&ZOOM=WIDE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&ZOOM=TELE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&FOCUS=CLOSE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&FOCUS=FAR', '/cgi-bin/cmd/encoder?USER=%USERNAME%&PWD=%PASSWORD%&PTZ_PRESET_SET=%ID%', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&PTZ_PRESET_GO=%ID%', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=STOP', 1, 0, 0, '80', 'http'),
(3, 'Panasonic', '/cgi-bin/camctrl?pan=Right', '/cgi-bin/camctrl?pan=Left', '/cgi-bin/camctrl?tilt=Up', '/cgi-bin/camctrl?tilt=Down', '', '', '', '', '/cgi-bin/camctrl?times=up', '/cgi-bin/camctrl?times=down', '/cgi-bin/camctrl?focus=near', '/cgi-bin/camctrl?focus=far', '', '', '', 0, 1, 0, '80', 'http');
