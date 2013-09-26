create table DevicesStatus (
	device_id integer NOT NULL,
	status_code tinyint(4) NOT NULL,
	status_timestamp integer NOT NULL,
	PRIMARY KEY (device_id),
	CONSTRAINT fk_Devices_id FOREIGN KEY (device_id) REFERENCES Devices (id) ON DELETE CASCADE
) ENGINE=INNODB DEFAULT CHARSET=latin1;