CREATE TABLE "AvailableSources" (
    id integer PRIMARY KEY NOT NULL,
    device character(255),
    name character(255),
    status boolean,
    path character(255)
);

CREATE TABLE "Devices" (
    id integer PRIMARY KEY NOT NULL,
    device_name character(100),
    resolutionX smallint,
    resolutionY smallint,
    contol_path character(255),
    protocol character(50),
    remote_host character(255),
    remote_path character(255),
    remote_port character(5),
    invert boolean,
    source_video character(255),
    channel character(255),
    source_audio_in character(255),
    source_audio_out character(255),
    saturation smallint,
    brightness smallint,
    hue smallint,
    contrast smallint,
    video_quality smallint DEFAULT 2,
    signal_type character(6),
    buffer_prerecording smallint DEFAULT 5,
    buffer_postrecording smallint DEFAULT 5,
    ptz_contol_path character(255),
    ptz_control_protocol character(15),
    ptz_baud_rate character(6),
    ip_ptz_control_type smallint,
    preset_type_ID smallint,
    motion_detection_on boolean,
    motion_detection_threshold integer,
    file_chop_type smallint
);

/* file_chop_type: 0 = None, 1 = Hourly, 2 = Daily
 * Only counts when motion_detection_on == false */

-- video_quality: 1 = Low, 2 = Medium, 3 = High

CREATE TABLE "Events" (
    id integer PRIMARY KEY NOT NULL,
    source_ID integer,
    date date,
    file_path varchar,			-- actual file of the recorded event
    recording_type character(3),	-- file extension ?
    recording_length integer,		-- in seconds
    file_size integer,			-- in bytes
    archived boolean DEFAULT false
);

CREATE TABLE "GlobalSettings" (
    id integer PRIMARY KEY NOT NULL,
    parameter character(60),
    value character(255)
);

CREATE TABLE "Users" (
    id integer PRIMARY KEY NOT NULL,
    login character(40),
    password character(32),
    name character(60),
    email character(60),
    phone character(15),
    notes varchar,
    access_setup boolean,
    access_remote boolean,
    access_web boolean,
    access_backup boolean,
    access_relay boolean,
    access_device_list character(255),
    access_ptz_list character(255),
    access_schedule character(168)
);
