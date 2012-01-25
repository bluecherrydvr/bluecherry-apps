CREATE TABLE `ipPtzCommandPresets` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(128) NOT NULL,
  `mright` varchar(128) NOT NULL,
  `mleft` varchar(128) NOT NULL,
  `up` varchar(128) NOT NULL,
  `down` varchar(128) NOT NULL,
  `up_right` varchar(128) NOT NULL,
  `up_left` varchar(128) NOT NULL,
  `down_right` varchar(128) NOT NULL,
  `down_left` varchar(128) NOT NULL,
  `wide` varchar(128) NOT NULL,
  `tight` varchar(128) NOT NULL,
  `focus_in` varchar(128) NOT NULL,
  `focus_out` varchar(128) NOT NULL,
  `preset_save` varchar(128) NOT NULL,
  `preset_go` varchar(128) NOT NULL,
  `stop` varchar(128) NOT NULL,
  `stop_zoom` varchar(128) NOT NULL,
  `needs_stop` tinyint(1) NOT NULL DEFAULT '0',
  `http_auth` tinyint(1) NOT NULL DEFAULT '1',
  `custom` tinyint(1) NOT NULL DEFAULT '0',
  `port` varchar(5) NOT NULL DEFAULT '80',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

INSERT INTO `ipPtzCommandPresets` (`id`, `name`, `mright`, `mleft`, `up`, `down`, `up_right`, `up_left`, `down_right`, `down_left`, `wide`, `tight`, `focus_in`, `focus_out`, `preset_save`, `preset_go`, `stop`, `needs_stop`, `http_auth`, `custom`, `port`) VALUES
(1, 'Pixord', '/SetPTZ.cgi?Dir=Right', '/SetPTZ.cgi?Dir=Left', '/SetPTZ.cgi?Dir=Up', '/SetPTZ.cgi?Dir=Down', '', '', '', '', '/SetPTZ.cgi?Dir=ZoomOut', '/SetPTZ.cgi?Dir=ZoomIn', '/SetPTZ.cgi?Dir=FocusNear', '/SetPTZ.cgi?Dir=FocusFar', '', '', '', 0, 1, 0, '80'),
(2, 'ACTi', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=RIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=LEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UP', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWN', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UPRIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=UPLEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWNRIGHT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=DOWNLEFT', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&ZOOM=WIDE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&ZOOM=TELE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&FOCUS=CLOSE', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&FOCUS=FAR', '/cgi-bin/cmd/encoder?USER=%USERNAME%&PWD=%PASSWORD%&PTZ_PRESET_SET=%ID%', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&PTZ_PRESET_GO=%ID%', '/cgi-bin/encoder?USER=%USERNAME%&PWD=%PASSWORD%&MOVE=STOP', 1, 0, 0, '80');
