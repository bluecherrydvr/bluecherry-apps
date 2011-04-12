-- Event levels depict the severity of the event
CREATE TABLE EventLevels (
	id varchar(10) PRIMARY KEY NOT NULL
);
INSERT INTO EventLevels VALUES ('info');
INSERT INTO EventLevels VALUES ('warn');
INSERT INTO EventLevels VALUES ('alrm');
INSERT INTO EventLevels VALUES ('crit');

-- Event types are hard coded and specific to the location
CREATE TABLE EventTypesCam (
	id varchar(10) PRIMARY KEY NOT NULL
);
INSERT INTO EventTypesCam VALUES ('motion');
INSERT INTO EventTypesCam VALUES ('continuous');
INSERT INTO EventTypesCam VALUES ('not found');
INSERT INTO EventTypesCam VALUES ('video signal loss');
INSERT INTO EventTypesCam VALUES ('audio signal loss');
INSERT INTO EventTypesCam VALUES ('user');

CREATE TABLE EventTypesSys (
	id varchar(10) PRIMARY KEY NOT NULL
);
INSERT INTO EventTypesSys VALUES ('disk-space');
INSERT INTO EventTypesSys VALUES ('crash');
INSERT INTO EventTypesSys VALUES ('boot');
INSERT INTO EventTypesSys VALUES ('shutdown');
INSERT INTO EventTypesSys VALUES ('reboot');
INSERT INTO EventTypesSys VALUES ('power-outage');

-- A separate table for camera events. If length is -1, it means
-- this event is still going. If length is 0, it means the event
-- Did not take up any time (singular events).
CREATE TABLE EventsCam (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	time integer NOT NULL,
	level_id varchar(10),
	device_id integer,
	type_id varchar(10),
	length integer DEFAULT 0,		-- length of event in seconds
	archive boolean NOT NULL DEFAULT FALSE,	-- archive the event's video/audio?
	media_id integer,
	FOREIGN KEY (media_id) REFERENCES Media(id),
	FOREIGN KEY (level_id) REFERENCES EventLevels(id),
        FOREIGN KEY (device_id) REFERENCES Devices(id),
        FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
);

CREATE TABLE EventsCamSnapshot (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	event_id integer NOT NULL,
	file_size integer NOT NULL,
	file_data BLOB NOT NULL,
	file_type varchar(100) NOT NULL DEFAULT 'image/jpeg',
	FOREIGN KEY (event_id) REFERENCES EventsCam(id)
);

-- A separate table for system events
CREATE TABLE EventsSystem (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	time integer NOT NULL,
	level_id varchar(10),
	type_id varchar(10),
	more varchar(64),	-- E.g. disk-space could have more='/dev/sdb1'
	FOREIGN KEY (level_id) REFERENCES EventLevels(id),
	FOREIGN KEY (type_id) REFERENCES EventTypesSys(id)
);

-- Comments on an event (Cam only?)
CREATE TABLE EventComments (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	time integer NOT NULL,
	event_id integer,
	user_id integer,
	comment varchar(160),
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (user_id) REFERENCES Users(id)
);

-- Event tags. We start with pre-defined ones and let the user change them
CREATE TABLE TagNames (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	name varchar(10)
);
INSERT INTO TagNames (name) VALUES ('suspicious');
INSERT INTO TagNames (name) VALUES ('burglary');
INSERT INTO TagNames (name) VALUES ('theft');
INSERT INTO TagNames (name) VALUES ('shoplifting');
INSERT INTO TagNames (name) VALUES ('customer');
INSERT INTO TagNames (name) VALUES ('employee');
INSERT INTO TagNames (name) VALUES ('person');
INSERT INTO TagNames (name) VALUES ('incident');
INSERT INTO TagNames (name) VALUES ('vandalism');

CREATE TABLE EventTags (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	time integer NOT NULL,
	event_id integer,
	tag_id varchar(10),
	user_id integer,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (tag_id) REFERENCES TagNames(name),
	FOREIGN KEY (user_id) REFERENCES Users(id)
);
