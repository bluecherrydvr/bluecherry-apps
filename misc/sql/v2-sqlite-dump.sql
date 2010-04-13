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
    saturation smallint DEFAULT 50,
    brightness smallint DEFAULT 50,
    hue smallint DEFAULT 50,
    contrast smallint DEFAULT 50,
    video_quality smallint DEFAULT 2,
    signal_type character(6),
    buffer_prerecording smallint DEFAULT 5,
    buffer_postrecording smallint DEFAULT 5,
    ptz_contol_path character(255),
    ptz_control_protocol character(15),
    ptz_baud_rate character(6),
    ip_ptz_control_type smallint,
    preset_type_ID smallint
);

INSERT INTO Devices VALUES (1, 'Bedroom', 704, 480, NULL, 'V4L2', NULL, NULL, NULL, 0, '/dev/video2', NULL, NULL, NULL, 128, 128, 128, 128, 2, 'NTSC', 5, 5, NULL, NULL, NULL, 0, 0);

CREATE TABLE "Events" (
    id integer PRIMARY KEY NOT NULL,
    source_ID smallint,
    date date,
    recording_path character(512),
    recording_type character(1),
    recording_length integer,
    file_size smallint,
    format character(5),
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
    notes text,
    access_setup boolean,
    access_remote boolean,
    access_web boolean,
    access_backup boolean,
    access_relay boolean,
    access_device_list character(255),
    access_ptz_list character(255),
    access_schedule character(168)
);
