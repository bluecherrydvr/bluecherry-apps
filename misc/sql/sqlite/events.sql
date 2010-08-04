CREATE TABLE EventLevels (
	id varchar(10) PRIMARY KEY NOT NULL,
);

CREATE TABLE EventTypesCam (
	id varchar(10) PRIMARY KEY NOT NULL,
);

CREATE TABLE EventTypesSys (
	id varchar(10) PRIMARY KEY NOT NULL,
);

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

CREATE TABLE EventsSystem (
	id integer PRIMARY KEY NOT NULL,
	date date,
	level_id varchar(10),
	type_id varchar(10),
	FOREIGN KEY (level_id) REFERENCES EventLevels(id),
	FOREIGN KEY (type_id) REFERENCES EventTypesCam(id)
);

CREATE TABLE EventComments (
	id integer PRIMARY KEY NOT NULL,
	date date,
	event_id integer,
	user_id integer,
	comment varchar,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (users_id) REFERENCES Users(id)
);

CREATE TABLE TagNames (
	id varchar(10) PRIMARY KEY NOT NULL
);

CREATE TABLE EventTags (
	id integer PRIMARY KEY NOT NULL,
	date date,
	event_id integer,
	tag_id varchar(10),
	user_id integer,
	FOREIGN KEY (event_id) REFERENCES EventsCam(id),
	FOREIGN KEY (tag_id) REFERENCES TagNames(id),
	FOREIGN KEY (users_id) REFERENCES Users(id)
);
