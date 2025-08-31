
/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
DROP TABLE IF EXISTS `ActiveUsers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ActiveUsers` (
  `id` int(11) DEFAULT NULL,
  `ip` varchar(64) DEFAULT NULL,
  `from_client` tinyint(1) DEFAULT NULL,
  `time` int(11) DEFAULT NULL,
  `kick` tinyint(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `AvailableSources`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `AvailableSources` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device` varchar(255) DEFAULT NULL,
  `driver` varchar(255) DEFAULT NULL,
  `card_id` varchar(255) NOT NULL,
  `video_type` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `device` (`device`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Devices`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Devices` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_name` varchar(100) DEFAULT NULL,
  `protocol` varchar(50) DEFAULT NULL,
  `device` varbinary(256) DEFAULT NULL,
  `driver` varchar(255) DEFAULT NULL,
  `rtsp_username` varbinary(256) DEFAULT NULL,
  `rtsp_password` varbinary(256) DEFAULT NULL,
  `rtsp_rtp_prefer_tcp` smallint(5) NOT NULL DEFAULT '0',
  `mjpeg_path` varbinary(256) DEFAULT NULL,
  `onvif_port` mediumint(5) DEFAULT '80',
  `ptz_control_path` varbinary(256) DEFAULT NULL,
  `ptz_control_protocol` varchar(32) DEFAULT NULL,
  `ptz_serial_values` varchar(64) DEFAULT NULL,
  `resolutionX` smallint(6) DEFAULT NULL,
  `resolutionY` smallint(6) DEFAULT NULL,
  `model` varchar(64) DEFAULT NULL,
  `channel` int(11) NOT NULL DEFAULT '0',
  `invert` tinyint(1) NOT NULL DEFAULT '0',
  `audio_volume` smallint(6) DEFAULT '50',
  `saturation` smallint(6) DEFAULT '50',
  `brightness` smallint(6) DEFAULT '50',
  `hue` smallint(6) DEFAULT '50',
  `contrast` smallint(6) DEFAULT '50',
  `video_quality` smallint(6) DEFAULT '100',
  `video_interval` smallint(6) DEFAULT NULL,
  `signal_type` varchar(6) DEFAULT NULL,
  `buffer_prerecording` smallint(6) DEFAULT '3',
  `buffer_postrecording` smallint(6) DEFAULT '3',
  `motion_map` varchar(768) NOT NULL DEFAULT '333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333',
  `motion_algorithm` tinyint(1) DEFAULT '1',
  `frame_downscale_factor` decimal(4,3) DEFAULT '0.500',
  `min_motion_area` smallint(6) DEFAULT '3',
  `max_motion_area` smallint(5) DEFAULT '90',
  `motion_analysis_ssw_length` int(11) DEFAULT '-1',
  `motion_analysis_percentage` int(11) DEFAULT '-1',
  `schedule` varchar(168) NOT NULL DEFAULT 'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC',
  `schedule_override_global` tinyint(1) DEFAULT '0',
  `disabled` tinyint(1) DEFAULT '0',
  `audio_disabled` tinyint(1) DEFAULT '1',
  `debug_level` tinyint(4) DEFAULT '0',
  `reencode_livestream` tinyint(1) DEFAULT '0',
  `reencode_bitrate` int(11) NOT NULL DEFAULT '64000',
  `reencode_frame_width` smallint(6) NOT NULL DEFAULT '352',
  `reencode_frame_height` smallint(6) NOT NULL DEFAULT '240',
  `substream_mode` tinyint(1) DEFAULT '0',
  `substream_path` varchar(255) DEFAULT NULL,
  `onvif_events_enabled` tinyint(1) DEFAULT '0',
  `min_motion_frames` smallint(5) DEFAULT '15',
  `max_motion_frames` smallint(5) DEFAULT '20',
  `motion_blend_ratio` smallint(5) DEFAULT '15',
  `motion_debug` tinyint(1) DEFAULT '0',
  `hls_segment_duration` decimal(6,3) DEFAULT '3.000',
  `hls_segment_size` int(11) DEFAULT '2695168',
  `hls_window_size` smallint(5) DEFAULT '5',

  PRIMARY KEY (`id`),
  UNIQUE KEY `device_name` (`device_name`),
  UNIQUE KEY `device` (`device`,`mjpeg_path`,`protocol`,`channel`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventComments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventComments` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` int(11) NOT NULL,
  `event_id` int(11) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL,
  `comment` varchar(160) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `event_id` (`event_id`),
  KEY `user_id` (`user_id`),
  CONSTRAINT `EventComments_ibfk_1` FOREIGN KEY (`event_id`) REFERENCES `EventsCam` (`id`) ON DELETE CASCADE,
  CONSTRAINT `EventComments_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventLevels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventLevels` (
  `id` varchar(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventTags`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventTags` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` int(11) NOT NULL,
  `event_id` int(11) DEFAULT NULL,
  `tag_id` varchar(10) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `event_id` (`event_id`),
  KEY `user_id` (`user_id`),
  CONSTRAINT `EventTags_ibfk_1` FOREIGN KEY (`event_id`) REFERENCES `EventsCam` (`id`) ON DELETE CASCADE,
  CONSTRAINT `EventTags_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventTypesCam`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventTypesCam` (
  `id` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventTypesSys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventTypesSys` (
  `id` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventsCam`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventsCam` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` int(11) NOT NULL,
  `level_id` varchar(10) DEFAULT NULL,
  `device_id` int(11) DEFAULT NULL,
  `type_id` varchar(10) DEFAULT NULL,
  `length` int(11) DEFAULT '0',
  `archive` tinyint(1) NOT NULL DEFAULT '0',
  `media_id` int(11) DEFAULT NULL,
  `details` text,
  PRIMARY KEY (`id`),
  KEY `media_id` (`media_id`),
  KEY `level_id` (`level_id`),
  KEY `device_id` (`device_id`),
  KEY `type_id` (`type_id`),
  KEY `time_index` (`time`) USING BTREE,
  CONSTRAINT `EventsCam_ibfk_1` FOREIGN KEY (`media_id`) REFERENCES `Media` (`id`) ON DELETE CASCADE,
  CONSTRAINT `EventsCam_ibfk_2` FOREIGN KEY (`level_id`) REFERENCES `EventLevels` (`id`),
  CONSTRAINT `EventsCam_ibfk_3` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `EventsCam_ibfk_4` FOREIGN KEY (`type_id`) REFERENCES `EventTypesCam` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `EventsSystem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `EventsSystem` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` int(11) NOT NULL,
  `level_id` varchar(10) DEFAULT NULL,
  `type_id` varchar(10) DEFAULT NULL,
  `more` varchar(64) DEFAULT NULL,
  `details` text,
  PRIMARY KEY (`id`),
  KEY `level_id` (`level_id`),
  KEY `type_id` (`type_id`),
  CONSTRAINT `EventsSystem_ibfk_1` FOREIGN KEY (`level_id`) REFERENCES `EventLevels` (`id`),
  CONSTRAINT `EventsSystem_ibfk_2` FOREIGN KEY (`type_id`) REFERENCES `EventTypesSys` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `GlobalSettings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `GlobalSettings` (
  `parameter` varchar(60) NOT NULL,
  `value` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`parameter`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

DROP TABLE IF EXISTS `Media`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Media` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `start` int(11) NOT NULL,
  `end` int(11) NOT NULL DEFAULT '0',
  `size` bigint(20) DEFAULT NULL,
  `device_id` int(11) NOT NULL,
  `filepath` varchar(1024) NOT NULL,
  `archive` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `device_id` (`device_id`),
  CONSTRAINT `Media_ibfk_1` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `OnvifEvents`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `OnvifEvents` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_id` int(11) NOT NULL,
  `event_time` datetime NOT NULL,
  `onvif_topic` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `OnvifEvents_ibfk_1` (`device_id`),
  CONSTRAINT `OnvifEvents_ibfk_1` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `PTZPresets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PTZPresets` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_id` int(11) NOT NULL,
  `preset_id` int(11) NOT NULL,
  `preset_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `device_id_2` (`device_id`,`preset_id`),
  UNIQUE KEY `device_id` (`device_id`,`preset_name`),
  CONSTRAINT `PTZPresets_ibfk_1` FOREIGN KEY (`device_id`) REFERENCES `Devices` (`id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `RtspAuthTokens`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RtspAuthTokens` (
  `user_id` int(11) NOT NULL,
  `token` varchar(100) NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE KEY `uniq_session` (`user_id`,`token`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `HlsAuthTokens`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `HlsAuthTokens` (
  `user_id` int(11) NOT NULL,
  `token` varchar(100) NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE KEY `uniq_session` (`user_id`,`token`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `ServerStatus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ServerStatus` (
  `pid` int(11) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `message` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Storage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Storage` (
  `priority` int(11) NOT NULL,
  `path` varchar(255) NOT NULL,
  `max_thresh` decimal(5,2) NOT NULL,
  `min_thresh` decimal(5,2) NOT NULL,
  PRIMARY KEY (`priority`),
  UNIQUE KEY `path` (`path`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `TagNames`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `TagNames` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(40) DEFAULT NULL,
  `password` varchar(64) DEFAULT NULL,
  `salt` char(4) DEFAULT NULL,
  `name` varchar(60) DEFAULT NULL,
  `email` varchar(60) DEFAULT NULL,
  `phone` varchar(15) DEFAULT NULL,
  `notes` varchar(255) DEFAULT NULL,
  `access_setup` tinyint(1) DEFAULT NULL,
  `access_remote` tinyint(1) DEFAULT NULL,
  `access_web` tinyint(1) DEFAULT NULL,
  `access_backup` tinyint(1) DEFAULT NULL,
  `access_relay` tinyint(1) DEFAULT NULL,
  `access_device_list` varchar(255) DEFAULT '',
  `access_ptz_list` varchar(255) DEFAULT '',
  `access_schedule` varchar(168) DEFAULT '111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111',
  `change_password` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `ipPtzCommandPresets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ipPtzCommandPresets` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(128) NOT NULL,
  `driver` varchar(12) DEFAULT '',
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
  `stop_zoom` varchar(128) DEFAULT '',
  `needs_stop` tinyint(1) NOT NULL DEFAULT '0',
  `http_auth` tinyint(1) NOT NULL DEFAULT '1',
  `custom` tinyint(1) NOT NULL DEFAULT '0',
  `port` varchar(5) NOT NULL DEFAULT '80',
  `protocol` varchar(10) DEFAULT 'http',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;


DROP TABLE IF EXISTS `webhooks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `webhooks` (
	`id` SMALLINT(5) UNSIGNED NOT NULL AUTO_INCREMENT,
	`label` VARCHAR(50) NOT NULL,
	`url` VARCHAR(255) NOT NULL,
	`events` VARCHAR(255) NULL DEFAULT NULL,
	`cameras` VARCHAR(255) NULL DEFAULT NULL,
	`status` TINYINT(4) NOT NULL DEFAULT '0',
	`last_update` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;


DROP TABLE IF EXISTS `notificationSchedules`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `notificationSchedules` (
  `id` mediumint(9) NOT NULL AUTO_INCREMENT,
  `day` varchar(7) NOT NULL,
  `s_hr` tinyint(4) NOT NULL,
  `s_min` tinyint(4) NOT NULL,
  `e_hr` tinyint(4) NOT NULL,
  `e_min` tinyint(4) NOT NULL,
  `cameras` varchar(255) NOT NULL,
  `users` varchar(255) NOT NULL,
  `nlimit` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `disabled` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `notificationsSent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `notificationsSent` (
  `rule_id` smallint(5) unsigned NOT NULL,
  `time` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `userLayouts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `userLayouts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) DEFAULT NULL,
  `layout_name` varchar(50) DEFAULT NULL,
  `layout` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

