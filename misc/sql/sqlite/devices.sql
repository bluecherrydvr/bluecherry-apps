DROP TABLE IF EXISTS AvailableSources;
CREATE TABLE "AvailableSources" (
    id integer PRIMARY KEY NOT NULL,
    devicepath character(255),
    name character(255),
    driver character(255),
    status boolean,
    alsasounddev character(255)
);

DROP TABLE IF EXISTS Devices;
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
    audio_volume smallint,
    audio_rate integer,
    audio_format integer,
    audio_channels smallint,
    saturation smallint,
    brightness smallint,
    hue smallint,
    contrast smallint,
    video_quality smallint DEFAULT 2,
    video_interval smallint,
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
    file_chop_interval smallint
);
