CREATE TABLE IF NOT EXISTS `notificationSchedules` (
  `id` mediumint(9) NOT NULL AUTO_INCREMENT,
  `day` varchar(7) NOT NULL,
  `s_hr` tinyint(4) NOT NULL,
  `s_min` tinyint(4) NOT NULL,
  `e_hr` tinyint(4) NOT NULL,
  `e_min` tinyint(4) NOT NULL,
  `cameras` varchar(255) NOT NULL,
  `users` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `notificationsSent` (
  `rule_id` smallint(5) unsigned NOT NULL,
  `time` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
