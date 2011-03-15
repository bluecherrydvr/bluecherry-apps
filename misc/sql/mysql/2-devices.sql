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
	-- When protocol='IP' then IP, PORT and PATH to the device, e.g
	--    192.168.2.50|554|/mpeg4/media.amp
	device varbinary(256),

	-- When protocol='V4L2', then the same as AvailableSources.driver
	-- When protocol='IP', then type of IP device (e.g. RTSP-GENERIC)
	driver varchar(256),

	-- IP camera specific values, varbinary because it's case sensitive
	rtsp_username varbinary(256),
	rtsp_password varbinary(256),
	mjpeg_path varbinary(256),

	-- This can be a serial tty or URL. It's not dependent on the camera
	-- being IP or V4L2 (iow, an IP device can have a local serial tty
	-- to control the PTZ). It should be /dev/ttyS0 format or
	-- http://IP:PORT/PATH for IP based PTZ control. If it's an IP based
	-- path, you can either include the username and password in this
	-- field, or we will try the rtsp_{username,password} fields if
	-- authentication is required.
	ptz_control_path varbinary(256),
	-- The control protocol is almost always PELCO for serial tty
	ptz_control_protocol varchar(32),
	-- Comman separated values for serial tty based PTZ controls in
	-- Addr,Baud,Bit,Parity,StopBit format. E.g. 1,9600,8,n,1
	ptz_serial_values varchar(64),

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
	motion_map BLOB(400),			-- motion map
	schedule BLOB(168),			-- Recording schedule
	schedule_override_global boolean DEFAULT FALSE,
						-- ^^ Whether to override the global
	disabled boolean DEFAULT FALSE,		-- If this camera is disabled
	UNIQUE (device_name),
	UNIQUE (device,channel)
);

CREATE TABLE PTZPresets (
	id integer PRIMARY KEY NOT NULL AUTO_INCREMENT,
	device_id integer NOT NULL,
	preset_id integer NOT NULL,
	preset_name varchar(64),
	UNIQUE (device_id,preset_name),
	UNIQUE (device_id,preset_id),
	FOREIGN KEY (device_id) REFERENCES Devices(id)
);
