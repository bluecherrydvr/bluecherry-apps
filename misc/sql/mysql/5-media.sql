-- Media listings
CREATE TABLE Media (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	start integer NOT NULL,
	end integer NOT NULL DEFAULT 0, -- 0 means in-progress
	size integer, -- NULL means there was an error in stating the file
	device_id integer NOT NULL,
	filepath varchar(1024) NOT NULL,
	archive boolean NOT NULL DEFAULT FALSE,
	FOREIGN KEY (device_id) REFERENCES Devices(id)
);
