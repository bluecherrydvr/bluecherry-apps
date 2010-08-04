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
	date date,
	level_id varchar(10),
	device_id integer,
	type_id varchar(10),
	length integer,		-- length of event in seconds
	archive boolean,	-- archive the event's video/audio?
	FOREIGN KEY (level_id) REFERENCES EventLevels(id),
        FOREIGN KEY (device_id) REFERENCES Devices(id),
        FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
);

-- A separate table for system events
CREATE TABLE EventsSystem (
	id integer PRIMARY KEY NOT NULL,
	date date,
	level_id varchar(10),
	type_id varchar(10),
	FOREIGN KEY (level_id) REFERENCES EventLevels(id),
	FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
);

-- Comments on an event (Cam only?)
CREATE TABLE EventComments (
	id integer PRIMARY KEY NOT NULL,
	date date,
	event_id integer,
	user_id integer,
	comment varchar,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (user_id) REFERENCES Users(id)
);

-- Event tags. We start with pre-defined ones and let the user change them
CREATE TABLE TagNames (
	id integer PRIMARY KEY NOT NULL,
	name varchar(10)
);
INSERT INTO TagNames VALUES (0, 'suspicious');
INSERT INTO TagNames VALUES (1, 'burglary');
INSERT INTO TagNames VALUES (2, 'theft');
INSERT INTO TagNames VALUES (3, 'shoplifting');
INSERT INTO TagNames VALUES (4, 'customer');
INSERT INTO TagNames VALUES (5, 'employee');
INSERT INTO TagNames VALUES (6, 'person');
INSERT INTO TagNames VALUES (7, 'incident');

CREATE TABLE EventTags (
	id integer PRIMARY KEY NOT NULL,
	date date,
	event_id integer,
	tag_id varchar(10),
	user_id integer,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (tag_id) REFERENCES TagNames(name),
	FOREIGN KEY (user_id) REFERENCES Users(id)
);
