DROP TABLE IF EXISTS EventLevels;
CREATE TABLE "EventLevels" (
   id integer PRIMARY KEY NOT NULL,
   name varchar
);

DROP TABLE IF EXISTS EventTypesCam;
CREATE TABLE "EventTypesCam" (
	id integer PRIMARY KEY NOT NULL,
	name varchar
);

DROP TABLE IF EXISTS EventTypesSys;
CREATE TABLE "EventTypesSys" (
	id integer PRIMARY KEY NOT NULL,
	name varchar
);

DROP TABLE IF EXISTS EventsCam;
CREATE TABLE "Events" (
    id integer PRIMARY KEY NOT NULL,
    date date,
    level_id integer, -- references EventLevels id
    device_id integer, -- references Devices id
    type_id integer, -- references EventTypesCam id
    length integer, -- length of event in seconds (motion type or similar)
    archive boolean -- Whether to archive the events video/audio (if applicable)
);

DROP TABLE IF EXISTS EventsSystem;
CREATE TABLE EventsSystem (
	id integer PRIMARY KEY NOT NULL,
	date date,
	level_id integer, -- references EventLevels id
	type_id integer -- references EventTypesSys id
);

DROP TABLE IF EXISTS EventComments;
CREATE TABLE "EventComments" (
    id integer PRIMARY KEY NOT NULL,
    date date,
    event_id integer, -- references event table
    user_id integer, -- references users table
    comment varchar
);

DROP TABLE IF EXISTS EventTags;
CREATE TABLE "EventTags" (
    id integer PRIMARY KEY NOT NULL,
    date date,
    event_id integer, -- references event table
    tag_id integer, -- references tag list table
    user_id integer -- references users table
);

-- User defined tags
DROP TABLE IF EXISTS TagList;
CREATE TABLE "TagList" (
    id integer PRIMARY KEY NOT NULL,
    name varchar
);
