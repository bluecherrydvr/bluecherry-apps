#!/bin/bash

echo '
CREATE TABLE `GpioConfig` (
  `card_id` varchar(255) NOT NULL,
  `input_pin_id` int(11) NOT NULL,
  `trigger_output_pins` varchar(255),
  `trigger_devices` varchar(255),
  UNIQUE KEY `uniq_input_pin` (`card_id`,`input_pin_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
' | mysql -D"$dbname" -u"$user" -p"$password"
