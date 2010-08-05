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

CREATE TABLE EventTypesSys (
	id varchar(10) PRIMARY KEY NOT NULL
);
INSERT INTO EventTypesSys VALUES ('disk-space');
INSERT INTO EventTypesSys VALUES ('crash');
INSERT INTO EventTypesSys VALUES ('boot');
INSERT INTO EventTypesSys VALUES ('shutdown');
INSERT INTO EventTypesSys VALUES ('reboot');
INSERT INTO EventTypesSys VALUES ('power-outage');

-- A separate table for camera events
CREATE TABLE EventsCam (
	id integer PRIMARY KEY NOT NULL,
	time timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
	level_id varchar(10),
	device_id integer,
	type_id varchar(10),
	length integer NOT NULL DEFAULT 0,	-- length of event in seconds
	archive boolean NOT NULL DEFAULT FALSE,	-- archive the event's video/audio?
	FOREIGN KEY (level_id) REFERENCES EventLevels(id)
		ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY (device_id) REFERENCES Devices(id)
		ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
		ON UPDATE CASCADE ON DELETE CASCADE
);

-- A separate table for system events
CREATE TABLE EventsSystem (
	id integer PRIMARY KEY NOT NULL,
	time timestamp DEFAULT CURRENT_TIMESTAMP,
	level_id varchar(10),
	type_id varchar(10),
	FOREIGN KEY (level_id) REFERENCES EventLevels(id)
		ON UPDATE CASCADE ON DELETE CASCADE,
	FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
		ON UPDATE CASCADE ON DELETE CASCADE
);

-- Comments on an event (Cam only?)
CREATE TABLE EventComments (
	id integer PRIMARY KEY NOT NULL,
	time timestamp DEFAULT CURRENT_TIMESTAMP,
	event_id integer,
	user_id integer,
	comment varchar,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id)
		ON UPDATE CASCADE ON DELETE CASCADE,
	FOREIGN KEY (user_id) REFERENCES Users(id)
		ON UPDATE CASCADE ON DELETE CASCADE
);

-- Event tags. We start with pre-defined ones and let the user change them
CREATE TABLE TagNames (
	id integer PRIMARY KEY NOT NULL,
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
	id integer PRIMARY KEY NOT NULL,
	time timestamp DEFAULT CURRENT_TIMESTAMP,
	event_id integer,
	tag_id varchar(10),
	user_id integer,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id)
		ON UPDATE CASCADE ON DELETE CASCADE,
	FOREIGN KEY (tag_id) REFERENCES TagNames(name)
		ON UPDATE CASCADE ON DELETE CASCADE,
	FOREIGN KEY (user_id) REFERENCES Users(id)
		ON UPDATE CASCADE ON DELETE CASCADE
);
