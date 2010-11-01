CREATE TABLE Users (
	id integer PRIMARY KEY NOT NULL,
	username varchar(40),
	password varchar(64),
	salt CHAR(4),
	name varchar(60),
	email varchar(60),
	phone varchar(15),
	notes varchar(255),
	access_setup boolean,
	access_remote boolean,
	access_web boolean,
	access_backup boolean,
	access_relay boolean,
	access_device_list varchar(255),
	access_ptz_list varchar(255),
	access_schedule varchar(168)
);
