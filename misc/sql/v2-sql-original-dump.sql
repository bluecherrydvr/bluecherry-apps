--
-- PostgreSQL database dump
--

-- Started on 2010-02-08 15:21:05 CST

SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- TOC entry 1783 (class 1262 OID 17041)
-- Name: v2; Type: DATABASE; Schema: -; Owner: -
--

CREATE DATABASE v2 WITH TEMPLATE = template0 ENCODING = 'UTF8';


\connect v2

SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- TOC entry 6 (class 2615 OID 17043)
-- Name: v2; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA v2;


--
-- TOC entry 1786 (class 0 OID 0)
-- Dependencies: 6
-- Name: SCHEMA v2; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA v2 IS 'Bluecherry v2';


SET search_path = v2, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 1485 (class 1259 OID 17096)
-- Dependencies: 6
-- Name: AvailableSources; Type: TABLE; Schema: v2; Owner: -; Tablespace: 
--

CREATE TABLE "AvailableSources" (
    "ID" integer NOT NULL,
    device character(255),
    name character(255),
    status boolean,
    path character(255)
);


--
-- TOC entry 1787 (class 0 OID 0)
-- Dependencies: 1485
-- Name: TABLE "AvailableSources"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON TABLE "AvailableSources" IS 'To be filled on start-up with available local sources compatible with DVR.';


--
-- TOC entry 1484 (class 1259 OID 17094)
-- Dependencies: 6 1485
-- Name: AvailableSources_ID_seq; Type: SEQUENCE; Schema: v2; Owner: -
--

CREATE SEQUENCE "AvailableSources_ID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1788 (class 0 OID 0)
-- Dependencies: 1484
-- Name: AvailableSources_ID_seq; Type: SEQUENCE OWNED BY; Schema: v2; Owner: -
--

ALTER SEQUENCE "AvailableSources_ID_seq" OWNED BY "AvailableSources"."ID";


--
-- TOC entry 1789 (class 0 OID 0)
-- Dependencies: 1484
-- Name: AvailableSources_ID_seq; Type: SEQUENCE SET; Schema: v2; Owner: -
--

SELECT pg_catalog.setval('"AvailableSources_ID_seq"', 1, false);


--
-- TOC entry 1483 (class 1259 OID 17062)
-- Dependencies: 1756 1757 1758 1759 1760 1761 1762 6
-- Name: Devices; Type: TABLE; Schema: v2; Owner: -; Tablespace: 
--

CREATE TABLE "Devices" (
    "ID" integer NOT NULL,
    device_name character(100),
    "resolutionX" smallint,
    "resolutionY" smallint,
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
    "preset_type_ID" smallint
);


--
-- TOC entry 1790 (class 0 OID 0)
-- Dependencies: 1483
-- Name: TABLE "Devices"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON TABLE "Devices" IS 'Active video sources/devices.';


--
-- TOC entry 1791 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".device_name; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".device_name IS 'Custon name, such as camera location.';


--
-- TOC entry 1792 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".remote_host; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".remote_host IS 'IP camera host';


--
-- TOC entry 1793 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".remote_path; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".remote_path IS 'IP camera path to video/mjpeg';


--
-- TOC entry 1794 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".remote_port; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".remote_port IS 'IP camera port';


--
-- TOC entry 1795 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".invert; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".invert IS 'Invertion for upside down camreas.';


--
-- TOC entry 1796 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".source_video; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".source_video IS 'Local source path';


--
-- TOC entry 1797 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".channel; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".channel IS 'Local source channel';


--
-- TOC entry 1798 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".source_audio_in; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".source_audio_in IS 'Audio source in';


--
-- TOC entry 1799 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".source_audio_out; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".source_audio_out IS 'Audio source out';


--
-- TOC entry 1800 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".saturation; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".saturation IS 'Image adjustments';


--
-- TOC entry 1801 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".brightness; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".brightness IS 'Image adjustments';


--
-- TOC entry 1802 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".hue; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".hue IS 'Image adjustments';


--
-- TOC entry 1803 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".contrast; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".contrast IS 'Image adjustments';


--
-- TOC entry 1804 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".video_quality; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".video_quality IS 'Low=1/Medium=2/High=3';


--
-- TOC entry 1805 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".signal_type; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".signal_type IS 'NTSC/PAL';


--
-- TOC entry 1806 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".buffer_prerecording; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".buffer_prerecording IS 'Pre-recording buffer';


--
-- TOC entry 1807 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices".buffer_postrecording; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices".buffer_postrecording IS 'Post-recording buffer';


--
-- TOC entry 1808 (class 0 OID 0)
-- Dependencies: 1483
-- Name: COLUMN "Devices"."preset_type_ID"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Devices"."preset_type_ID" IS 'Load standard "preset" settings from $prefix.Presets';


--
-- TOC entry 1482 (class 1259 OID 17060)
-- Dependencies: 1483 6
-- Name: Devices_ID_seq; Type: SEQUENCE; Schema: v2; Owner: -
--

