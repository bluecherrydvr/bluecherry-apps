CREATE TABLE IF NOT EXISTS `Notifications` (
  `id` int(11) NOT NULL,
  `event` varchar(20) NOT NULL,
  `event_prop` text NOT NULL,
  `notification_limit` tinyint(4) NOT NULL,
  `email` text NOT NULL,
  `template_id` tinyint(4) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `NotificationsSchedules` (
  `notification_id` int(11) NOT NULL,
  `weekdays` varchar(7) NOT NULL,
  `time_start` smallint(6) NOT NULL,
  `time_end` smallint(6) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `NotificationsTemplates` (
  `id` int(11) NOT NULL,
  `template_name` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `body` text NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `NotificationsSent` (
  `notification_id` int(11) NOT NULL,
  `email` varchar(120) NOT NULL,
  `time_sent` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;



