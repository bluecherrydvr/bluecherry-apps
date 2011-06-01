kCREATE TABLE `ipCameraPTZDriver` (
  `id` varchar(32) NOT NULL,
  `move_up` varchar(256) DEFAULT NULL,
  `move_down` varchar(256) DEFAULT NULL,
  `move_right` varchar(256) DEFAULT NULL,
  `move_left` varchar(256) DEFAULT NULL,
  `move_upleft` varchar(256) DEFAULT NULL,
  `move_upright` varchar(256) DEFAULT NULL,
  `move_downleft` varchar(256) DEFAULT NULL,
  `move_downright` varchar(256) DEFAULT NULL,
  `move_stop` varchar(256) DEFAULT NULL,
  `zoom_in` varchar(256) DEFAULT NULL,
  `zoom_out` varchar(256) DEFAULT NULL,
  `focus_in` varchar(256) DEFAULT NULL,
  `focus_out` varchar(256) DEFAULT NULL,
  `preset_save` varchar(256) DEFAULT NULL,
  `preset_call` varchar(256) DEFAULT NULL,
  `preset_update` varchar(256) DEFAULT NULL,
  `preset_delete` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

