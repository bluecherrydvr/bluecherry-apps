CREATE TABLE `userLayouts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) DEFAULT NULL,
  `layout_name` varchar(50) DEFAULT NULL,
  `layout` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
);
