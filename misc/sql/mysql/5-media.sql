-- Media listings
CREATE TABLE Media (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	start integer NOT NULL,
	end integer NOT NULL DEFAULT 0, -- 0 means in-progress
	size integer, -- NULL means there was an error in stating the file
	device_id integer NOT NULL,
	container varchar(10) NOT NULL, -- e.g. mkv
	video varchar(10) NOT NULL, -- e.g. m4v
	audio varchar(10) NOT NULL, -- e.g. mp3
	filepath varchar(1024) NOT NULL,
	archive boolean NOT NULL DEFAULT FALSE,
	FOREIGN KEY (device_id) REFERENCES Devices(id)
);