CREATE SEQUENCE "Devices_ID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1809 (class 0 OID 0)
-- Dependencies: 1482
-- Name: Devices_ID_seq; Type: SEQUENCE OWNED BY; Schema: v2; Owner: -
--

ALTER SEQUENCE "Devices_ID_seq" OWNED BY "Devices"."ID";


--
-- TOC entry 1810 (class 0 OID 0)
-- Dependencies: 1482
-- Name: Devices_ID_seq; Type: SEQUENCE SET; Schema: v2; Owner: -
--

SELECT pg_catalog.setval('"Devices_ID_seq"', 1, false);


--
-- TOC entry 1487 (class 1259 OID 17107)
-- Dependencies: 1765 6
-- Name: Events; Type: TABLE; Schema: v2; Owner: -; Tablespace: 
--

CREATE TABLE "Events" (
    "ID" integer NOT NULL,
    "source_ID" smallint,
    date date,
    recording_path character(512),
    recording_type character(1),
    recording_length integer,
    file_size smallint,
    format character(5),
    archived boolean DEFAULT false
);


--
-- TOC entry 1811 (class 0 OID 0)
-- Dependencies: 1487
-- Name: TABLE "Events"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON TABLE "Events" IS 'Recorded events';


--
-- TOC entry 1812 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events"."source_ID"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events"."source_ID" IS 'Source from which the video was recorded.';


--
-- TOC entry 1813 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".date; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".date IS 'Date/time of the recording.';


--
-- TOC entry 1814 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".recording_path; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".recording_path IS 'Path to video file.';


--
-- TOC entry 1815 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".recording_type; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".recording_type IS 'Type of the recording/alarm type.';


--
-- TOC entry 1816 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".recording_length; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".recording_length IS 'Length of the recording in seconds.';


--
-- TOC entry 1817 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".file_size; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".file_size IS 'Size of the file on disk, bytes.';


--
-- TOC entry 1818 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".format; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".format IS 'Format in which this recording is stored.';


--
-- TOC entry 1819 (class 0 OID 0)
-- Dependencies: 1487
-- Name: COLUMN "Events".archived; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON COLUMN "Events".archived IS 'Keep the video upon cleanup if true.';


--
-- TOC entry 1486 (class 1259 OID 17105)
-- Dependencies: 1487 6
-- Name: Events_ID_seq; Type: SEQUENCE; Schema: v2; Owner: -
--

CREATE SEQUENCE "Events_ID_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1820 (class 0 OID 0)
-- Dependencies: 1486
-- Name: Events_ID_seq; Type: SEQUENCE OWNED BY; Schema: v2; Owner: -
--

ALTER SEQUENCE "Events_ID_seq" OWNED BY "Events"."ID";


--
-- TOC entry 1821 (class 0 OID 0)
-- Dependencies: 1486
-- Name: Events_ID_seq; Type: SEQUENCE SET; Schema: v2; Owner: -
--

SELECT pg_catalog.setval('"Events_ID_seq"', 1, false);


--
-- TOC entry 1481 (class 1259 OID 17055)
-- Dependencies: 6
-- Name: GlobalSettings; Type: TABLE; Schema: v2; Owner: -; Tablespace: 
--

CREATE TABLE "GlobalSettings" (
    parameter character(60),
    value character(255)
);


--
-- TOC entry 1822 (class 0 OID 0)
-- Dependencies: 1481
-- Name: TABLE "GlobalSettings"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON TABLE "GlobalSettings" IS 'Global (unique) DVR settings.';


--
-- TOC entry 1480 (class 1259 OID 17046)
-- Dependencies: 6
-- Name: Users; Type: TABLE; Schema: v2; Owner: -; Tablespace: 
--

