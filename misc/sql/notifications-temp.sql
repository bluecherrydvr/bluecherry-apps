CREATE TABLE `Notifications` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` smallint(5) unsigned DEFAULT NULL,
  `event_type` varchar(10) DEFAULT NULL,
  `device_id` smallint(5) unsigned DEFAULT NULL,
  `level_id` varchar(10) DEFAULT NULL,
  `notification_type` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`id`)
)
