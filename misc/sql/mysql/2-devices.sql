-- The device here is format of <id-type>|<bcuid>|<dev where
-- id-type is either BCPCI or BCUID (describing the bcuid format).
CREATE TABLE AvailableSources (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	device varchar(256),			-- E.g. BCPCI|0000:01.0|0
	driver varchar(256),			-- E.g. solo6010
	card_id varchar(256) NOT NULL,		-- E.g. BCPCI|0000:01.0
	UNIQUE (device)
);

CREATE TABLE Devices (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,	-- Unique ID for this device

	-- These are required
	device_name varchar(100),		-- User given name
	protocol varchar(50),			-- IP or V4L2

	-- When protocol='V4L2' then same as AvailableSources.device
	-- When protocol='IP' then URL to the device, e.g
	--    rtsp://foo@bar:192.168.1.100:436/media.amp
	device varchar(256),

	-- When protocol='V4L2', then the same as AvailableSources.driver
	-- When protocol='IP', then type of IP device (e.g. RTSP-AXIS, RTSP-GENERIC)
	driver varchar(256),

	-- The rest of these may or may not be supported by the device
	resolutionX smallint,
	resolutionY smallint,
	channel integer NOT NULL DEFAULT 0,	-- If device supports multiple channels
	invert boolean NOT NULL DEFAULT 0,	-- True if cam is inverted
	audio_volume smallint DEFAULT 50,	-- Audio gain (0 - 100)
	saturation smallint  DEFAULT 50,	-- 0 - 100 (50 default)
	brightness smallint  DEFAULT 50,	-- 0 - 100 (50 default)
	hue smallint  DEFAULT 50,		-- 0 - 100 (50 default)
	contrast smallint  DEFAULT 50,		-- 0 - 100 (50 default)
	video_quality smallint DEFAULT 2,	-- 2 default (range 1-3, l/m/h))
	video_interval smallint,		-- Interval of encoding (fps-ish)
	signal_type varchar(6),			-- NTSC or PAL
	buffer_prerecording smallint DEFAULT 5,	-- Seconds of pre/post recording for
	buffer_postrecording smallint DEFAULT 5,--   motion stop/start
	ptz_contol_path varchar(255),		-- Serial port device for PTZ
	ptz_control_protocol varchar(15),	-- Protocol for PTZ controls
	ptz_baud_rate varchar(6),		-- Baudrate and such for PTZ (8,n,1)
	ip_ptz_control_type smallint,		-- Control type of IP PTZ
	motion_map BLOB(400),			-- motion map
	schedule BLOB(168),			-- Recording schedule
	schedule_override_global boolean DEFAULT FALSE,
						-- ^^ Whether to override the global
	disabled boolean DEFAULT FALSE,		-- If this camera is disabled
	UNIQUE (device_name),
	UNIQUE (device,channel)
);