CREATE TABLE "Users" (
    id integer NOT NULL,
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


--
-- TOC entry 1823 (class 0 OID 0)
-- Dependencies: 1480
-- Name: TABLE "Users"; Type: COMMENT; Schema: v2; Owner: -
--

COMMENT ON TABLE "Users" IS '$prefix.Users';


--
-- TOC entry 1479 (class 1259 OID 17044)
-- Dependencies: 1480 6
-- Name: Users_id_seq; Type: SEQUENCE; Schema: v2; Owner: -
--

CREATE SEQUENCE "Users_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1824 (class 0 OID 0)
-- Dependencies: 1479
-- Name: Users_id_seq; Type: SEQUENCE OWNED BY; Schema: v2; Owner: -
--

ALTER SEQUENCE "Users_id_seq" OWNED BY "Users".id;


--
-- TOC entry 1825 (class 0 OID 0)
-- Dependencies: 1479
-- Name: Users_id_seq; Type: SEQUENCE SET; Schema: v2; Owner: -
--

SELECT pg_catalog.setval('"Users_id_seq"', 1, false);


--
-- TOC entry 1763 (class 2604 OID 17099)
-- Dependencies: 1485 1484 1485
-- Name: ID; Type: DEFAULT; Schema: v2; Owner: -
--

ALTER TABLE "AvailableSources" ALTER COLUMN "ID" SET DEFAULT nextval('"AvailableSources_ID_seq"'::regclass);


--
-- TOC entry 1755 (class 2604 OID 17065)
-- Dependencies: 1482 1483 1483
-- Name: ID; Type: DEFAULT; Schema: v2; Owner: -
--

ALTER TABLE "Devices" ALTER COLUMN "ID" SET DEFAULT nextval('"Devices_ID_seq"'::regclass);


--
-- TOC entry 1764 (class 2604 OID 17110)
-- Dependencies: 1487 1486 1487
-- Name: ID; Type: DEFAULT; Schema: v2; Owner: -
--

ALTER TABLE "Events" ALTER COLUMN "ID" SET DEFAULT nextval('"Events_ID_seq"'::regclass);


--
-- TOC entry 1754 (class 2604 OID 17049)
-- Dependencies: 1480 1479 1480
-- Name: id; Type: DEFAULT; Schema: v2; Owner: -
--

ALTER TABLE "Users" ALTER COLUMN id SET DEFAULT nextval('"Users_id_seq"'::regclass);


--
-- TOC entry 1779 (class 0 OID 17096)
-- Dependencies: 1485
-- Data for Name: AvailableSources; Type: TABLE DATA; Schema: v2; Owner: -
--

COPY "AvailableSources" ("ID", device, name, status, path) FROM stdin;
\.


--
-- TOC entry 1778 (class 0 OID 17062)
-- Dependencies: 1483
-- Data for Name: Devices; Type: TABLE DATA; Schema: v2; Owner: -
--

COPY "Devices" ("ID", device_name, "resolutionX", "resolutionY", contol_path, protocol, remote_host, remote_path, remote_port, invert, source_video, channel, source_audio_in, source_audio_out, saturation, brightness, hue, contrast, video_quality, signal_type, buffer_prerecording, buffer_postrecording, ptz_contol_path, ptz_control_protocol, ptz_baud_rate, ip_ptz_control_type, "preset_type_ID") FROM stdin;
\.


--
-- TOC entry 1780 (class 0 OID 17107)
-- Dependencies: 1487
-- Data for Name: Events; Type: TABLE DATA; Schema: v2; Owner: -
--

COPY "Events" ("ID", "source_ID", date, recording_path, recording_type, recording_length, file_size, format, archived) FROM stdin;
\.


--
-- TOC entry 1777 (class 0 OID 17055)
-- Dependencies: 1481
-- Data for Name: GlobalSettings; Type: TABLE DATA; Schema: v2; Owner: -
--

COPY "GlobalSettings" (parameter, value) FROM stdin;
\.


--
-- TOC entry 1776 (class 0 OID 17046)
-- Dependencies: 1480
-- Data for Name: Users; Type: TABLE DATA; Schema: v2; Owner: -
--

COPY "Users" (id, login, password, name, email, phone, notes, access_setup, access_remote, access_web, access_backup, access_relay, access_device_list, access_ptz_list, access_schedule) FROM stdin;
\.


--
-- TOC entry 1773 (class 2606 OID 17104)
-- Dependencies: 1485 1485
-- Name: AvailableSources_pkey; Type: CONSTRAINT; Schema: v2; Owner: -; Tablespace: 
--

ALTER TABLE ONLY "AvailableSources"
    ADD CONSTRAINT "AvailableSources_pkey" PRIMARY KEY ("ID");


--
-- TOC entry 1771 (class 2606 OID 17077)
-- Dependencies: 1483 1483
-- Name: Devices_pkey; Type: CONSTRAINT; Schema: v2; Owner: -; Tablespace: 
--

ALTER TABLE ONLY "Devices"
    ADD CONSTRAINT "Devices_pkey" PRIMARY KEY ("ID");


--
-- TOC entry 1775 (class 2606 OID 17115)
-- Dependencies: 1487 1487
-- Name: Events_pkey; Type: CONSTRAINT; Schema: v2; Owner: -; Tablespace: 
--

ALTER TABLE ONLY "Events"
    ADD CONSTRAINT "Events_pkey" PRIMARY KEY ("ID");


--
-- TOC entry 1769 (class 2606 OID 17059)
-- Dependencies: 1481 1481
-- Name: GlobalSettings_parameter_key; Type: CONSTRAINT; Schema: v2; Owner: -; Tablespace: 
--

ALTER TABLE ONLY "GlobalSettings"
    ADD CONSTRAINT "GlobalSettings_parameter_key" UNIQUE (parameter);


--
-- TOC entry 1767 (class 2606 OID 17054)
-- Dependencies: 1480 1480
-- Name: Users_pkey; Type: CONSTRAINT; Schema: v2; Owner: -; Tablespace: 
--

ALTER TABLE ONLY "Users"
    ADD CONSTRAINT "Users_pkey" PRIMARY KEY (id);


--
-- TOC entry 1785 (class 0 OID 0)
-- Dependencies: 3
-- Name: public; Type: ACL; Schema: -; Owner: -
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2010-02-08 15:21:07 CST

--
-- PostgreSQL database dump complete
--

