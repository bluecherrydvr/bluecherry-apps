
DROP TABLE IF EXISTS "CAMERAS";
DROP TABLE IF EXISTS "manufacturers";


CREATE TABLE "manufacturers" (
  "id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "manufacturer" text(31) NOT NULL,
  "model_id" text(31) NOT NULL
);


CREATE TABLE "cameras" (
  "id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "api_id" text NOT NULL,
  "model" text NOT NULL,
  "manufacturer" integer NOT NULL,
  "jpeg_url" text NOT NULL,
  "mjpeg_url" text NOT NULL,
  "h264_url" text NOT NULL,
  "resolution" text NOT NULL,
  "shape" text NOT NULL,
  "default_username" text NOT NULL DEFAULT '',
  "default_password" text NOT NULL DEFAULT '',
  "psia" text NOT NULL DEFAULT '',
  "onvif" integer NOT NULL DEFAULT '0',
  "ptz" integer NOT NULL DEFAULT '0',
  "infrared" integer NOT NULL DEFAULT '0',
  "varifocal" integer NOT NULL DEFAULT '0',
  "sd_card" integer NOT NULL DEFAULT '0',
  "upnp" integer NOT NULL DEFAULT '0',
  "poe" integer NOT NULL DEFAULT '0',
  "audio_in" integer NOT NULL DEFAULT '0',
  "audio_out" integer NOT NULL DEFAULT '0',
  "discontinued" integer NOT NULL DEFAULT '0',
  "wifi" integer NOT NULL DEFAULT '0',
  "images" text NOT NULL DEFAULT '',
  "official_url" text NOT NULL,
  FOREIGN KEY ("manufacturer") REFERENCES "manufacturers" ("id") ON DELETE NO ACTION ON UPDATE NO ACTION
);


-- MANUFACTURERS --
INSERT INTO "manufacturers" VALUES ('1', 'Basler', 'basler');
INSERT INTO "manufacturers" VALUES ('2', 'Siemens', 'siemens');
INSERT INTO "manufacturers" VALUES ('3', '3S Vision', '3s-vision');
INSERT INTO "manufacturers" VALUES ('4', 'Brickcom', 'brickcom');
INSERT INTO "manufacturers" VALUES ('5', 'Teltonika', 'teltonika');
INSERT INTO "manufacturers" VALUES ('6', 'TP-Link', 'tp-link');
INSERT INTO "manufacturers" VALUES ('7', 'Ubiquiti', 'ubiquiti');
INSERT INTO "manufacturers" VALUES ('8', 'Afreey', 'afreey');
INSERT INTO "manufacturers" VALUES ('9', 'Agasio', 'agasio');
INSERT INTO "manufacturers" VALUES ('10', 'Aposonic', 'aposonic');
INSERT INTO "manufacturers" VALUES ('11', 'ABS', 'abs');
INSERT INTO "manufacturers" VALUES ('12', 'Wansview', 'wansview');
INSERT INTO "manufacturers" VALUES ('13', 'Linksys', 'linksys');
INSERT INTO "manufacturers" VALUES ('14', 'Asoni', 'asoni');
INSERT INTO "manufacturers" VALUES ('15', 'Dallmeier', 'dallmeier');
INSERT INTO "manufacturers" VALUES ('16', 'Dahua', 'dahua');
INSERT INTO "manufacturers" VALUES ('17', 'ABUS', 'abus');
INSERT INTO "manufacturers" VALUES ('18', 'Evercam', 'evercam');
INSERT INTO "manufacturers" VALUES ('19', 'Foscam', 'foscam');
INSERT INTO "manufacturers" VALUES ('20', 'Lumenera', 'lumenera');
INSERT INTO "manufacturers" VALUES ('21', 'Planet', 'planet');
INSERT INTO "manufacturers" VALUES ('22', 'ACTi', 'acti');
INSERT INTO "manufacturers" VALUES ('23', 'AVTECH', 'avtech');
INSERT INTO "manufacturers" VALUES ('24', 'IQinVision', 'iqinvision');
INSERT INTO "manufacturers" VALUES ('25', 'Airlive', 'airlive');
INSERT INTO "manufacturers" VALUES ('26', 'Arecont Vision', 'arecont-vision');
INSERT INTO "manufacturers" VALUES ('27', 'Sony', 'sony');
INSERT INTO "manufacturers" VALUES ('28', 'Vivotek', 'vivotek');
INSERT INTO "manufacturers" VALUES ('29', 'Panasonic', 'panasonic');
INSERT INTO "manufacturers" VALUES ('30', 'Mobotix', 'mobotix');
INSERT INTO "manufacturers" VALUES ('31', 'Zavio', 'zavio');
INSERT INTO "manufacturers" VALUES ('32', 'D-Link', 'dlink');
INSERT INTO "manufacturers" VALUES ('33', 'Grandstream', 'grandstream');
INSERT INTO "manufacturers" VALUES ('34', 'Y-cam', 'ycam');
INSERT INTO "manufacturers" VALUES ('35', 'Axis', 'axis');
INSERT INTO "manufacturers" VALUES ('36', 'Samsung', 'samsung');
INSERT INTO "manufacturers" VALUES ('37', 'Hikvision', 'hikvision');

-- CAMERAS --
INSERT INTO "cameras" 
VALUES (NULL, 'cvmw3025-ir', 'CVMW3025-IR', '2', '', '', '', '2048x1536', 'Dome', 'root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cvms2025-ir', 'CVMS2025-IR', '2', '', '', '', '1920x1080', 'Dome', 'root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccid1410-st', 'CCID1410-ST', '2', '', '', '', '640x480', 'Dome', 'root', '', '', 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccid1445-dn36', 'CCID1445-DN36', '2', '', '', '', '720x576', 'Dome', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccid1445-dn28', 'CCID1445-DN28', '2', '', '', '', '720x576', 'Dome', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccid1445-dn18', 'CCID1445-DN18', '2', '', '', '', '720x576', 'Dome', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cfmw3025', 'CFMW3025', '2', '', '', '', '2048x1536', 'Dome', 'root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cfms2025', 'CFMS2025', '2', '', '', '', '680x512', 'Dome', 'root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cfmw1025', 'CFMW1025', '2', '', '', '', '680x512', 'Dome', 'root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cfis1425', 'CFIS1425', '2', '', '', '', '680x512', 'Dome', 'root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccic1410-law', 'CCIC1410-LAW', '2', '', '', '', '640x480', 'Box', 'root', '', '', 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccmw3025', 'CCMW3025', '2', '', '', '', '2048x1536', 'Box', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccms2025', 'CCMS2025', '2', '', '', '', '1920x1080', 'Box', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccmw1025', 'CCMW1025', '2', '', '', '', '1280x720', 'Box', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ccis1425', 'CCIS1425', '2', '', '', '', '680x512', 'Box', 'root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8012', 'N8012', '3', '', '', '', '2560x1920', 'Cube', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8011', 'N8011', '3', '', '', '', '2560x1920', 'Cube', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8032', 'N8032', '3', '', '', '', '2048x1536', 'Cube', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8031', 'N8031', '3', '', '', '', '2048x1536', 'Cube', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8072-c', 'N8072-C', '3', '', '', '', '1600x1200', 'Cube', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8072', 'N8072', '3', '', '', '', '1600x1200', 'Cube', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8071-c', 'N8071-C', '3', '', '', '', '1600x1200', 'Cube', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n8071', 'N8071', '3', '', '', '', '1600x1200', 'Cube', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n1011', 'N1011', '3', '', '', '', '2560x1920', 'Box', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n1031', 'N1031', '3', '', '', '', '2048x1536', 'Box', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n1073', 'N1073', '3', '', '', '', 'NTSC:720x480/PAL:720x576', 'Box', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n1072', 'N1072', '3', '', '', '', '1600x1200', 'Box', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6015', 'N6015', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6019', 'N6019', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6013-a', 'N6013-A', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6011', 'N6011', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6013-c', 'N6013-C', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6013', 'N6013', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6048', 'N6048', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6042', 'N6042', '3', '', '', '', '2560x1920', 'Bullet', 'root', 'root', '', 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6035', 'N6035', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6031', 'N6031', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6033', 'N6033', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6038', 'N6038', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6037', 'N6037', '3', '', '', '', '2048x1536', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6071', 'N6071', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6073', 'N6073', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6072', 'N6072', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6075', 'N6075', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6074', 'N6074', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6077', 'N6077', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6076-c', 'N6076-C', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6076', 'N6076', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6079', 'N6079', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6078-c', 'N6078-C', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6078', 'N6078', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n6082', 'N6082', '3', '', '', '', '1600x1200', 'Bullet', 'root', 'root', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3011-c', 'N3011-C', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3011', 'N3011', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9042', 'N9042', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9045', 'N9045', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9012', 'N9012', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9041', 'N9041', '3', '', '', '', '2560x1920', 'Dome', 'root', 'root', '', 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3031', 'N3031', '3', '', '', '', '2048x1536', 'Dome', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9034', 'N9034', '3', '', '', '', '2048x1536', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9033', 'N9033', '3', '', '', '', '2048x1536', 'Dome', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9032', 'N9032', '3', '', '', '', '2048x1536', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3075', 'N3075', '3', '', '', '', 'NTSC:704x480/PAL:704x576', 'Dome', 'root', 'root', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3074', 'N3074', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3073', 'N3073', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3072-c', 'N3072-C', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3072', 'N3072', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n3071', 'N3071', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9074', 'N9074', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9073', 'N9073', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9082', 'N9082', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9085', 'N9085', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9071-c', 'N9071-C', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9071', 'N9071', '3', '', '', '', '1600x1200', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9018', 'N9018', '3', '', '', '', '1920x1080', 'Dome', 'root', 'root', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n9017', 'N9017', '3', '', '', '', '1920x1080', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n5013', 'N5013', '3', '', '', '', 'NTSC:720x480/PAL:720x576', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n5012', 'N5012', '3', '', '', '', '1920x1080', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n5011', 'N5011', '3', '', '', '', '1280x720', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n4013', 'N4013', '3', '', '', '', 'NTSC:704x480/PAL:704x576', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n4012', 'N4012', '3', '', '', '', '1920x1080', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'n4011', 'N4011', '3', '', '', '', '1280x720', 'Dome', 'root', 'root', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ph-100ah-kit-b', 'PH-100Ah Kit-B', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x720', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ph-100ah-kit-a', 'PH-100Ah Kit-A', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x720', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'pz-040e-12xp', 'PZ-040E 12xp', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', 'PAL:720x576', 'Other', 'admin', 'admin', '', 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'pz-040d-12xp', 'PZ-040D 12xp', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', 'NTSC:720x480', 'Other', 'admin', 'admin', '', 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'osd-200a-20xp-se', 'OSD-200A 20xp-SE', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'osd-200a-20xp', 'OSD-200A 20xp', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'osd-040e-36xp', 'OSD-040E 36xp', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', 'PAL:720x576', 'Dome', 'admin', 'admin', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'osd-040d-36xp', 'OSD-040D 36xp', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', 'NTSC:720x480', 'Dome', 'admin', 'admin', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-502ap-v5', 'VD-502Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-501af', 'VD-501Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-500ap-v5', 'VD-500Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-500af', 'VD-500Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-302np-v5', 'VD-302Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-302np', 'VD-302Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-302ap-v5', 'VD-302Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-301af', 'VD-301Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-300np-v5', 'VD-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-300np', 'VD-300Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-300nf', 'VD-300Nf', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-300ap-v5', 'VD-300Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-300af', 'VD-300Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-202np-v5', 'VD-202Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-202np', 'VD-202Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-202ap-v5', 'VD-202Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-201af', 'VD-201Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-200np-v5', 'VD-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-200np', 'VD-200Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-200nf', 'VD-200Nf', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-200ap-v5', 'VD-200Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-200af', 'VD-200Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-131af', 'VD-131Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'vd-130np', 'VD-130Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-502ap-v5', 'FD-502Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-501af', 'FD-501Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-500ap-v5', 'FD-500Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-302np-v5', 'FD-302Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-302ap-v5', 'FD-302Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-301af', 'FD-301Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-300np-v5', 'FD-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-300ap-v5', 'FD-300Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-202np-v5', 'FD-202Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-202ap-v5', 'FD-202Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-201af', 'FD-201Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-200np-v5', 'FD-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-200ap-v5', 'FD-200Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-131af', 'FD-131Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fd-130np', 'FD-130Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wmb-500ap', 'WMB-500Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wmb-300ap', 'WMB-300Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wmb-200ap', 'WMB-200Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wmb-130ap', 'WMB-130Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mb-500ap', 'MB-500Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mb-300ap', 'MB-300Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mb-200ap', 'MB-200Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mb-130ap', 'MB-130Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-500ap-360p', 'MD-500Ap-360P', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-500ap', 'MD-500Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-300np-360p', 'MD-300Np-360P', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-300np', 'MD-300Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-300ap', 'MD-300Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-200np', 'MD-200Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-200ap', 'MD-200Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-100ap', 'MD-100Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'md-100ae', 'MD-100Ae', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-500ap-v5', 'WOB-500Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-300np-v5', 'WOB-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-300np', 'WOB-300Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-300ap-v5', 'WOB-300Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-200np-v5', 'WOB-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-200np', 'WOB-200Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wob-200ap-v5', 'WOB-200Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-502ap-v5', 'OB-502Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-502ae-v5', 'OB-502Ae V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-500ap-v5', 'OB-500Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-500af-v5', 'OB-500Af V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-500af', 'OB-500Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-302np-v5', 'OB-302Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-302np', 'OB-302Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-302ne-v5', 'OB-302Ne V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-302ap-v5', 'OB-302Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-302ae-v5', 'OB-302Ae V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300np-v5', 'OB-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300np', 'OB-300Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300nf-v5', 'OB-300Nf V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300ap-v5', 'OB-300Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300af-v5', 'OB-300Af V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-300af', 'OB-300Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-202np-v5', 'OB-202Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-202np', 'OB-202Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-202ne-v5', 'OB-202Ne V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-202ap-v5', 'OB-202Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-202ae-v5', 'OB-202Ae V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200np-v5', 'OB-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200np', 'OB-200Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200nf-v5', 'OB-200Nf V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200ap-v5', 'OB-200Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200af-v5', 'OB-200Af V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-200af', 'OB-200Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-130np', 'OB-130Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ob-130af', 'OB-130Af', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-500ap-v5', 'GOB-500Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-300np-v5', 'GOB-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-300np', 'GOB-300Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-300ap-v5', 'GOB-300Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-200np-v5', 'GOB-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-200np', 'GOB-200Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'gob-200ap-v5', 'GOB-200Ap V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wfb-300np-v5', 'WFB-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'wfb-130np', 'WFB-130Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-500ap', 'FB-500Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-300np-v5', 'FB-300Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-300ap', 'FB-300Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-205np', 'FB-205Np', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-200np-v5', 'FB-200Np V5', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-200np-v4', 'FB-200Np V4', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-200ap', 'FB-200Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fb-100ae', 'FB-100Ae', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-502ap', 'CB-502Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-500ap-360p', 'CB-500Ap-360P', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-500ap', 'CB-500Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2592x1944', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-302ap', 'CB-302Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-300ap', 'CB-300Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '2048x1536', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-202ap', 'CB-202Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-200ap', 'CB-200Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1920x1080', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-102ae-v2', 'CB-102Ae V2', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Other', 'admin', 'admin', '', 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-100ap', 'CB-100Ap', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-100ae-vga', 'CB-100Ae-VGA', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '640x480', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'cb-100ae', 'CB-100Ae', '4', 'cgi-bin/media.cgi?action=getSnapshot', 'channel2', 'channel1', '1280x800', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mvc300', 'MVC300', '5', '', '', 'PSIA/Streaming/channels/2?videoCodecType=H.264', '1920x1080', 'Cube', '', '', '', 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'mvc330', 'MVC330', '5', '', '', 'PSIA/Streaming/channels/2?videoCodecType=H.264', '1920x1080', 'Box', '', '', '', 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc2020n', 'TL-SC2020N', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/808/original/55f31606e7601fd27f4d2e98bd4fccf8.jpg?1406042661";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc2020', 'TL-SC2020', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/809/original/29784aad81e9baa7ef98f62762c922bd.jpg?1406042678";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3171g', 'TL-SC3171G', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Box', 'admin', 'admin', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/810/original/7c936c48f65f46d39b55324f6afdae59.jpg?1406042704";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3171', 'TL-SC3171', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Box', 'admin', 'admin', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/811/original/a5923349d507ca959529a920829d834b.jpg?1406042740";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3130g', 'TL-SC3130G', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/812/original/f7e93ca19d0f47320463b946d1dd7dc8.jpg?1406042775";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3130', 'TL-SC3130', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/813/original/03401163d3991663ae8bdd3a613c35b4.jpg?1406042794";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3230n', 'TL-SC3230N', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '1280x1024', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/814/original/f2d7226f2bb8b9ca708b52b362f90102.jpg?1406042834";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'tl-sc3230', 'TL-SC3230', '6', 'jpg/image.jpg', 'video.mjpg', 'video.h264', '1280x1024', 'Other', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/815/original/7b2b37be2d66bccd93c8a27946006875.jpg?1406042853";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'uvc-pro', 'UVC-PRO', '7', 'snapshot.cgi', '', 'live/ch00_0', '1920x1080', 'Bullet', 'ubnt', 'ubnt', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/800/original/e847aaf3df9735a6b4bfe98008c77076.jpg?1406040654";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'uvc-dome', 'UVC-Dome', '7', 'snapshot.cgi', '', 'live/ch00_0', '1280x720', 'Dome', 'ubnt', 'ubnt', '', 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/801/original/ff05b3d67572dcc1ec47e9cfd0e7f12c.jpg?1406040680";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'uvc', 'UVC', '7', 'snapshot.cgi', '', 'live/ch00_0', '1280x720', 'Bullet', 'ubnt', 'ubnt', '', 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/802/original/0688914b74bd3bdfa0c4607e1d538b7c.jpg?1406040706";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'aircam-mini', 'airCam Mini', '7', 'snapshot.cgi', '', 'live/ch00_0', '1280x720', 'Other', 'ubnt', 'ubnt', '', 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/803/original/f9235ca057bc58cd3bc599cebeb8a6fa.jpg?1406040749";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/804/original/f60dbc7501ce1165b5bd678c0eaf624d.jpg?1406040751";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'aircam-dome', 'airCam Dome', '7', 'snapshot.cgi', '', 'live/ch00_0', '1280x720', 'Dome', 'ubnt', 'ubnt', '', 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/805/original/5ef4a25202846b074dd4be0986287997.jpg?1406040815";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'aircam', 'airCam', '7', 'snapshot.cgi', '', 'live/ch00_0', '1280x720', 'Bullet', 'ubnt', 'ubnt', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/806/original/e3d4dc8c570d936dbbe4d4645758a488.jpg?1406040841";}}', 'www.ubnt.com');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-5320gsf', 'ANC-5320GSF', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Box', '', '', '', 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-5320vsf', 'ANC-5320VSF', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Box', '', '', '', 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'adc-3018s', 'ADC-3018S', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'adc-3018m', 'ADC-3018M', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'adc-3018mbf', 'ADC-3018MBF', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'adc-3008mb', 'ADC-3008MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2860s', 'ANC-2860S', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2860m', 'ANC-2860M', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2160s', 'ANC-2160S', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2160m', 'ANC-2160M', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2160mf', 'ANC-2160MF', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2260mb', 'ANC-2260MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2360mb', 'ANC-2360MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-2460mb', 'ANC-2460MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Bullet', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3211s', 'ANC-3211S', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3211m', 'ANC-3211M', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3211ha', 'ANC-3211HA', '8', 'imagep/picture.jpg', '', '', '1280x900', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3210s', 'ANC-3210S', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3210m', 'ANC-3210M', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3210ha', 'ANC-3210HA', '8', 'imagep/picture.jpg', '', '', '1280x900', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3311mb', 'ANC-3311MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3321mb', 'ANC-3321MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3221mb', 'ANC-3221MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-3220mb', 'ANC-3220MB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Dome', '', '', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-606gs', 'ANC-606GS', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-606gm', 'ANC-606GM', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818gs', 'ANC-818GS', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818gm', 'ANC-818GM', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818gha', 'ANC-818GHA', '8', 'imagep/picture.jpg', '', '', '1280x900', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-808gmb', 'ANC-808GMB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-606vs', 'ANC-606VS', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-606vm', 'ANC-606VM', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818vs', 'ANC-818VS', '8', 'imagep/picture.jpg', '', '', '2560x1920', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818vm', 'ANC-818VM', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-818vha', 'ANC-818VHA', '8', 'imagep/picture.jpg', '', '', '1280x900', 'Other', '', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'anc-808pmb', 'ANC-808PMB', '8', 'imagep/picture.jpg', '', '', '1920x1080', 'Other', '', '', '', 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.afreey.com.tw/');
INSERT INTO "cameras" 
VALUES (NULL, 'a612-poe', 'A612-POE', '9', '', 'videostream.cgi', '11', '640x480', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a512-poe', 'A512-POE', '9', '', 'videostream.cgi', '11', '640x480', 'Other', 'admin', '', '', 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a602w', 'A602W', '9', '', 'videostream.cgi', '11', '640x480', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a502w', 'A502W', '9', '', 'videostream.cgi', '11', '640x480', 'Other', 'admin', '', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a603w', 'A603W', '9', '', 'videostream.cgi', '11', '640x480', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'm105i', 'M105I', '9', '', 'videostream.cgi', '11', '640x480', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'm1bf', 'M1BF', '9', '', 'videostream.cgi', '11', '640x480', 'Dome', 'admin', '', '', 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a500ww', 'A500WW', '9', '', 'videostream.cgi', '11', '640x480', 'Other', 'admin', '', '', 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a500w', 'A500W', '9', '', 'videostream.cgi', '11', '640x480', 'Other', 'admin', '', '', 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a533w', 'A533W', '9', '', 'videostream.cgi', '11', '1280x720', 'Other', 'admin', '', '', 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a503w', 'A503W', '9', '', 'videostream.cgi', '11', '640x480', 'Other', 'admin', '', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a622w', 'A622W', '9', '', 'videostream.cgi', '11', '640x480', 'Dome', 'admin', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 'a:0:{}', 'http://agasio.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'a-ipbi02', 'A-IPBI02', '10', '', '', '', '1280x960', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-ipbiv02', 'A-IPBIV02', '10', '', '', '', '1920x1080', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-ipmiv01', 'A-IPMIV01', '10', '', '', '', '1920x1080', 'Dome', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-ipbiv01', 'A-IPBIV01', '10', '', '', '', '1920x1080', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-iph7a26w', 'A-IPH7A26W', '10', '', '', '', '1920x1080', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-iph7a88', 'A-IPH7A88', '10', '', '', '', '2592x1920', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'a-iphfe01', 'A-IPHFE01', '10', '', '', '', '2592x1920', 'Dome', 'admin', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.aposoniccctv.com/index.php');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-422m', 'Megacam 422M', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '1280x1024', 'Other', 'admin', '', '', 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-4220', 'Megacam 4220', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '640x480', 'Other', 'admin', '', '', 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-421m', 'Megacam 421M', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '1280x1024', 'Other', 'admin', '', '', 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-4210', 'Megacam 4210', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '640x480', 'Other', 'admin', '', '', 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-312m', 'MegaCam 312M', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '1280x1024', 'Other', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'megacam-311m', 'MegaCam 311M', '11', 'jpg/1/image.jpg', 'mjpg/1/video.mjpg', '', '1280x1024', 'Other', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://abs.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam635', 'CAM635', '14', 'getimage.cgi', 'getdata.cgi', '', '640x480', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam615', 'CAM615', '14', 'getimage.cgi', 'getdata.cgi', '', 'NTSC:720x480/PAL:720x576', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam661h', 'CAM661H', '14', 'getimage.cgi', 'getdata.cgi', '', '1280x800', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam662hir', 'CAM662HIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6704hir', 'CAM6704HIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6714hir', 'CAM6714HIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam641', 'CAM641', '14', 'getimage.cgi', 'getdata.cgi', '', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam619mir', 'CAM619MIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam619m', 'CAM619M', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam646mir', 'CAM646MIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6923fir', 'CAM6923FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6623fir', 'CAM6623FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6723fir', 'CAM6723FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6691ficr', 'CAM6691FICR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6661fir', 'CAM6661FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6724fir', 'CAM6724FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6733f', 'CAM6733F', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6734fir', 'CAM6734FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6801ticr', 'CAM6801TICR', '14', 'getimage.cgi', 'getdata.cgi', '', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6833eir', 'CAM6833EIR', '14', 'getimage.cgi', 'getdata.cgi', '', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6834eir', 'CAM6834EIR', '14', 'getimage.cgi', 'getdata.cgi', '', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam748fir', 'CAM748FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam746fir', 'CAM746FIR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam743ficr', 'CAM743FICR', '14', 'getimage.cgi', 'getdata.cgi', '', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam6833ev', 'CAM6833EV', '14', 'getimage.cgi', 'getdata.cgi', '', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam636mv', 'CAM636MV', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam649mv', 'CAM649MV', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'cam619mv', 'CAM619MV', '14', 'getimage.cgi', 'getdata.cgi', '', '1600x1200', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.a-soni.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncl-615w', 'NCL-615W', '12', 'snapshot.cgi', 'video.cgi', '', '640x480', 'Bullet', 'admin', '123456', '', 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncl-611w', 'NCL-611W', '12', 'snapshot.cgi', 'video.cgi', '', '640x480', 'Other', 'admin', '123456', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncl-610w', 'NCL-610W', '12', 'snapshot.cgi', 'video.cgi', '', '640x480', 'Other', 'admin', '123456', '', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-555mw', 'NCZ-555MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-554mw', 'NCZ-554MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-553mw', 'NCZ-553MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-552mw', 'NCZ-552MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-551mw', 'NCZ-551MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Bullet', 'admin', '123456', '', 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-550mw', 'NCZ-550MW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-629w', 'NCM-629W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-628w', 'NCM-628W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-627w', 'NCM-627W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-626w', 'NCM-626W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-625w', 'NCM-625W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-624w', 'NCM-624W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-623w', 'NCM-623W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-622w', 'NCM-622W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-621kw', 'NCM-621KW', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-621w', 'NCM-621W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncm-620w', 'NCM-620W', '12', 'snapshot.cgi', 'video.cgi', '', '1280x720', 'Other', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'ncz-550w', 'NCZ-550W', '12', 'snapshot.cgi', 'video.cgi', '', '640x480', 'Other', 'admin', '123456', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://wansview.net/');
INSERT INTO "cameras" 
VALUES (NULL, 'lcam0336od', 'LCAM0336OD', '13', 'img/snapshot.cgi', 'img/video.mjpeg', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.linksys.com/en-eu/business');
INSERT INTO "cameras" 
VALUES (NULL, 'lcab03vlnod', 'LCAB03VLNOD', '13', 'img/snapshot.cgi', 'img/video.mjpeg', 'channel1', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.linksys.com/en-eu/business');
INSERT INTO "cameras" 
VALUES (NULL, 'lcad03vlnod', 'LCAD03VLNOD', '13', 'img/snapshot.cgi', 'img/video.mjpeg', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.linksys.com/en-eu/business');
INSERT INTO "cameras" 
VALUES (NULL, 'lcad03fln', 'LCAD03FLN', '13', 'img/snapshot.cgi', 'img/video.mjpeg', 'channel1', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.linksys.com/en-eu/business');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c101', 'BL-C101', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c121', 'BL-C121', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c140', 'BL-C140', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c160', 'BL-C160', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c210', 'BL-C210', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-c230', 'BL-C230', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-vp101', 'BL-VP101', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-vp104', 'BL-VP104', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-vp104w', 'BL-VP104W', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Other', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-vt164', 'BL-VT164', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Other', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'bl-vt164w', 'BL-VT164W', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Other', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz3018-yy-slash-rp-slash-ip-domera', 'DDZ3018-YY/RP/IP DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', '', 'NTSC:720x480/PAL:720x576', 'Dome', '', '3', '', 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz3036-wm-slash-hs-slash-ip-domera', 'DDZ3036-WM/HS/IP DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', '', 'NTSC:720x480/PAL:720x576', 'Dome', '', '3', '', 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz3026-yy-slash-hs-slash-ip-domera', 'DDZ3026-YY/HS/IP DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', '', 'NTSC:720x480/PAL:720x576', 'Dome', '', '3', '', 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz3018-yy-slash-hs-slash-ip-domera', 'DDZ3018-YY/HS/IP DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', '', 'NTSC:720x480/PAL:720x576', 'Dome', '', '3', '', 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz4010-yy-slash-hs-slash-hd-domera', 'DDZ4010-YY/HS/HD DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1920x1080', 'Dome', '', '3', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddz4020-yy-slash-hs-slash-hd-domera', 'DDZ4020-YY/HS/HD DOMERA', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1920x1080', 'Dome', '', '3', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4010hdv', 'DDF4010HDV', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1920x1080', 'Dome', '', '3', '', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4820hdv-dn', 'DDF4820HDV-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '2048x1536', 'Dome', '', '3', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4220hdv-picodome', 'DDF4220HDV Picodome', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Dome', '', '3', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4320hd-dn', 'DDF4320HD-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Dome', '', '3', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4620hdv-dn', 'DDF4620HDV-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Dome', '', '3', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4520hdv-dn', 'DDF4520HDV-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Dome', '', '3', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'ddf4920hdv-dn', 'DDF4920HDV-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '2592x1920', 'Dome', '', '3', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'mdf4220hd', 'MDF4220HD', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Box', '', '3', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'df4620hd-dn', 'DF4620HD-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '1280x960', 'Box', '', '3', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'df4820hd-dn', 'DF4820HD-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '2048x1536', 'Box', '', '3', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'df4920hd-dn', 'DF4920HD-DN', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '2592x1920', 'Box', '', '3', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'df4920hd-dn-slash-ir', 'DF4920HD-DN/IR', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '2048x1536', 'Box', '', '3', '', 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'panomera-s4', 'Panomera S4', '15', 'live/image0.jpg', 'live/image0.jpg', 'encoder1', '40MP', 'Box', '', '3', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'dallmeier.com');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6466e-hn', 'DH-SD6466E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6465e-hn', 'DH-SD6465E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6463e-hn', 'DH-SD6463E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6470-hn', 'DH-SD6470-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6436e-hn', 'DH-SD6436E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6423e-hn', 'DH-SD6423E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6423c-hn', 'DH-SD6423C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '640x480', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6366e-hn', 'DH-SD6366E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6365e-hn', 'DH-SD6365E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6363e-hn', 'DH-SD6363E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6370-hn', 'DH-SD6370-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6336e-hn', 'DH-SD6336E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6323e-hn', 'DH-SD6323E-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', 'NTSC:976x494/PAL:976x582', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6323c-hn', 'DH-SD6323C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '640x480', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd3282d-gn', 'DH-SD3282D-GN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd6981c-hn', 'SD6981C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd6981b-hn', 'SD6981B-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6581c-hn', 'DH-SD6581C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6581b-hn', 'DH-SD6581B-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'hd-sd6980c-hn', 'HD-SD6980C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'hd-sd6980-hn', 'HD-SD6980-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65c80c-hn', 'DH-SD65C80C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65c80-hn', 'DH-SD65C80-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6580c-hn', 'DH-SD6580C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6580-hn', 'DH-SD6580-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dhsd-6982c-hn', 'DHSD-6982C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6982a-hn', 'DH-SD6982A-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65c82c-hn', 'DH-SD65C82C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65c82a-hn', 'DH-SD65C82A-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6582c-hn', 'DH-SD6582C-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6582a-hn', 'DH-SD6582A-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6983a-hn', 'DH-SD6983A-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6583a-hn', 'DH-SD6583A-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6c220s-hn', 'DH-SD6C220S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd63220s-hn', 'DH-SD63220S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd42c212s-hn', 'DH-SD42C212S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd42212s-hn', 'DH-SD42212S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd40212s-hn', 'DH-SD40212S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6c120s-hn', 'DH-SD6C120S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd63120s-hn', 'DH-SD63120S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6c230s-hn', 'DH-SD6C230S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd63230s-hn', 'DH-SD63230S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd59120s-hn', 'DH-SD59120S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '1', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd59230s-hn', 'DH-SD59230S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '1', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd59220s-hn', 'DH-SD59220S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '1', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd59212s-hn', 'DH-SD59212S-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '1', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw230-hn', 'DH-SD6AW230-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw220-hn', 'DH-SD6AW220-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw230-hni', 'DH-SD6AW230-HNI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw220-hni', 'DH-SD6AW220-HNI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw130-hn', 'DH-SD6AW130-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6aw320-hn', 'DH-SD6AW320-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a320-hn', 'DH-SD6A320-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65320-hn', 'DH-SD65320-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65s220-hni', 'DH-SD65S220-HNI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65230-hni', 'DH-SD65230-HNI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65220-hni', 'DH-SD65220-HNI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a130-hn', 'DH-SD6A130-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a120-hn', 'DH-SD6A120-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a118-hn', 'DH-SD6A118-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd65130-hn', 'SD65130-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd65120-hn', 'SD65120-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd65118-hn', 'SD65118-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6as220-hn', 'DH-SD6AS220-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a230-hn', 'DH-SD6A230-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd6a220-hn', 'DH-SD6A220-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65s220-hn', 'DH-SD65S220-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65230-hn', 'DH-SD65230-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-sd65220-hn', 'DH-SD65220-HN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1944x1092', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k200w', 'DH-IPC-K200W', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k200a', 'DH-IPC-K200A', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k200', 'DH-IPC-K200', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-kw100w', 'DH-IPC-KW100W', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-kw100a', 'DH-IPC-KW100A', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-kw100', 'DH-IPC-KW100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k100w', 'DH-IPC-K100W', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k100a', 'DH-IPC-K100A', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-k100', 'DH-IPC-K100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-kw12-w', 'DH-IPC-KW12(W)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-kw10-w', 'DH-IPC-KW10(W)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Other', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw1105', 'DH-IPC-HFW1105', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw1100', 'DH-IPC-HFW1100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw1105s', 'DH-IPC-HDW1105S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw1100s', 'DH-IPC-HDW1100S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hd1105c', 'DH-IPC-HD1105C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hd1100c', 'DH-IPC-HD1100C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x720', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hd2200c', 'DH-IPC-HD2200C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw2200s', 'DH-IPC-HDW2200S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw2200-b', 'DH-IPC-HFW2200-B', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw2200', 'DH-IPC-HFW2200', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw2100', 'DH-IPC-HDW2100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hd2100', 'DH-IPC-HD2100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw2100', 'DH-IPC-HFW2100', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw2200s', 'DH-IPC-HFW2200S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw2100s', 'DH-IPC-HFW2100S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw1105s', 'DH-IPC-HFW1105S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw1100s', 'DH-IPC-HFW1100S', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4300f-pt', 'DH-IPC-HDB4300F-PT', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4200f-pt', 'DH-IPC-HDB4200F-PT', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4100f-pt', 'DH-IPC-HDB4100F-PT', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5202n-di', 'IPC-HDBW5202N-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5200n-di', 'IPC-HDBW5200N-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5202p-di', 'IPC-HDBW5202P-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5200p-di', 'IPC-HDBW5200P-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdb5202n-di', 'IPC-HDB5202N-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdb5200n-di', 'IPC-HDB5200N-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdb5202p-di', 'IPC-HDB5202P-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdb5200p-di', 'IPC-HDB5200P-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4200c', 'DH-IPC-HDW4200C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4100c', 'DH-IPC-HDW4100C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4300c', 'DH-IPC-HDW4300C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2052x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4300e', 'DH-IPC-HFW4300E', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2052x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4200e', 'DH-IPC-HFW4200E', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4100e', 'DH-IPC-HFW4100E', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4200c-a', 'DH-IPC-HDB4200C-(A)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4300c-a', 'DH-IPC-HDB4300C-(A)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2052x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb4100c-a', 'DH-IPC-HDB4100C-(A)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4300sn', 'DH-IPC-HDW4300SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4300sp', 'DH-IPC-HDW4300SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4200sn', 'DH-IPC-HDW4200SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4200sp', 'DH-IPC-HDW4200SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4100sn', 'DH-IPC-HDW4100SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdw4100sp', 'DH-IPC-HDW4100SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4300sn', 'DH-IPC-HFW4300SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4300sp', 'DH-IPC-HFW4300SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4200sn', 'DH-IPC-HFW4200SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4200sp', 'DH-IPC-HFW4200SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4100sn', 'DH-IPC-HFW4100SN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw4100sp', 'DH-IPC-HFW4100SP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5202n', 'DH-IPC-HDBW5202N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5202p', 'DH-IPC-HDBW5202P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5200n', 'DH-IPC-HDBW5200N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5200p', 'DH-IPC-HDBW5200P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5100n', 'DH-IPC-HDBW5100N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdbw5100p', 'DH-IPC-HDBW5100P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5202n', 'DH-IPC-HDB5202N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5200n', 'DH-IPC-HDB5200N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5202p', 'DH-IPC-HDB5202P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5200p', 'DH-IPC-HDB5200P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5100n', 'DH-IPC-HDB5100N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hdb5100p', 'DH-IPC-HDB5100P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5202c', 'DH-IPC-HFW5202C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5200c', 'DH-IPC-HFW5200C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100cn', 'DH-IPC-HFW5100CN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100cp', 'DH-IPC-HFW5100CP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hf5200n', 'IPC-HF5200N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hf5200p', 'IPC-HF5200P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hf5100n', 'IPC-HF5100N', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hf5100p', 'IPC-HF5100P', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5200dn', 'DH-IPC-HFW5200DN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5200dp', 'DH-IPC-HFW5200DP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100dn', 'DH-IPC-HFW5100DN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100dp', 'DH-IPC-HFW5100DP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5200n-ira', 'DH-IPC-HFW5200N-IRA', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5200p-ira', 'DH-IPC-HFW5200P-IRA', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100n-ira', 'DH-IPC-HFW5100N-IRA', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw5100p-ira', 'DH-IPC-HFW5100P-IRA', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5302', 'IPC-HDBW5302', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5300', 'IPC-HDBW5300', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hfw5302c', 'IPC-HFW5302C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hfw5300c', 'IPC-HFW5300C', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw4100e-a-s', 'IPC-HDBW4100E-(A)(S)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw4200e-a-s', 'IPC-HDBW4200E-(A)(S)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw4300e-a-s', 'IPC-HDBW4300E-(A)(S)', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2052x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5302-di', 'IPC-HDBW5302-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw5300-di', 'IPC-HDBW5300-DI', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdbw8301', 'IPC-HDBW8301', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'ipc-hdb8301', 'IPC-HDB8301', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Dome', 'admin', 'admin', '1', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hf8301en', 'DH-IPC-HF8301EN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Box', 'admin', 'admin', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hf8301ep', 'DH-IPC-HF8301EP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Box', 'admin', 'admin', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw8301en', 'DH-IPC-HFW8301EN', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'dh-ipc-hfw8301ep', 'DH-IPC-HFW8301EP', '16', 'onvif/media_service/snapshot', '', 'cam/realmonitor', '2048x1536', 'Bullet', 'admin', 'admin', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.dahuasecurity.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip81100', 'TVIP81100', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip81000', 'TVIP81000', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip82100', 'TVIP82100', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip82000', 'TVIP82000', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip52502', 'TVIP52502', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip51550', 'TVIP51550', '17', '', 'video.mjpg', 'video.h264', '1280x1024', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip60550', 'TVIP60550', '17', '', 'video.mjpg', 'video.h264', '640x480', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip62500', 'TVIP62500', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip41550', 'TVIP41550', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip72000', 'TVIP72000', '17', '', 'video.mjpg', 'video.h264', '1600x1200', 'Dome', 'admin', '12345', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip71551', 'TVIP71551', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip71501', 'TVIP71501', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip72500', 'TVIP72500', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip31001', 'TVIP31001', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip31551', 'TVIP31551', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip31501', 'TVIP31501', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip31050', 'TVIP31050', '17', '', 'video.mjpg', 'video.h264', '1280x1024', 'Dome', 'admin', '12345', '', 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip32500', 'TVIP32500', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip41500', 'TVIP41500', '17', '', 'video.mjpg', 'video.h264', '1280x960', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip0055b', 'TVIP0055B', '17', '', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', '12345', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip10005b', 'TVIP10005B', '17', '', 'video.mjpg', 'video.h264', '640x480', 'Other', 'admin', '12345', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip21552', 'TVIP21552', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip21502', 'TVIP21502', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip22500', 'TVIP22500', '17', '', 'video.mjpg', 'video.h264', '1920x1080', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip11552', 'TVIP11552', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'tvip11502', 'TVIP11502', '17', '', 'video.mjpg', 'video.h264', '1280x720', 'Other', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://abus.com/');
INSERT INTO "cameras" 
VALUES (NULL, 'back-camera', 'Back Camera', '18', 'back.jpg', '', '', '', 'Other', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/783/original/9ca9bc91ca20fa9d697226e4d2001592.jpg?1404835892";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'front-camera', 'Front Camera', '18', 'front.jpg', '', '', '', 'Other', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/782/original/99092a1dbf1cc99adec1a3e1bb985a7f.jpg?1404835859";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8620', 'FI8620', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '729x576', 'Dome', 'admin', '', '', 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9819w', 'FI9819W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Dome', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9828w', 'FI9828W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Dome', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9851p', 'FI9851P', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Dome', 'admin', '', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8608w-b', 'FI8608W(B)', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8602w-s', 'FI8602W(S)', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '729x576', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8601w-s', 'FI8601W(S)', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '729x576', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9802w', 'FI9802W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9801w', 'FI9801W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8907w', 'FI8907W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8909w', 'FI8909W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8904w', 'FI8904W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Bullet', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8905e', 'FI8905E', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Bullet', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8905w', 'FI8905W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Bullet', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8906w', 'FI8906W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Bullet', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8916w', 'FI8916W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8918w', 'FI8918W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8910e', 'FI8910E', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi8910w', 'FI8910W', '19', 'snapshot.cgi', 'videostream.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9804w', 'FI9804W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9805e', 'FI9805E', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9805w', 'FI9805W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Bullet', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9803p', 'FI9803P', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Box', 'admin', '', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9818w', 'FI9818W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9821w', 'FI9821W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9821p', 'FI9821P', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9831w', 'FI9831W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9826w', 'FI9826W', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fi9826p', 'FI9826P', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x960', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'fosbaby', 'Fosbaby', '19', 'cgi-bin/CGIProxy.fcgi?cmd=snapPicture2', 'cgi-bin/CGIStream.cgi?cmd=GetMJStream', 'videoMain', '1280x720', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'li175mb', 'Li175MB', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', 'h264', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/li175m.php');
INSERT INTO "cameras" 
VALUES (NULL, 'li175cb', 'Li175CB', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', 'h264', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/li175ca.php');
INSERT INTO "cameras" 
VALUES (NULL, 'li165', 'Li165', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', 'h264', '1376x1032', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/li165.php');
INSERT INTO "cameras" 
VALUES (NULL, 'li045', 'Li045', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', 'h264', '720x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/li045.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le575', 'Le575', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '2560x1920', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le575.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le375', 'Le375', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le375.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le175m', 'Le175M', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le175m.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le175ca', 'Le175CA', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le175ca.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le165', 'Le165', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '1376x1032', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le165.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le075', 'Le075', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', '', '640x460', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le075.php');
INSERT INTO "cameras" 
VALUES (NULL, 'le045', 'Le045', '20', 'cgi-bin/nph-image.jpg', 'cgi-bin/nph-video', 'h264', '720x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.lumenera.com/products/surveillance-cameras/le045.php');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm830w', 'ICA-HM830W', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48401');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-w8500', 'ICA-W8500', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1920', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48518');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-w8100-cld', 'ICA-W8100-CLD', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1280x720', 'Box', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48464');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-8500', 'ICA-8500', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1920', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48508');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-8350', 'ICA-8350', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1536x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=45908');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm620', 'ICA-HM620', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=44938');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-h652', 'ICA-H652', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', 'ntsc720x480/pal720x576', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48442');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm227w', 'ICA-HM227W', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=42087');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm136', 'ICA-HM136', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=33705');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-5550v', 'ICA-5550V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48428');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-5350v', 'ICA-5350V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=42578');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-5250v', 'ICA-5250V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48427');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm132', 'ICA-HM132', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=35010');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-4500v', 'ICA-4500V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2592x1944', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48250');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-4210p', 'ICA-4210P', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48551');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-4200v', 'ICA-4200V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=45907');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm316w', 'ICA-HM316W', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=47248');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm316', 'ICA-HM316', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=47382');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm312', 'ICA-HM312', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=38844');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3550v', 'ICA-3550V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2592x1944', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48445');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3350v', 'ICA-3350V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=42951');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3260', 'ICA-3260', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48549');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3250v', 'ICA-3250V', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=45906');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm351', 'ICA-HM351', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=37829');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-2250vt', 'ICA-2250VT', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48514');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm127', 'ICA-HM127', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=39230');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-2500', 'ICA-2500', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '2592x1944', 'Box', 'admin', 'admin', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=44281');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-2200', 'ICA-2200', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=42576');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-w8100', 'ICA-W8100', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1280x720', 'Box', 'admin', 'admin', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48447');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-w1200', 'ICA-W1200', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48260');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm101w', 'ICA-HM101W', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48536');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm101', 'ICA-HM101', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1600x1200', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48535');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-1200', 'ICA-1200', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=47777');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-hm131', 'ICA-HM131', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=42712');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-5250', 'ICA-5250', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48399');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-4230s', 'ICA-4230S', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48540');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3200', 'ICA-3200', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48438');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-5150', 'ICA-5150', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48533');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-4130s', 'ICA-4130S', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1280x960', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48539');
INSERT INTO "cameras" 
VALUES (NULL, 'ica-3110', 'ICA-3110', '21', 'cgi-bin/jpg/image.cgi', ':8080', 'h264', '1280x800', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.planet.com.tw/en/product/product.php?id=48532');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-7811', 'TCM-7811', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-7811');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-7411', 'TCM-7411', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-7411');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-6630', 'TCM-6630', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '720x480', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-6630');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-5611', 'TCM-5611', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5611');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-5601', 'TCM-5601', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5601');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-5311', 'TCM-5311', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5311');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-5111', 'TCM-5111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5111');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-4511', 'TCM-4511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-4511');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-4001', 'TCM-4001', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '640x480', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-4001');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-3511', 'TCM-3511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-3511');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-3411', 'TCM-3411', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-3401/TCM-3411');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-3401', 'TCM-3401', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-3401/TCM-3411');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-3111', 'TCM-3111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-3111');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-1511', 'TCM-1511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Bullet', 'admin', '123456', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-1511');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-1231', 'TCM-1231', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-1231');
INSERT INTO "cameras" 
VALUES (NULL, 'tcm-1111', 'TCM-1111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-1111');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-7211', 'KCM-7211', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-7211');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5311e', 'KCM-5311E', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5311E');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5111', 'KCM-5111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5111');
INSERT INTO "cameras" 
VALUES (NULL, 'e74', 'E74', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E74');
INSERT INTO "cameras" 
VALUES (NULL, 'e43', 'E43', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E43');
INSERT INTO "cameras" 
VALUES (NULL, 'e42', 'E42', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E42');
INSERT INTO "cameras" 
VALUES (NULL, 'e23', 'E23', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E23');
INSERT INTO "cameras" 
VALUES (NULL, 'd22v', 'D22V', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D22V');
INSERT INTO "cameras" 
VALUES (NULL, 'd21v', 'D21V', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D21V');
INSERT INTO "cameras" 
VALUES (NULL, 'd21f', 'D21F', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D21F');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-8511', 'ACM-8511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'ntsc:720x480/pal:720x576', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-8511');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-7411', 'ACM-7411', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-7411');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-5811', 'ACM-5811', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'ntsc:720x480/pal:720x576', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-5811');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-5611', 'ACM-5611', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5611');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-5601', 'ACM-5601', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=TCM-5601');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-4201', 'ACM-4201', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Box', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-4201');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3703', 'ACM-3703', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3703');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3701e', 'ACM-3701E', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3701E');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3701', 'ACM-3701', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3701');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3603', 'ACM-3603', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '640x480', 'Dome', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3603');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3601', 'ACM-3601', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '640x480', 'Dome', 'admin', '123456', '', 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3601');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-3311', 'ACM-3311', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'ntsc:720x480/pal:720x576', 'Dome', 'admin', '123456', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-3311');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-1511', 'ACM-1511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Bullet', 'admin', '123456', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-1511');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-1431', 'ACM-1431', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'ntsc:720x480/pal:720x576', 'Bullet', 'admin', '123456', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-1431');
INSERT INTO "cameras" 
VALUES (NULL, 'acm-1231', 'ACM-1231', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x1024', 'Bullet', 'admin', '123456', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=ACM-1231');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-8211', 'KCM-8211', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-8211');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-8111', 'KCM-8111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-8111');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-7911', 'KCM-7911', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-7911');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-7311', 'KCM-7311', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-7311');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-7111', 'KCM-7111', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-7111');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5611', 'KCM-5611', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5611');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5511', 'KCM-5511', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5511');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5211e', 'KCM-5211E', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5211E');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-5211', 'KCM-5211', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-5211');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-3911', 'KCM-3911', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-3911');
INSERT INTO "cameras" 
VALUES (NULL, 'kcm-3311', 'KCM-3311', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2032x1920', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=KCM-3311');
INSERT INTO "cameras" 
VALUES (NULL, 'i98', 'I98', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I98');
INSERT INTO "cameras" 
VALUES (NULL, 'i97', 'I97', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I97');
INSERT INTO "cameras" 
VALUES (NULL, 'i96', 'I96', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I96');
INSERT INTO "cameras" 
VALUES (NULL, 'i95', 'I95', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I95');
INSERT INTO "cameras" 
VALUES (NULL, 'i94', 'I94', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I94');
INSERT INTO "cameras" 
VALUES (NULL, 'i93', 'I93', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I93');
INSERT INTO "cameras" 
VALUES (NULL, 'i928', 'I928', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I928');
INSERT INTO "cameras" 
VALUES (NULL, 'i927', 'I927', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I927');
INSERT INTO "cameras" 
VALUES (NULL, 'i925', 'I925', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I925');
INSERT INTO "cameras" 
VALUES (NULL, 'i920', 'I920', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I920');
INSERT INTO "cameras" 
VALUES (NULL, 'i92', 'I92', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I92');
INSERT INTO "cameras" 
VALUES (NULL, 'i919', 'I919', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I919');
INSERT INTO "cameras" 
VALUES (NULL, 'i917', 'I917', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I917');
INSERT INTO "cameras" 
VALUES (NULL, 'i916', 'I916', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I916');
INSERT INTO "cameras" 
VALUES (NULL, 'i914', 'I914', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I914');
INSERT INTO "cameras" 
VALUES (NULL, 'i913', 'I913', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I913');
INSERT INTO "cameras" 
VALUES (NULL, 'i912', 'I912', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I912');
INSERT INTO "cameras" 
VALUES (NULL, 'i911', 'I911', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I911');
INSERT INTO "cameras" 
VALUES (NULL, 'i910', 'I910', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I910');
INSERT INTO "cameras" 
VALUES (NULL, 'i91', 'I91', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I91');
INSERT INTO "cameras" 
VALUES (NULL, 'i88', 'I88', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I88');
INSERT INTO "cameras" 
VALUES (NULL, 'i87', 'I87', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I87');
INSERT INTO "cameras" 
VALUES (NULL, 'i86', 'I86', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I86');
INSERT INTO "cameras" 
VALUES (NULL, 'i85', 'I85', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I85');
INSERT INTO "cameras" 
VALUES (NULL, 'i84', 'I84', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I84');
INSERT INTO "cameras" 
VALUES (NULL, 'i83', 'I83', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I83');
INSERT INTO "cameras" 
VALUES (NULL, 'i810', 'I810', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I810');
INSERT INTO "cameras" 
VALUES (NULL, 'i75', 'I75', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I75');
INSERT INTO "cameras" 
VALUES (NULL, 'i74', 'I74', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I74');
INSERT INTO "cameras" 
VALUES (NULL, 'i73', 'I73', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I73');
INSERT INTO "cameras" 
VALUES (NULL, 'i72', 'I72', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I72');
INSERT INTO "cameras" 
VALUES (NULL, 'i71', 'I71', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I71');
INSERT INTO "cameras" 
VALUES (NULL, 'i53', 'I53', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I53');
INSERT INTO "cameras" 
VALUES (NULL, 'i52', 'I52', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I52');
INSERT INTO "cameras" 
VALUES (NULL, 'i51', 'I51', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I51');
INSERT INTO "cameras" 
VALUES (NULL, 'i49', 'I49', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I49');
INSERT INTO "cameras" 
VALUES (NULL, 'i48', 'I48', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I48');
INSERT INTO "cameras" 
VALUES (NULL, 'i47', 'I47', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I47');
INSERT INTO "cameras" 
VALUES (NULL, 'i46', 'I46', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I46');
INSERT INTO "cameras" 
VALUES (NULL, 'i45', 'I45', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I45');
INSERT INTO "cameras" 
VALUES (NULL, 'i44', 'I44', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I44');
INSERT INTO "cameras" 
VALUES (NULL, 'i43', 'I43', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I43');
INSERT INTO "cameras" 
VALUES (NULL, 'i29', 'I29', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I29');
INSERT INTO "cameras" 
VALUES (NULL, 'i28', 'I28', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I28');
INSERT INTO "cameras" 
VALUES (NULL, 'i27', 'I27', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2688x1520', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I27');
INSERT INTO "cameras" 
VALUES (NULL, 'i26', 'I26', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I26');
INSERT INTO "cameras" 
VALUES (NULL, 'i25', 'I25', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I25');
INSERT INTO "cameras" 
VALUES (NULL, 'i24', 'I24', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I24');
INSERT INTO "cameras" 
VALUES (NULL, 'i23', 'I23', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=I23');
INSERT INTO "cameras" 
VALUES (NULL, 'e99', 'E99', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E99');
INSERT INTO "cameras" 
VALUES (NULL, 'e98', 'E98', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E98');
INSERT INTO "cameras" 
VALUES (NULL, 'e97', 'E97', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E97');
INSERT INTO "cameras" 
VALUES (NULL, 'e96', 'E96', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E96');
INSERT INTO "cameras" 
VALUES (NULL, 'e95', 'E95', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E95');
INSERT INTO "cameras" 
VALUES (NULL, 'e94', 'E94', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E94');
INSERT INTO "cameras" 
VALUES (NULL, 'e931', 'E931', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E931');
INSERT INTO "cameras" 
VALUES (NULL, 'e930', 'E930', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E930');
INSERT INTO "cameras" 
VALUES (NULL, 'e93', 'E93', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E93');
INSERT INTO "cameras" 
VALUES (NULL, 'e929', 'E929', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E929');
INSERT INTO "cameras" 
VALUES (NULL, 'e928', 'E928', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E928');
INSERT INTO "cameras" 
VALUES (NULL, 'e927', 'E927', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E927');
INSERT INTO "cameras" 
VALUES (NULL, 'e926', 'E926', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E926');
INSERT INTO "cameras" 
VALUES (NULL, 'e925', 'E925', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E925');
INSERT INTO "cameras" 
VALUES (NULL, 'e924', 'E924', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E924');
INSERT INTO "cameras" 
VALUES (NULL, 'e923m', 'E923M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E923M');
INSERT INTO "cameras" 
VALUES (NULL, 'e923', 'E923', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E923');
INSERT INTO "cameras" 
VALUES (NULL, 'e922m', 'E922M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E922M');
INSERT INTO "cameras" 
VALUES (NULL, 'e922', 'E922', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E922');
INSERT INTO "cameras" 
VALUES (NULL, 'e921m', 'E921M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E921M');
INSERT INTO "cameras" 
VALUES (NULL, 'e921', 'E921', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E921');
INSERT INTO "cameras" 
VALUES (NULL, 'e920m', 'E920M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E920M');
INSERT INTO "cameras" 
VALUES (NULL, 'e920', 'E920', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E920');
INSERT INTO "cameras" 
VALUES (NULL, 'e92', 'E92', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E92');
INSERT INTO "cameras" 
VALUES (NULL, 'e919m', 'E919M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E919M');
INSERT INTO "cameras" 
VALUES (NULL, 'e919', 'E919', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E919');
INSERT INTO "cameras" 
VALUES (NULL, 'e918m', 'E918M', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E918M');
INSERT INTO "cameras" 
VALUES (NULL, 'e918', 'E918', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E918');
INSERT INTO "cameras" 
VALUES (NULL, 'e910', 'E910', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E910');
INSERT INTO "cameras" 
VALUES (NULL, 'e91', 'E91', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E91');
INSERT INTO "cameras" 
VALUES (NULL, 'e89', 'E89', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E89');
INSERT INTO "cameras" 
VALUES (NULL, 'e88', 'E88', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E88');
INSERT INTO "cameras" 
VALUES (NULL, 'e86a', 'E86A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E86A');
INSERT INTO "cameras" 
VALUES (NULL, 'e86', 'E86', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E86');
INSERT INTO "cameras" 
VALUES (NULL, 'e85', 'E85', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E85');
INSERT INTO "cameras" 
VALUES (NULL, 'e84a', 'E84A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E84A');
INSERT INTO "cameras" 
VALUES (NULL, 'e84', 'E84', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E84');
INSERT INTO "cameras" 
VALUES (NULL, 'e83a', 'E83A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E83A');
INSERT INTO "cameras" 
VALUES (NULL, 'e83', 'E83', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E83');
INSERT INTO "cameras" 
VALUES (NULL, 'e82a', 'E82A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E82A');
INSERT INTO "cameras" 
VALUES (NULL, 'e822', 'E822', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E822');
INSERT INTO "cameras" 
VALUES (NULL, 'e821', 'E821', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E821');
INSERT INTO "cameras" 
VALUES (NULL, 'e820', 'E820', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E820');
INSERT INTO "cameras" 
VALUES (NULL, 'e82', 'E82', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E82');
INSERT INTO "cameras" 
VALUES (NULL, 'e819', 'E819', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E819');
INSERT INTO "cameras" 
VALUES (NULL, 'e817', 'E817', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E817');
INSERT INTO "cameras" 
VALUES (NULL, 'e816', 'E816', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E816');
INSERT INTO "cameras" 
VALUES (NULL, 'e815', 'E815', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E815');
INSERT INTO "cameras" 
VALUES (NULL, 'e814', 'E814', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E814');
INSERT INTO "cameras" 
VALUES (NULL, 'e810', 'E810', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E810');
INSERT INTO "cameras" 
VALUES (NULL, 'e81', 'E81', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E81');
INSERT INTO "cameras" 
VALUES (NULL, 'e78', 'E78', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E78');
INSERT INTO "cameras" 
VALUES (NULL, 'e77', 'E77', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E77');
INSERT INTO "cameras" 
VALUES (NULL, 'e76', 'E76', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E76');
INSERT INTO "cameras" 
VALUES (NULL, 'e75', 'E75', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E75');
INSERT INTO "cameras" 
VALUES (NULL, 'e74a', 'E74A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E74A');
INSERT INTO "cameras" 
VALUES (NULL, 'e73', 'E73', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E73');
INSERT INTO "cameras" 
VALUES (NULL, 'e72a', 'E72A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E72A');
INSERT INTO "cameras" 
VALUES (NULL, 'e72', 'E72', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E72');
INSERT INTO "cameras" 
VALUES (NULL, 'e710', 'E710', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E710');
INSERT INTO "cameras" 
VALUES (NULL, 'e71', 'E71', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E71');
INSERT INTO "cameras" 
VALUES (NULL, 'e69', 'E69', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E69');
INSERT INTO "cameras" 
VALUES (NULL, 'e68', 'E68', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E68');
INSERT INTO "cameras" 
VALUES (NULL, 'e67', 'E67', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E67');
INSERT INTO "cameras" 
VALUES (NULL, 'e66a', 'E66A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E66A');
INSERT INTO "cameras" 
VALUES (NULL, 'e66', 'E66', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E66');
INSERT INTO "cameras" 
VALUES (NULL, 'e65a', 'E65A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E65A');
INSERT INTO "cameras" 
VALUES (NULL, 'e65', 'E65', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E65');
INSERT INTO "cameras" 
VALUES (NULL, 'e64', 'E64', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E64');
INSERT INTO "cameras" 
VALUES (NULL, 'e63a', 'E63A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E63A');
INSERT INTO "cameras" 
VALUES (NULL, 'e63', 'E63', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E63');
INSERT INTO "cameras" 
VALUES (NULL, 'e62a', 'E62A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E62A');
INSERT INTO "cameras" 
VALUES (NULL, 'e621', 'E621', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E621');
INSERT INTO "cameras" 
VALUES (NULL, 'e620', 'E620', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E620');
INSERT INTO "cameras" 
VALUES (NULL, 'e62', 'E62', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E62');
INSERT INTO "cameras" 
VALUES (NULL, 'e618', 'E618', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E618');
INSERT INTO "cameras" 
VALUES (NULL, 'e617', 'E617', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E617');
INSERT INTO "cameras" 
VALUES (NULL, 'e616', 'E616', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E616');
INSERT INTO "cameras" 
VALUES (NULL, 'e611', 'E611', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E611');
INSERT INTO "cameras" 
VALUES (NULL, 'e610', 'E610', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E610');
INSERT INTO "cameras" 
VALUES (NULL, 'e61', 'E61', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E61');
INSERT INTO "cameras" 
VALUES (NULL, 'e59', 'E59', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E59');
INSERT INTO "cameras" 
VALUES (NULL, 'e58', 'E58', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E58');
INSERT INTO "cameras" 
VALUES (NULL, 'e57', 'E57', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E57');
INSERT INTO "cameras" 
VALUES (NULL, 'e56', 'E56', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E56');
INSERT INTO "cameras" 
VALUES (NULL, 'e54', 'E54', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E54');
INSERT INTO "cameras" 
VALUES (NULL, 'e53', 'E53', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E53');
INSERT INTO "cameras" 
VALUES (NULL, 'e52', 'E52', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E52');
INSERT INTO "cameras" 
VALUES (NULL, 'e51', 'E51', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E51');
INSERT INTO "cameras" 
VALUES (NULL, 'e49', 'E49', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E49');
INSERT INTO "cameras" 
VALUES (NULL, 'e48', 'E48', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E48');
INSERT INTO "cameras" 
VALUES (NULL, 'e47', 'E47', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E47');
INSERT INTO "cameras" 
VALUES (NULL, 'e46', 'E46', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E46');
INSERT INTO "cameras" 
VALUES (NULL, 'e45', 'E45', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E45');
INSERT INTO "cameras" 
VALUES (NULL, 'e44', 'E44', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E44');
INSERT INTO "cameras" 
VALUES (NULL, 'e43b', 'E43B', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E43B');
INSERT INTO "cameras" 
VALUES (NULL, 'e43a', 'E43A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E43A');
INSERT INTO "cameras" 
VALUES (NULL, 'e42b', 'E42B', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E41B');
INSERT INTO "cameras" 
VALUES (NULL, 'e42a', 'E42A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E42A');
INSERT INTO "cameras" 
VALUES (NULL, 'e420', 'E420', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E420');
INSERT INTO "cameras" 
VALUES (NULL, 'e41a', 'E41A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E41A');
INSERT INTO "cameras" 
VALUES (NULL, 'e418', 'E418', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E418');
INSERT INTO "cameras" 
VALUES (NULL, 'e417', 'E417', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E417');
INSERT INTO "cameras" 
VALUES (NULL, 'e415', 'E415', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E415');
INSERT INTO "cameras" 
VALUES (NULL, 'e414', 'E414', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E414');
INSERT INTO "cameras" 
VALUES (NULL, 'e413', 'E413', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E413');
INSERT INTO "cameras" 
VALUES (NULL, 'e41', 'E41', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E41');
INSERT INTO "cameras" 
VALUES (NULL, 'e38', 'E38', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E38');
INSERT INTO "cameras" 
VALUES (NULL, 'e37', 'E37', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E37');
INSERT INTO "cameras" 
VALUES (NULL, 'e36', 'E36', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E36');
INSERT INTO "cameras" 
VALUES (NULL, 'e35', 'E35', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E35');
INSERT INTO "cameras" 
VALUES (NULL, 'e34', 'E34', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E34');
INSERT INTO "cameras" 
VALUES (NULL, 'e33', 'E33', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E33');
INSERT INTO "cameras" 
VALUES (NULL, 'e32', 'E32', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E32');
INSERT INTO "cameras" 
VALUES (NULL, 'e31', 'E31', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E31');
INSERT INTO "cameras" 
VALUES (NULL, 'e281', 'E281', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E281');
INSERT INTO "cameras" 
VALUES (NULL, 'e280', 'E280', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E280');
INSERT INTO "cameras" 
VALUES (NULL, 'e271', 'E271', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E271');
INSERT INTO "cameras" 
VALUES (NULL, 'e270', 'E270', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E270');
INSERT INTO "cameras" 
VALUES (NULL, 'e25a', 'E25A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E25A');
INSERT INTO "cameras" 
VALUES (NULL, 'e25', 'E25', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E25');
INSERT INTO "cameras" 
VALUES (NULL, 'e24a', 'E24A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E24A');
INSERT INTO "cameras" 
VALUES (NULL, 'e24', 'E24', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E24');
INSERT INTO "cameras" 
VALUES (NULL, 'e23a', 'E23A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E23A');
INSERT INTO "cameras" 
VALUES (NULL, 'e22va', 'E22VA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E22VA');
INSERT INTO "cameras" 
VALUES (NULL, 'e22v', 'E22V', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E22V');
INSERT INTO "cameras" 
VALUES (NULL, 'e22fa', 'E22FA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E22FA');
INSERT INTO "cameras" 
VALUES (NULL, 'e22f', 'E22F', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E22F');
INSERT INTO "cameras" 
VALUES (NULL, 'e224', 'E224', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E224');
INSERT INTO "cameras" 
VALUES (NULL, 'e223', 'E223', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E223');
INSERT INTO "cameras" 
VALUES (NULL, 'e222', 'E222', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E222');
INSERT INTO "cameras" 
VALUES (NULL, 'e221', 'E221', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E221');
INSERT INTO "cameras" 
VALUES (NULL, 'e220', 'E220', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E220');
INSERT INTO "cameras" 
VALUES (NULL, 'e21va', 'E21VA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E21VA');
INSERT INTO "cameras" 
VALUES (NULL, 'e21v', 'E21V', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E21V');
INSERT INTO "cameras" 
VALUES (NULL, 'e21fa', 'E21FA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E21FA');
INSERT INTO "cameras" 
VALUES (NULL, 'e21f', 'E21F', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E21F');
INSERT INTO "cameras" 
VALUES (NULL, 'e219', 'E219', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E219');
INSERT INTO "cameras" 
VALUES (NULL, 'e217', 'E217', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E217');
INSERT INTO "cameras" 
VALUES (NULL, 'e215', 'E215', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E215');
INSERT INTO "cameras" 
VALUES (NULL, 'e214', 'E214', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E214');
INSERT INTO "cameras" 
VALUES (NULL, 'e213', 'E213', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E213');
INSERT INTO "cameras" 
VALUES (NULL, 'e212', 'E212', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E212');
INSERT INTO "cameras" 
VALUES (NULL, 'e16', 'E16', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E16');
INSERT INTO "cameras" 
VALUES (NULL, 'e15', 'E15', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E15');
INSERT INTO "cameras" 
VALUES (NULL, 'e14', 'E14', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E14');
INSERT INTO "cameras" 
VALUES (NULL, 'e13', 'E13', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E13');
INSERT INTO "cameras" 
VALUES (NULL, 'e12', 'E12', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E12');
INSERT INTO "cameras" 
VALUES (NULL, 'e11', 'E11', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=E11');
INSERT INTO "cameras" 
VALUES (NULL, 'd92', 'D92', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D92');
INSERT INTO "cameras" 
VALUES (NULL, 'd91', 'D91', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D91');
INSERT INTO "cameras" 
VALUES (NULL, 'd82a', 'D82A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D82A');
INSERT INTO "cameras" 
VALUES (NULL, 'd82', 'D82', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D82');
INSERT INTO "cameras" 
VALUES (NULL, 'd81a', 'D81A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D81A');
INSERT INTO "cameras" 
VALUES (NULL, 'd81', 'D81', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D81');
INSERT INTO "cameras" 
VALUES (NULL, 'd72a', 'D72A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D72A');
INSERT INTO "cameras" 
VALUES (NULL, 'd72', 'D72', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D72');
INSERT INTO "cameras" 
VALUES (NULL, 'd71a', 'D71A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D71A');
INSERT INTO "cameras" 
VALUES (NULL, 'd71', 'D71', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D71');
INSERT INTO "cameras" 
VALUES (NULL, 'd65a', 'D65A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D65A');
INSERT INTO "cameras" 
VALUES (NULL, 'd65', 'D65', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D65');
INSERT INTO "cameras" 
VALUES (NULL, 'd64', 'D64', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D64');
INSERT INTO "cameras" 
VALUES (NULL, 'd62', 'D62', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D62');
INSERT INTO "cameras" 
VALUES (NULL, 'd61a', 'D61A', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D61A');
INSERT INTO "cameras" 
VALUES (NULL, 'd61', 'D61', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D61');
INSERT INTO "cameras" 
VALUES (NULL, 'd55', 'D55', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D55');
INSERT INTO "cameras" 
VALUES (NULL, 'd54', 'D54', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D54');
INSERT INTO "cameras" 
VALUES (NULL, 'd52', 'D52', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D52');
INSERT INTO "cameras" 
VALUES (NULL, 'd51', 'D51', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D51');
INSERT INTO "cameras" 
VALUES (NULL, 'd42', 'D42', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D42');
INSERT INTO "cameras" 
VALUES (NULL, 'd41', 'D41', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D41');
INSERT INTO "cameras" 
VALUES (NULL, 'd32', 'D32', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D32');
INSERT INTO "cameras" 
VALUES (NULL, 'd31', 'D31', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D31');
INSERT INTO "cameras" 
VALUES (NULL, 'd22va', 'D22VA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D22VA');
INSERT INTO "cameras" 
VALUES (NULL, 'd22fa', 'D22FA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D22FA');
INSERT INTO "cameras" 
VALUES (NULL, 'd22f', 'D22F', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D22F');
INSERT INTO "cameras" 
VALUES (NULL, 'd21va', 'D21VA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D21VA');
INSERT INTO "cameras" 
VALUES (NULL, 'd21fa', 'D21FA', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D21FA');
INSERT INTO "cameras" 
VALUES (NULL, 'd12', 'D12', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D12');
INSERT INTO "cameras" 
VALUES (NULL, 'd11', 'D11', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x720', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=D11');
INSERT INTO "cameras" 
VALUES (NULL, 'b99', 'B99', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B99');
INSERT INTO "cameras" 
VALUES (NULL, 'b97', 'B97', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B97');
INSERT INTO "cameras" 
VALUES (NULL, 'b96', 'B96', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B96');
INSERT INTO "cameras" 
VALUES (NULL, 'b95', 'B95', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B95');
INSERT INTO "cameras" 
VALUES (NULL, 'b94', 'B94', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B94');
INSERT INTO "cameras" 
VALUES (NULL, 'b937', 'B937', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B937');
INSERT INTO "cameras" 
VALUES (NULL, 'b936', 'B936', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B936');
INSERT INTO "cameras" 
VALUES (NULL, 'b933', 'B933', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B933');
INSERT INTO "cameras" 
VALUES (NULL, 'b932', 'B932', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B932');
INSERT INTO "cameras" 
VALUES (NULL, 'b931', 'B931', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B931');
INSERT INTO "cameras" 
VALUES (NULL, 'b930', 'B930', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B930');
INSERT INTO "cameras" 
VALUES (NULL, 'b93', 'B93', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B93');
INSERT INTO "cameras" 
VALUES (NULL, 'b929', 'B929', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B929');
INSERT INTO "cameras" 
VALUES (NULL, 'b928', 'B928', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B928');
INSERT INTO "cameras" 
VALUES (NULL, 'b926', 'B926', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B926');
INSERT INTO "cameras" 
VALUES (NULL, 'b925', 'B925', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B925');
INSERT INTO "cameras" 
VALUES (NULL, 'b924', 'B924', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B924');
INSERT INTO "cameras" 
VALUES (NULL, 'b921', 'B921', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B921');
INSERT INTO "cameras" 
VALUES (NULL, 'b920', 'B920', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B920');
INSERT INTO "cameras" 
VALUES (NULL, 'b92', 'B92', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B92');
INSERT INTO "cameras" 
VALUES (NULL, 'b919', 'B919', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B919');
INSERT INTO "cameras" 
VALUES (NULL, 'b918', 'B918', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B918');
INSERT INTO "cameras" 
VALUES (NULL, 'b917', 'B917', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B917');
INSERT INTO "cameras" 
VALUES (NULL, 'b916', 'B916', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B916');
INSERT INTO "cameras" 
VALUES (NULL, 'b915', 'B915', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Dome', 'admin', '123456', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B915');
INSERT INTO "cameras" 
VALUES (NULL, 'b914', 'B914', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B914');
INSERT INTO "cameras" 
VALUES (NULL, 'b913', 'B913', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B913');
INSERT INTO "cameras" 
VALUES (NULL, 'b910', 'B910', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B910');
INSERT INTO "cameras" 
VALUES (NULL, 'b91', 'B91', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B91');
INSERT INTO "cameras" 
VALUES (NULL, 'b89', 'B89', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B89');
INSERT INTO "cameras" 
VALUES (NULL, 'b88', 'B88', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B88');
INSERT INTO "cameras" 
VALUES (NULL, 'b87', 'B87', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B87');
INSERT INTO "cameras" 
VALUES (NULL, 'b85', 'B85', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B85');
INSERT INTO "cameras" 
VALUES (NULL, 'b84', 'B84', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B84');
INSERT INTO "cameras" 
VALUES (NULL, 'b815', 'B815', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B815');
INSERT INTO "cameras" 
VALUES (NULL, 'b814', 'B814', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B814');
INSERT INTO "cameras" 
VALUES (NULL, 'b813', 'B813', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B813');
INSERT INTO "cameras" 
VALUES (NULL, 'b812', 'B812', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B812');
INSERT INTO "cameras" 
VALUES (NULL, 'b811', 'B811', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B811');
INSERT INTO "cameras" 
VALUES (NULL, 'b81', 'B81', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B81');
INSERT INTO "cameras" 
VALUES (NULL, 'b74', 'B74', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B74');
INSERT INTO "cameras" 
VALUES (NULL, 'b69', 'B69', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B69');
INSERT INTO "cameras" 
VALUES (NULL, 'b68', 'B68', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B68');
INSERT INTO "cameras" 
VALUES (NULL, 'b67', 'B67', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B67');
INSERT INTO "cameras" 
VALUES (NULL, 'b65', 'B65', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B65');
INSERT INTO "cameras" 
VALUES (NULL, 'b64', 'B64', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B64');
INSERT INTO "cameras" 
VALUES (NULL, 'b614', 'B614', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B614');
INSERT INTO "cameras" 
VALUES (NULL, 'b613', 'B613', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B613');
INSERT INTO "cameras" 
VALUES (NULL, 'b61', 'B61', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B61');
INSERT INTO "cameras" 
VALUES (NULL, 'b59', 'B59', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B59');
INSERT INTO "cameras" 
VALUES (NULL, 'b56', 'B56', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B56');
INSERT INTO "cameras" 
VALUES (NULL, 'b55', 'B55', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B55');
INSERT INTO "cameras" 
VALUES (NULL, 'b54', 'B54', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B54');
INSERT INTO "cameras" 
VALUES (NULL, 'b53', 'B53', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B53');
INSERT INTO "cameras" 
VALUES (NULL, 'b52', 'B52', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B52');
INSERT INTO "cameras" 
VALUES (NULL, 'b51', 'B51', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Dome', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B51');
INSERT INTO "cameras" 
VALUES (NULL, 'b48', 'B48', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B48');
INSERT INTO "cameras" 
VALUES (NULL, 'b47', 'B47', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B47');
INSERT INTO "cameras" 
VALUES (NULL, 'b45', 'B45', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B45');
INSERT INTO "cameras" 
VALUES (NULL, 'b44', 'B44', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B44');
INSERT INTO "cameras" 
VALUES (NULL, 'b422', 'B422', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B422');
INSERT INTO "cameras" 
VALUES (NULL, 'b421', 'B421', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B421');
INSERT INTO "cameras" 
VALUES (NULL, 'b420', 'B420', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B420');
INSERT INTO "cameras" 
VALUES (NULL, 'b418', 'B418', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B418');
INSERT INTO "cameras" 
VALUES (NULL, 'b417', 'B417', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B417');
INSERT INTO "cameras" 
VALUES (NULL, 'b416', 'B416', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B416');
INSERT INTO "cameras" 
VALUES (NULL, 'b415', 'B415', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B415');
INSERT INTO "cameras" 
VALUES (NULL, 'b414', 'B414', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B414');
INSERT INTO "cameras" 
VALUES (NULL, 'b413', 'B413', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B413');
INSERT INTO "cameras" 
VALUES (NULL, 'b411', 'B411', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B411');
INSERT INTO "cameras" 
VALUES (NULL, 'b410', 'B410', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B410');
INSERT INTO "cameras" 
VALUES (NULL, 'b41', 'B41', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Bullet', 'admin', '123456', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B41');
INSERT INTO "cameras" 
VALUES (NULL, 'b28', 'B28', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B28');
INSERT INTO "cameras" 
VALUES (NULL, 'b27', 'B27', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B27');
INSERT INTO "cameras" 
VALUES (NULL, 'b25', 'B25', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B25');
INSERT INTO "cameras" 
VALUES (NULL, 'b24', 'B24', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1280x960', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B24');
INSERT INTO "cameras" 
VALUES (NULL, 'b222', 'B222', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B222');
INSERT INTO "cameras" 
VALUES (NULL, 'b221', 'B221', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B221');
INSERT INTO "cameras" 
VALUES (NULL, 'b220', 'B220', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', 'Not Specified', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B220');
INSERT INTO "cameras" 
VALUES (NULL, 'b217', 'B217', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B217');
INSERT INTO "cameras" 
VALUES (NULL, 'b216', 'B216', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B216');
INSERT INTO "cameras" 
VALUES (NULL, 'b215', 'B215', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B215');
INSERT INTO "cameras" 
VALUES (NULL, 'b214', 'B214', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '1920x1080', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B214');
INSERT INTO "cameras" 
VALUES (NULL, 'b213', 'B213', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '4000x3000', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B213');
INSERT INTO "cameras" 
VALUES (NULL, 'b212', 'B212', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3072x2048', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B212');
INSERT INTO "cameras" 
VALUES (NULL, 'b211', 'B211', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2048x1536', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B211');
INSERT INTO "cameras" 
VALUES (NULL, 'b210', 'B210', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '3648x2736', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B210');
INSERT INTO "cameras" 
VALUES (NULL, 'b21', 'B21', '22', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&SNAPSHOT', 'cgi-bin/encoder?USER=[USERNAME]&PWD=[PASSWORD]&GET_STREAM', '', '2592x1944', 'Box', 'admin', '123456', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.acti.com/corpweb/Product/ProductInfo.aspx?q=B21');
INSERT INTO "cameras" 
VALUES (NULL, 'iqr53nr-f9', 'IQR53NR-F9', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqr52nr-f9', 'IQR52NR-F9', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqr51nr-f9', 'IQR51NR-F9', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm53wr-b5', 'IQM53WR-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm52wr-b5', 'IQM52WR-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm51wr-b5', 'IQM51WR-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd53wv-f2', 'IQD53WV-F2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd52wv-f2', 'IQD52WV-F2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd51wv-f2', 'IQD51WV-F2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763wi-w2', 'IQ763WI-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763wi-v16', 'IQ763WI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763wi-v6', 'IQ763WI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762wi-w2', 'IQ762WI-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762wi-v16', 'IQ762WI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762wi-v6', 'IQ762WI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq761wi-v16', 'IQ761WI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq761wi-v6', 'IQ761WI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863we-w2', 'IQ863WE-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863we-v16', 'IQ863WE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863we-v6', 'IQ863WE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862we-w2', 'IQ862WE-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862we-v16', 'IQ862WE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862we-v6', 'IQ862WE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq861we-v16', 'IQ861WE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq861we-v6', 'IQ861WE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33wx-a3', 'IQA33WX-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32wx-a3', 'IQA32WX-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33we-a3', 'IQA33WE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32we-a3', 'IQA32WE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31we-a3', 'IQA31WE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33wi-a3', 'IQA33WI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32wi-a3', 'IQA32WI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31wi-a3', 'IQA31WI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33wx-b5', 'IQA33WX-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33we-b5', 'IQA33WE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32wx-b5', 'IQA32WX-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32we-b5', 'IQA32WE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31we-b5', 'IQA31WE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33wi-b5', 'IQA33WI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2048x1536', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32wi-b5', 'IQA32WI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31wi-b5', 'IQA31WI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35nx-a3', 'IQA35NX-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35ne-a3', 'IQA35NE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35ni-a3', 'IQA35NI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32nx-a3', 'IQA32NX-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32ne-a3', 'IQA32NE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32ni-a3', 'IQA32NI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31ne-a3', 'IQA31NE-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31ni-a3', 'IQA31NI-A3', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd32sv-b7', 'IQD32SV-B7', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd31sv-b7', 'IQD31SV-B7', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd30sv-b7', 'IQD30SV-B7', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '720x480', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq031si-v11', 'IQ031SI-V11', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Box', 'root', 'system', '1', 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq865ne-w2', 'IQ865NE-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq865ne-v7', 'IQ865NE-V7', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863ne-w2', 'IQ863NE-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863ne-v16', 'IQ863NE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862ne-w2', 'IQ862NE-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862ne-v16', 'IQ862NE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq861ne-v16', 'IQ861NE-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq761ni-v16', 'IQ761NI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762ni-v16', 'IQ762NI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762ni-w2', 'IQ762NI-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763ni-v16', 'IQ763NI-V16', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763ni-w2', 'IQ763NI-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq765ni-v7', 'IQ765NI-V7', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq765ni-w2', 'IQ765NI-W2', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq030si-v11', 'IQ030SI-V11', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '720x480', 'Box', 'root', 'system', '1', 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq032si-v11', 'IQ032SI-V11', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Box', 'root', 'system', '1', 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq862ne-v6', 'IQ862NE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq765ni-v6', 'IQ765NI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq763ni-v6', 'IQ763NI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq762ni-v6', 'IQ762NI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq761ni-v6', 'IQ761NI-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq865ne-v6', 'IQ865NE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq863ne-v6', 'IQ863NE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iq861ne-v6', 'IQ861NE-V6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Bullet', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35nx-b6', 'IQA35NX-B6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35ne-b6', 'IQA35NE-B6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33ne-b5', 'IQA33NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32nx-b5', 'IQA32NX-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32ne-b5', 'IQA32NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31ni-b5', 'IQA31NI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa31ne-b5', 'IQA31NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd32sv-f1', 'IQD32SV-F1', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd31sv-f1', 'IQD31SV-F1', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqd30sv-f1', 'IQD30SV-F1', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '720x480', 'Dome', 'root', 'system', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa35ni-b6', 'IQA35NI-B6', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1920', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa33ni-b5', 'IQA33NI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '2560x1440', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqa32ni-b5', 'IQA32NI-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm32ne-b5', 'IQM32NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1920x1080', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm31ne-b5', 'IQM31NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '1280x720', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'iqm30ne-b5', 'IQM30NE-B5', '24', 'now.jpg', 'now.jpg?snap=spush', 'now.mp4', '720x480', 'Dome', 'root', 'system', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.iqeye.com/products/product-selector-tool#');
INSERT INTO "cameras" 
VALUES (NULL, 'avn803ez', 'AVN803EZ', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=282');
INSERT INTO "cameras" 
VALUES (NULL, 'avn701ez', 'AVN701EZ', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '640x480', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=224');
INSERT INTO "cameras" 
VALUES (NULL, 'avn815ez', 'AVN815EZ', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=283');
INSERT INTO "cameras" 
VALUES (NULL, 'avm302a', 'AVM302A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=239');
INSERT INTO "cameras" 
VALUES (NULL, 'avn812', 'AVN812', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=73');
INSERT INTO "cameras" 
VALUES (NULL, 'avn80x', 'AVN80X', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=46');
INSERT INTO "cameras" 
VALUES (NULL, 'avm500', 'AVM500', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=243');
INSERT INTO "cameras" 
VALUES (NULL, 'avm417a', 'AVM417A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=225');
INSERT INTO "cameras" 
VALUES (NULL, 'avo402', 'AVO402', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=83');
INSERT INTO "cameras" 
VALUES (NULL, 'avn216', 'AVN216', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '640x480', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=80');
INSERT INTO "cameras" 
VALUES (NULL, 'avm400b', 'AVM400B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=78');
INSERT INTO "cameras" 
VALUES (NULL, 'avm317b', 'AVM317B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=77');
INSERT INTO "cameras" 
VALUES (NULL, 'avn806', 'AVN806', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=72');
INSERT INTO "cameras" 
VALUES (NULL, 'avm332', 'AVM332', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=349');
INSERT INTO "cameras" 
VALUES (NULL, 'avm401', 'AVM401', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=339');
INSERT INTO "cameras" 
VALUES (NULL, 'avn320', 'AVN320', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=327');
INSERT INTO "cameras" 
VALUES (NULL, 'avn420', 'AVN420', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=326');
INSERT INTO "cameras" 
VALUES (NULL, 'avm503', 'AVM503', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=325');
INSERT INTO "cameras" 
VALUES (NULL, 'avm402', 'AVM402', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=320');
INSERT INTO "cameras" 
VALUES (NULL, 'avm542b', 'AVM542B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=278');
INSERT INTO "cameras" 
VALUES (NULL, 'avm428d', 'AVM428D', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=273');
INSERT INTO "cameras" 
VALUES (NULL, 'avm328d', 'AVM328D', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=272');
INSERT INTO "cameras" 
VALUES (NULL, 'avm511', 'AVM511', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=264');
INSERT INTO "cameras" 
VALUES (NULL, 'avm542a', 'AVM542A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=255');
INSERT INTO "cameras" 
VALUES (NULL, 'avm311', 'AVM311', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=248');
INSERT INTO "cameras" 
VALUES (NULL, 'avm521a', 'AVM521A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=242');
INSERT INTO "cameras" 
VALUES (NULL, 'avm411', 'AVM411', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=263');
INSERT INTO "cameras" 
VALUES (NULL, 'avn805', 'AVN805', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=61');
INSERT INTO "cameras" 
VALUES (NULL, 'avo403', 'AVO403', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=65');
INSERT INTO "cameras" 
VALUES (NULL, 'avm328b', 'AVM328B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=63');
INSERT INTO "cameras" 
VALUES (NULL, 'avm583', 'AVM583', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=328');
INSERT INTO "cameras" 
VALUES (NULL, 'avp571', 'AVP571', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=280');
INSERT INTO "cameras" 
VALUES (NULL, 'avm458c', 'AVM458C', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=333');
INSERT INTO "cameras" 
VALUES (NULL, 'avm358c', 'AVM358C', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=332');
INSERT INTO "cameras" 
VALUES (NULL, 'avm459c', 'AVM459C', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=324');
INSERT INTO "cameras" 
VALUES (NULL, 'avm403', 'AVM403', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=321');
INSERT INTO "cameras" 
VALUES (NULL, 'avm552b', 'AVM552B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=279');
INSERT INTO "cameras" 
VALUES (NULL, 'avm459b', 'AVM459B', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=277');
INSERT INTO "cameras" 
VALUES (NULL, 'avm552c', 'AVM552C', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=292');
INSERT INTO "cameras" 
VALUES (NULL, 'avm359a', 'AVM359A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=56');
INSERT INTO "cameras" 
VALUES (NULL, 'avn257', 'AVN257', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '640x480', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=55');
INSERT INTO "cameras" 
VALUES (NULL, 'avm457a', 'AVM457A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=54');
INSERT INTO "cameras" 
VALUES (NULL, 'avm357a', 'AVM357A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=53');
INSERT INTO "cameras" 
VALUES (NULL, 'avm561', 'AVM561', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=240');
INSERT INTO "cameras" 
VALUES (NULL, 'avm565a', 'AVM565A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=70');
INSERT INTO "cameras" 
VALUES (NULL, 'avm365a', 'AVM365A', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x800', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=68');
INSERT INTO "cameras" 
VALUES (NULL, 'avn362v', 'AVN362V', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=67');
INSERT INTO "cameras" 
VALUES (NULL, 'avn363v', 'AVN363V', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '1280x1024', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=66');
INSERT INTO "cameras" 
VALUES (NULL, 'avm265', 'AVM265', '23', 'cgi-bin/guest/Video.cgi?media=jpeg', 'live/mjpeg', 'live/h264', '720x576', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.avtech.com.tw/index.php?option=com_k2&view=item&id=52');
INSERT INTO "cameras" 
VALUES (NULL, 'q1755-e', 'Q1755-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1755e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1755', 'Q1755', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1755/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1614-e', 'Q1614-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1614e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1614', 'Q1614', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1614/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1604-e', 'Q1604-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1604e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1604', 'Q1604', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1604/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1602-e', 'Q1602-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '768x576', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1602e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1602', 'Q1602', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '768x576', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1602/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1357-e', 'P1357-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1357e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1357', 'P1357', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1357/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1355-e', 'P1355-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1355e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1355', 'P1355', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1355/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1354-e', 'P1354-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1354e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1354', 'P1354', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1354/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1353-e', 'P1353-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1353e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1353', 'P1353', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1353/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1114-e', 'M1114-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1114/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1114', 'M1114', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1114/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1113-e', 'M1113-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1113e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1113', 'M1113', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1113/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd732f-e', 'DS-2CD732F(-E)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '704x576', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=30');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd762mf-i-fb-h', 'DS-2CD762MF-(I)FB(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=484');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd752mf-i-fb-h', 'DS-2CD752MF-(I)FB(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=494');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd702-slash-712p-n-f-e', 'DS-2CD702/712P(N)F-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'NTSC:704x480/PAL:704x576', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=1922');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd832f-e', 'DS-2CD832F(-E)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '704x576', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=27');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd752mf-e', 'DS-2CD752MF-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '', 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=28');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8133f-e', 'DS-2CD8133F-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=412');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd763p-n-f-e-i-z', 'DS-2CD763P(N)F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=503');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7133-e', 'DS-2CD7133-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=504');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7353f-e-i', 'DS-2CD7353F-E(I)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=7364');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir1', 'DS-2CD802/812/892P(N)-IR1', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'NTSC:704x480/PAL:704x576', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=497');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir3', 'DS-2CD802/812/892P(N)-IR3', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'NTSC:704x480/PAL:704x576', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=499');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir5', 'DS-2CD802/812/892P(N)-IR5', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'NTSC:704x480/PAL:704x576', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=502');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812p-n-f-e-w', 'DS-2CD802/812P(N)F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'NTSC:704x480/PAL:704x576', 'Box', 'admin', '12345', '', 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=481');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd863p-n-f-e-w', 'DS-2CD863P(N)F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=500');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd876bf-mf', 'DS-2CD876BF(MF)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=90');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8153f-e', 'DS-2CD8153F-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.hikvision.com/en/products_show.asp?id=410');
INSERT INTO "cameras" 
VALUES (NULL, 'fe-200cu', 'FE-200CU', '25', 'cgi-bidmin/snapshot.cgi', '', '', '1600x1200', 'Box', 'admin', 'airlive', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/328/original/81bc8dcea441c93d13cf794fe72bb426.jpg?1403278860";}}', 'http://www.airlivesecurity.com/product/FE-200CU');
INSERT INTO "cameras" 
VALUES (NULL, 'wn-150cam', 'WN-150CAM', '25', 'cgi-bin/jpg/image', 'video.mjpg', '', '640x480', 'Box', 'admin', 'airlive', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/321/original/bcf4a030127c36482b6e5589a61dc817.jpg?1403278850";}}', 'http://www.airlivesecurity.com/product/WN-150CAM');
INSERT INTO "cameras" 
VALUES (NULL, 'cu-720pir', 'CU-720PIR', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1280x720', 'Box', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/322/original/5da211c291aee1bc317150173b084b8b.jpg?1403278852";}}', 'http://www.airlivesecurity.com/product/CU-720PIR');
INSERT INTO "cameras" 
VALUES (NULL, 'ip-200phd-24', 'IP-200PHD-24', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1600x1200', 'Box', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/334/original/2892053388b475c938ab1b084c832b97.jpg?1403278868";}}', 'http://www.airlivesecurity.com/product/IP-200PHD-24');
INSERT INTO "cameras" 
VALUES (NULL, 'wn-200hd', 'WN-200HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1600x1200', 'Box', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/331/original/219b855c3ea48c206283c95abf7b9440.jpg?1403278864";}}', 'http://www.airlivesecurity.com/product/WN-200HD');
INSERT INTO "cameras" 
VALUES (NULL, 'cw-720', 'CW-720', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1280x720', 'Box', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/320/original/c829dfc3440196dfa4d3e46f2e938f7c.jpg?1403278849";}}', 'http://www.airlivesecurity.com/product/CW-720');
INSERT INTO "cameras" 
VALUES (NULL, 'wl-350hd', 'WL-350HD', '25', 'cgi-bin/jpg/image', 'video.mjpg', 'video.h264', '1280x1024', 'Box', 'admin', 'airlive', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/309/original/eeebec929686c4be4f61b735f9d8fafe.jpg?1403278834";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/325/original/9511ba2deb0c3a7aca1340e88fc251e0.jpg?1403278856";}}', 'http://www.airlivesecurity.com/product/WL-350HD');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-720', 'BU-720', '25', 'cgi-bin/jpg/image', '', 'video.pro1', '1280x800', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/312/original/d080769f73b66936bc868166ef5bc6d7.jpg?1403278838";}}', 'http://www.airlivesecurity.com/product/BU-720');
INSERT INTO "cameras" 
VALUES (NULL, 'od-2025hd', 'OD-2025HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1920x1080', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/316/original/3edad6a5faea3b87f8310fdad8542518.jpg?1403278843";}}', 'http://www.airlivesecurity.com/product/OD-2025HD');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-3025', 'BU-3025', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/stream1.cgi', '2048x1536', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/315/original/50733d07c6651ffec44bfa622ed2694c.jpg?1403278842";}}', 'http://www.airlivesecurity.com/product/BU-3025');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-3025v2', 'BU-3025v2', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/stream1.cgi', '2048x1536', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/308/original/856f5e7ad8977f31ae372f2683e00488.jpg?1403278833";}}', 'http://www.airlivesecurity.com/product/BU-3025v2');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-2015', 'BU-2015', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/stream1.cgi', '1920x1080', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/323/original/8c3a6a99767fcf34fe3d5815f5c4d0c9.jpg?1403278853";}}', 'http://www.airlivesecurity.com/product/BU-2015');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-3026', 'BU-3026', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'axis-media/media.amp?videocodec=h264', '2048x1536', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/336/original/2d7146c0826011dca01f6adf824456c9.jpg?1403278871";}}', 'http://www.airlivesecurity.com/product/BU-3026');
INSERT INTO "cameras" 
VALUES (NULL, 'bu-3026-ivs', 'BU-3026-IVS', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'axis-media/media.amp?videocodec=h264', '2048x1536', 'Bullet', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/329/original/83c33d88a48c1c3426cfd9c8aa9dddf0.jpg?1403278861";}}', 'http://www.airlivesecurity.com/product/BU-3026-IVS');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-200cam-v2', 'POE-200CAM v2', '25', 'cgi-bin/jpg/image', 'video.mjpg', '', '640x480', 'Dome', 'admin', 'airlive', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:0:{}', 'http://www.airlivesecurity.com/product/POE-200CAMv2');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-250hd', 'POE-250HD', '25', 'jpg/image.jpg', 'mjpg/video.mjpg', 'h264/media.amp', '1280x1024', 'Dome', 'admin', 'airlive', '', 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/305/original/c9e7a5dbda1f67462fc1b5ebfcc04119.jpg?1403278829";}}', 'http://www.airlivesecurity.com/product/POE-250HD');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-260cam', 'POE-260CAM', '25', 'cgi-bin/jpg/image', 'video.mjpg', '', '640x480', 'Dome', 'admin', 'airlive', '', 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/304/original/f0082ffc6326155c1707911ce7a3ab4d.jpg?1403278827";}}', 'http://www.airlivesecurity.com/product/POE-260CAM');
INSERT INTO "cameras" 
VALUES (NULL, 'wn-2600hd', 'WN-2600HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1920x1080', 'Dome', 'admin', 'airlive', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/330/original/5d7b11f30f7706988fd00b4873faba99.jpg?1403278862";}}', 'http://www.airlivesecurity.com/product/WN-2600HD');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-2600hd', 'POE-2600HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1920x1080', 'Dome', 'admin', 'airlive', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/314/original/911eeccd245cb93c0f9b7bd1ad91918a.jpg?1403278841";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/318/original/fd82a21e95632c6f498a9a0a2a66c7aa.jpg?1403278846";}}', 'http://www.airlivesecurity.com/product/POE-2600HD');
INSERT INTO "cameras" 
VALUES (NULL, 'dm-720', 'DM-720', '25', 'cgi-bin/jpg/image', '', 'video.pro1', '1280x800', 'Dome', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/324/original/00eaa51f28027f3ae8a37029e1dc2d14.jpg?1403278854";}}', 'http://www.airlivesecurity.com/product/DM-720');
INSERT INTO "cameras" 
VALUES (NULL, 'od-2060hd', 'OD-2060HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1920x1080', 'Dome', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/317/original/c655c3b22a1b8aec6b02004f667a09f5.jpg?1403278845";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/335/original/11b2a010f71920d70d9d1e690e98a3c9.jpg?1403278869";}}', 'http://www.airlivesecurity.com/product/OD-2060HD');
INSERT INTO "cameras" 
VALUES (NULL, 'fe-200dm', 'FE-200DM', '25', 'cgi-bidmin/snapshot.cgi', '', '', '1600x1200', 'Dome', 'admin', 'airlive', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/333/original/ad04dbdc2e450c652492d05941164699.jpg?1403278866";}}', 'http://www.airlivesecurity.com/product/FE-200DM');
INSERT INTO "cameras" 
VALUES (NULL, 'fe-200vd', 'FE-200VD', '25', 'cgi-bidmin/snapshot.cgi', '', '', '1600x1200', 'Dome', 'admin', 'airlive', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/303/original/cd440c151deb689104a68dcc1335bc5c.jpg?1403278826";}}', 'http://www.airlivesecurity.com/product/FE-200VD');
INSERT INTO "cameras" 
VALUES (NULL, 'od-2050hd', 'OD-2050HD', '25', 'image.jpg', 'video.mjpg', 'h264/media.amp', '1920x1080', 'Dome', 'admin', 'airlive', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/326/original/dd0889631f7afaf3cc42478e6c31d4ba.jpg?1403278857";}}', 'http://www.airlivesecurity.com/product/OD-2050HD');
INSERT INTO "cameras" 
VALUES (NULL, 'md-720', 'MD-720', '25', 'cgi-bin/jpg/image', '', 'video.pro1', '1280x800', 'Dome', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/313/original/e01903d93f09c160fa49ed8a6b38e5fb.jpg?1403278839";}}', 'http://www.airlivesecurity.com/product/MD-720');
INSERT INTO "cameras" 
VALUES (NULL, 'sd-2020', 'SD-2020', '25', 'cgi-bin/jpg/image.cgi?', 'cgi-bin/jpg/image.cgi?', 'h264', '1920x1080', 'Dome', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/306/original/6fdab03638fa94cf33bfc7c7c16d63c6.jpg?1403278831";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/307/original/158f6ca21b49a5629d2f8b71f3357a41.jpg?1403278832";}}', 'http://www.airlivesecurity.com/product/SD-2020');
INSERT INTO "cameras" 
VALUES (NULL, 'md-3025', 'MD-3025', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/h264.cgi', '2048x1536', 'Dome', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/337/original/d053e326fe933d9b69789e02d92d590f.jpg?1403278872";}}', 'http://www.airlivesecurity.com/product/MD-3025');
INSERT INTO "cameras" 
VALUES (NULL, 'md-3025-ivs', 'MD-3025-IVS', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/h264.cgi', '2048x1536', 'Dome', 'admin', 'airlive', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/332/original/f2138a424f578db9de9cb006c6e9d2dc.jpg?1403278865";}}', 'http://www.airlivesecurity.com/product/MD-3025-IVS');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-100hd-icr', 'POE-100HD-ICR', '25', 'jpg/image.jpg', 'mjpg/video.mjpg', 'h264/media.amp', '1280x1024', 'Box', 'admin', 'airlive', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/311/original/4b807f7eed061d87ad79e9e9cf261331.jpg?1403278837";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/319/original/6298f1e6e6dee58c442cf53b1cab9059.jpg?1403278847";}}', 'http://www.airlivesecurity.com/product/POE-100HD-ICR');
INSERT INTO "cameras" 
VALUES (NULL, 'poe-5010hd', 'POE-5010HD', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/h264.cgi', '2592x1920', 'Box', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/302/original/3b7611795d07ab3e7eb5ff9fef4ea8ab.jpg?1403278825";}}', 'http://www.airlivesecurity.com/product/POE-5010HD');
INSERT INTO "cameras" 
VALUES (NULL, 'bc-5010', 'BC-5010', '25', 'cgi/jpg/image.cgi', 'cgi/mjpg/mjpeg.cgi', 'cgi/h264/h264.cgi', '2592x1920', 'Box', 'admin', 'airlive', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/327/original/65e72e886b4686667282a32df911ff18.jpg?1403278858";}}', 'http://www.airlivesecurity.com/product/BC-5010');
INSERT INTO "cameras" 
VALUES (NULL, 'av8365', 'AV8365', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/187/original/86d8ba0794eef502106515c19e2426d1.jpg?1403278654";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8365');
INSERT INTO "cameras" 
VALUES (NULL, 'av8360', 'AV8360', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/182/original/d086c06a6a7dabc1c3de7e70415c24a9.jpg?1403278647";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8360');
INSERT INTO "cameras" 
VALUES (NULL, 'av8185', 'AV8185', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/255/original/9421748f8c5d5097ed4acd936031ca84.jpg?1403278756";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8185');
INSERT INTO "cameras" 
VALUES (NULL, 'av8180', 'AV8180', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/241/original/5afed29bd42b8e0b23841ecb58907537.jpg?1403278737";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8180');
INSERT INTO "cameras" 
VALUES (NULL, 'av40185dn', 'AV40185DN', '26', 'image', 'mjpeg', 'h264.sdp', '14592x2752', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/215/original/521b1cfe6df6b954b50cd893c51de3f2.jpg?1403278698";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV40185DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av20365dn', 'AV20365DN', '26', 'image', 'mjpeg', 'h264.sdp', '10240x1920', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/142/original/b61e456d61c6884c9492661eea171eea.jpg?1403278582";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV20365DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av20365co', 'AV20365CO', '26', 'image', 'mjpeg', 'h264.sdp', '10240x1920', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/211/original/c18872fbb34e518fe61c16199289d1c6.jpg?1403278692";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV20365CO');
INSERT INTO "cameras" 
VALUES (NULL, 'av20185dn', 'AV20185DN', '26', 'image', 'mjpeg', 'h264.sdp', '10240x1920', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/184/original/aecd5730460fddaf2f20c2191ea13d45.jpg?1403278650";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV20185DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av20185co', 'AV20185CO', '26', 'image', 'mjpeg', 'h264.sdp', '10240x1920', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/191/original/5d2e9e03b26a40b416491fa5af57e7b4.jpg?1403278660";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV20185CO');
INSERT INTO "cameras" 
VALUES (NULL, 'av12366dn', 'AV12366DN', '26', 'image', 'mjpeg', 'h264.sdp', '8192x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/257/original/f39ef4ab7723e53c863a180871c4b618.jpg?1403278759";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV12366DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av12186dn', 'AV12186DN', '26', 'image', 'mjpeg', 'h264.sdp', '8192x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/138/original/d1aa6cce55d0830c3e6231c9211cd2b7.jpg?1403278576";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV12186DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av8365dn', 'AV8365DN', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/163/original/bc992612bfcf9331cbd7abc395764e11.jpg?1403278612";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8365DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av8365co', 'AV8365CO', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/181/original/668ec973398f357c18b9cd5417032125.jpg?1403278645";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8365CO');
INSERT INTO "cameras" 
VALUES (NULL, 'av8185dn', 'AV8185DN', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/150/original/617dd760a5792480f4788f1572edb610.jpg?1403278593";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8185DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av8185co', 'AV8185CO', '26', 'image', 'mjpeg', 'h264.sdp', '6400x1200', 'Dome', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/176/original/3b4cfa378fc92baa6cc18243af07f700.jpg?1403278633";}}', 'http://www.arecontvision.com/product/SurroundVideo+Series/AV8185CO');
INSERT INTO "cameras" 
VALUES (NULL, 'av5455dn-s', 'AV5455DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/270/original/7d3291c38672a24d6a4139896ce87968.jpg?1403278779";}}', 'http://www.arecontvision.com/product/microdome/av5455dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av5455dn-f', 'AV5455DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/283/original/a07af0c132d30d2dc5c952a1b5e6913c.jpg?1403278798";}}', 'http://www.arecontvision.com/product/microdome/av5455dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av3456dn-s', 'AV3456DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/260/original/34b1526f50e38588bfb63b8fc3cae737.jpg?1403278763";}}', 'http://www.arecontvision.com/product/microdome/av3456dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av3456dn-f', 'AV3456DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/205/original/84b1d846d91f9f8a9ee832b7b78dbd34.jpg?1403278682";}}', 'http://www.arecontvision.com/product/microdome/av3456dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av3455dn-s', 'AV3455DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/145/original/76bcf2a6fc0183dc7d4f5b47ac5a2bf0.jpg?1403278586";}}', 'http://www.arecontvision.com/product/microdome/av3455dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av3455dn-f', 'AV3455DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/140/original/f5e942e21fea85fd920b5854eda0501b.jpg?1403278579";}}', 'http://www.arecontvision.com/product/microdome/av3455dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av2456dn-s', 'AV2456DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/239/original/5a557157d3fcfc7111f99874f5e8b616.jpg?1403278734";}}', 'http://www.arecontvision.com/product/microdome/av2456dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av2456dn-f', 'AV2456DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/281/original/507dfbd0031eefe5499578d1a080be27.jpg?1403278795";}}', 'http://www.arecontvision.com/product/microdome/av2456dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av2455dn-s', 'AV2455DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/264/original/899d0d2e53d29b47a18230276deaf130.jpg?1403278769";}}', 'http://www.arecontvision.com/product/microdome/av2455dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av2455dn-f', 'AV2455DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/256/original/e09c815875268d5830b554508434fd00.jpg?1403278758";}}', 'http://www.arecontvision.com/product/microdome/av2455dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av1455dn-s', 'AV1455DN-S', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/123/original/2f07293c2260d59ad70e394781597084.jpg?1403278556";}}', 'http://www.arecontvision.com/product/microdome/av1455dn-s');
INSERT INTO "cameras" 
VALUES (NULL, 'av1455dn-f', 'AV1455DN-F', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/179/original/a3bc643da47320a0f26b64a09f45446c.jpg?1403278643";}}', 'http://www.arecontvision.com/product/microdome/av1455dn-f');
INSERT INTO "cameras" 
VALUES (NULL, 'av10255pmtir-h', 'AV10255PMTIR-H', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2753', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/136/original/1acd9f52692c8e373f218561e36676fc.jpg?1403278574";}}', 'http://www.arecontvision.com/product/megadome2/av10255pmtir-h');
INSERT INTO "cameras" 
VALUES (NULL, 'av10255amir', 'AV10255AMIR', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/223/original/c4904966091ed9cb24a0bc1248bd361c.jpg?1403278708";}}', 'http://www.arecontvision.com/product/megadome2/av10255amir');
INSERT INTO "cameras" 
VALUES (NULL, 'av5255pmtir-h', 'AV5255PMTIR-H', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/276/original/33e6d4e18e0e4392a995dcd7fa096969.jpg?1403278788";}}', 'http://www.arecontvision.com/product/megadome2/av5255pmtir-h');
INSERT INTO "cameras" 
VALUES (NULL, 'av5255am', 'AV5255AM', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/155/original/6789b45376b7beffb83ba231dce690f3.jpg?1403278601";}}', 'http://www.arecontvision.com/product/megadome2/av5255am');
INSERT INTO "cameras" 
VALUES (NULL, 'av3256pmtir', 'AV3256PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/183/original/3782e5ae613a7f73ac97fb7de1e3b793.jpg?1403278648";}}', 'http://www.arecontvision.com/product/megadome2/av3256pmtir');
INSERT INTO "cameras" 
VALUES (NULL, 'av3256pm', 'AV3256PM', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/229/original/3286cd5476e7fd6c41b4bba2972ad535.jpg?1403278716";}}', 'http://www.arecontvision.com/product/megadome2/av3256pm');
INSERT INTO "cameras" 
VALUES (NULL, 'av3255pmtir-h', 'AV3255PMTIR-H', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/152/original/523cf076ea10f036befb8699c86fab5d.jpg?1403278596";}}', 'http://www.arecontvision.com/product/megadome2/av3255pmtir-h');
INSERT INTO "cameras" 
VALUES (NULL, 'av3255am', 'AV3255AM', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/207/original/f2724b7cd67578530e0c884ed0a7667b.jpg?1403278685";}}', 'http://www.arecontvision.com/product/megadome2/av3255am');
INSERT INTO "cameras" 
VALUES (NULL, 'av2256pmtir', 'AV2256PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/269/original/c7a3d537229c5fdce1975df0c1f9d0f9.jpg?1403278777";}}', 'http://www.arecontvision.com/product/megadome2/av2256pmtir');
INSERT INTO "cameras" 
VALUES (NULL, 'av2256pm', 'AV2256PM', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/172/original/7e618352cf90cc0863cc2d374827c8d0.jpg?1403278625";}}', 'http://www.arecontvision.com/product/megadome2/av2256pm');
INSERT INTO "cameras" 
VALUES (NULL, 'av2255pmtir-h', 'AV2255PMTIR-H', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/203/original/8173494907466d0b16e2078957a37c7a.jpg?1403278679";}}', 'http://www.arecontvision.com/product/megadome2/av2255pmtir-h');
INSERT INTO "cameras" 
VALUES (NULL, 'av2255am', 'AV2255AM', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/222/original/21e9532de0b9d041b1ff2be0b0a133b2.jpg?1403278707";}}', 'http://www.arecontvision.com/product/megadome2/av2255am');
INSERT INTO "cameras" 
VALUES (NULL, 'av1255am', 'AV1255AM', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/274/original/1d016c3e941437642881b60f11915ee2.jpg?1403278785";}}', 'http://www.arecontvision.com/product/megadome2/av1255am');
INSERT INTO "cameras" 
VALUES (NULL, 'av5155', 'AV5155', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/164/original/d418126dfc8af2470cbd159a751ae9ff.jpg?1403278613";}}', 'http://www.arecontvision.com/product/megadome+series/av5155');
INSERT INTO "cameras" 
VALUES (NULL, 'av3155', 'AV3155', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/129/original/9fe0a934d8af66e0503a6b8106f48f72.jpg?1403278564";}}', 'http://www.arecontvision.com/product/megadome+series/av3155');
INSERT INTO "cameras" 
VALUES (NULL, 'av2155', 'AV2155', '26', 'image', 'mjpeg', 'h264.sdp', '1600x1200', 'Dome', '', '', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/279/original/39ecebf234d26fbc885af302d61ba459.jpg?1403278792";}}', 'http://www.arecontvision.com/product/megadome+series/av2155');
INSERT INTO "cameras" 
VALUES (NULL, 'av1355', 'AV1355', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/242/original/8874a6fad375f0fd7cd75fd7deb591ed.jpg?1403278738";}}', 'http://www.arecontvision.com/product/megadome+series/av1355');
INSERT INTO "cameras" 
VALUES (NULL, 'av10225pmtir', 'AV10225PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/214/original/e9b9577cb5610e0dc0706d8e1cccc3c2.jpg?1403278696";}}', 'http://www.arecontvision.com/product/MegaView2/AV10225PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av10225pmir', 'AV10225PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/157/original/cb62eaa376dbc86d874d558b86541bed.jpg?1403278604";}}', 'http://www.arecontvision.com/product/MegaView2/AV10225PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av5225pmtir', 'AV5225PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/289/original/bb2c1d527558285812cc7a7d9778107a.jpg?1403278806";}}', 'http://www.arecontvision.com/product/MegaView2/AV5225PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av5225pmir', 'AV5225PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/286/original/9f256dda81de661f460f484df9e0a984.jpg?1403278802";}}', 'http://www.arecontvision.com/product/MegaView2/AV5225PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av3226pmtir', 'AV3226PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/282/original/7771b8b772d643d78ddfef1101db1fd3.jpg?1403278796";}}', 'http://www.arecontvision.com/product/MegaView2/AV3226PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av3226pmir', 'AV3226PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/153/original/3aae7b94966e5e2385f5a0063681b27a.jpg?1403278597";}}', 'http://www.arecontvision.com/product/MegaView2/AV3226PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av3225pmtir', 'AV3225PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/190/original/ceb107eeee1033632309937d64496a8b.jpg?1403278659";}}', 'http://www.arecontvision.com/product/MegaView2/AV3225PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av3225pmir', 'AV3225PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/186/original/5a24b4dbd7609e77ff5906b9397e5aa4.jpg?1403278653";}}', 'http://www.arecontvision.com/product/MegaView2/AV3225PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av2226pmtir', 'AV2226PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/125/original/2435eb4f894c818e95c1d333092cd705.jpg?1403278558";}}', 'http://www.arecontvision.com/product/MegaView2/AV2226PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av2226pmir', 'AV2226PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/141/original/27537176bfd87282c201b60c7f35b8ae.jpg?1403278580";}}', 'http://www.arecontvision.com/product/MegaView2/AV2226PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av2225pmtir', 'AV2225PMTIR', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/137/original/f46f6c972e51e8f26150937569f44f0f.jpg?1403278575";}}', 'http://www.arecontvision.com/product/MegaView2/AV2225PMTIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av2225pmir', 'AV2225PMIR', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/213/original/9b35320d56b6c94592615bff9a03beb5.jpg?1403278695";}}', 'http://www.arecontvision.com/product/MegaView2/AV2225PMIR');
INSERT INTO "cameras" 
VALUES (NULL, 'av5125irv1', 'AV5125IRv1', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/235/original/940add5bbe174d510bfcf957708d0390.jpg?1403278725";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV5125IRv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av5125dnv1', 'AV5125DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/247/original/246e2e00aedc5a9f146a8476363e431c.jpg?1403278745";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV5125DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av5125', 'AV5125', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/249/original/16ad2e9143aff5235f8c07755ff5fe52.jpg?1403278748";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV5125');
INSERT INTO "cameras" 
VALUES (NULL, 'av3125irv1', 'AV3125IRv1', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/277/original/ae82781c11cb220ef3bbf7c1bb5c02e9.jpg?1403278789";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV3125IRv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av3125dnv1', 'AV3125DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/251/original/1eb6edde2fe4bc4864f7c624378f7467.jpg?1403278751";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV3125DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av3125', 'AV3125', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/121/original/fa2905ad82890e3cc3fcd5f87b94e98c.jpg?1403278553";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV3125');
INSERT INTO "cameras" 
VALUES (NULL, 'av2825', 'AV2825', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/122/original/dc0f2b87cf6d3a453fad723df949b9b0.jpg?1403278554";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2825');
INSERT INTO "cameras" 
VALUES (NULL, 'av2125irv1', 'AV2125IRv1', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/278/original/063d2069ed35f689bc6c90fa2a664105.jpg?1403278791";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2125IRv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av2125dnv1', 'AV2125DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/198/original/8afd360da7c89186945e17e9b5b9eafe.jpg?1403278672";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2125DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av2125', 'AV2125', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/290/original/7de72e7940ee6da6978d92aecd8d1df7.jpg?1403278807";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2125');
INSERT INTO "cameras" 
VALUES (NULL, 'av1325', 'AV1325', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/120/original/a308c2ce91a38a7412eeb8d3a04a7e46.jpg?1403278551";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1325');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125irv1', 'AV1125IRv1', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/166/original/dd7afcc2983f6ddf61f125009c3b6bf3.jpg?1403278616";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125IRv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125ir', 'AV1125IR', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/295/original/d0af5ed660ba3df718f6e7104474ef3d.jpg?1403278815";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125IR');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125dnv1', 'AV1125DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/128/original/10bfe883e4f111f35448d9670e4d9e5e.jpg?1403278562";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125', 'AV1125', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/232/original/e74f37e40a55c6563c8815e6dd809ba2.jpg?1403278721";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125');
INSERT INTO "cameras" 
VALUES (NULL, 'av5125irv1x', 'AV5125IRv1x', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/250/original/83048be618001740373c51567eed846d.jpg?1403278749";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV5125IRv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av5125dnv1x', 'AV5125DNv1x', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/253/original/3a8c23043670af20b22f1d5c34169ad2.jpg?1403278754";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV5125DNv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av3125irv1x', 'AV3125IRv1x', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/231/original/76a99d74dc33cf369b2e7719f98652d6.jpg?1403278719";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV3125IRv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av3125dnv1x', 'AV3125DNv1x', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/146/original/aa221ad9bde7b1ab5aa596141582e361.jpg?1403278588";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV3125DNv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av2125irv1x', 'AV2125IRv1x', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/135/original/bed461dc8449b65c6f6599709737108f.jpg?1403278572";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2125IRv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av2125dnv1x', 'AV2125DNv1x', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/168/original/defa51eafa79b1587fe7ec917626a8c1.jpg?1403278619";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV2125DNv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125irv1x', 'AV1125IRv1x', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/208/original/557c9aae4f5b67a777546fca57b08846.jpg?1403278687";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125IRv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av1125dnv1x', 'AV1125DNv1x', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Bullet', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/288/original/d66fa1a7d7d226dc073d243048eedb5a.jpg?1403278805";}}', 'http://www.arecontvision.com/product/MegaView+Series/AV1125DNv1x');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245pm-w', 'AV5245PM-W', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/275/original/49dcc39e0fdded99980b22fd5d420361.jpg?1403278786";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245PM-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245pm-d-lg', 'AV5245PM-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/300/original/2b77ed02b47bcf4f01870b095a15814b.jpg?1403278822";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245PM-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245pm-d', 'AV5245PM-D', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/292/original/b6bfc8585bc61c87b23b61724deabbae.jpg?1403278810";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245PM-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245dn-01-w', 'AV5245DN-01-W', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/131/original/7202010078c2ccc657a2d84906dba843.jpg?1403278566";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245DN-01-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245dn-01-d-lg', 'AV5245DN-01-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/261/original/b0f5b8b31d1e0c7102f7a8a47dc0e001.jpg?1403278764";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245DN-01-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av5245dn-01-d', 'AV5245DN-01-D', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/285/original/0729bd407b9e5b7bf8705e3e9220e43b.jpg?1403278800";}}', 'http://www.arecontvision.com/product/MegaBall2/AV5245DN-01-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av3246pm-w', 'AV3246PM-W', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/273/original/b5cc717d2ba84cddf201688c98e476bf.jpg?1403278783";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3246PM-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av3246pm-d-lg', 'AV3246PM-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/200/original/10afc9210e187b08f45e994f5c34c780.jpg?1403278675";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3246PM-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av3246pm-d', 'AV3246PM-D', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/169/original/a1667f8ef39c1c85fc9bbd917284f52b.jpg?1403278620";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3246PM-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av3245pm-w', 'AV3245PM-W', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/204/original/026247fc3349d2bbd20aa0e735a2ac9f.jpg?1403278681";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3245PM-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av3245pm-d-lg', 'AV3245PM-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/266/original/21710fd970dbc749fe1ced76e95870ec.jpg?1403278773";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3245PM-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av3245pm-d', 'AV3245PM-D', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/189/original/b4360167b5a1d95ca7378caba070f2cb.jpg?1403278657";}}', 'http://www.arecontvision.com/product/MegaBall2/AV3245PM-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2245pm-d', 'AV2245PM-D', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/195/original/04ae0a32f85f23247c756ed6a35c1500.jpg?1403278668";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2245PM-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2246pm-w', 'AV2246PM-W', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/262/original/f7ecb11de881fcfcbca740fa8874024f.jpg?1403278766";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2246PM-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av2246pm-d-lg', 'AV2246PM-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/258/original/a3b1bddc7e3dcd419da0ac4de7615ca7.jpg?1403278760";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2246PM-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av2246pm-d', 'AV2246PM-D', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/298/original/b335b467ac669bc417292873770ec455.jpg?1403278819";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2246PM-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2245pm-w', 'AV2245PM-W', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Other', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/151/original/4d8143332108a989de35edd541de3398.jpg?1403278595";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2245PM-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av2245pm-d-lg', 'AV2245PM-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/126/original/b9afbff5c1f14a07534e7235bbdd0077.jpg?1403278560";}}', 'http://www.arecontvision.com/product/MegaBall2/AV2245PM-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av5145-3310-w', 'AV5145-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/237/original/fbc2bb3b8f900f5785cff4e2745419e7.jpg?1403278729";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV5145-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av5145-3310-d', 'AV5145-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/188/original/515ed4dadb6aeed1e916bc648e938ff1.jpg?1403278656";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV5145-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av3145-3310-w', 'AV3145-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/165/original/20fdc4648cad9af9ca04b84901ca01c9.jpg?1403278614";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3145-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av3145-3310-d', 'AV3145-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/162/original/aa7af5fc52db1f0da8cd61d65d2609d7.jpg?1403278610";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3145-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2145-3310-w', 'AV2145-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/173/original/0bdcb033b01da89b7e40bd4f95b8b8f5.jpg?1403278627";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2145-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av2145-3310-d', 'AV2145-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/193/original/0584cb811b4133b5622bf654d3b928b3.jpg?1403278664";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2145-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av1145-3310-w', 'AV1145-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/291/original/f985329ca90ca89ad501b597dcd29e81.jpg?1403278809";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV1145-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av1145-3310-d', 'AV1145-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/174/original/293943b4ec7a9247eedfe493a9653cd5.jpg?1403278628";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV1145-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av5145dn-3310-w', 'AV5145DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/178/original/43ff18db2da56156e8809186a71a2e52.jpg?1403278640";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV5145DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av5145dn-3310-d-lg', 'AV5145DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/299/original/9f61e3ee3b01d7904fee54946fbb64c8.jpg?1403278820";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV5145DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av5145dn-3310-d', 'AV5145DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/287/original/d17fa022b0f1953621b7a31a7a41876e.jpg?1403278803";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV5145DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av3146dn-3310-w', 'AV3146DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/194/original/7da514297112804abfefd0897fd4e3bf.jpg?1403278666";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3146DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av3146dn-3310-d-lg', 'AV3146DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/192/original/f394e3fed8f3c29d921354e8eb962429.jpg?1403278662";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3146DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av3146dn-3310-d', 'AV3146DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/144/original/74e719feb1d1c51ca8ddfe13e861c482.jpg?1403278585";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3146DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av3145dn-3310-w', 'AV3145DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/252/original/4b5d17161ee782d99ae2520f9b800212.jpg?1403278752";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3145DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av3145dn-3310-d-lg', 'AV3145DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/156/original/0eb38afc878a8ac3d9fd3681da345238.jpg?1403278602";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3145DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av3145dn-3310-d', 'AV3145DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/245/original/87b88bd10e5e298d63d02d599fed101d.jpg?1403278742";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV3145DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2146dn-3310-w', 'AV2146DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/284/original/fe3c7b271f47db5f518ed80bb5cf2a2d.jpg?1403278799";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2146DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av2146dn-3310-d-lg', 'AV2146DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/133/original/da8e00da471405606e3b1bdd4df32227.jpg?1403278569";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2146DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av2146dn-3310-d', 'AV2146DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/160/original/8f096d5a7179891f1e55c97a85573538.jpg?1403278608";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2146DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av2145dn-3310-w', 'AV2145DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/233/original/dc10728514dc5e4756d066a8a5b73d26.jpg?1403278722";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2145DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av2145dn-3310-d-lg', 'AV2145DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/280/original/4faff4da284f27149b24fa08babd459c.jpg?1403278793";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2145DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av2145dn-3310-d', 'AV2145DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/132/original/b4ecf33e59ab912ba72db91b47b0d3c1.jpg?1403278568";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV2145DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av1145dn-3310-w', 'AV1145DN-3310-W', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Other', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/263/original/8af9041400354d182d5c0f3b20d1db9b.jpg?1403278767";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV1145DN-3310-W');
INSERT INTO "cameras" 
VALUES (NULL, 'av1145dn-3310-d-lg', 'AV1145DN-3310-D-LG', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/244/original/e7e86471f2d315cc0a0be4a04588ac5b.jpg?1403278741";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV1145DN-3310-D-LG');
INSERT INTO "cameras" 
VALUES (NULL, 'av1145dn-3310-d', 'AV1145DN-3310-D', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/147/original/b6ea2a63b8c91b772f9c236765a5dad2.jpg?1403278589";}}', 'http://www.arecontvision.com/product/MegaBall+Series/AV1145DN-3310-D');
INSERT INTO "cameras" 
VALUES (NULL, 'av10115', 'AV10115', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/228/original/0dd33aeafa65d27ecaa6baafbd434a67.jpg?1403278715";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV10115');
INSERT INTO "cameras" 
VALUES (NULL, 'av5115', 'AV5115', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/127/original/9f25f214c97949a4774547ba4518ab61.jpg?1403278561";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV5115');
INSERT INTO "cameras" 
VALUES (NULL, 'av3115', 'AV3115', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/161/original/02fc2b91db85cd007e9aebc3fa73bae3.jpg?1403278609";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV3115');
INSERT INTO "cameras" 
VALUES (NULL, 'av2815', 'AV2815', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/227/original/99269c1193dfb63b5e4758c9c28c7dec.jpg?1403278714";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2815');
INSERT INTO "cameras" 
VALUES (NULL, 'av2116dn', 'AV2116DN', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/268/original/c73767eea0e8db3444f424b4352d7726.jpg?1403278776";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2116DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av2115', 'AV2115', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/246/original/336d44139576b3471bd56ee72d93d8e4.jpg?1403278743";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2115');
INSERT INTO "cameras" 
VALUES (NULL, 'av1315', 'AV1315', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/185/original/3b61a4bc27c5fae5dd39849d7528a8eb.jpg?1403278651";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV1315');
INSERT INTO "cameras" 
VALUES (NULL, 'av1115', 'AV1115', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/180/original/9b6cf764427ddd1b032dc193647195d2.jpg?1403278644";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV1115');
INSERT INTO "cameras" 
VALUES (NULL, 'av10115v1', 'AV10115v1', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/196/original/5d452c5cba30d3b913715c1c800442d9.jpg?1403278669";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV10115v1');
INSERT INTO "cameras" 
VALUES (NULL, 'av5115v1', 'AV5115v1', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/175/original/66a5cf5c8d8a2761afe81493bdaa3506.jpg?1403278630";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV5115v1');
INSERT INTO "cameras" 
VALUES (NULL, 'av5110', 'AV5110', '26', 'image', 'mjpeg', '', '2592x1944', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/294/original/de358f8787b5a44b2d3c65787fd1f08d.jpg?1403278813";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV5110');
INSERT INTO "cameras" 
VALUES (NULL, 'av3236dn', 'AV3236DN', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/149/original/564c90c69593862283ee5dbb28e0e730.jpg?1403278592";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV3236DN');
INSERT INTO "cameras" 
VALUES (NULL, 'av3116dnv1', 'AV3116DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/238/original/ccf0b7c3f6b1dd04beef3f3602439b4e.jpg?1403278730";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV3116DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av3115v1', 'AV3115v1', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/226/original/5ef633ed3b267d22156033a7a0aff66a.jpg?1403278713";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV3115v1');
INSERT INTO "cameras" 
VALUES (NULL, 'av3110', 'AV3110', '26', 'image', 'mjpeg', '', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/225/original/aa649c73df779fba48399c2bc5822603.jpg?1403278711";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV3110');
INSERT INTO "cameras" 
VALUES (NULL, 'av2116dnv1', 'AV2116DNv1', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/171/original/a7087c9043488571d9153d6227787a2c.jpg?1403278623";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2116DNv1');
INSERT INTO "cameras" 
VALUES (NULL, 'av2115v1', 'AV2115v1', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/124/original/1c62f819e41749d83d9bb51214205770.jpg?1403278557";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2115v1');
INSERT INTO "cameras" 
VALUES (NULL, 'av2110', 'AV2110', '26', 'image', 'mjpeg', '', '1600x1200', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/259/original/7f5897f53ad04dbe31606525e0af0e36.jpg?1403278762";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV2110');
INSERT INTO "cameras" 
VALUES (NULL, 'av1310', 'AV1310', '26', 'image', 'mjpeg', '', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/221/original/e05a4786ffb63c5e18d9b5c8c3270e81.jpg?1403278706";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV1310');
INSERT INTO "cameras" 
VALUES (NULL, 'av1115v1', 'AV1115v1', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/296/original/fdd5cec0696eb1321e74f6bdf7e29a71.jpg?1403278817";}}', 'http://www.arecontvision.com/product/MegaVideo+Compact+Series/AV1115v1');
INSERT INTO "cameras" 
VALUES (NULL, 'av10005', 'AV10005', '26', 'image', 'mjpeg', 'h264.sdp', '3648x2752', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/254/original/98448cbafcf17b3b6a5dbe6fa42c428c.jpg?1403278755";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV10005');
INSERT INTO "cameras" 
VALUES (NULL, 'av5105', 'AV5105', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/220/original/16771dc500f184b2927f85d30aa7eb1c.jpg?1403278704";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV5105');
INSERT INTO "cameras" 
VALUES (NULL, 'av5100', 'AV5100', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/158/original/6be0afafb692fc012b16e6d47e05815f.jpg?1403278605";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV5100');
INSERT INTO "cameras" 
VALUES (NULL, 'av3130', 'AV3130', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/243/original/f92cbc8d392da552e72641f2c7cc0b05.jpg?1403278739";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV3130');
INSERT INTO "cameras" 
VALUES (NULL, 'av3105', 'AV3105', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/272/original/d318e108c2f8bae3196edbd91a26563b.jpg?1403278782";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV3105');
INSERT INTO "cameras" 
VALUES (NULL, 'av3100m', 'AV3100M', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/170/original/58e32cdda0b35b6015dcd3e8afdd13fc.jpg?1403278622";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV3100M');
INSERT INTO "cameras" 
VALUES (NULL, 'av3100', 'AV3100', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/197/original/8c1d5697525ef79f011520b6a6734074.jpg?1403278670";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV3100');
INSERT INTO "cameras" 
VALUES (NULL, 'av2805', 'AV2805', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Box', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/154/original/0b675276027271c9eb4ba58d0d62dcf0.jpg?1403278599";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV2805');
INSERT INTO "cameras" 
VALUES (NULL, 'av2105', 'AV2105', '26', 'image', 'mjpeg', 'h264.sdp', '1600x1200', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/148/original/7b4eda046b75daffd025c27c357f026d.jpg?1403278590";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV2105');
INSERT INTO "cameras" 
VALUES (NULL, 'av2100m', 'AV2100M', '26', 'image', 'mjpeg', 'h264.sdp', '1600x1200', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/248/original/5dccc21cb13ad43f037d37f7541a2fda.jpg?1403278746";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV2100M');
INSERT INTO "cameras" 
VALUES (NULL, 'av2100', 'AV2100', '26', 'image', 'mjpeg', 'h264.sdp', '1600x1200', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/206/original/0a0bb15fca6d08fb01765ec208924245.jpg?1403278684";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV2100');
INSERT INTO "cameras" 
VALUES (NULL, 'av1305', 'AV1305', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/297/original/18c08a9ecf010881532bd29bb7a48a8d.jpg?1403278818";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV1305');
INSERT INTO "cameras" 
VALUES (NULL, 'av1300m', 'AV1300M', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/234/original/2e19103ca619c52ae31615ac304407bf.jpg?1403278723";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV1300M');
INSERT INTO "cameras" 
VALUES (NULL, 'av1300', 'AV1300', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/236/original/31839a846f1fbeae9ac89cd36562981f.jpg?1403278727";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV1300');
INSERT INTO "cameras" 
VALUES (NULL, 'av3135', 'AV3135', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/218/original/e707687178ce33e37eeb6e07a63c2731.jpg?1403278702";}}', 'http://www.arecontvision.com/product/MegaVideo+Series/AV3135');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av5115-3312', 'D4SO-AV5115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/201/original/11f9e97fff2cf0fa627f06587bbf4483.jpg?1403278676";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV5115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av5115-3312', 'D4S-AV5115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/199/original/f84af6cba3ef778fd98a69e00caaf77b.jpg?1403278673";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV5115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av5115-3312', 'D4F-AV5115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/267/original/f182849808ec7157bfe30137ff860de1.jpg?1403278774";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV5115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av3115-3312', 'D4SO-AV3115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/224/original/74fc509c8dc694c0b983448e59ebaade.jpg?1403278710";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV3115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av3115-3312', 'D4S-AV3115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/143/original/e75a43099e4aba379e32ca16e94bb758.jpg?1403278583";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV3115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av3115-3312', 'D4F-AV3115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/230/original/b0417f8a832bce175ac9d09aa6d2da42.jpg?1403278718";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV3115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av2115-3312', 'D4SO-AV2115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/139/original/72dcdab5b7dc4e476f1f0873185aad74.jpg?1403278578";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV2115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av2115-3312', 'D4S-AV2115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/271/original/7e43dc86be41aaff9eadb12b343600da.jpg?1403278780";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV2115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av2115-3312', 'D4F-AV2115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/202/original/808917183de1ca485c7e254fecd91e68.jpg?1403278677";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV2115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av1115-3312', 'D4SO-AV1115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/212/original/de9aedda7607f32698f9d019e463f876.jpg?1403278693";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV1115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av1115-3312', 'D4S-AV1115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/265/original/8b15b9664685eabb476b7ead6c479506.jpg?1403278771";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV1115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av1115-3312', 'D4F-AV1115-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/301/original/5be4dd92a44bcc2c20621e8548f4a192.jpg?1403278823";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV1115-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av5115v1-3312', 'D4SO-AV5115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/217/original/e07b7deafe4a5c982a2967c74bd0f6b7.jpg?1403278700";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV5115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av5115v1-3312', 'D4S-AV5115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/159/original/9628fbe0c23ee9bc56edc09f9c108104.jpg?1403278606";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV5115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av5115v1-3312', 'D4F-AV5115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2592x1944', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/210/original/ef3f88a4a696b25a63bd62cd33ef844c.jpg?1403278690";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV5115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av3115v1-3312', 'D4SO-AV3115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/130/original/118594cf2eb1b1c373323966a5eba098.jpg?1403278565";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV3115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av3115v1-3312', 'D4S-AV3115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/209/original/cc2afc52c0a8680f38ce912dda773b1b.jpg?1403278689";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV3115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av3115v1-3312', 'D4F-AV3115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '2048x1536', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/167/original/663d432dccff9e6b2eb45417d9f5746c.jpg?1403278617";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV3115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av2115v1-3312', 'D4SO-AV2115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/177/original/63e062ce966caa6e23a03a0c79bf35f6.jpg?1403278637";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV2115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av2115v1-3312', 'D4S-AV2115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/219/original/5637a23de4bad36f5cac758407321c44.jpg?1403278703";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV2115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av2115v1-3312', 'D4F-AV2115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1920x1080', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/134/original/8f248e6504ecdd1cc9351265690faef5.jpg?1403278571";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV2115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4so-av1115v1-3312', 'D4SO-AV1115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/293/original/252f7989ccef9c4a2f246a1665e66df6.jpg?1403278812";}}', 'http://www.arecontvision.com/product/D4+Series/D4SO-AV1115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4s-av1115v1-3312', 'D4S-AV1115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/240/original/53a5f764d9a6fc5934f1617489151835.jpg?1403278735";}}', 'http://www.arecontvision.com/product/D4+Series/D4S-AV1115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'd4f-av1115v1-3312', 'D4F-AV1115v1-3312', '26', 'image', 'mjpeg', 'h264.sdp', '1280x1024', 'Dome', '', '', '1', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/216/original/4378fb3601b20f9f0e00adcf259d5700.jpg?1403278699";}}', 'http://www.arecontvision.com/product/D4+Series/D4F-AV1115v1-3312');
INSERT INTO "cameras" 
VALUES (NULL, 'sncxm637', 'SNCXM637', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncxm636', 'SNCXM636', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncxm632', 'SNCXM632', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncwr632', 'SNCWR632', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncwr630', 'SNCWR630', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncwr602', 'SNCWR602', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncwr600', 'SNCWR600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm632r', 'SNCVM632R', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm631', 'SNCVM631', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm630', 'SNCVM630', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm602r', 'SNCVM602R', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm601b', 'SNCVM601B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm601', 'SNCVM601', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm600b', 'SNCVM600B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvm600', 'SNCVM600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncrh164', 'SNCRH164', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/839/original/bfda0fd5d9a4109e842922d6694778f4.jpg?1402995242";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/843/original/e28c232fc799649d6aaceacf5ebe0d55.jpg?1402995247";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/875/original/ba89345aba9219317e55c9afd334e633.jpg?1402995292";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncrh124', 'SNCRH124', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/771/original/2ea12a8fad04138b02c37ea45c9f7fb4.jpg?1402995145";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/778/original/93d3ec94645c40331a425fe3f9b6fd0b.jpg?1402995154";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/811/original/23b39bec61e4f4da7b8f7334d0afc704.jpg?1402995202";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/847/original/26c4aad5bc599bcadac675734af1fc23.jpg?1402995252";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snchm662', 'SNCHM662', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '2560x1920', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/784/original/acc0afda9d964bb0d1514e367d717afc.jpg?1402995163";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncem632r', 'SNCEM632R', '27', 'in.flv', 'mjpeg', 'media/video1', '14592x2752', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/830/original/baf35fbeda26d6b3fd7b40ed3c7ea12a.jpg?1402995229";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncem631', 'SNCEM631', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/845/original/19065c6c964b90b476b9b74e713c95b2.jpg?1402995250";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncem602r', 'SNCEM602R', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/794/original/8538a0378c099d014dda53bb3e82d855.jpg?1402995178";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncem601', 'SNCEM601', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/776/original/9e41fef4f570029ab9d53b1643e056c3.jpg?1402995152";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncem600', 'SNCEM600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/777/original/b3afb462b805c8f67aa2e1feb0434098.jpg?1402995153";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh280', 'SNCDH280', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/787/original/8e33b4fd6c0747a580f6de039c8ccb1c.jpg?1402995168";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/803/original/f5d6ee0b7b8265db117c7e58d0e27386.jpg?1402995192";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/817/original/1921008866d36b54013461c588f91bdf.jpg?1402995211";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/832/original/7ef579ce25ce9b4c14dd39620650074a.jpg?1402995232";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh260', 'SNCDH260', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/878/original/f0a636daa19e96c02da0e9ac17ea1bbb.jpg?1402995296";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh240t', 'SNCDH240T', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/765/original/7d066fa4d480eed8858273b30c01a0cb.jpg?1402995136";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/823/original/2bbe89b2c2d15b1cc6ea1ff7217484ea.jpg?1402995219";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/874/original/c955c87a7ccb5af0f15adf42320132dc.jpg?1402995290";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh240', 'SNCDH240', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/809/original/3fd64948ccb702d4c9f33b65fda6781e.jpg?1402995199";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/818/original/324bd5ef91478c44319d31faaf3a41df.jpg?1402995212";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/838/original/03c6fe7f8b136c5aa6bafcc22bfc14b7.jpg?1402995240";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/858/original/ae967bc7825004bcafe86a30b232e0c9.jpg?1402995268";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh220t', 'SNCDH220T', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/782/original/bc9cc967675c4cb3476f7ecd716a5318.jpg?1402995160";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/797/original/1050d7029a1add30b31a9f542cd7382e.jpg?1402995182";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/808/original/f98df03d802f634f31653c1692b04a4b.jpg?1402995198";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/859/original/a00f6d21617bf895b6da76e322c49bfd.jpg?1402995269";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh220', 'SNCDH220', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/767/original/26ce84dc7443b10dc12f967904c89019.jpg?1402995139";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/785/original/ea16b50ba6b1ab2ad504371ec2ac5b00.jpg?1402995164";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/804/original/31d3ca3cbbbc0d7941d6fdb638566837.jpg?1402995193";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh210t-slash-w', 'SNCDH210T/W', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh210t-slash-b', 'SNCDH210T/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh210-slash-w', 'SNCDH210/W', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh210-slash-b', 'SNCDH210/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh180', 'SNCDH180', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/786/original/772ba964dd04c8c5c8d50a6da8d6bc6e.jpg?1402995166";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/852/original/5a396cfc70e461ac19b8405c768768b0.jpg?1402995259";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/853/original/8c8e365d4baad1eea53065d28a9acd7c.jpg?1402995261";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/866/original/6de2a79888aebfa5abbc784840b13ccb.jpg?1402995279";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh160', 'SNCDH160', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/795/original/515f1ef011be45361651bc525e9ea779.jpg?1402995179";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/810/original/736e0442a95eb923c99dc92b07fa9f66.jpg?1402995201";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/829/original/b7c64bf50ee0a37ed04b67d2bd3e1f70.jpg?1402995228";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/865/original/3fd1de8a1a9e20fb23cc36f6a973bc8c.jpg?1402995278";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh140t', 'SNCDH140T', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/813/original/ed7ea2a7667e8278c01a5805af31618e.jpg?1402995205";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/840/original/435aff0c9a6606e75b47af199f9a6936.jpg?1402995243";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/881/original/57a55cdea14ce054e05bc7f0d1fcc874.jpg?1402995300";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh140', 'SNCDH140', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/770/original/23fc0ab73fea42f6ccda18e46c917954.jpg?1402995144";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/774/original/5add4bb3de990b35925dae4b8502297f.jpg?1402995149";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/822/original/fb73be6b15f7ddb8336fd15e887a5fc0.jpg?1402995218";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/826/original/fdf23209ac5a0a43fc6adae48e01c8d1.jpg?1402995223";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh120t', 'SNCDH120T', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/799/original/374345e334eaea26397733672cf9d3a6.jpg?1402995185";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/831/original/7ea2d27f9e7aeadfe117dc03f678acbd.jpg?1402995231";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/862/original/452ae58b5d291235e3af9289be6d4a6c.jpg?1402995273";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/867/original/57113b12688de9635d3898e85a100012.jpg?1402995280";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh120', 'SNCDH120', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/835/original/ff0850193c999c90efc9538e48688da0.jpg?1402995237";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/854/original/e3e0a473e0070239b3c2a0b0c50ec79f.jpg?1402995262";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/856/original/6b1f627c3dd1c8fee86d84ca73c2739d.jpg?1402995265";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/860/original/27c66365b848437c639e64271bce019f.jpg?1402995270";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh110t-slash-w', 'SNCDH110T/W', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Dome', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh110t-slash-b', 'SNCDH110T/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x960', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh110-slash-w', 'SNCDH110/W', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x960', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncdh110-slash-b', 'SNCDH110/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x960', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch280', 'SNCCH280', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/781/original/fd93b8ebf96bd14f02b3f863df47f709.jpg?1402995159";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/788/original/714d127681f41dee52d27151cf2259e9.jpg?1402995169";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/789/original/1e2c97aaf71d1091e15371307a4621c9.jpg?1402995171";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/828/original/c39ae3478698a95d75225f603b49adb8.jpg?1402995226";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch260', 'SNCCH260', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/807/original/4081329ce113384be40bb706cec2014e.jpg?1402995196";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/870/original/91cd1d6fa9050222a9f01e360e1ff7a6.jpg?1402995284";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/871/original/2c81b5b2c9b5c2b94d666e7b46a40cac.jpg?1402995286";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch180', 'SNCCH180', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/798/original/4fade88059762e838be1f83c191f509d.jpg?1402995184";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/805/original/a39cec2348b58475369a570df026f2e6.jpg?1402995195";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/825/original/25bae4cbcfda6a2f01d3c0162360d2bb.jpg?1402995222";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/855/original/5cf7d247e0e2f248a1df0d620f9c8983.jpg?1402995263";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch160', 'SNCCH160', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Bullet', 'admin', 'admin', '', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/790/original/91e21e28c3992929ccf54009abff8716.jpg?1402995172";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/850/original/6cf980afd1402763f7104860d867fe5f.jpg?1402995257";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/879/original/f9502c365646f47afb428c107373038b.jpg?1402995297";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/882/original/5694afb89a37ed2429c1525baaac03e1.jpg?1402995301";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvb635', 'SNCVB635', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/768/original/650537a3d2e41f8fe233d56024257f8a.jpg?1402995141";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvb630', 'SNCVB630', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/842/original/6d82c74a88f259ff58f2e6ecd508c4ee.jpg?1402995246";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/868/original/56b958d65165a92497205ee77d6d0911.jpg?1402995282";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvb600b', 'SNCVB600B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/841/original/2b2b36452f397376f94bde3d4bd2e542.jpg?1402995245";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/872/original/2193b36e3a733b1a5be0bd0f35397981.jpg?1402995287";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncvb600', 'SNCVB600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/792/original/e060070c8825d4860ad790c2084f3107.jpg?1402995175";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/876/original/4ab4d53a69dc663c6271335138661c08.jpg?1402995293";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snceb630b', 'SNCEB630B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/869/original/4d3545219b3e14b7eb6a2c70f2f30120.jpg?1402995283";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snceb630', 'SNCEB630', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/873/original/e063d5788568a0a2e5bb62bb320a50c5.jpg?1402995289";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snceb600b', 'SNCEB600B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/844/original/7b909092192fa22035b13b99e93fe79d.jpg?1402995248";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snceb600', 'SNCEB600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/816/original/d9c5118a939f7c15f7204ee6ca17ed16.jpg?1402995209";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snccx600w', 'SNCCX600W', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/849/original/66cc724be2bec9b5addf7be6c2e6ee10.jpg?1402995255";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'snccx600', 'SNCCX600', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x720', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/880/original/dd459c961c91c9e14452f2242cddb253.jpg?1402995299";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch240', 'SNCCH240', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1921x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/779/original/8b8dd000543853a3047eeb1a83e241c4.jpg?1402995156";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/780/original/6bea768cde052baea577f26d42ed8591.jpg?1402995157";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/848/original/b20c596fec858ee83c9d7c00e3e51eda.jpg?1402995254";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/863/original/e841b93f8ac3f3079ad9fc2e53108f16.jpg?1402995275";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch220', 'SNCCH220', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/801/original/7eb5f47ca545ed7c91f88f91bd9cdcda.jpg?1402995189";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch210-slash-s', 'SNCCH210/S', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch210-slash-b', 'SNCCH210/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1920x1080', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch140', 'SNCCH140', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/769/original/8782bfd7a62554e7af7fbc0f4dc4b7ae.jpg?1402995142";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/773/original/538a7e752bc6370648933a790bcaf7ba.jpg?1402995148";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/775/original/bbe57dfc63b94b2aa4d8544f8ac982ee.jpg?1402995150";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/815/original/7cf81f480f220a68f4820cea990177b3.jpg?1402995208";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch120', 'SNCCH120', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x1024', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/766/original/e1900f9fb5f95f707162ac6d9e55bd4b.jpg?1402995138";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/812/original/a9157fae3c4a9865f2c71d2379f5dd3b.jpg?1402995204";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/814/original/e3e5b8b16698a6a564d05486a4790641.jpg?1402995206";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/820/original/3eb32064464f14c1fa15881c6b739260.jpg?1402995215";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch110-slash-s', 'SNCCH110/S', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x960', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'sncch110-slash-b', 'SNCCH110/B', '27', 'oneshotimage.jpg', 'mjpeg', 'media/video1', '1280x960', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-942l', 'DCS-942L', '32', 'image/jpeg.cgi', 'video/mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/731/original/e41e5286207fc912302935534078f8fa.jpg?1403708759";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-942l-enhanced-day-night-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-933l', 'DCS-933L', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://www.dlink.com/rs/sr/support/product/dcs-933l-day-night-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-932l', 'DCS-932L', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/732/original/dc3412690163becb20659fcc28f1e230.jpg?1403708789";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-932l-day-night-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-932', 'DCS-932', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/733/original/3af643d7f00e845b5c86c67dfa9e6e0a.jpg?1403708803";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-932-wireless-n-day-and-night-home-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-930l', 'DCS-930L', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/734/original/7867e4be8b169ef8c1a69138c02cc31d.jpg?1403708818";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-930l-wireless-n-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-930', 'DCS-930', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/735/original/51afa396ba6200dd37896825bb304850.jpg?1403708832";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-930-wireless-n-home-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-900', 'DCS-900', '32', 'image.jpg', 'mjpeg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/736/original/1763d89c9fdfae92fd7c9999baeaa74f.jpg?1403708846";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-900-internet-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-5605', 'DCS-5605', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'play.sdp', 'ntsc:704x480/pal:704x576', 'Box', 'admin', '', '', 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/754/original/1af4af66dd482315697940decf6cbba1.jpg?1403709126";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-5605-securicam-h-264-ptz-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-5230', 'DCS-5230', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'play.sdp', '1280x1024', 'Box', 'admin', '', '', 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/755/original/1dc96a36f55979955ff750af6ff8d657.jpg?1403709141";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-5230-wireless-n-ptz-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-5222l', 'DCS-5222L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'play.sdp', '1280x720', 'Box', 'admin', '', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dlink.com/rs/sr/support/product/dcs-5222l-pan-tilt-zoom-cloud-camera?revision=deu_revb');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-5020l', 'DCS-5020L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/756/original/81421e52d9bef3f0c95a4795dc894dfe.jpg?1403709170";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-5020l-pan-tilt-day-night-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2332l', 'DCS-2332L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/763/original/1fdfe7cd9fc9f67ed31fafa17102eba5.jpg?1403709279";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2332l-outdoor-hd-wireless-day-night-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2330l', 'DCS-2330L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x720', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/762/original/a7993f126c59ff9a71898714c698fb25.jpg?1403709264";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2330l-outdoor-hd-wireless-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2310l', 'DCS-2310L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/764/original/3eb4c21a13f9e94983ca52212b60f75d.jpg?1403709294";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2310l-outdoor-hd-poe-day-night-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2230', 'DCS-2230', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/765/original/d92e8ad3e22eecf960c83b9992168973.jpg?1403709309";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2230-full-hd-wireless-day-night-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2210', 'DCS-2210', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/766/original/4ed02aa9718f7a1d47bbcdf89a99ecc5.jpg?1403709324";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2210-full-hd-poe-day-night-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2136l', 'DCS-2136L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x720', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/767/original/372a538c9207f914e95956c3269d118b.jpg?1403709339";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2136l-hd-wireless-day-night-camera-with-color-night-vision');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2132l', 'DCS-2132L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Box', 'admin', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.dlink.com/rs/sr/support/product/dcs-2132l-hd-wireless-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2130', 'DCS-2130', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Box', 'admin', '', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/768/original/c024ed681e3fce2af6aac67a9408bb24.jpg?1403709367";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2130-hd-wireless-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2121', 'DCS-2121', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/769/original/04c82bff8c7c4e82a6a4f1c5328d970f.jpg?1403709381";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2121-megapixel-wireless-internet-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2120', 'DCS-2120', '32', 'cgi-bin/video.jpg', '', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/770/original/4c18d2abcae0e1758daabde41d8a1b78.jpg?1403709396";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2120-wireless-internet-camera-with-3g-mobile-video-support');
INSERT INTO "cameras" 
VALUES (NULL, 'f3215', 'F3215', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1922x1080', 'Box', '', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/778/original/5d6da8b7952dd23716b9cb2eece8ec66.jpg?1403709517";}}', 'http://zavio.com/product.php?id=84');
INSERT INTO "cameras" 
VALUES (NULL, 'f3210', 'F3210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1921x1080', 'Box', '', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://zavio.com/product.php?id=70');
INSERT INTO "cameras" 
VALUES (NULL, 'f3206', 'F3206', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=29');
INSERT INTO "cameras" 
VALUES (NULL, 'f3201', 'F3201', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://zavio.com/product.php?id=28');
INSERT INTO "cameras" 
VALUES (NULL, 'f3115', 'F3115', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=64');
INSERT INTO "cameras" 
VALUES (NULL, 'f3110', 'F3110', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Box', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://zavio.com/product.php?id=63');
INSERT INTO "cameras" 
VALUES (NULL, 'f3107', 'F3107', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=66');
INSERT INTO "cameras" 
VALUES (NULL, 'f3102', 'F3102', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://zavio.com/product.php?id=65');
INSERT INTO "cameras" 
VALUES (NULL, 'f3005', 'F3005', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '640x480', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=68');
INSERT INTO "cameras" 
VALUES (NULL, 'f3000', 'F3000', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '640x480', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=67');
INSERT INTO "cameras" 
VALUES (NULL, 'f1105', 'F1105', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x720', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=58');
INSERT INTO "cameras" 
VALUES (NULL, 'f1100', 'F1100', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x720', 'Box', 'admin', 'admin', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://zavio.com/product.php?id=56');
INSERT INTO "cameras" 
VALUES (NULL, 'pz8121w', 'PZ8121W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Box', 'Root', '', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.vivotek.com/pz81x1w/');
INSERT INTO "cameras" 
VALUES (NULL, 'pz8121', 'PZ8121', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Box', 'Root', '', '', 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/pz81x1/');
INSERT INTO "cameras" 
VALUES (NULL, 'pz8111w', 'PZ8111W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Box', 'Root', '', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.vivotek.com/pz81x1w/');
INSERT INTO "cameras" 
VALUES (NULL, 'pz8111', 'PZ8111', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Box', 'Root', '', '', 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/pz81x1/');
INSERT INTO "cameras" 
VALUES (NULL, 'pt8133w', 'PT8133W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 'a:0:{}', 'http://www.vivotek.com/pt8133w/');
INSERT INTO "cameras" 
VALUES (NULL, 'pt8133', 'PT8133', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/pt8133/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8336w', 'IP8336W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://www.vivotek.com/ip8336w/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8133w', 'IP8133W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.vivotek.com/ip8133w/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8133', 'IP8133', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8133/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8131w', 'IP8131W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.vivotek.com/ip8131w/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8131', 'IP8131', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8131/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8130w', 'IP8130W', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:0:{}', 'http://www.vivotek.com/ip8130w/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8130', 'IP8130', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8130/');
INSERT INTO "cameras" 
VALUES (NULL, 'cc8130', 'CC8130', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Box', 'Root', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/cc8130/');
INSERT INTO "cameras" 
VALUES (NULL, 'sf8174v', 'SF8174V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sf8174v/');
INSERT INTO "cameras" 
VALUES (NULL, 'sf8174', 'SF8174', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sf8174/');
INSERT INTO "cameras" 
VALUES (NULL, 'sf8172v', 'SF8172V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sf8172v/');
INSERT INTO "cameras" 
VALUES (NULL, 'sf8172', 'SF8172', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sf8172/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8363e', 'SD8363E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd8363e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8362e-ss', 'SD8362E-SS', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd8362e-ss/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8362e', 'SD8362E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd8362e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8326e', 'SD8326E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x6e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8324e', 'SD8324E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x4e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8323e', 'SD8323E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x3e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8322e', 'SD8322E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x2e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8321e', 'SD8321E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x1e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8316e', 'SD8316E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x6e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8314e', 'SD8314E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x4e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8313e', 'SD8313E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x3e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8312e', 'SD8312E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x2e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8311e', 'SD8311E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/sd83x1e/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8121', 'SD8121', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/sd81x1/');
INSERT INTO "cameras" 
VALUES (NULL, 'sd8111', 'SD8111', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'ntsc720x480', 'Dome', 'Root', '', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/sd81x1/');
INSERT INTO "cameras" 
VALUES (NULL, 'pd8136', 'PD8136', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/pd8136/');
INSERT INTO "cameras" 
VALUES (NULL, 'md8562', 'MD8562', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/md8562/');
INSERT INTO "cameras" 
VALUES (NULL, 'md8531h', 'MD8531H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x960', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/md8531h/');
INSERT INTO "cameras" 
VALUES (NULL, 'md7560x', 'MD7560X', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', '', '1600x1200', 'Dome', 'Root', '', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/md7560x/');
INSERT INTO "cameras" 
VALUES (NULL, 'md7560', 'MD7560', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', '', '1600x1200', 'Dome', 'Root', '', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/md7560/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8372', 'IP8372', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2560x1920', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8372/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8371e', 'IP8371E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8371e/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8371', 'IP8371', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8371/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8365eh', 'IP8365EH', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8365eh/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8364-c', 'IP8364-C', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8364-c/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8362', 'IP8362', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8362/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8361', 'IP8361', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8361/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8355eh', 'IP8355EH', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8355eh/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8352', 'IP8352', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/ip8352/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8337h-c', 'IP8337H-C', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8337h-c/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8335h', 'IP8335H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8335h/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8332-c', 'IP8332-C', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8332-c/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8332', 'IP8332', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8332/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8331', 'IP8331', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '640x480', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8331/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8173h', 'IP8173H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Box', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8173h/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8172p', 'IP8172P', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2560x1920', 'Box', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8172p/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8172', 'IP8172', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2560x1920', 'Box', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8172/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8162p', 'IP8162P', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Box', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8162p/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8162', 'IP8162', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Box', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8162/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8161', 'IP8161', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Box', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8161/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8152', 'IP8152', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Box', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8152/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8151p', 'IP8151P', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Box', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8151p/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip8151', 'IP8151', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Box', 'Root', '', '', 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip8151/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip7361', 'IP7361', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', '', '1600x1200', 'Bullet', 'Root', '', '', 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip7361/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip7161', 'IP7161', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', '', '1600x1200', 'Box', 'Root', '', '', 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip7161/');
INSERT INTO "cameras" 
VALUES (NULL, 'ip7160', 'IP7160', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', '', '1600x1200', 'Box', 'Root', '', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ip7160/');
INSERT INTO "cameras" 
VALUES (NULL, 'ib8354-c', 'IB8354-C', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Bullet', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ib8354-c/');
INSERT INTO "cameras" 
VALUES (NULL, 'ib8168', 'IB8168', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Bullet', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ib8168/');
INSERT INTO "cameras" 
VALUES (NULL, 'ib8156', 'IB8156', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Bullet', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ib8156/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8174v', 'FE8174V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fe8174v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8174', 'FE8174', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fe8174/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8173', 'FE8173', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1536x1536', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fe8173/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8172v', 'FE8172V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/fe8172v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8172', 'FE8172', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', 'Root', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/fe8172/');
INSERT INTO "cameras" 
VALUES (NULL, 'fe8171v', 'FE8171V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1536x1536', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/fe8171v');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8372', 'FD8372', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2560x1920', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8372/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8371v', 'FD8371V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8371v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8371ev', 'FD8371EV', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8371ev/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8365ehv', 'FD8365EHV', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8365ehv/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8363', 'FD8363', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8363/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8362e', 'FD8362E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8362e/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8362', 'FD8362', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8362/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8361l', 'FD8361L', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/777/original/75e97a861b587b599b87c6814833db7b.jpg?1403709501";}}', 'http://www.vivotek.com/fd8361l/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8361', 'FD8361', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8361/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8355ehv', 'FD8355EHV', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8355ehv/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8335h', 'FD8335H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8335h/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8171', 'FD8171', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '2048x1536', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8171/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8168', 'FD8168', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8168/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8166', 'FD8166', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8166/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8164v', 'FD8164V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8164v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8164', 'FD8164', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8164/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8163', 'FD8163', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8163/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8162', 'FD8162', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8162/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8161', 'FD8161', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8161/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8156', 'FD8156', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8156/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8154v', 'FD8154V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8154v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8154', 'FD8154', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8154/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8151v', 'FD8151V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x1024', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8151v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8137hv', 'FD8137HV', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8137hv/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8137h', 'FD8137H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8137h/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8136', 'FD8136', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8136/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8135h', 'FD8135H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8135h/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8134v', 'FD8134V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8134v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8134', 'FD8134', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8134/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8133v', 'FD8133V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8133v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8133', 'FD8133', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8133/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8131v', 'FD8131V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8131v/');
INSERT INTO "cameras" 
VALUES (NULL, 'fd8131', 'FD8131', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', 'Root', '', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/fd8131/');
INSERT INTO "cameras" 
VALUES (NULL, 'bs5332e', 'BS5332E', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', 'pal720x576', 'Dome', '', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/bs5332e/');
INSERT INTO "cameras" 
VALUES (NULL, 'bd5336', 'BD5336', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1080', 'Dome', '', '', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/bd5336/');
INSERT INTO "cameras" 
VALUES (NULL, 'bd5115', 'BD5115', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Dome', '', '', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.vivotek.com/bd5115/');
INSERT INTO "cameras" 
VALUES (NULL, 'bb5315', 'BB5315', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', '', '', '', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/bb5315/');
INSERT INTO "cameras" 
VALUES (NULL, 'bb5116', 'BB5116', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1600x1200', 'Box', '', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/bb5116/');
INSERT INTO "cameras" 
VALUES (NULL, 'af5127v', 'AF5127V', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', '', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/af5127v/');
INSERT INTO "cameras" 
VALUES (NULL, 'af5127', 'AF5127', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1920x1920', 'Dome', '', '', '', 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:0:{}', 'http://www.vivotek.com/af5127/');
INSERT INTO "cameras" 
VALUES (NULL, 'ab5353h', 'AB5353H', '28', 'cgi-bin/viewer/video.jpg', 'video.mjpg', 'live.sdp', '1280x800', 'Bullet', '', '', '', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.vivotek.com/ab5353h/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw115', 'WV-SW115', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/462/original/4aea64a26336a4b536d906a9f52f7fba.jpg?1402996298";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/496/original/0817a1d741cca050a854411b80f96556.jpg?1402996375";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/510/original/264515afe66fac96bbbc3c7658935e62.jpg?1402996396";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfr611l', 'WV-SFR611L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/466/original/6adb6d5244f9cbbb0f612d415abc3727.png?1402996304";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/471/original/c259367674676561d6b870c9829c1080.png?1402996313";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/491/original/a45f3a18b4f2c1948281a20e727cc9eb.png?1402996366";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/535/original/034fe60fe2572ed754e3141a845b0ee7.png?1402996435";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfr631l', 'WV-SFR631L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/475/original/b3006c86b41297024c30156f7db6ffc2.jpg?1402996320";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfv631l', 'WV-SFV631L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/464/original/b05560ba6a8dfc8d4470840509eaa5e2.png?1402996301";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/470/original/103903ba9340126f239fe9f935502702.png?1402996311";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/481/original/459c871dec7c34bd2647804a1f293064.png?1402996329";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/527/original/9cbaf12d9c22ad65c5dfa4454e5fed92.png?1402996422";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfv611l', 'WV-SFV611L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/460/original/e007b3d6f2121faca7e42000927fae29.jpg?1402996296";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw158', 'WV-SW158', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/476/original/465f0c5ad9b03e61de84fab8f81343ad.jpg?1402996322";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/511/original/b1fe185500d847076b934ec9d6d421b5.jpg?1402996398";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/530/original/600dbde7318c71e1cfd4754ce5401e4d.jpg?1402996428";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw559', 'WV-SW559', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/494/original/640e14d6f04cbcf8b958f5c32502c1ca.jpg?1402996371";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw558', 'WV-SW558', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/517/original/3596a850c90149b354ecab5b0fdbfe76.jpg?1402996407";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw155', 'WV-SW155', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/514/original/7a79e045837b813fad0156e86db753d1.jpg?1402996402";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw152', 'WV-SW152', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/503/original/1dc4711f1c5e11fbb0e621dbe9327a9c.jpg?1402996385";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw352', 'WV-SW352', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/506/original/ec86b267ab5448f84a5f5184e80e767c.jpg?1402996389";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw355', 'WV-SW355', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/495/original/34692531a6c2779951d92d710e4ff565.jpg?1402996373";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf346', 'WV-SF346', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/531/original/eb9b9c04625b8af31a3313be2a4ea99d.jpg?1402996429";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf342', 'WV-SF342', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x720', 'Dome', 'admin', '12345', '', 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/474/original/122c3558a4621d26c3fc8b23cfbd9f89.jpg?1402996318";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-nw502s', 'WV-NW502S', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/524/original/0b1884709a8b4ed28f4941e506fa7b58.jpg?1402996417";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sc588', 'WV-SC588', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/459/original/31d239b1892bcc8439a2cedaf03d6ea8.jpg?1402996294";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/533/original/f6512cf6c141c9f877010eda00921c18.jpg?1402996433";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-cw594a', 'WV-CW594A', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/522/original/d88e628df0daa1925927f3ef23203c1a.jpg?1402996415";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw395a', 'WV-SW395A', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/478/original/11c28bb3da76758a7b9e4423f6f37cfc.jpg?1402996324";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw396a', 'WV-SW396A', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/468/original/1caeaa21f3e9481445d7286144bce51b.png?1402996308";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw598', 'WV-SW598', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/500/original/3d583b043dd359800da84984b9662581.jpg?1402996380";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/501/original/df5cb5f14091929adfada07e750f0544.png?1402996382";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw458', 'WV-SW458', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/505/original/213eb14f4d6faa63a865adb8c503ac56.jpg?1402996388";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf438', 'WV-SF438', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/479/original/2d70aa1aaded1dc051e24cb206acc8e4.jpg?1402996326";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sc386', 'WV-SC386', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/486/original/7ccfa8df6a3a873d04cefdaea0dcf3fc.jpg?1402996359";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sc384', 'WV-SC384', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/525/original/9495793670fe17f390e563d928b0d502.jpg?1402996419";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw396', 'WV-SW396', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/472/original/b12c2cf2aec57e8dc79c634b2d86fee7.jpg?1402996315";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw395', 'WV-SW395', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/528/original/8f16113d4c7e0aa1124ae777c70f3363.jpg?1402996424";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sc385', 'WV-SC385', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/509/original/b5f8c35d3e2b768d8f180a401e806150.jpg?1402996394";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-cp310', 'WV-CP310', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '976x494', 'Dome', 'admin', '12345', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/463/original/ff75a0eed5684c43201f78c4cc759c74.jpg?1402996300";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf549', 'WV-SF549', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '2048x1536', 'Dome', 'admin', '12345', '', 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/465/original/52779dfc5b91dc7657d474b6b5e50049.jpg?1402996303";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf548', 'WV-SF548', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/499/original/927369f8f271956d7e65c6b86d76bcf7.jpg?1402996379";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf539', 'WV-SF539', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/467/original/a7a392266b7c9e78e893767dd45c11fa.jpg?1402996306";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf538', 'WV-SF538', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/484/original/b19831e31b31c34e4829c2f8b8137b9f.jpg?1402996356";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-cp300', 'WV-CP300', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '', 'Dome', 'admin', '12345', '', 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/504/original/082d76379d3bb44e60c1586b68e6a74a.jpg?1402996387";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf138', 'WV-SF138', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/487/original/ba5a8ee628d0ea7b4c7f45260dc23c8b.jpg?1402996361";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf135', 'WV-SF135', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/532/original/7a84f048ef33da2e72537104a849d9f7.jpg?1402996431";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf132', 'WV-SF132', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Dome', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/469/original/40b1476b785e5520a707ed8029d3a886.jpg?1402996310";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf336', 'WV-SF336', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/488/original/1c95723114d73bc0eecfd20c00947a27.jpg?1402996362";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf335', 'WV-SF335', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/458/original/c4dfb63e1de8cfbb577f7d0c3427c87e.jpg?1402996293";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sf332', 'WV-SF332', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Dome', 'admin', '12345', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/477/original/d9316e7bfad98e57ba8c7c97db8b0e52.jpg?1402996323";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfn631l', 'WV-SFN631L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '2048x1536', 'Dome', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/493/original/6a77ea18946f59bc373440a36893af0d.jpg?1402996370";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/523/original/b5b35036e3875ec004b216c5806c9a30.jpg?1402996416";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sfn611l', 'WV-SFN611L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Dome', 'admin', '12345', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/515/original/0b500654b65826793ac3f0d1a609a386.jpg?1402996404";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/518/original/e738b69d2f1bc1d0ec89e281b3cf878c.jpg?1402996408";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw174w', 'WV-SW174W', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/492/original/db3297e62f81d4a143e145c5c1d55184.jpg?1402996368";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw316l', 'WV-SW316L', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/502/original/643e7aac2f581fe0d1bcf7a85cf92f26.jpg?1402996383";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp509', 'WV-SP509', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Box', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/507/original/6258f8cbc7ea182219f56033737033cb.jpg?1402996391";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp508', 'WV-SP508', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1920x1080', 'Box', 'admin', '12345', '', 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/519/original/cc6ce94a27bd902f8aff9745bef940c3.jpg?1402996410";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw316', 'WV-SW316', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/513/original/b6c141664fa12ba57783eb2303971604.jpg?1402996401";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw314', 'WV-SW314', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/482/original/02319f5775ce4732083e5e9932c52ba3.jpg?1402996353";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-st165', 'WV-ST165', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/497/original/5a6983b13e2b036713aab56ad5e7c6f3.jpg?1402996376";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/508/original/b70cd29f52f68ed960424f6de3a44055.jpg?1402996392";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/516/original/29cb3d9e751f34e7103a3a8d28960208.jpg?1402996406";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw175', 'WV-SW175', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/485/original/f7af4d7ce04764ffbd0cf2e13ca1d9c7.jpg?1402996358";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/520/original/f3b486d8d805955954a3279ee5345c2c.jpg?1402996411";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/521/original/14778666589a3d85c4126c43595225b7.jpg?1402996413";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-st162', 'WV-ST162', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/461/original/ea1d6f02e9204a73f6a3e1d3d2c5cde9.jpg?1402996297";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/526/original/d426b43f70990b4c986e43d193604270.jpg?1402996420";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/534/original/a72e306777b3edc8c9143e199c4b866e.jpg?1402996434";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sw172', 'WV-SW172', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '800x600', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/483/original/896d3c9e0372a218421d1daf15699250.jpg?1402996355";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/498/original/92a8a749e48ee127a54d6d640e3f300c.jpg?1402996377";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/529/original/2fb798fc248d4978b1ab2ba9d75b9b70.jpg?1402996426";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp306', 'WV-SP306', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/512/original/0468bf8b2d58a373745de4df901704e9.jpg?1402996399";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp305', 'WV-SP305', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/489/original/c73f2f48936dc271f8dc56785297ba7d.jpg?1402996363";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp105', 'WV-SP105', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '1280x960', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/473/original/553b079e7a0075ee165477d6d7c2296e.jpg?1402996317";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'wv-sp102', 'WV-SP102', '29', 'SnapshotJPEG', 'nphMotionJpeg', 'MediaInput/h264', '640x480', 'Box', 'admin', '12345', '', 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/490/original/5255d8c72c283e8c10d58175e8a6bb34.jpg?1402996365";}}', 'http://security.panasonic.com/pss/security/');
INSERT INTO "cameras" 
VALUES (NULL, 'd24', 'D24', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2048x1536', 'Box', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'd15', 'D15', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2048x1536', 'Box', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'm24', 'M24', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2048x1536', 'Box', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'm12', 'M12', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2045x1536', 'Box', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'q24', 'Q24', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2048x1536', 'Dome', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 's15', 'S15', '30', 'record/current.jpg', 'cgi-bin/faststream.jpg', '', '2048x1536', 'Box', 'admin', 'meinsm', '', 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'p6210', 'P6210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', '', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=85');
INSERT INTO "cameras" 
VALUES (NULL, 'p5210', 'P5210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=74');
INSERT INTO "cameras" 
VALUES (NULL, 'p5116', 'P5116', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:0:{}', 'http://www.zavio.com/product.php?id=73');
INSERT INTO "cameras" 
VALUES (NULL, 'p5111', 'P5111', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=72');
INSERT INTO "cameras" 
VALUES (NULL, 'f7210', 'F7210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Box', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 'a:0:{}', 'http://www.zavio.com/product.php?id=51');
INSERT INTO "cameras" 
VALUES (NULL, 'd7510', 'D7510', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '2560x1920', 'Dome', '', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=87');
INSERT INTO "cameras" 
VALUES (NULL, 'd7320', 'D7320', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '2048x1536', 'Dome', '', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=80');
INSERT INTO "cameras" 
VALUES (NULL, 'd7210', 'D7210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=62');
INSERT INTO "cameras" 
VALUES (NULL, 'd7111', 'D7111', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=60');
INSERT INTO "cameras" 
VALUES (NULL, 'd5210', 'D5210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=61');
INSERT INTO "cameras" 
VALUES (NULL, 'd5113', 'D5113', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=59');
INSERT INTO "cameras" 
VALUES (NULL, 'd4210', 'D4210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', '', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=83');
INSERT INTO "cameras" 
VALUES (NULL, 'd3200', 'D3200', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=75');
INSERT INTO "cameras" 
VALUES (NULL, 'd3100', 'D3100', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=76');
INSERT INTO "cameras" 
VALUES (NULL, 'b7510', 'B7510', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '2560x1920', 'Bullet', '', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=88');
INSERT INTO "cameras" 
VALUES (NULL, 'b7320', 'B7320', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '2048x1536', 'Bullet', '', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=86');
INSERT INTO "cameras" 
VALUES (NULL, 'b7210', 'B7210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=45');
INSERT INTO "cameras" 
VALUES (NULL, 'b5210', 'B5210', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=79');
INSERT INTO "cameras" 
VALUES (NULL, 'b5111', 'B5111', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '1280x800', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=71');
INSERT INTO "cameras" 
VALUES (NULL, 'b5010', 'B5010', '31', 'jpg/image.jpg', 'video.mjpg', 'video.pro[profile_number]', '640x480', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.zavio.com/product.php?id=78');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7513', 'DCS-7513', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '1920x1080', 'Bullet', 'admin', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/737/original/1b04ad052805695f3352c27c5ec24f53.jpg?1403708861";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7513-outdoor-full-hd-wdr-poe-day-night-fixed-bullet-camera-with-ir-led');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7510', 'DCS-7510', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', 'ntsc:704x480/pal:704x576', 'Box', 'admin', '', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/738/original/4ba82f7ed4963752ada68e4c246ea9e1.jpg?1403708876";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7510-day-and-night-outdoor-poe-network-camera-with-icr');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7413', 'DCS-7413', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/739/original/e2c7cecebf5cc2750158d2f706d28263.jpg?1403708891";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7413-outdoor-full-hd-poe-day-night-fixed-bullet-camera-with-ir-led?revision=deu_reva');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7410', 'DCS-7410', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', 'ntsc:704x480/pal:704x576', 'Box', 'admin', '', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/740/original/6c4906f302360bfb341db6bb5096e6fa.jpg?1403708906";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7410-securicam-day-and-night-outdoor-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7110', 'DCS-7110', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/741/original/3b2c5148a79f3dfffdca3ab441df9d91.jpg?1403708921";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7110-outdoor-hd-poe-day-night-fixed-bullet-camera-with-ir-led?revision=deu+reva');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-7010l', 'DCS-7010L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x800', 'Bullet', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/742/original/9ec2cdd7e456e906801f8898099c8ab4.jpg?1403708935";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-7010loutdoor-hd-poe-day-night-fixed-mini-bullet-cloud-camera-with-ir-led');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6915', 'DCS-6915', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '1920x1080', 'Dome', 'admin', '', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/743/original/3c6abbed812baa8d6a197caa3a79a3aa.jpg?1403708949";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6915-outdoor-20x-full-hd-wdr-day-night-speed-dome-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6818', 'DCS-6818', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', 'pal:704x576', 'Dome', 'admin', '', '', 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 'a:0:{}', 'http://www.dlink.com/rs/sr/support/product/dcs-6818-outdoor-36x-wdr-day-night-speed-dome-camera?revision=deu_reva');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6815', 'DCS-6815', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', 'ntsc:704x480/pal:704x576', 'Dome', 'admin', '', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/744/original/a1675ed553d1f7e620e3648a3544b64d.jpg?1403708977";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6815-outdoor-18x-wdr-day-night-speed-dome-camera?revision=deu_reva');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6616', 'DCS-6616', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', 'ntsc:704x480/pal:704x576', 'Dome', 'admin', '', '', 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/745/original/da84c6c839c51a61e6e15c55917138ea.jpg?1403708992";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6616-12x-wdr-day-night-speed-dome-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6513', 'DCS-6513', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '2048x1536', 'Dome', 'admin', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/746/original/829ac62f56cf007cca49bfc555dff502.jpg?1403709007";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6513-full-hd-wdr-day-night-outdoor-dome-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6511', 'DCS-6511', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x1024', 'Dome', 'admin', '', '', 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/747/original/0af682cb36b4b836711b0b8061cb250d.jpg?1403709021";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6511-outdoor-hd-poe-day-night-vandal-resistant-fixed-dome-camera-with-ir-led');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6314', 'DCS-6314', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Dome', 'admin', '', '', 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/748/original/e68218075eef69136916494d93b2f2cd.jpg?1403709036";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6314-full-hd-wdr-varifocal-day-night-outdoor-dome-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6210', 'DCS-6210', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Dome', 'admin', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/749/original/9904666490778cece5619edbc2047d10.jpg?1403709051";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6210-outdoor-hd-poe-day-night-vandal-resistant-fixed-dome-camera-with-ir-led');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6113', 'DCS-6113', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '1920x1080', 'Dome', 'admin', '', '', 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/750/original/ba351cd04944e225fd909ac43981e293.jpg?1403709066";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6113-full-hd-poe-day-night-fixed-dome-camera-with-ir-led');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6111', 'DCS-6111', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '640x480', 'Dome', 'admin', '', '', 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/751/original/fd5075d67fa3861ae3adef1b5d577bf6.jpg?1403709081";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6111-wdr-poe-day-night-fixed-dome-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6110', 'DCS-6110', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '640x480', 'Dome', 'admin', '', '', 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/752/original/1fab142d5be7967ee21b7a6355fde42c.jpg?1403709097";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6110-fixed-dome-poe-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-6010l', 'DCS-6010L', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1600x1200', 'Dome', 'admin', '', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/753/original/c505e07351deeb15fe64a43469956512.jpg?1403709111";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-6010l-2-megapixel-panoramic-wireless-cloud-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-3716', 'DCS-3716', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1920x1080', 'Box', 'admin', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/757/original/4807c8c179b46fec13d2065ee1d66b3b.jpg?1403709187";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-3716-full-hd-wdr-poe-day-night-fixed-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-3710', 'DCS-3710', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '1280x1024', 'Box', 'admin', '', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/758/original/7fde3906d2ec163cfecab859f30c87dc.jpg?1403709201";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-3710-hd-wdr-poe-day-night-fixed-camera?revision=deu_reva');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-3430', 'DCS-3430', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', 'live.sdp', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/759/original/bddae79e44d0e13191d4fae5478059fa.jpg?1403709216";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-3430-wireless-day-night-fixed-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-3110', 'DCS-3110', '32', 'cgi-bin/video.jpg', 'video/mjpg.cgi', 'live.sdp', '1280x1024', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/760/original/ee29b72a6253317f4075b8a30b4996ac.jpg?1403709232";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-3110-megapixel-poe-fixed-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-3010', 'DCS-3010', '32', 'image/jpeg.cgi', 'video/mjpeg.cgi', 'live.sdp', '1280x800', 'Box', 'admin', '', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/761/original/abdda4a44faab126f79bb3b129a8d2cb.jpg?1403709247";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-3010-hd-poe-fixed-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2100g', 'DCS-2100G', '32', 'cgi-bin/video.jpg', '', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/771/original/9f7870e692915d705bd886ca8f487d93.jpg?1403709411";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2100g-wireless-g-internet-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2100-plus', 'DCS-2100+', '32', 'cgi-bin/video.jpg', 'video/mjpg.cgi', '', '640x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/772/original/b35b8d1dcc680e84897f1fafb42387b5.jpg?1403709426";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2100plus-enhanced-2-4ghz-802-11b-wireless-internet-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-2000', 'DCS-2000', '32', 'cgi-bin/video.jpg', 'video/mjpg.cgi', '', '640x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/773/original/4e025be207a15144f4925361eb760064.jpg?1403709440";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-2000-internet-camera-with-audio-monitoring');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-1130', 'DCS-1130', '32', 'image/jpeg.cgi', 'video/mjpg.cgi', '', '640x480', 'Box', 'admin', '', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/774/original/8dd58dba3d9a9cdcfb4246726c4913bb.jpg?1403709455";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-1130-wireless-n-network-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-1000w', 'DCS-1000W', '32', 'image.jpg', 'video.cgi', '', '640x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/775/original/1fe67a2327a24730b42a286a0a48bf09.jpg?1403709470";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-1000w');
INSERT INTO "cameras" 
VALUES (NULL, 'dcs-1000', 'DCS-1000', '32', 'image.jpg', 'video.cgi', '', '640x480', 'Box', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/776/original/254f8a5d61c04e4c73c97cabe7165c52.jpg?1403709484";}}', 'http://www.dlink.com/rs/sr/support/product/dcs-1000-internet-securty-camera');
INSERT INTO "cameras" 
VALUES (NULL, 'default', 'Default', '2', '', '', '', '', 'Other', 'root', '', '', 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/684/original/5a0f26c4a1b648556dce1db6992e447c.jpg?1403707973";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/685/original/8c9719dd3e933e2b20dc7ca471ed7d26.jpg?1403707989";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/686/original/d826b1d35e1618518e5a3fe719d6c0f7.jpg?1403708005";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/687/original/341dbac839d8739316a99bd0db9ae2b6.jpg?1403708022";}}', 'http://www.tp-link.us/');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3601-hd', 'GXV3601_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1600x1200', 'Bullet', 'admin', 'admin', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/688/original/e02346d1e4f1b5ac553f5c7a46dce7d4.jpg?1403708042";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/689/original/55a1ea1305702bba3c254d1fb4f9851d.jpg?1403708059";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/690/original/c02575e3556d6d475f6a35c724d2fbbc.jpg?1403708074";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3601-hd');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3611-hd', 'GXV3611_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1600x900', 'Dome', 'admin', 'admin', '', 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/691/original/4c966daa6ee3b16588c3d53d4dcc869f.jpg?1403708089";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/692/original/5c6f7f15b37c6af572a0fd457f4126b6.jpg?1403708104";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/693/original/5df5c7cc4e011142a0577a331ed35a15.jpg?1403708120";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3611-hd');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3615w', 'GXV3615W', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', 'NTSC:704x480/PAL:704x576', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/695/original/9c3cc68db9f535fd0a95e420a66ba9cd.jpg?1403708150";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3615');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3615', 'GXV3615', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', 'ntsc:704x480/pal:704x576', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/694/original/6f119a77fd514219d8ba31c368c14ece.jpg?1403708135";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3615');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3615wp-hd', 'GXV3615WP_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1600x1200', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/696/original/fc1d4b26e348a495f39e972b96d6071d.jpg?1403708183";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/697/original/c800fee943fee75942a3f7d0456d1aa2.jpg?1403708202";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3615wp_hd');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3662-fhd', 'GXV3662_FHD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/699/original/426f1666d08960098230ccff5f1990ee.jpg?1403708233";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/701/original/fff8a0771479ed48fb0c0e7a0bfdc84e.jpg?1403708263";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/702/original/d8224716f512d4f864d605f303665a76.jpg?1403708280";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/704/original/c1bf9eb97d6eff72758dc8f034f77295.jpg?1403708312";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3662');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3662-hd', 'GXV3662_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/698/original/a8cab5f5b39b65883d28366745bccc40.jpg?1403708218";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/700/original/98cfbfb79f06fd270527ae998dfd152a.jpg?1403708248";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/703/original/fedf64e0921c330abc319d15d31229d3.jpg?1403708297";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/705/original/8620c44c29969bf807ce22e369e20b73.jpg?1403708327";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3662');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3651-fhd', 'GXV3651_FHD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '2592x1944', 'Box', 'admin', 'admin', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/706/original/f3e169ffd76cdb0183c3db6ed5d5be4f.jpg?1403708342";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/707/original/178635d82712395561012ea289a563ba.jpg?1403708356";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3651');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3610-fhd', 'GXV3610_FHD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '2048x1536', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/709/original/9d2b2b7a8e1b9d20fe1efec2a5edd9d9.jpg?1403708400";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/710/original/bdc3eedee84a50b0c2d9cc190c745e74.jpg?1403708419";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/712/original/8403aa2ff9c02f90f3e9fb0bc3e3df26.jpg?1403708449";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3610');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3610-hd', 'GXV3610_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1280x960', 'Dome', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/708/original/dee421274a2fd763fa2c5760db820ef5.jpg?1403708384";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/711/original/2ff426a6e124b5adc6b1bbd2eae3a798.jpg?1403708434";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/713/original/7966705b10afd8510f4c306b4bdd6fb4.jpg?1403708465";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3610');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3674-fhd-vf', 'GXV3674_FHD_VF', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '2048x1536', 'Box', 'admin', 'admin', '', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/715/original/163770a771d7ecde6561546642ea7da6.jpg?1403708495";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/716/original/a70ce24c1c806c7355618afc34e62192.jpg?1403708510";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/719/original/f732da7ca35a759679ecaa307410fb0d.jpg?1403708556";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3674');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3674-hd-vf', 'GXV3674_HD_VF', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1280x960', 'Box', 'admin', 'admin', '', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/714/original/d643bab507b555b1acc7d389938dc3e1.jpg?1403708480";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/717/original/e3169ebc830987f505c30d4d6403bc3b.jpg?1403708526";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/718/original/af1e4542117952b00115a6dcbdb14332.jpg?1403708541";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3674');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3672-fhd-36', 'GXV3672_FHD_36', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '2048x1536', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/723/original/0c63d78a8dc6f97179047e8918a463cb.jpg?1403708633";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/729/original/a16c3c352ae886fb7b13b9ec5cbfd2b9.jpg?1403708727";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/721/original/7948ba11466e479b590a18ab64596b69.jpg?1403708602";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3672');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3672-hd-36', 'GXV3672_HD_36', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1280x960', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/722/original/a79218bdc3a936c8bfb7a672e3842ec2.jpg?1403708617";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/724/original/c3d229d88cf6433234eb974f26481e56.jpg?1403708648";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/730/original/1281a5a2f9d2976396145cfa04194f2c.jpg?1403708742";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3672');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3672-fhd', 'GXV3672_FHD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1920x1080', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/726/original/c22b8feda9ef4618c37a1db76474bf7e.jpg?1403708681";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/727/original/e618fa17c6e3de5a8115b4df189105b3.jpg?1403708695";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/720/original/46b9ba6bbd89978478060db19ef67c28.jpg?1403708586";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3672');
INSERT INTO "cameras" 
VALUES (NULL, 'gxv3672-hd', 'GXV3672_HD', '33', 'snapshot/view[CHANNEL].jpg', 'goform/stream?cmd=get&channel=[CHANNEL]', 'ipcam_h264.sdp', '1280x720', 'Bullet', 'admin', 'admin', '', 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/725/original/1e044c06616c54eebe6a4cd009232000.jpg?1403708664";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/728/original/5c3c5e95204f0fb7b59b1551da08320c.jpg?1403708711";}}', 'http://www.grandstream.com/index.php/products/ip-video-surveillance/gxv3672');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-hd-1080-2nd-gen', 'Bullet HD 1080 (2nd Gen)', '34', 'live/0/jpeg.jpg', 'live/0/mjpeg.jpg', 'live/0/h264.sdp', '1920x1080', 'Bullet', 'admin', '1234', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/634/original/7a250e30277c9edf72b0196344ed0c16.jpg?1402915676";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/635/original/6b355135a6213bf9358ea573aa76341b.jpg?1402915677";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/636/original/369b5b0659badc590ff86b8f7a0f8ea3.jpg?1402915678";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/637/original/31a328df80f8e442d90d2432f61eaa0b.jpg?1402915679";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/638/original/4e8c6ef1512015073de3215645ca6d83.jpg?1402915680";}}', 'http://www.y-cam.com/y-cam-bullet-hd-1080-2nd-gen/');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-hd-720-2nd-gen', 'Bullet HD 720 (2nd Gen)', '34', 'live/0/jpeg.jpg', 'live/0/mjpeg.jpg', 'live/0/h264.sdp', '1280x720', 'Bullet', 'admin', '1234', '', 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/629/original/09b9959b328894f4b9810002150498ec.jpg?1402915434";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/630/original/465756404e0ca8c5fb12541d2c51e945.jpg?1402915436";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/631/original/a60f989bd59b000b48de79e470517e5f.jpg?1402915437";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/632/original/903ee1ac9c0c84d43659667e000e199a.jpg?1402915439";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/633/original/74db6f1f504caf3e9ed0c6c4ff1b5138.jpg?1402915441";}}', 'http://www.y-cam.com/y-cam-bullet-hd-720-2nd-gen/');
INSERT INTO "cameras" 
VALUES (NULL, 'cube-hd-1080', 'Cube HD 1080', '34', 'live/0/jpeg.jpg', 'live/0/mjpeg.jpg', 'live/0/h264.sdp', '1920x1080', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/622/original/c8c7060663a4bd09902d80a08514c705.jpg?1402914855";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/623/original/3f93586d8b3a3f7051268898f0eb6220.jpg?1402914857";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/624/original/92a2eb9a0b820f7e3da8cb4d1929aef2.jpg?1402914858";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/625/original/c51a5b07021b7c656763740cd7ee26a0.jpg?1402914859";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/626/original/53be8c116cf253bd107c3c8a53af2e3a.jpg?1402914860";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/627/original/6f326e408592b9dfcd75b729b5c5a4c2.jpg?1402914862";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/628/original/f5549a64d43769017b4f2e5eeab82caa.jpg?1402914863";}}', 'http://www.y-cam.com/y-cam-cube-hd-1080/');
INSERT INTO "cameras" 
VALUES (NULL, 'cube-hd-720', 'Cube HD 720', '34', 'live/0/jpeg.jpg', 'live/0/mjpeg.jpg', 'live/0/h264.sdp', '1280x720', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/615/original/10dfd0407e8eb81975c830675464af46.jpg?1402914525";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/616/original/8ca9890056b9554f1001dae8b872ed70.jpg?1402914526";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/617/original/cf30102f1c6d9fa3305ed2e016bde37c.jpg?1402914528";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/618/original/4ff3feb670f1af61daddb98c33439633.jpg?1402914529";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/619/original/eff448ac727343fda754664bdf904539.jpg?1402914530";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/620/original/0c1e08937bbfaefe2d6013e512b6489d.jpg?1402914531";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/621/original/ff8b6972987ac37195d2f23677ce9d17.jpg?1402914532";}}', 'http://www.y-cam.com/y-cam-cube-hd-720/');
INSERT INTO "cameras" 
VALUES (NULL, 'cube', 'Cube', '34', 'live/0/jpeg.jpg', 'live/0/mjpeg.jpg', 'live/0/h264.sdp', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/608/original/3ab9e93366d704a830debbc1449ded79.jpg?1402913642";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/609/original/7802ced88173653844240c11bf060747.jpg?1402913643";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/610/original/ebb60f4f3c857c12967238f95ffba8e1.jpg?1402913644";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/611/original/edbc9cb27431b868f977d6fcbaee44cf.jpg?1402913645";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/612/original/2fb563b448bfe1d8ce79ca450230d23e.jpg?1402913646";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/613/original/40d104c64ef0e514ad9b3b685e73b749.jpg?1402913647";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/614/original/d163775b3832d9cd965e52715b65462b.jpg?1402913651";}}', 'http://www.y-cam.com/y-cam-cube/');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-hd-1080', 'Bullet HD 1080', '34', 'snapshot.jpg', 'cgi-bin/snapshot.fcgi', 'v01', '1920x1080', 'Bullet', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/600/original/56576c40c3f87839a8de1662f392fd89.jpg?1402909349";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/601/original/b575f1a1fb23c1be34d0451c1a08f338.jpg?1402909350";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/602/original/3410039503b45dc3801093125ed2c4bd.jpg?1402909351";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/603/original/60b1d18cbfb44c1ee7a1b0a331f63cc9.jpg?1402909352";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/604/original/5ea0ac36380e71fe5b11eb19b37ed1b5.jpg?1402909354";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/605/original/49440ad34fa77986830cc33e3fca4774.jpg?1402909355";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/606/original/9343127d70ae5535890f61e64e63e51b.jpg?1402909356";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/607/original/4240bc2b3b6431a8c282437d21dae703.jpg?1402909358";}}', 'http://www.y-cam.com/y-cam-bullet-hd-1080/');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-hd-720', 'Bullet HD 720', '34', 'snapshot.jpg', 'stream.jpg', 'live_h264.sdp', '1280x720', 'Bullet', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/595/original/2c09a3bd79f39df034a25483b2d0e83a.jpg?1402908851";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/596/original/9d31eda2c44f430722aff23a8b59a050.jpg?1402908853";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/597/original/45dcfe2ada5b2e369d2e9a1030fd60b9.jpg?1402908855";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/598/original/bb448a81a558f3ab5fdae6538ec0c277.jpg?1402908856";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/599/original/97efc02f30377cca707ee05ee0ebdfb2.jpg?1402908859";}}', 'http://www.y-cam.com/y-cam-bullet-hd/');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8264fwd-ei-z', 'DS-2CD8264FWD-EI(Z', '37', '', '', '', '', 'Other', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/205/original/895a4e1205db16051b05b0ed81bd28f5.jpg?1399905716";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/206/original/be44bd7473ca007c10027a53b175a797.jpg?1399905717";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/207/original/cb3767facfd856c73d984f302eb9e269.jpg?1399905718";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'knight-sd', 'Knight SD', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/584/original/6fe2c41345e32a8bf77257c6a715f818.jpg?1401717366";}}', 'http://www.y-cam.com/y-cam-knight-sd/');
INSERT INTO "cameras" 
VALUES (NULL, 'knight-s', 'Knight S', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/583/original/a7fb21b3675fa1ac8f79c16bb784de25.jpg?1401717177";}}', 'http://www.y-cam.com/y-cam-knight-s/');
INSERT INTO "cameras" 
VALUES (NULL, 'black-s', 'Black S', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/582/original/05912abe2fcdb0ed951a777d8e29a732.jpg?1401716990";}}', 'http://www.y-cam.com/y-cam-black-s/');
INSERT INTO "cameras" 
VALUES (NULL, 'white-s', 'White S', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/581/original/9e02580723c5dabb54ca99d9973d8dbf.jpg?1401716871";}}', 'http://www.y-cam.com/y-cam-white-s/');
INSERT INTO "cameras" 
VALUES (NULL, 'white-sd', 'White SD', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/580/original/5987485e1eaa5211ae0b3df1d8b95fed.jpg?1401716719";}}', 'http://www.y-cam.com/y-cam-white-sd/');
INSERT INTO "cameras" 
VALUES (NULL, 'black-sd', 'Black SD', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/579/original/83db9846d34eb74e7c675c4afd16e66c.jpg?1401716574";}}', 'http://www.y-cam.com/y-cam-black-sd/');
INSERT INTO "cameras" 
VALUES (NULL, 'eyeball', 'EyeBall', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/578/original/6d96a72d55e02621074210490fd7b4a4.jpg?1401716318";}}', 'http://www.y-cam.com/y-cam-eyeball/');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-white', 'Bullet White', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/577/original/afa13368a515268b40227ee7c8be79cb.jpg?1401716090";}}', 'http://www.y-cam.com/y-cam-bullet-white/');
INSERT INTO "cameras" 
VALUES (NULL, 'bullet-black', 'Bullet Black', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/576/original/c0bfab1c2747eef39ec26582f565682e.jpg?1401715893";}}', 'http://www.y-cam.com/y-cam-bullet-white/');
INSERT INTO "cameras" 
VALUES (NULL, 'black-sd-poe', 'Black SD POE', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/575/original/e736c16e8d3f93dfd8f688c1c6ac6cce.jpg?1401715497";}}', 'http://www.y-cam.com/y-cam-black-sd-poe/');
INSERT INTO "cameras" 
VALUES (NULL, 'white-s-poe', 'White S POE', '34', 'snapshot.jpg', 'stream.jpg', '', '640x480', 'Box', 'admin', '1234', '', 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/574/original/c0d562a2b4e5dd424207886edaf0e836.jpg?1401715137";}}', 'http://www.y-cam.com/y-cam-white-s-poe/');
INSERT INTO "cameras" 
VALUES (NULL, '207', '207', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/572/original/9b5954d172ed57e715b2143aeb699be0.jpg?1401712227";}}', 'http://www.axis.com/techsup/cam_servers/cam_207/');
INSERT INTO "cameras" 
VALUES (NULL, 'm1031-w', 'M1031-W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Box', 'root', 'pass', '', 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/571/original/12c8a3baadf4f41e4d9238fd7c66cd48.jpg?1401712032";}}', 'http://www.axis.com/products/cam_m1031w/');
INSERT INTO "cameras" 
VALUES (NULL, '207w', '207W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/570/original/1dfbc0879050f756192ecd08e7b8a4eb.jpg?1401711709";}}', 'http://www.axis.com/products/cam_207w/');
INSERT INTO "cameras" 
VALUES (NULL, '207mw', '207MW', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x1024', 'Box', 'root', 'pass', '', 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/569/original/7c0d1ceb07eccca4669ca59344ab314b.jpg?1401711549";}}', 'http://www.axis.com/products/cam_207mw/');
INSERT INTO "cameras" 
VALUES (NULL, 'm1011', 'M1011', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/568/original/afcc768b3bf6f7790fba7f826c0aa17c.jpg?1401711247";}}', 'http://www.axis.com/products/cam_m1011/');
INSERT INTO "cameras" 
VALUES (NULL, '213-ptz', '213 PTZ', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '704x576', 'Box', 'root', 'pass', '', 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/566/original/201398d60709aafd8392dd9482ecd628.jpg?1401710053";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/567/original/9c55452ee28d154a210654b688f12247.jpg?1401710054";}}', 'http://www.axis.com/products/cam_213/');
INSERT INTO "cameras" 
VALUES (NULL, '205', '205', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/565/original/30650497870715014fd9a79d52e8a659.jpg?1401708706";}}', 'http://www.axis.com/products/cam_205/');
INSERT INTO "cameras" 
VALUES (NULL, '210a', '210A', '35', 'axis-cgi/jpg/image.cgi', '', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/564/original/8c86e63aed0bf1aa72e4e653d43c079c.jpg?1401707807";}}', 'http://www.axis.com/en/products/cam_210a/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '233d', '233D', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', 'NTSC:704x480/PAL:704x576', 'Dome', 'root', 'pass', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/563/original/0d2b8b77bfee2589e66f94c2c2a6d025.jpg?1401707405";}}', 'http://www.axis.com/en/products/cam_233d/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '221', '221', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/562/original/bdac16b0a21a30def69f382e0d400643.jpg?1401706853";}}', 'http://www.axis.com/products/cam_221/');
INSERT INTO "cameras" 
VALUES (NULL, '211w', '211W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Box', 'root', 'pass', '', 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/561/original/a86860b333142f30ab5c6953263028be.jpg?1401705736";}}', 'http://www.axis.com/products/cam_211w/');
INSERT INTO "cameras" 
VALUES (NULL, 'm1025', 'M1025', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/560/original/6594d69d933bb3fc59703397e3ec26f9.jpg?1401705063";}}', 'http://www.axis.com/products/cam_m1025/');
INSERT INTO "cameras" 
VALUES (NULL, '215-ptz-e', '215 PTZ-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', 'NTSC:704x480/PAL:704x576', 'Dome', 'root', 'pass', '', 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/559/original/d50fb2ffb93de323c616bf03821a457b.jpg?1401703870";}}', 'http://www.axis.com/en/products/cam_215e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '215-ptz', '215 PTZ', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', 'NTSC:704x480/PAL:704x576', 'Dome', 'root', 'pass', '', 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/558/original/ece85870fa7604772b43452364d996a5.jpg?1401703573";}}', 'http://www.axis.com/products/cam_215/');
INSERT INTO "cameras" 
VALUES (NULL, '216mfd', '216MFD', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '1280x1024', 'Dome', 'root', 'pass', '', 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/557/original/72354fa8c5830670b03f577b5559ee8b.jpg?1401701129";}}', 'http://www.axis.com/en/products/cam_216mfd/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '216fd', '216FD', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Dome', 'root', 'pass', '', 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/556/original/ea83a25a9a860de0275ea3a9f684674e.jpg?1401700336";}}', 'http://www.axis.com/en/products/cam_216/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '209fd', '209FD', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', '', '640x480', 'Dome', 'root', 'pass', '', 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/555/original/822e783f3f67a9ee8b4ee4f1921e5eed.jpg?1401699459";}}', 'http://www.axis.com/products/cam_209fd/');
INSERT INTO "cameras" 
VALUES (NULL, 'q1931-e', 'Q1931-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '384x288', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1931e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1922-e', 'Q1922-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/445/original/c0252df21fdd6aadb7084fd8465d5837.jpg?1401191582";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/450/original/4bd47118a43bbae680b65247a861d063.jpg?1401191659";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/197/original/e9f361c5b3a55625bcf824e117590b14.jpg?1402995811";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/264/original/48000a75cb29d7fb3385f1cd4c7a548c.jpg?1402995946";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/339/original/8eb36ca7a512ed95a09313cad2907cbf.jpg?1402996117";}}', 'http://www.axis.com/products/cam_q1922e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1922', 'Q1922', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:12:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/438/original/5a1a09d3303d837d81795eb6253d2e40.jpg?1401191454";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/477/original/54b863c52ac3eb43b5dceb0463e333db.jpg?1401191945";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/521/original/f21ece7d01e54e78ec93ed903a18c3b0.jpg?1401192386";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/528/original/5ce978b8f48222da1bd432d5cb157875.jpg?1401192425";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/542/original/82490cb891c6d2ea09bd4e39012a7d2b.jpg?1401192516";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/549/original/9c401a7275fc6ef678678c844fc360e2.jpg?1401192547";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/209/original/9476b2c68959584203ec352231d54a4f.jpg?1402995828";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/270/original/1b73bc80813c3fa5b0c68b59a018089a.jpg?1402995955";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/299/original/d93324f2afdb2b3ca98d8786b102a233.jpg?1402996040";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/314/original/e570288b8c23810a52db6336639fb698.jpg?1402996083";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/352/original/83b83f39f02b4a6539c012c264144f51.jpg?1402996135";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/374/original/2fcf1e3a4e1d8869f2342254661067f9.jpg?1402996165";}}', 'http://www.axis.com/products/cam_q1922/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1921-e', 'Q1921-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '384x288', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/188/original/63037604eeb678bdd85a6e2716bfaef1.jpg?1402995798";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/273/original/17eb0d4d1f81b8ecdc45747d1d47a1e7.jpg?1402995960";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/297/original/145f332c3ffddb74fa39bef3b72afb6c.jpg?1402996037";}}', 'http://www.axis.com/products/cam_q1921e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1921', 'Q1921', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '384x288', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/449/original/54ef4712a4da9fb3ccab78fd4bbca19b.jpg?1401191635";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/482/original/430c46731b43ca39907faea5e950e53c.jpg?1401191989";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/512/original/3ca92de49af36ad1b54a6cf335f4d8ae.jpg?1401192256";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/523/original/661810bb9fdc5f377b55520a011cf953.jpg?1401192414";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/141/original/396feeb9c47065fb68edbb8896555bc0.jpg?1402995732";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/244/original/1c23727279e0c7c2f4c5a4204a2e390a.jpg?1402995919";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/340/original/d9ce4607d521310b759b1c099a5bd22b.jpg?1402996118";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/348/original/88d6cc2bf6c0764d5dbcfefab3351810.jpg?1402996129";}}', 'http://www.axis.com/products/cam_q1921/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1910-e', 'Q1910-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '160x128', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/415/original/a15b81a9727d5b30d7e8df2727d6079b.jpg?1401191277";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/151/original/dbca7b24b05b8f0568ad1f4d2e3ded76.jpg?1402995747";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/212/original/f32523ac307af62b805222db3c4b7d33.jpg?1402995832";}}', 'http://www.axis.com/products/cam_q1910e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1910', 'Q1910', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '160x128', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/447/original/50b430e6cc1512971558b3bbea8ed17e.jpg?1401191608";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/500/original/4e158a69f5ba9ad88e4d752260632228.jpg?1401192162";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/517/original/e554873d92b01efab08ef30dfb26c4c0.jpg?1401192308";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/156/original/af5a0c8118dd2b9866d12245069ab3b9.jpg?1402995754";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/206/original/55a18445b30533088567e76c24891dd5.jpg?1402995824";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/369/original/da8fe60ea09e236b46301e39cba32193.jpg?1402996158";}}', 'http://www.axis.com/products/cam_q1910/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q8722-e', 'Q8722-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q8722-E');
INSERT INTO "cameras" 
VALUES (NULL, 'q8721-e', 'Q8721-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q8721-E');
INSERT INTO "cameras" 
VALUES (NULL, 'q6045-s', 'Q6045-S', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6045-S');
INSERT INTO "cameras" 
VALUES (NULL, 'q6045-c', 'Q6045-C', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6045-C');
INSERT INTO "cameras" 
VALUES (NULL, 'q6045-e', 'Q6045-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6045-E');
INSERT INTO "cameras" 
VALUES (NULL, 'q6045', 'Q6045', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6045');
INSERT INTO "cameras" 
VALUES (NULL, 'q6044-s', 'Q6044-S', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6044-S');
INSERT INTO "cameras" 
VALUES (NULL, 'q6044-c', 'Q6044-C', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6044-C');
INSERT INTO "cameras" 
VALUES (NULL, 'q6044-e', 'Q6044-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6044-E');
INSERT INTO "cameras" 
VALUES (NULL, 'q6044', 'Q6044', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6044');
INSERT INTO "cameras" 
VALUES (NULL, 'q6042-s', 'Q6042-S', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '736x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6042-S');
INSERT INTO "cameras" 
VALUES (NULL, 'q6042-c', 'Q6042-C', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '736x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:0:{}', 'Q6042-C');
INSERT INTO "cameras" 
VALUES (NULL, 'q6042-e', 'Q6042-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '736x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6042-E');
INSERT INTO "cameras" 
VALUES (NULL, 'q6042', 'Q6042', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '736x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'Q6042');
INSERT INTO "cameras" 
VALUES (NULL, 'p5544', 'P5544', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5544');
INSERT INTO "cameras" 
VALUES (NULL, 'p5534-e', 'P5534-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5534-E');
INSERT INTO "cameras" 
VALUES (NULL, 'p5534', 'P5534', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5534');
INSERT INTO "cameras" 
VALUES (NULL, 'p5532-e', 'P5532-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '720x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5532-E');
INSERT INTO "cameras" 
VALUES (NULL, 'p5532', 'P5532', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '720x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5532');
INSERT INTO "cameras" 
VALUES (NULL, 'p5522-e', 'P5522-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '720x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5522-E');
INSERT INTO "cameras" 
VALUES (NULL, 'p5522', 'P5522', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '720x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5522');
INSERT INTO "cameras" 
VALUES (NULL, 'p5512-e', 'P5512-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '704x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5512-E');
INSERT INTO "cameras" 
VALUES (NULL, 'p5512', 'P5512', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '704x576', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5512');
INSERT INTO "cameras" 
VALUES (NULL, 'p5415-e', 'P5415-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5415-E');
INSERT INTO "cameras" 
VALUES (NULL, 'p5414-e', 'P5414-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'P5414-E');
INSERT INTO "cameras" 
VALUES (NULL, '214-ptz', '214 PTZ', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '704x576', 'Dome', 'root', 'pass', '', 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/821/original/5833d95b1650aab57abd604714a463c8.jpg?1409650794";}}', '214 PTZ');
INSERT INTO "cameras" 
VALUES (NULL, 'm5014-v', 'M5014-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'M5014-V');
INSERT INTO "cameras" 
VALUES (NULL, 'm5014', 'M5014', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'M5014');
INSERT INTO "cameras" 
VALUES (NULL, 'm5013-v', 'M5013-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'M5013-V');
INSERT INTO "cameras" 
VALUES (NULL, 'm5013', 'M5013', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'M5013');
INSERT INTO "cameras" 
VALUES (NULL, '212-ptz-v', '212 PTZ-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/820/original/92487bfe58f9ed3196095ae77e6f7774.jpg?1409650720";}}', 'http://www.axis.com/products/cam_212v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, '212-ptz', '212 PTZ', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Dome', 'root', 'pass', '', 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:1:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/002/819/original/2e377dbdc0e74178c231db31ae45c4e0.jpg?1409650620";}}', 'http://www.axis.com/products/cam_212/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3367-ve', 'P3367-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:15:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/422/original/6dabee3ec534ad8ddd5a0261967a72cb.jpg?1401191334";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/424/original/200f096e6ecddf60e08dffd02baed47b.jpg?1401191337";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/439/original/3965fb4a3459c2814788779eb6d0b9a3.jpg?1401191477";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/475/original/72d6ceb0c19537c215179fafce5e9f7c.jpg?1401191941";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/505/original/b5eb7b0cd42bd2831cc9ca96e4a9f7c5.jpg?1401192186";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/509/original/5b40f7cdf1b4709045bd7cb6b1fd36f8.jpg?1401192226";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/167/original/f7afe74d2173f885c76dff0f00707fc6.jpg?1402995770";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/183/original/e2d034b463d15c7dcba0a849232e89fe.jpg?1402995791";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/210/original/5e54c2c6ff5524a1c2686fa38878a558.jpg?1402995829";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/234/original/cf94ce23b4a91988445d17cef23bef90.jpg?1402995884";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/269/original/4e8eb5ba98d167a4a5239c504dc1f438.jpg?1402995953";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/305/original/d7fda61aea0f1a4d6342f737451a78c7.jpg?1402996049";}i:12;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/325/original/69a6edb05f6a882a87db8f0532bab5ff.jpg?1402996098";}i:13;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/370/original/57970463ebd58cb09822147f7dbda037.jpg?1402996160";}i:14;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/377/original/a3b4d528b5f9b236cdbf6624f1bdc03d.jpg?1402996170";}}', 'http://www.axis.com/products/cam_p3367ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3367-v', 'P3367-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/446/original/843d86faf6006b096ce55635631619cb.jpg?1401191606";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/483/original/bd9e8452b3461cf94e064ce4c8c9d0d5.jpg?1401192012";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/137/original/a8b7d8552a2a7ef036364245a34bb596.jpg?1402995727";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/189/original/1c563060ff30b4ed4ed4efd2ffde7249.jpg?1402995800";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/193/original/3044b8fc3cb30ef564c9b8bc599a9c3f.jpg?1402995806";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/240/original/83bdd07923140bba348c36d07ad148fc.jpg?1402995914";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/248/original/68bed36afb7e23e5d99fdebdb58b0548.jpg?1402995925";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/382/original/22f860bb23c52dda1d0570ebcb6c5807.jpg?1402996178";}}', 'http://www.axis.com/products/cam_p3367v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3346-ve', 'P3346-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2048x1536', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/444/original/d6d3d487294f87ac04ce7d899493be8f.jpg?1401191580";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/136/original/eda531b19fe66b606881fe19b5e308a9.jpg?1402995725";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/175/original/214ee599e1f534e54932ad17b2cfe3b3.jpg?1402995780";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/187/original/55bd7e56fb9a5ab2b492e287f5b29cda.jpg?1402995797";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/316/original/149cc6980ae24eedaf409937e9fa4029.jpg?1402996086";}}', 'http://www.axis.com/products/cam_p3346ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3346-v', 'P3346-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2048x1536', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/501/original/f79018117f7898eea23dae971569a722.jpg?1401192166";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/181/original/56d0e360c5e835a3cc4eba00b8cdbc9c.jpg?1402995789";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/207/original/50e7b78a946a306cfe14eef9fde608b9.jpg?1402995825";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/300/original/fcf09b92fe96531f410be3fdc9da3e04.jpg?1402996042";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/344/original/efe629007135df926aca82c2ff6d7bd4.jpg?1402996124";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/365/original/c00ed0660d2dc70c6e09fc3de711ce62.jpg?1402996153";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/376/original/76e2cc668f5bbfc11942da65616c031f.jpg?1402996168";}}', 'http://www.axis.com/products/cam_p3346v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3346', 'P3346', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2048x1536', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/488/original/a5bb8cccc6860c8839de2b04b8df35dc.jpg?1401192064";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/162/original/494e7ee27a65041aea8afb7bc533f37b.jpg?1402995763";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/185/original/45ec52ebb68cbe6b99d01db86668f900.jpg?1402995794";}}', 'http://www.axis.com/products/cam_p3346/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3384-ve', 'P3384-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/420/original/ef3fa9d5674b63e35917b55121784b48.jpg?1401191327";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/425/original/e5bb6b3a79ad4dd42fa0627573de8bb0.jpg?1401191360";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/139/original/204f2ebcf8a0916cb2e8f190ac9515f1.jpg?1402995729";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/202/original/02304068a45895856249c77b569bf7d8.jpg?1402995818";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/204/original/ad47d4a81bf1178bec2a7e9632720088.jpg?1402995821";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/229/original/4cbec97ef8673025bf95dd15cf059586.jpg?1402995855";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/285/original/3b9a6028940df5162dbf3221e2bb5e28.jpg?1402995999";}}', 'http://www.axis.com/products/cam_p3384ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3384-v', 'P3384-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/430/original/a28c0e42d309eff3b9268b5e3f6b9ee8.jpg?1401191416";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/451/original/48f4a84e4e348a0d712a5c92fd6a8480.jpg?1401191682";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/160/original/ff6d84141559ac60c9ba1fedab8fcad5.jpg?1402995760";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/169/original/25b8ef6145c5a408b769c7ea0aea9ede.jpg?1402995773";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/180/original/b66a62b4b258aed040b1f2e44ab99d85.jpg?1402995787";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/231/original/0ec4da1d04b3dde075b59249ccca4d71.jpg?1402995879";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/255/original/1a9e3d7c8cb0608cd3c388d8cfa881d3.jpg?1402995934";}}', 'http://www.axis.com/products/cam_p3384v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3364-ve', 'P3364-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/199/original/ee5c863164279796b388e39d60f02e1f.jpg?1402995814";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/250/original/a3d0a5476f008f323c6b200a5ef8542e.jpg?1402995927";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/302/original/d6cac6ddbdd762351fa36bffcd378ec1.jpg?1402996044";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/353/original/ac3b468a7a82ce58fd482c0ed3b359a7.jpg?1402996136";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/357/original/be9d5b3dc53f2eb4487b3911331cdd45.jpg?1402996142";}}', 'http://www.axis.com/products/cam_p3364ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3364-v', 'P3364-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/502/original/003ba516d39ec8d1b4837cd4d0167a82.jpg?1401192173";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/194/original/d9a95bb493216ed4f9f519268b30c5fc.jpg?1402995807";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/219/original/267ac3c5c34fe92daf904bf7b2c8886a.jpg?1402995841";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/220/original/52535f2bf815a9e537efda9c5dff5a8e.jpg?1402995843";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/247/original/290248df65edd9e9786004dd56189c26.jpg?1402995923";}}', 'http://www.axis.com/products/cam_p3364v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3354', 'P3354', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/468/original/96a9ed7537c95ffa9b93764f12a5f615.jpg?1401191865";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/498/original/b6177939dc4d413fca3c14c59ca92572.jpg?1401192151";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/235/original/23bf3ec3f631ce26d0e2466a13b02121.jpg?1402995885";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/237/original/687fe3ead41c58a1627ec894697f1c9e.jpg?1402995888";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/249/original/ab9ee3dcc233d2837b6deb013cc95a22.jpg?1402995926";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/287/original/1eeb192db2bc0681dffc0f1b30a5b4c2.jpg?1402996002";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/288/original/49a8fa1fc5e531783d8d190b2d4fedfe.jpg?1402996003";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/291/original/4587976e9bf02ec921d0c13bdcfd8420.jpg?1402996008";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/324/original/9eef32b3dbdbc74472789a0d831200be.jpg?1402996097";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/334/original/3a914bb7a06476f8181a123d57616666.jpg?1402996110";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/356/original/f4ad6096457b7e82014ad46433009a51.jpg?1402996140";}}', 'http://www.axis.com/products/cam_p3354/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3363-ve', 'P3363-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:9:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/414/original/e5dd0327fcd54d19e7daf099e78a2804.jpg?1401191274";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/469/original/639077b407082133db7d27ce925be7d8.jpg?1401191866";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/485/original/289bd2b9c940cd57e1b91a4ebfe647ce.jpg?1401192016";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/513/original/7785ac8d39e0d6d598b7bd115763297f.jpg?1401192258";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/142/original/898f656e043c86ad31305868a6fb4b33.jpg?1402995734";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/159/original/7147f8455c4adf617a7476cca4f5262d.jpg?1402995759";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/261/original/7c58ea23d8c555dd1346b3ff203f72b7.jpg?1402995942";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/329/original/495df917ec58e205c12d5b49f46418ed.jpg?1402996103";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/336/original/9ab51ab6832e3b2fdc529e52ca865122.jpg?1402996113";}}', 'http://www.axis.com/products/cam_p3363ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3363-v', 'P3363-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/419/original/5619e944d581470df400f6680175d246.jpg?1401191303";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/458/original/a1e9098069115f5fabe75df7593ed1c6.jpg?1401191717";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/525/original/48688c12f966b11bf55ceb0ac7beb34a.jpg?1401192417";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/184/original/e67f7945bb610a5cc6c0b6e1f995c64f.jpg?1402995793";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/213/original/afd7e2c0e43012f573073a7f3e24eefb.jpg?1402995833";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/286/original/3ad7a5f58b941d0c2ffe7729112a1ae5.jpg?1402996000";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/343/original/d99a9e6df71f2e331d26e0cb2e78af06.jpg?1402996122";}}', 'http://www.axis.com/products/cam_p3363v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3353', 'P3353', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:16:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/421/original/8d339455b1a828a3059b309c743c6fff.jpg?1401191332";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/464/original/324c71d56f9d2c1b67c18744bb7ccb5a.jpg?1401191791";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/465/original/177ba0409a9ff2b474fad62bbbddfec5.jpg?1401191814";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/471/original/f7d0d29b299e88f3276ad09ed3c2a2b2.jpg?1401191890";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/508/original/2bd685f6da6d1fbbe638d126909c71c5.jpg?1401192223";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/524/original/c3331c36d4526cdb81d53da3072a7208.jpg?1401192415";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/537/original/08c8ca0c3a83cb54c5fe1ed9f9042ab2.jpg?1401192489";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/163/original/557ab8d46ad83cbfafc1c6bc94e38963.jpg?1402995764";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/172/original/fa73ddb55bf183ba3f22dd31214abe0b.jpg?1402995776";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/218/original/00b275dbef56047a0546b21e6b1e1b38.jpg?1402995840";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/241/original/2ba98f6fbb41a99e00cf82836bcdc65a.jpg?1402995915";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/259/original/523d61a31290436047f9cf9424abfbeb.jpg?1402995939";}i:12;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/263/original/8470472332c7c272c51550e09350aea6.jpg?1402995945";}i:13;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/267/original/3b81617e8c9f5e9db2caed45d1b49ba8.jpg?1402995950";}i:14;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/323/original/3b4c3952e0868217e0e46b55b5247af6.jpg?1402996095";}i:15;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/342/original/2f02ee6b406e7e9d447fa92cebea4654.jpg?1402996121";}}', 'http://www.axis.com/products/cam_p3353/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p3304-v', 'P3304-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'p3304', 'P3304', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'p3301-v', 'P3301-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'p3301', 'P3301', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '640x480', 'Dome', 'root', 'pass', '', 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'm3204-v', 'M3204-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/434/original/468c5a8018bf54b0d8c34a05380eb4f9.jpg?1401191424";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/452/original/ba91a540a80ebdf5422269a7b78a55b5.jpg?1401191704";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/216/original/41e01715eb3c2ccb001607d86787e586.jpg?1402995837";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/366/original/931837a381c7f360fd2e185590dc1626.jpg?1402996154";}}', 'http://www.axis.com/products/cam_m3204v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3204', 'M3204', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/428/original/874157552d162481c28f81d54a81bf4e.jpg?1401191390";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/496/original/39c28e41b3c8ef9eb330891a084a5019.jpg?1401192149";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/239/original/5bf55bf3696303438a07bf35b25724b6.jpg?1402995913";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/337/original/b2d7d8d5da19a9603cc62b53924c5d0f.jpg?1402996114";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/354/original/2d47fc38ea63612e2bc703c0b7a5ec7e.jpg?1402996138";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/379/original/7fbd4a52f0d685ec19fa200809a486c0.jpg?1402996173";}}', 'http://www.axis.com/products/cam_m3204/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3203-v', 'M3203-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/282/original/2ad6f54e3f0ea1e87898b421142cb27a.jpg?1402995994";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/378/original/4d9ecd8b49b6259380bd88a957276f18.jpg?1402996171";}}', 'http://www.axis.com/products/cam_m3203v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3203', 'M3203', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/448/original/2fd28617a0c37612855375500e90ef8d.jpg?1401191632";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/519/original/56cf3648893c5a2211191bf13cd3ac01.jpg?1401192341";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/224/original/f72424cffc18d45d40a4cd1df74e7178.jpg?1402995848";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/251/original/64265f51a2f3e16ddde509f06f0679a2.jpg?1402995929";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/351/original/bcc746eb3800186de4a87fa486f8ddc6.jpg?1402996134";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/364/original/fe6340aa677fcbdbb3f426d4d85cf59e.jpg?1402996152";}}', 'http://www.axis.com/products/cam_m3203/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3114-ve', 'M3114-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/480/original/00902b1d811ddb91baed7d94b62a6220.jpg?1401191982";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/494/original/73cb040cadda838cdd65630e9a3187b9.jpg?1401192142";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/174/original/0192467f2ad263a95c2eae514b668b40.jpg?1402995779";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/230/original/da302548ff40adaa9c87e01c07e0cff1.jpg?1402995856";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/280/original/b813a54577ed1dbb6fb412fe73e9f8a2.jpg?1402995992";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/341/original/ea4f5f3a3b013d65a1b9490415963a71.jpg?1402996120";}}', 'http://www.axis.com/products/cam_m3114ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3113-ve', 'M3113-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/441/original/b6633a2ca433c4ef0f83e54c136647d4.jpg?1401191502";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/453/original/562c681636bb59790e6f5c432230832d.jpg?1401191706";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/165/original/1171f9fb611ccee64037a510ada920db.jpg?1402995767";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/214/original/3079e56dae452b4a6103791a28a14b53.jpg?1402995835";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/271/original/1ca0b551c1771366c38e2ca8292b6e10.jpg?1402995956";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/315/original/037bf870d464696a1987514ae828812e.jpg?1402996084";}}', 'http://www.axis.com/products/cam_m3113ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3114-r', 'M3114-R', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/499/original/8e3a1b8bb45ff8d109f5a014d7445e20.jpg?1401192158";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/278/original/45741a2768fe8349ddca3fcc625e3fc7.jpg?1402995989";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/319/original/ca40de7e0ad077b194781804b24765d8.jpg?1402996090";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/355/original/a7a773d5858daf7de1a9eb55f2edfba0.jpg?1402996139";}}', 'http://www.axis.com/products/cam_m3114r/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3113-r', 'M3113-R', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/479/original/f5552bef9c785f6a29489ac949911aef.jpg?1401191978";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/533/original/dadcbe2b5b78da8a0ce4f51a4cd546d2.jpg?1401192476";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/265/original/0ab8018bcc4e82e8d2b57847824e8928.jpg?1402995948";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/367/original/8dead8d1b05207dc17f1d74e964e5787.jpg?1402996156";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/383/original/9eac27a8ab2cb6d3598a266f0907c4fb.jpg?1402996180";}}', 'http://www.axis.com/products/cam_m3113r/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3014', 'M3014', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/541/original/533da056b0c3d066383dfb6c1c9b315b.jpg?1401192515";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/153/original/cf4df1b651f1c3be332b45db73bcab49.jpg?1402995750";}}', 'http://www.axis.com/products/cam_m3014/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3027-pve', 'M3027-PVE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/518/original/b91d57ed4b11708339b784acd4605860.jpg?1401192339";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/520/original/fb3af0d2deef0b87853793af140eb290.jpg?1401192364";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/295/original/531acafe58871cc7ac51137ca992e0fa.jpg?1402996035";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/298/original/4cc9d4dbf32c4e7581d27204c55f6def.jpg?1402996039";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/317/original/eed17cdd88117c511a405012ba88c0ad.jpg?1402996087";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/373/original/fcb1f6e910022f5baf248150ca49cd5e.jpg?1402996164";}}', 'http://www.axis.com/products/cam_m3027pve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3026-ve', 'M3026-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2048x1536', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/418/original/249825e5bedf8746d168d9aaad77342e.jpg?1401191302";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/435/original/474609ee3a7bd39c3e55540d15dd2354.jpg?1401191425";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/456/original/67d6a6d670fdfd81c98bdd5d1bf6a150.jpg?1401191713";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/529/original/c599bb4ee93e80b23165a04ec3c40ae0.jpg?1401192456";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/546/original/970901f55b0fc6a04c0024f3dbbff22b.jpg?1401192532";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/145/original/157d101daa2de67b8a340f660c72a537.jpg?1402995738";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/233/original/0528880f55955265bfded47b499e0760.jpg?1402995882";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/266/original/c9da59c3cbcf805d1845d8638c133a44.jpg?1402995949";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/275/original/990bd3e1673f8641ef8a3eae5767f1bc.jpg?1402995963";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/301/original/b9dddd41f0f34017157af2d1f3f995ca.jpg?1402996043";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/375/original/82886629b171b1bfc973bec1af3b75a2.jpg?1402996167";}}', 'http://www.axis.com/products/cam_m3026ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3025-ve', 'M3025-VE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/416/original/cc88cb575c0ee462128179af1fe7a995.jpg?1401191299";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/486/original/042af39d3baf9724e3b4feaa2f8f5abf.jpg?1401192019";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/516/original/46419d35e135c43ad4ecc761345129c4.jpg?1401192307";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/532/original/dcd489ff2e2d9bec5b26f9bafde04ee5.jpg?1401192471";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/534/original/dc593fcb26b22a75b5572289f3438c4f.jpg?1401192478";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/140/original/ea5e7b07282631d3d910a6d0b8daf68d.jpg?1402995731";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/158/original/03d46a0f6095697a809d1a1c7586b3aa.jpg?1402995757";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/176/original/6ac199d3f7a2a92b46b29a5eae613010.jpg?1402995782";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/205/original/4a46e91b5e404aa36bc211dc1b6a6d9f.jpg?1402995822";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/215/original/0a3a85a42fd9c60a4f9c7ee22becd4d3.jpg?1402995836";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/312/original/9a436d768f6137f7740d17fe28116d21.jpg?1402996059";}}', 'http://www.axis.com/products/cam_m3025ve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3024-lve', 'M3024-LVE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:10:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/427/original/fea6b302a4b3923aabb7f541f29b481b.jpg?1401191386";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/454/original/735fccc360ec4bcaa6fb2a3bea58d0b2.jpg?1401191707";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/467/original/4c9ff9448d02cc5061fa9ac3b1d191f4.jpg?1401191838";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/544/original/ebd21fc20f8a644bf849d232f426257f.jpg?1401192523";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/149/original/e843b48729bf67b16ceff0b6c6025aac.jpg?1402995744";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/173/original/5f01376e258e6217b72f256bd4132da3.jpg?1402995778";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/200/original/08ae154ea0c52c08b67c8f07b421c8f7.jpg?1402995815";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/201/original/073b6652da7d63027426a0d9698d9066.jpg?1402995817";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/222/original/6460d7d2009bd77a6b872a58815f6405.jpg?1402995845";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/327/original/9e11f41589207034fcd71a98d8218bda.jpg?1402996101";}}', 'http://www.axis.com/products/cam_m3024lve/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3007-pv', 'M3007-PV', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:12:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/442/original/d87bacc2f747438f0f284987d4d16930.jpg?1401191525";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/457/original/d93e0ea99f53dd7412e2cdbf85708e3f.jpg?1401191714";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/474/original/74927bce380fea46076869fd2d4e463f.jpg?1401191938";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/510/original/41b1d6abc64778b7e958b7a6622cbe3d.jpg?1401192253";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/547/original/eecc2bee478e0a930ebd18efccf02a9d.jpg?1401192537";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/144/original/c2a0b9b09fb938701fb4b7c906bd0f92.jpg?1402995737";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/152/original/b19c43a4e3d1ecffebedb16d22d3008b.jpg?1402995749";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/179/original/c0fe8a67f5eab3df73807f275cc2fd29.jpg?1402995786";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/292/original/6b0c21b000f4752b84e2c3e002d04f7c.jpg?1402996009";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/304/original/bc1483bab9606369c6d72dbfc9ef7b5d.jpg?1402996047";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/333/original/b1a1a74ab54e71e824da853d0a8d5f5b.jpg?1402996109";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/346/original/66ef32a234b7affa96fb17ed1cbd2f4f.jpg?1402996127";}}', 'http://www.axis.com/products/cam_m3007pv/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3007-p', 'M3007-P', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/473/original/10599866a5c82e1d45f2e2e54a84b0c8.jpg?1401191916";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/539/original/a87087094a904526618a41ffbd8b8e0f.jpg?1401192501";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/221/original/7b905a7af98f3e424974e23af7ef2da5.jpg?1402995844";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/256/original/ad35b7ba7bb6be6494f7b5050b2a52ab.jpg?1402995935";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/311/original/4d211b9051db6f7cf76693ae007e6016.jpg?1402996057";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/371/original/e0b5f0dfe7728388c37e64a18c3dc573.jpg?1402996161";}}', 'http://www.axis.com/products/cam_m3007p/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3006-v', 'M3006-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2048x1536', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/527/original/2d0aa7e840f953f5bac42266ed1a6931.jpg?1401192422";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/540/original/7760f64c34e499d7479a78df141b96a6.jpg?1401192507";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/543/original/6b3a047dd4a7a112643ccf0c9a7db6cf.jpg?1401192522";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/545/original/e8b015cd17f35ad83981124d24df8ef1.jpg?1401192525";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/182/original/b80dc7baacd35c9c9877c8d21a7b24fb.jpg?1402995790";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/262/original/7dc6cf77c32f2bc262d6c7bbf7674e55.jpg?1402995943";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/283/original/cc9c7e1385b0d1caa910ba155488a5e1.jpg?1402995996";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/294/original/044da54f1bf93b385c11dbbb3ea45429.jpg?1402996033";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/332/original/45d96be920c80293ed2c0fd9fc1f0dbe.jpg?1402996107";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/363/original/3391f0ed6835863dc42ce994eef1ffa6.jpg?1402996150";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/368/original/b4da1e83fc68594ea9b09b290a42671f.jpg?1402996157";}}', 'http://www.axis.com/products/cam_m3006v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3005-v', 'M3005-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/432/original/f8696744b4cf1a3869a2772ab74d5e28.jpg?1401191418";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/437/original/58065d96840b073333891548c785314d.jpg?1401191452";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/455/original/d577a864bb9659ba8406c69cf758dddc.jpg?1401191711";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/478/original/b98e8e71e21804b059a5b4791969b2d5.jpg?1401191973";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/507/original/14fe8ba83eddd3f98ae063117c744325.jpg?1401192190";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/138/original/c9753ed65ac308cdaaa1f326aa014daf.jpg?1402995728";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/168/original/7c95c7070d8879397de72789d3412a69.jpg?1402995771";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/223/original/68d864e1f813049ade27544bbd76bff8.jpg?1402995847";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/289/original/337aa058e1b530004d48fd7eb3da5527.jpg?1402996004";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/309/original/fc0b9f4848044796732fcd52fa1bf6c0.jpg?1402996054";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/321/original/21fe1ba3aef6f315e7976b299b228ae8.jpg?1402996093";}}', 'http://www.axis.com/products/cam_m3005v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm3004-v', 'M3004-V', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Dome', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/466/original/ea4105fdbc43c3b25f9e6c62c2f8f3ca.jpg?1401191836";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/178/original/d8c48e386e39115c624b04534a71820e.jpg?1402995784";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/217/original/1f493bdb65711d2a4a90d54c4d6b9250.jpg?1402995838";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/238/original/c2fa3fd56155784dc7924b2916eb3c37.jpg?1402995890";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/254/original/96159c1b16d959f74f595e20b617c47c.jpg?1402995932";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/272/original/e4743628377f3c277ac4bef3c9acd16c.jpg?1402995958";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/276/original/0a9ff6c8daf10ee3798a878d438df7af.jpg?1402995965";}}', 'http://www.axis.com/products/cam_m3004v/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p8514', 'P8514', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p8514/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1765-le', 'Q1765-LE', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/436/original/31470c5eeacdada284ee6db417c44f94.jpg?1401191428";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/526/original/7fe4c030929295e06ab6f9282dd45183.jpg?1401192418";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/548/original/3adaf1ec3c796e0c00673683d27d4354.jpg?1401192541";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/550/original/4311f1827e6564502d121aefcbe85bec.jpg?1401192553";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/170/original/45383be709da2bd0143c13d018567770.jpg?1402995774";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/203/original/8712587c660d5e9e805e44be154e4aec.jpg?1402995820";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/258/original/d4c9bde9b856004ca972a0136b7771e8.jpg?1402995938";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/281/original/1bbf2b8b0eda50c15fa8079d82171538.jpg?1402995993";}}', 'http://www.axis.com/products/cam_q1765le/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1755-e-b', 'Q1755-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1755e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1755-a', 'Q1755(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'q1614-e-b', 'Q1614-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1614e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1614-a', 'Q1614(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1614/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1604-e-b', 'Q1604-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1604e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1604-a', 'Q1604(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1604/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1602-e-b', 'Q1602-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '768x576', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1602e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'q1602-a', 'Q1602(a', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '768x576', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_q1602/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1357-e-b', 'P1357-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1357e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1357-a', 'P1357(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '2592x1944', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1357/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1355-e-b', 'P1355-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1355e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1355-a', 'P1355(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1920x1080', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1355/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1354-e-b', 'P1354-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1354e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1354-a', 'P1354(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x960', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1354/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1353-e-b', 'P1353-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1353e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1353-a', 'P1353(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_p1353/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1224-e', 'P1224-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x723', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/538/original/cf6cdcb85e53c657dbe29a9cd856a5ed.jpg?1401192491";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/143/original/b8e00f301d04595dcc05925d310edb6c.jpg?1402995735";}}', 'http://www.axis.com/products/cam_p1224e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1214', 'P1214', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x722', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/476/original/83403cccbf624151a0e6d724e2c15105.jpg?1401191943";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/515/original/b7db8dc3b05ce66a730e7dc989834c1a.jpg?1401192283";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/226/original/ce1a56cd45b8473093632a77d4e3f48b.jpg?1402995851";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/252/original/25eac2a45926dc0eef5d6471c732af6f.jpg?1402995930";}}', 'http://www.axis.com/products/cam_p1214/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1214-e', 'P1214-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x721', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/460/original/c071f9de44f1067c09bb39b9c840a754.jpg?1401191720";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/190/original/93e71ae02295e2df2f6d27619ba6b32b.jpg?1402995801";}}', 'http://www.axis.com/products/cam_p1214e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'p1204', 'P1204', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/318/original/d6d6285e8a4ec12a9d9d441beaf6efc9.jpg?1402996089";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/330/original/5a79690985fe62e4f1707569fe614f39.jpg?1402996105";}}', 'http://www.axis.com/products/cam_p1204/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm2014-e', 'M2014-E', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x720', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:12:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/431/original/e774961f696bf65d5dbe1ba552492f9e.jpg?1401191417";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/433/original/eeb669b0a573b1d0144df0131d21a758.jpg?1401191420";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/440/original/cd99c53e8596a9c310d40f8c39fd258c.jpg?1401191499";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/470/original/5feb1f3f4f2b451632ba9d27c57ab6e1.jpg?1401191889";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/495/original/15ae1b0c910b50a16da4cfe957500d9e.jpg?1401192147";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/531/original/ff3bf5476792d83e2a9999c99a731390.jpg?1401192468";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/155/original/d0a2f0b31da1d408111c8d8cd7acf870.jpg?1402995753";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/177/original/3518ae8d25dd9c29bd7ff3a2fc1afec5.jpg?1402995783";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/198/original/9f6411ab6ce842cb4527970aff2563bb.jpg?1402995813";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/306/original/7d3534b632d7fba17b64facd424ed74a.jpg?1402996050";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/326/original/b66d0dca611fa25655c65b1b682238e1.jpg?1402996099";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/361/original/e143e30ae35c7c834b34fbe39bbb71a5.jpg?1402996147";}}', 'http://www.axis.com/products/cam_m2014e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1144-l', 'M1144-L', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:10:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/459/original/4db5c62e0d2535448b85c12611b85bda.jpg?1401191719";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/490/original/92e091bf05684c744f2919d3e476d045.jpg?1401192067";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/506/original/05beb7c6a97d58a8e6aa02364e03c085.jpg?1401192188";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/514/original/521643c56739587db74d314fe9ad28e0.jpg?1401192281";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/150/original/d45336f52b76da173a755a76f2b4dff2.jpg?1402995746";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/246/original/00e71788ae513dccc0e205bd28711514.jpg?1402995922";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/284/original/2f8c0fbe8187337dd354f7bf94ad9a50.jpg?1402995997";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/335/original/cbfe5b6b78c1a8b2dd1ae23dd72c0f84.jpg?1402996112";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/345/original/0f2f936bc0a7d0785b4f2e85325a53a1.jpg?1402996125";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/347/original/4ef47fb0a489c9cf3995090fb3bb5f45.jpg?1402996128";}}', 'http://www.axis.com/products/cam_m1144l/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1143-l', 'M1143-L', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/481/original/8e26df3fa341edd2732bd24f12e2d139.jpg?1401191987";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/511/original/4c4c6ec76663fb3f6c5c4077f6b5604c.jpg?1401192255";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/171/original/599ec523eba9a3e5746169c4fffb2eb4.jpg?1402995775";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/192/original/25e4304c475dd0cd541ef2b8ba099939.jpg?1402995804";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/211/original/a820a0632e3b00fb1bc9050d67e22ece.jpg?1402995830";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/225/original/f503edd42f7f70a4ce8796f4c343987c.jpg?1402995850";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/313/original/906a357d3e7b449d31c5a6a896099ecb.jpg?1402996081";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/358/original/af071c158f7701d13fb746b56544aa04.jpg?1402996143";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'm1114-e-b', 'M1114-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1114/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1114-a', 'M1114(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1114/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1113-e-b', 'M1113-E(b)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1113e/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1113-a', 'M1113(a)', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:0:{}', 'http://www.axis.com/products/cam_m1113/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1104', 'M1104', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/423/original/a70af2f404de6113113a053196349ef3.jpg?1401191335";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/462/original/30b210beb0175a5ae25910e25aa97369.jpg?1401191745";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/497/original/f748335cad53bdd3e5b63d788c0db21f.jpg?1401192150";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/536/original/9377adf01c5a59fd5f78a3279b1eaad8.jpg?1401192486";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/186/original/605d9c63c79e9b4e8b873310b983b4e8.jpg?1402995795";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/208/original/855b6bdb06522cecf73d485004e9cdc8.jpg?1402995826";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/253/original/a2d39906140d7721243cce91d1cb4c21.jpg?1402995931";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/268/original/423e40d05582ed68a22929d630b88b8c.jpg?1402995952";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/303/original/0a28c79d382fe89b869ef93d4b17904b.jpg?1402996046";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/307/original/c921c67761d8b3785ef728bc9e18c2e3.jpg?1402996052";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/338/original/d6a77b00c27f8a8c41f772e8ce1fee64.jpg?1402996116";}}', 'http://www.axis.com/products/cam_m1104/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1103', 'M1103', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:10:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/443/original/84a3138767e1312550c77e01adcdb32b.jpg?1401191548";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/463/original/e1677d29b8b61ec7159d8ac03b69cd23.jpg?1401191768";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/492/original/42a675983734223f5e8b7265f6d153cb.jpg?1401192115";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/135/original/1978468e239aee6dce1f75cd8e72180f.jpg?1402995724";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/166/original/f231e09295352000ee6a21ad9b2fe3db.jpg?1402995768";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/195/original/63997bdd3ebff8974dfb0cdec0903d4d.jpg?1402995808";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/277/original/cd2c4191bbf9a4325758abf4f301fbbc.jpg?1402995966";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/279/original/47ad741198068269cf6dca6c8d8b5af3.jpg?1402995990";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/293/original/de6bd2d5468166ecebb9e454e517cd6f.jpg?1402996010";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/372/original/1f12c0f94b6b8b5a4f2f3413c05c397a.jpg?1402996163";}}', 'http://www.axis.com/products/cam_m1103/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1054', 'M1054', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:12:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/493/original/842ecd024bff87ed3c088f4ad25826d9.jpg?1401192118";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/504/original/c6e103e391d27d18ac06af57e690337c.jpg?1401192183";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/522/original/29f56931277122abf90b0df54b66102f.jpg?1401192388";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/530/original/bd6596d60a5d03447dd90c813eccccf9.jpg?1401192464";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/535/original/0f6518155050dcf718bf9d49436e1443.jpg?1401192481";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/551/original/50e270f11383daa1b394576a387171d2.jpg?1401192561";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/148/original/36484cd099735af6d92d372b9d06293a.jpg?1402995743";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/157/original/33ca16691ba8e6b0105ec7a794ba86ad.jpg?1402995755";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/242/original/0a8ed593ee5ab55fc4e000e1e0536dfb.jpg?1402995917";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/331/original/f6c34282a7626a25fbc4df2d27cf012a.jpg?1402996106";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/362/original/964b986eda43a80ece80165b30194431.jpg?1402996149";}i:11;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/380/original/4b47a7472e887f5fefdc1e010f930969.jpg?1402996175";}}', 'http://www.axis.com/products/cam_m1054/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1034-w', 'M1034-W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/461/original/a2d74939393f463cf5955612d69f4d80.jpg?1401191722";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/161/original/ccfa16ff9d9bd8e67fd9a9ca11523731.jpg?1402995761";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/191/original/907387d3023f21b6eeda779f72db5f4f.jpg?1402995803";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/228/original/5fb9ada8b4c61e9ea75b2895153378af.jpg?1402995854";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/236/original/8b70d2581342be6900467b3b97c5ba88.jpg?1402995887";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/290/original/6eee92a6b619ec1eb5df942b6aebefa8.jpg?1402996006";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/359/original/a2eb0a5fc22202182925fe514c1763e2.jpg?1402996145";}}', 'http://www.axis.com/products/cam_m1034w/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1033-w', 'M1033-W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 'a:11:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/429/original/08bf7b09186327de8524963ad509f3c9.jpg?1401191393";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/472/original/d840c2a964c69f79c02d4f9c84a3105a.jpg?1401191893";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/484/original/682d457aefd91e547288f1afdfa821d0.jpg?1401192014";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/487/original/df6d1505b67e0399eba5e8c9405f64ce.jpg?1401192042";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/491/original/9a5ec5709238096687cfa690d421b4b8.jpg?1401192091";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/164/original/74c535e71b5b296933991bcb45ca3bb5.jpg?1402995765";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/196/original/52458cc4f1bac879678f23686c3c637f.jpg?1402995810";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/245/original/b22187ec9a97fc30edd3d6be7cecb640.jpg?1402995920";}i:8;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/274/original/dd2742c692b4c71f4fdb911fe7801fb2.jpg?1402995961";}i:9;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/349/original/d60167e0c006eb994ed9f99b849978b4.jpg?1402996131";}i:10;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/350/original/afb75e78fe66df834dd7170174bca53f.jpg?1402996132";}}', 'http://www.axis.com/products/cam_m1033w/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1014', 'M1014', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/426/original/c1402111ca8f6bbe90c9f9c208609066.jpg?1401191362";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/147/original/4a031cca13de075c8c34db5ca68cd448.jpg?1402995741";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/232/original/8c18a6a1ba393956ee5d1937030c06bb.jpg?1402995881";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/260/original/417aa3d6aad19a0f55aee156635a0cf3.jpg?1402995941";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/320/original/2bc9f6abf6021333a2240f88c60c62c5.jpg?1402996091";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/322/original/4dd9d633f9c8d004e9659e54ab9225f1.jpg?1402996094";}}', 'http://www.axis.com/products/cam_m1014/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1013', 'M1013', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '800x600', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/417/original/cf6c125d7f510b8ccd6a285136d39dd7.jpg?1401191300";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/503/original/d94445e3de4f2a048a490743451c963c.jpg?1401192175";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/146/original/ad572cb4445c64d1944ec039cd85fd0a.jpg?1402995739";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/154/original/6718ca8ee3d1da367c4ea28fe409a468.jpg?1402995751";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/243/original/0e87f7c187d157bfed2bff371c90b70a.jpg?1402995918";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/257/original/b44a6613d0d7f706b2520c78d5521b90.jpg?1402995937";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/296/original/2cbf35b5f1340bebbd7ba2580452d504.jpg?1402996036";}}', 'http://www.axis.com/products/cam_m1013/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'm1004-w', 'M1004-W', '35', 'axis-cgi/jpg/image.cgi', 'axis-cgi/mjpg/video.cgi', 'axis-media/media.amp', '1280x800', 'Box', 'root', 'pass', '', 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/489/original/c9047d9a832d39fef239fb6c0108f068.jpg?1401192065";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/227/original/ee977ca42266cbee5624124786c8a334.jpg?1402995852";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/308/original/d2ac8055e2312dfe7f41e4eba51d98de.jpg?1402996053";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/310/original/6762d5bb0848f2438ac97825eff648b2.jpg?1402996056";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/328/original/98841632e10965db34f89a48438c75f8.jpg?1402996102";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/381/original/9fc97b42361941d364e16c352d296c1f.jpg?1402996177";}}', 'http://www.axis.com/products/cam_m1004w/index.htm');
INSERT INTO "cameras" 
VALUES (NULL, 'snh-1011', 'SNH-1011', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Box', 'admin', '4321', '', 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/389/original/5e2fb2d0220b97cfb741bd19095725df.jpg?1400857455";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/409/original/a19ee4b2e595936fab7f19580f18ae71.jpg?1400857613";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/067/original/b26bfbbe3359af1534fd7f6b64487366.jpg?1402995625";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/077/original/8e5f000f37ba1beae745432454428b3b.jpg?1402995638";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/wireless-ip-cameras/SNH-1011.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snh-p6410bn', 'SNH-P6410BN', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Box', 'admin', '4321', '', 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/408/original/5d7559d30d17022ea6073ffcdfd56cb1.jpg?1400857612";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/352/original/c9a291a0bf35810875c74fd68ae0e98e.jpg?1400857325";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/057/original/0fb9388fa02706ead567dc5c6d4d7b40.jpg?1402995569";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/080/original/5096d7dbd75b1d1450225efe7c7d4491.jpg?1402995643";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/wireless-ip-cameras/SNH-P6410BN.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3122', 'SNP-3122', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3120vh.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3121', 'SNP-3121', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:0:{}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3120v.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3120', 'SNP-3120', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/326/original/4d6a03191cfd57c5f43cf2a12e5da359.jpg?1400857225";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/092/original/9b5849eb334b0bca8af97e15e13979df.jpg?1402995660";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3120.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3302h', 'SNP-3302H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/396/original/e1b114adf6a4f17310dc92c37a37608d.jpg?1400857508";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/405/original/f5e226295e1c9d0ff69e150dfe792447.jpg?1400857607";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/032/original/452b030912fb308a5e56d9c9cc13312b.jpg?1402995532";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/033/original/d04f41673b44793daf463dce3286f550.jpg?1402995533";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3302h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3371', 'SNP-3371', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/333/original/799117ca7e643b5dbd667fd279f75cbc.jpg?1400857256";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/019/original/43c0760a3c3d89e9ed5215310d1dee23.jpg?1402995513";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3371.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-3371th', 'SNP-3371TH', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/378/original/63950471818d5b7fd245a9d4f1ce9f6c.jpg?1400857375";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/125/original/227a4b8c0058c068ff515b0642896ae4.jpg?1402995710";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-3371th.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-5200h', 'SNP-5200H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/319/original/2d8c4a7601fdb3bbae8b3d396e196382.jpg?1400857151";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/320/original/32eb5c68ed95522fcaff2b554436f163.jpg?1400857174";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/089/original/863ec39730607154962f2e06f25f2536.jpg?1402995655";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/120/original/92a51c6b91d8a677cb6608bd6109f5d7.jpg?1402995702";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-5200h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-5200', 'SNP-5200', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/391/original/66ca4d4b3f56d631efbe4cbba04f0037.jpg?1400857501";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/012/original/4aaef2ced109e71cf5251993ab167197.jpg?1402995503";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-5200.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6200', 'SNP-6200', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/411/original/2875188c35125637fedd007fb96272a2.jpg?1400857638";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/009/original/edc96beec29e7286f71dba623326c778.jpg?1402995498";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6200.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6200h', 'SNP-6200H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/371/original/2a20a790a4a3afb806735d327dd3006f.jpg?1400857360";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/393/original/e761011b80eec0ca48d700c5def36cda.jpg?1400857504";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/011/original/faafa1d27b78d799c0f54a1a3e443241.jpg?1402995501";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/026/original/c234a81e41a97a8e80b20ffa33eab958.jpg?1402995523";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6200h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6201h', 'SNP-6201H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/373/original/8c9304a1c9af64f4ca8348074ef11693.jpg?1400857364";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/008/original/a3fca22b665c978e0afdd396075f7eb4.jpg?1402995497";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6201h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6201', 'SNP-6201', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/353/original/83e6e01e30cc86389ee188346cae0360.jpg?1400857328";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/063/original/d176e12bac1ac53711308589a5013c7d.jpg?1402995619";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6201.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-5300h', 'SNP-5300H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/360/original/50349d0959a1f90dc3bc61b4c85c6793.jpg?1400857339";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/065/original/c61c94719ed1c644ae9f0a61d07a79fd.jpg?1402995622";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-5300h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-5300', 'SNP-5300', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/401/original/cdc0b69b7f3a4fb4f7d5216b9dba264b.jpg?1400857580";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/091/original/2edd3707f350bba2f2ee62f238ef454b.jpg?1402995658";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-5300.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6200rh', 'SNP-6200RH', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/325/original/4659b36e27aa9471a8b7b077a7c58741.jpg?1400857224";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/399/original/8f4a01ff764133e01cec52967b18ec2d.jpg?1400857556";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/043/original/a4f6a3c085c3513d58af8fe8c11bb075.jpg?1402995548";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/108/original/0f30660a5762800c2302eba361017909.jpg?1402995684";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6200rh.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6320h', 'SNP-6320H', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/348/original/6159af99585b01148ed32c888f43bf00.jpg?1400857319";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/087/original/df5d92befef2b9e88e74b945b129f328.jpg?1402995653";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6320h.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snp-6320', 'SNP-6320', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/407/original/bffd874447820133db104f53c600d9dc.jpg?1400857610";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/122/original/e90d582576e2aad2ff734f0f399e5238.jpg?1402995705";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ptz-domes/snp-6320.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-1080r', 'SNV-1080R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/335/original/d61f233c977ca3f8050a0c8793b8185c.jpg?1400857302";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/015/original/87644a01018c9ea4ceac81a37addeb6d.jpg?1402995507";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-1080r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-1080', 'SNV-1080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/328/original/50fb12e987680cdf34219ed63e18a8c6.jpg?1400857227";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/040/original/31e326086baa4180372537ba1eb366a2.jpg?1402995544";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-1080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-3080', 'SNV-3080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/321/original/675dece2149df98218bc0cf8bd3458aa.jpg?1400857175";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/349/original/8dfca0b4650ae8c8c7ab596ff9cbb7e1.jpg?1400857320";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/020/original/d489449e45202aeb44fdd9e4d1eb1313.jpg?1402995514";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/056/original/5af7ad9aabbdb8f733d7b6c8154f0498.jpg?1402995568";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-3080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-3120', 'SNV-3120', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/312/original/6ef6c96e0d40392483f96a86fde71b9c.jpg?1400857141";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/374/original/08304a524f73dfe315754459541fb5c3.jpg?1400857365";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/007/original/8bf80206de3f9514df8645a53494393d.jpg?1402995495";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/055/original/f642c4d417e26cec36e62dcd746e76a9.jpg?1402995566";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-3120.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-3082', 'SNV-3082', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/404/original/7df2951f381af0830dd1ee2d894b0110.jpg?1400857585";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/010/original/3f366e2d61114fe4ce7e56e418229d6e.jpg?1402995500";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-3082.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-5010', 'SNV-5010', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/342/original/47db7936c7339ffff24d467c9ec28b83.jpg?1400857311";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/351/original/1e2ddf7e552ac88173c8d925cbb6aab5.jpg?1400857323";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/413/original/24de0ca586d140bce433542001c4c020.jpg?1400857662";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/017/original/1b846b7c5c0db7996c69a610c23408d0.jpg?1402995510";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/058/original/3c9023d207bae8d1ca4272e956b5e457.jpg?1402995612";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/064/original/31fa4ebf1c241b6b248e895f0cb1c5ff.jpg?1402995621";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-5010.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-5080', 'SNV-5080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/336/original/1faa564282faa93d39efd7f451ff1dc7.jpg?1400857303";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/365/original/14ab360bbafe3780d6a5c7fba9dbdac3.jpg?1400857345";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/115/original/159746804c0ae5d5243e8177c4eda2ec.jpg?1402995695";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/118/original/d241ff19215f7fe769c689188b53ac64.jpg?1402995699";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-5080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-7080r', 'SNV-7080R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/311/original/b147dcc18c47b6b4c820d05f92eb6246.jpg?1400857139";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/131/original/b5ee48aa79e5626bf59f0c42e6f577ed.jpg?1402995718";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-7080r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-7082', 'SNV-7082', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/332/original/8e43afacc8233b2bf780e68203e9ec62.jpg?1400857234";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/397/original/046b116a52a83ec87a583dc3ddc92ec9.jpg?1400857532";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/102/original/4892bd476dfda14f480c013968f4fa6b.jpg?1402995674";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/104/original/cc92748ca52564b10bf8e3b550364d56.jpg?1402995678";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-7082.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-7080', 'SNV-7080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/343/original/b33a8ab8f720a345ec6cd9955cd5d41f.jpg?1400857312";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/410/original/5c920968d454c4921020f509018007ad.jpg?1400857615";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/070/original/c7b68c9c446e078dfd7b23a91aa3fff0.jpg?1402995629";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/079/original/7abad40c189e43b96900d11960a637b0.jpg?1402995641";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-7080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-6084r', 'SNV-6084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/384/original/5122b73342d2ce9cb0513e1052104521.jpg?1400857426";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/018/original/714e0f9145b09b6e658e5444584bfd4a.jpg?1402995511";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-6084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-6084', 'SNV-6084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/367/original/6137bca7b524341e080a99eb22b66071.jpg?1400857348";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/045/original/8b2fc8b010d5437b7a34c6ec2a6a6f96.jpg?1402995551";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-6084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-5084', 'SNV-5084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/331/original/e39505870d9cbb111ae48359d013e740.jpg?1400857231";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/097/original/59c0d6b7e97015a583032ee18d5a2cbd.jpg?1402995667";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-5084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-6012m', 'SNV-6012M', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/350/original/5a985e91316eb4085415182c95c9ec7e.jpg?1400857322";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/356/original/73ef7f9a6c96e4963025297aec52e3a9.jpg?1400857333";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/364/original/0d45000acc01615b46bd272d2186562c.jpg?1400857344";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/068/original/27aee6f3bfbe174e4ab55d21902e6483.jpg?1402995626";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/071/original/fcca30cd38a55911b5ea5fc147c443d2.jpg?1402995630";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/124/original/32536943d6e1287da62624305a2f3640.jpg?1402995708";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-6012m.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-7084r', 'SNV-7084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/324/original/f653a33e82af459d425002f19598ede8.jpg?1400857222";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/029/original/21a7b1becfc70ed17be94b0622a347fd.jpg?1402995528";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-7084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-7084', 'SNV-7084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/347/original/cec5d6d93fb30b405922609942896277.jpg?1400857318";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/076/original/231cbf3407b306f66649a0c8a1f9c622.jpg?1402995637";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-7084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snv-6013', 'SNV-6013', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/322/original/365f84a4ffdf62728ebb3c453aa0503a.jpg?1400857198";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/359/original/53b8cc5f8e0aa49f882c1ca2fa7e0ee3.jpg?1400857337";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/376/original/799eb2e52b7c31eb6adc93b7f627b093.jpg?1400857369";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/027/original/c97362aa2c904ae9b8f38cbb73a7eed8.jpg?1402995525";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/041/original/36595229ceda07b57294791e4da406b3.jpg?1402995545";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/116/original/7eca41b2ee28e8717d0993d476f152bf.jpg?1402995697";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/ip-vandal-resistant-domes/snv-6013.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-1011', 'SND-1011', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/355/original/5dcc32dfa287de1af950ca9241e23e50.jpg?1400857331";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/388/original/a6d8a2646eccd5d31b6004b1dfc31654.jpg?1400857453";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/005/original/999acf4d703ea5713a72d2031c557370.jpg?1402995493";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/023/original/57b4239f64eb0e21b7439a6711da6891.jpg?1402995519";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-1011.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-1080', 'SND-1080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/314/original/834088d7ebf954b997969e58b5bd8b23.jpg?1400857144";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/021/original/cbf7f39e50997f4b7972e1c7ad7f2eab.jpg?1402995516";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-1080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-3080c', 'SND-3080C', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/024/original/cc2f855a2c32898a83be15e721e6566b.jpg?1402995520";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/053/original/79d39592f7d241d3bc7c008f4ebf5a93.jpg?1402995563";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/127/original/970c7638d3dd209a481f64e112f8b1f9.jpg?1402995712";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/130/original/3eeac8b8f89a0f876cfd37fa2e47ead8.jpg?1402995717";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-3080c-cf.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-3080', 'SND-3080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/317/original/efdbb9350261092fc2207395761b1f02.jpg?1400857147";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/002/original/c0bb3811ad658f48e12011a2dcaa8823.jpg?1402995487";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/081/original/a5f2e9ee28c57f287a09bd2e1d6701a4.jpg?1402995644";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/086/original/1bac18990ee1db65d83949311267b72d.jpg?1402995651";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/103/original/d750e5ca7cff777b4dc6e306b6a1cdb6.jpg?1402995676";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-3080-%20f.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-3082', 'SND-3082', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/022/original/2a4879a8e955655c903881bf66ca4ad7.jpg?1402995517";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/075/original/4bf7e71514edec5b7c97ee18860a5d1b.jpg?1402995636";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/090/original/3b859e8c87c4c3f5da52f3803cb74029.jpg?1402995657";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/128/original/9ae74ba5d1895ddc6314ce4e947cefc7.jpg?1402995714";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-3082-f.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-5011', 'SND-5011', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/323/original/dcc3f66a7cfbcebb2ecdc58bc4341271.jpg?1400857221";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/369/original/a7c395f68f2b61c104e083825f66c2f4.jpg?1400857356";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/014/original/59240b11ba23ab6ac9640911a270bc67.jpg?1402995506";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/044/original/d54149fb2afa7da615ae94f0c86b9c4a.jpg?1402995550";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-5011.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-5061', 'SND-5061', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/381/original/2984c86dba42f5ee8621b234879230e3.jpg?1400857380";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/001/original/3cdf3c5cfcce0a8dafd041abde38e088.jpg?1402995486";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-5061.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-5080', 'SND-5080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/308/original/2ebd34286a102301730f280b40ecdc98.jpg?1400857135";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/318/original/6f5f885aea6c6165f0f409afe3076722.jpg?1400857149";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/380/original/645ebbdbbc70fa21d86e42c551ae20dc.jpg?1400857378";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/998/original/69dfb2ac7bcb4e86588baacfd6239296.jpg?1402995482";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/003/original/76d9b71b14f9b745131c9ef513ecc7ec.jpg?1402995489";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/050/original/97dcae14666bb31b45d3f1e08f585fef.jpg?1402995558";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/107/original/aa3f2fd9514eef733e7ca2e99ea46425.jpg?1402995682";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-5080-f.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7011', 'SND-7011', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/304/original/06250caa7e5f4724c0c748885901688d.jpg?1400857130";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/394/original/4788b036b916dd5c99e2ade8cf64f41f.jpg?1400857505";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/082/original/53aad796e641dc4ca0559cf4f9bee432.jpg?1402995645";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/112/original/74d6ec3dd6bce6820ce16218d8062d12.jpg?1402995690";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7011.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7061', 'SND-7061', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/375/original/c275fd7e9a1850df09332819e3c83e95.jpg?1400857367";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/400/original/27940475a6b5c6d060f0c7d970461c49.jpg?1400857558";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/100/original/f6a54f795df6b1a2b1a10248b978b632.jpg?1402995671";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/121/original/ebcadb0975fe80186615e714d90c12cb.jpg?1402995704";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7061.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7082', 'SND-7082', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/338/original/195d30f23458f65d23e3f118f9f1a2d6.jpg?1400857305";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/016/original/a285bd858c00bb024fada6da683e031a.jpg?1402995509";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/059/original/08b1554abf6c72d5487b344df21c9645.jpg?1402995613";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/074/original/9a00a9d52e08c91dab2cd399474e37f0.jpg?1402995634";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/095/original/6476d8fa00365717d145ab2db95468e2.jpg?1402995664";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7082.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7080', 'SND-7080', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/346/original/7c27c237b3b12a44f18a21b105c113e2.jpg?1400857316";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/362/original/9218771fe01cb25747ab1c83a2236aa6.jpg?1400857341";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/363/original/422a8cfae1799755009bd384d7b18121.jpg?1400857343";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/030/original/bc757295a4d88d53a66e988d1d92025b.jpg?1402995529";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/037/original/46a6bc01e876e937662af822bd8660c8.jpg?1402995539";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/054/original/b77172e5c8531fc540fb0144ea54f99d.jpg?1402995564";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/073/original/94c3889174378c25d8f3a63e17436b81.jpg?1402995633";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7080.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-6011r', 'SND-6011R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/327/original/a181d707e33a83c78f1f304f559a7a4e.jpg?1400857226";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/098/original/d3e5026237c97d1c2b639e9841e78960.jpg?1402995668";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-6011r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-6084r', 'SND-6084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/387/original/8bf482c59d93df44968246a8d6323b6e.jpg?1400857431";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/047/original/3de1dbd0c7e0633a8cbdc7a8e5c0def9.jpg?1402995554";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-6084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-6084', 'SND-6084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/339/original/e26cb30bf3d724f05f1ec425692a4e9e.jpg?1400857307";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/060/original/12f17bdbdf1eb31b1b53eac339777932.jpg?1402995615";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-6084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-6083', 'SND-6083', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/390/original/92e30a774c1c21404ec8e577ae7ec9b0.jpg?1400857478";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/078/original/5ea4873a41711fc44a3f822ee86ffc45.jpg?1402995640";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-6083.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-5084', 'SND-5084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Dome', 'admin', '4321', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/334/original/b3478da132e49240f5d77faefe2703fb.jpg?1400857280";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/083/original/9fa49bf3d11c50647b0aea033df9a343.jpg?1402995647";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-5084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7084r', 'SND-7084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/392/original/69ed3247819df564918e350f3e369a60.jpg?1400857502";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/042/original/13224efe3b9f9331d7d130a3447c39ad.jpg?1402995547";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snd-7084', 'SND-7084', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/361/original/27c224eab76ca42d222489c46302e075.jpg?1400857340";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/039/original/f9b546a9103bf634419204de53ee4cf4.jpg?1402995542";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snd-7084.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snf-7010-vm', 'SNF-7010-VM', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:0:{}', 'dome');
INSERT INTO "cameras" 
VALUES (NULL, 'snf-7010v', 'SNF-7010V', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/316/original/be0e460c3cdec1a4145ba4c0dc0f7667.jpg?1400857146";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/383/original/034f068f578137a04a59eb30484f1258.jpg?1400857425";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/006/original/506acb67cb31e255493de05dfdfc9d5c.jpg?1402995494";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/101/original/03aebc95c552338eda4dfee915e4cbe1.jpg?1402995673";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/110/original/3a403b60e5054ba3a7eec482c525c0cc.jpg?1402995687";}}', 'dome');
INSERT INTO "cameras" 
VALUES (NULL, 'snf-7010', 'SNF-7010', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Dome', 'admin', '4321', '', 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/307/original/ac8627a377c67d9b745ce1a614447f37.jpg?1400857134";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/025/original/ed08e8f5a864075143263f3559d66071.jpg?1402995522";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/034/original/73fe64dab1438ed983459964a6adfff5.jpg?1402995535";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/105/original/71ac2133ef48901357ea75bebff5cae8.jpg?1402995679";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/fixed-domes/snf-7010.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'sno-6011r', 'SNO-6011R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Bullet', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/379/original/c1a9e0763a01db345ab2b8e9320cd2a6.jpg?1400857376";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/096/original/0fdefdca74edea1c56e9475f1703e172.jpg?1402995665";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/bullet-camera/sno-6011r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'sno-6084r', 'SNO-6084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Bullet', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/386/original/bf42b54fab0bb7566e45d8a9dac4c5d7.jpg?1400857429";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/069/original/2581d97d1522d058c4e0ccc088acd4d2.jpg?1402995627";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/bullet-camera/sno-6084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'sno-7080r', 'SNO-7080R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Bullet', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/403/original/54ba65f120f9d7eeeb7c3e57bd86109c.jpg?1400857583";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/046/original/5b3c1ac7fece5af50a13cad8c5a4554c.jpg?1402995552";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/bullet-camera/sno-7080r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'sno-7082r', 'SNO-7082R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Bullet', 'admin', '4321', '', 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/385/original/3410dff43a84a718c960a6c364a99c08.jpg?1400857427";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/052/original/958df9428ee6fbbd9345697c3bd4a685.jpg?1402995561";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/bullet-camera/sno-7082r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'sno-7084r', 'SNO-7084R', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Bullet', 'admin', '4321', '', 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/306/original/98029cf48697831260501950e18aa9f2.jpg?1400857133";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/035/original/2c03a59199f28d72e08ff6dac6dde986.jpg?1402995536";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/bullet-camera/sno-7084r.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snz-5200', 'SNZ-5200', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/337/original/b758b8c7d47c056f31aee991c77adc45.jpg?1400857304";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/340/original/19fd0843418985232d6f57884008bcdf.jpg?1400857308";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/111/original/a545350e38615d0825ecb85236cea255.jpg?1402995688";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/117/original/f380ac3cd52da8bbe74b718ed08065a8.jpg?1402995698";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snz-5200.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-2000', 'SNB-2000', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Box', 'admin', '4321', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/377/original/f00f8e78ff66ce37f0733ffff6e8123f.jpg?1400857371";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/088/original/243deab19d49a651a4efbb11155d8571.jpg?1402995654";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-2000.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-1001', 'SNB-1001', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '640x480', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:4:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/368/original/0ecfe9b832614e6d4a74216e231664fe.jpg?1400857351";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/382/original/1e4105b9c74fe30eba347ea6907c3a0a.jpg?1400857403";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/013/original/e4259846f91f3b6a32589297ec4d3ca6.jpg?1402995505";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/106/original/8735616c0e024728516705370959b2d7.jpg?1402995681";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-1001.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-3000', 'SNB-3000', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/370/original/db8a115f25aa570c559068e9eec23e26.jpg?1400857357";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/066/original/afe43f867054c6fc2812b511d7294e80.jpg?1402995623";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/123/original/fbb816b0cc5103235f92e5399121b626.jpg?1402995706";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-3000.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-3002', 'SNB-3002', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '704Ã—480', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/329/original/24cbec3e0ab0fa463cc207a8a76c0c5f.jpg?1400857229";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/031/original/b5493d9e58657a035b4091b6d124c7e2.jpg?1402995530";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/051/original/63590ab5609a8b8d471abbca9caa2533.jpg?1402995560";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-3002.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-5001', 'SNB-5001', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/313/original/95d364a0ddec3bbd5c78460218254f35.jpg?1400857142";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/358/original/5a9aaba532605dbec754102340d3a7be.jpg?1400857335";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/406/original/dae0c12efd7422a4e85f6e193c20c266.jpg?1400857609";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/072/original/172cef2abab36bd88ed5f092bb30c7d9.jpg?1402995631";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/084/original/b92ec4fc9a5f74adfa1e6321cbe0c227.jpg?1402995648";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/133/original/b3a0e9a4efe23dc920826fdd2805102f.jpg?1402995721";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-5001.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-5000', 'SNB-5000', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/372/original/6b920793baa7ada7cd592576ac1daa03.jpg?1400857362";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/049/original/71f418775a6b4ca007375b9dc6bafc59.jpg?1402995557";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-5000.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-5004', 'SNB-5004', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1280x1024', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/395/original/13d30fd18c7f67cdeee206c348662555.jpg?1400857506";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/398/original/86239fc83bcb90c0a6369731a5dfb328.jpg?1400857554";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/402/original/62c91acd6e465aa6a57a1767a87796f2.jpg?1400857582";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/412/original/42867d2a88d248cfcebe7fc36c07fdcc.jpg?1400857660";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/061/original/c901385a8e739dfab236041656faace3.jpg?1402995617";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/062/original/f2268418c78e5c819884249a2fe55f89.jpg?1402995618";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/093/original/a3a59be4e0c392bee656fc6e360da2b3.jpg?1402995661";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/094/original/207caa63aa3caae760877a5d58f8f111.jpg?1402995663";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-5004.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-6003', 'SNB-6003', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:7:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/303/original/7fe3627c3a1a6c44f6d4b457726dea7d.jpg?1400857128";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/305/original/56bb83af3394382479d9846caef7daf1.jpg?1400857131";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/315/original/ccf106a0fd57eb11866f33437ba6e9f5.jpg?1400857145";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/038/original/7874c5a527ead982d8276305943494c9.jpg?1402995541";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/085/original/7a2360096fa528b7d14fae0113e46c78.jpg?1402995650";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/114/original/31e829d50bcfbc61f759dc198fdbbe01.jpg?1402995693";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/119/original/765e4d7ca824eb23525395f4cbafe2ca.jpg?1402995701";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/network-box-cameras/snb-6003.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-6004', 'SNB-6004', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:5:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/309/original/fc72682e1d0a5dd265655eac99384a05.jpg?1400857137";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/004/original/7f52dba1959ce3a01740dbf4d2ee04ee.jpg?1402995491";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/036/original/757711277f8a4584a0cb22dacda8abed.jpg?1402995538";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/048/original/bb0b43e466158b3323a3c06101a3f8cb.jpg?1402995555";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/109/original/4f4f91d913d4b387af5cb0c94ccc89bb.jpg?1402995685";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/network-box-cameras/snb-6004.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-7001', 'SNB-7001', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'a:6:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/354/original/68dfbbcd3ade90dbd044439256b6f5b3.jpg?1400857329";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/357/original/628e36f6eaa50dbb7f76506e0aeb5b33.jpg?1400857334";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/366/original/93b04556f23af3d8506006fc45cf0579.jpg?1400857347";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/999/original/c26a8e9301b15095ad5c9268432c8b1e.jpg?1402995483";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/113/original/935a45accd6b80aec8cad4c00af62375.jpg?1402995691";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/126/original/f3f0822c825f95b2f541db08d9ec49e3.jpg?1402995711";}}', 'https://www.samsung-security.com/en/products/security-cameras/network-cameras/network-box-cameras/snb-7001.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-7002', 'SNB-7002', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '2048x1536', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 'a:2:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/341/original/9b9f5fd30d192f3e454a5b4f2fd1e4c0.jpg?1400857310";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/132/original/ad212c7060894d35713ee44379269789.jpg?1402995719";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-7002.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'snb-6010', 'SNB-6010', '36', 'stw-cgi/video.cgi?msubmenu=snapshot&action=view', 'stw-cgi/video.cgi?msubmenu=stream&action=view', 'H.264/media.smp', '1920x1080', 'Box', 'admin', '4321', '', 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 'a:8:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/310/original/76435135c66205345966a45fe8699594.jpg?1400857138";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/330/original/f0c16282ae52499f6f8d16ca93e029cc.jpg?1400857230";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/344/original/d58a85fcda3d70ab162db1389a75738d.jpg?1400857314";}i:3;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/345/original/a0abf44b8ece1acf63a4680ba7b973b9.jpg?1400857315";}i:4;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/000/original/4421dacf56f471ef34393ef9060de566.jpg?1402995485";}i:5;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/028/original/f28a2f83589f0a3156b434ecd4b6be69.jpg?1402995526";}i:6;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/099/original/062748463f9c9827d5a34fb693912e1c.jpg?1402995670";}i:7;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/001/129/original/d61c4179b1d095ea8e7a46a433d90bb8.jpg?1402995715";}}', 'https://www.samsung-security.com/products/security-cameras/network-cameras/network-box-cameras/snb-6010.aspx');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd732f-e-discontinued', 'DS-2CD732F(-E) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '704x576', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/300/original/5b6d8846da66ac847b55db91e7b6c067.jpg?1400766313";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/301/original/a7f3baf36322772e77a4d5edab1b6c7a.jpg?1400766314";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/302/original/e2a37e2a8b502a18a46b8c519cccdc5b.jpg?1400766315";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd762mf-i-fb-h-discontinued', 'DS-2CD762MF-(I)FB(H) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/297/original/9c0c9abecdbe6cb492963c7ade3fdb4d.jpg?1400766035";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/298/original/00b1e9e0c0a53f610cdfd41a16787aa3.jpg?1400766036";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/299/original/4357edd260c29299366e8ca5b22f4e56.jpg?1400766036";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd752mf-i-fb-h-discontinued', 'DS-2CD752MF-(I)FB(H) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/294/original/3fc05571c5d300033e5b18e6a581ccc8.jpg?1400765371";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/295/original/c2de4868bfb10f4bad59b653683f1185.jpg?1400765372";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/296/original/929886e3e77a6542f4a777725458fe4a.jpg?1400765373";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd702-slash-712p-n-f-e-discontinued', 'DS-2CD702/712P(N)F-E [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/291/original/0d2c68964583f49cd177790be78f42cc.jpg?1400765070";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/292/original/d6b90a3ae8cb1b4c98664528831ef071.jpg?1400765071";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/293/original/7351d40a80122403a6bfa92039d733a7.jpg?1400765072";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd832f-e-discontinued', 'DS-2CD832F(-E) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '704x576', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/288/original/dab1a9ff646585bcd9243b8494625a99.jpg?1400764788";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/289/original/cb508b69c6e1584b65b2a53ee7dcb2d1.jpg?1400764790";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/290/original/f104fc75e53ef948bffcbde3dae92b06.jpg?1400764791";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd752mf-e-discontinued', 'DS-2CD752MF-E [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '', 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/285/original/79069152a32eff067cabf7684d66e90e.jpg?1400764473";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/286/original/4eea2c8e089239d02ad0c8b4b3b2ddf5.jpg?1400764475";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/287/original/36d9b5c75c549676d8fe1b96852a9407.jpg?1400764476";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8133f-e-discontinued', 'DS-2CD8133F-E [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/282/original/a15eb64ea42601f2d2519ed4e8dd7fac.jpg?1400764158";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/283/original/8df6abb319373f8839ebd9fe4d1a466b.jpg?1400764159";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/284/original/6e4e3b727b3aa7b93855ea494ff9879b.jpg?1400764159";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd763p-n-f-e-i-z-discontinued', 'DS-2CD763P(N)F-E(I)(Z [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/279/original/d7cda464271f69d2e25eb13d14a8a73a.jpg?1400763947";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/280/original/54cd62020ef7902fe9667fcbab5bbfb3.jpg?1400763948";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/281/original/929c717fc9e83181aee9a12e3d74b10f.jpg?1400763949";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7133-e-discontinued', 'DS-2CD7133-E [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/276/original/d023b4ece9addbf0be25e52106444618.jpg?1400763639";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/277/original/79c87ec1be00f30235ee41b0b2d7e7b5.jpg?1400763641";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/278/original/d390c48d220b6be2b9316aa117c954a4.jpg?1400763641";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7353f-e-i-discontinued', 'DS-2CD7353F-E(I) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/273/original/b732f052d444966a6eba105c25955349.jpg?1400763358";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/274/original/b74bd284cadb43cdc37eed5d04151e0b.jpg?1400763359";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/275/original/046f65e4402a6fa73d515f8b1dab7ac1.jpg?1400763361";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir1-discontinued', 'DS-2CD802/812/892P(N)-IR1 [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/270/original/c744265cb977d4d4cb3956d9e642f7ed.jpg?1400761776";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/271/original/8a72a596968f09966dd29c85f6c5b357.jpg?1400761777";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/272/original/c8ec45fc38ea40c108aee1ec54318d69.jpg?1400761778";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir3-discontinued', 'DS-2CD802/812/892P(N)-IR3 [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/267/original/d0f4e9643e503406132b8cf638a616df.jpg?1400761572";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/268/original/4615db6b61a959170b4b3c5e01a6594d.jpg?1400761574";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/269/original/4a3193a3fbfe186945d01ee7ced31f3c.jpg?1400761576";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812-slash-892p-n-ir5-discontinued', 'DS-2CD802/812/892P(N)-IR5 [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/264/original/77d865905ff0d1b7756c6f1e66bc883e.jpg?1400761206";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/265/original/a53861ac30646d6f2bddf23942f5bced.jpg?1400761207";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/266/original/25a9c436d17d56a32b22a99125b9a621.jpg?1400761208";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd802-slash-812p-n-f-e-w-discontinued', 'DS-2CD802/812P(N)F-E(W) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Box', 'admin', '12345', '', 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/261/original/cfdfea0ccb8f7b57773e961c497652c1.jpg?1400760828";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/262/original/275ab5a16c97896d050cd1f28814374f.jpg?1400760829";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/263/original/138d4aca6c71f13d0387e8c38ae1a4c3.jpg?1400760830";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd863p-n-f-e-w-discontinued', 'DS-2CD863P(N)F-E(W) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/258/original/99ecae7c3486ea72f45cb244249890f9.jpg?1400760422";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/259/original/7e3e64f25d7654b3b85b6c1c70c37395.jpg?1400760424";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/260/original/dc20f2d1eeee710e1a9096e3dfd9af1c.jpg?1400760424";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd876bf-mf-discontinued', 'DS-2CD876BF(MF) [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/255/original/d2862adaec946c8c5f8cbb81a552c577.jpg?1400760162";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/256/original/9e97d3190509a4ed1149542bf9ed6963.jpg?1400760164";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/257/original/1a59bd23ed67dec2250411f93cd00a64.jpg?1400760165";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8153f-e-discontinued', 'DS-2CD8153F-E [DISCONTINUED]', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/252/original/7a3a56e1ad81dec5c2da764fe4b5be0d.jpg?1400759789";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/253/original/17cde23a1880086b036daec4c7184e05.jpg?1400759791";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/254/original/d1e13748a53032a9bf7969eb6377ad4c.jpg?1400759791";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd893pf-e-version-no-dot-1', 'DS-2CD893PF-E version no.1', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '704x576', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/249/original/0be0eeeb5c159fb1bec9e050213498a9.jpg?1400758852";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/250/original/8aabeddbe4f61287f3216f5b141dfcd3.jpg?1400758854";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/251/original/f5e3de9e85766a57dc1b5e71d48c0481.jpg?1400758855";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2015-i', 'DS-2CD2015-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/246/original/9837eafc4dadd47938a00c7a3b1d6c04.jpg?1400758477";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/247/original/44f7bab41e3dbe538879071537bf9505.jpg?1400758478";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/248/original/3464c96ffff72f85b42b4e54d6ca3bbb.jpg?1400758479";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2035-i', 'DS-2CD2035-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/243/original/1a7a38ca80656b3205b1977bf227416e.jpg?1400758252";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/244/original/30c7c5e532c9bfb01dd2114d69b1fc93.jpg?1400758253";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/245/original/979e1fa2a278858b18732eda01073548.jpg?1400758254";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2135-i', 'DS-2CD2135-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/240/original/94fc0f1e826b3e84bd2184bb9fa06554.jpg?1400757977";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/241/original/6b189a7e64d719bce169ace2e6eff843.jpg?1400757977";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/242/original/96e824edd95939e6c47e565e00fd66c0.jpg?1400757978";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2115-i', 'DS-2CD2115-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/237/original/d76ee0c6e313ed3163c2ac6f492890b2.jpg?1400757729";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/238/original/c98db04d14864f4d702ec6b4c111740e.jpg?1400757730";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/239/original/49c8d790579d8b3fee4405536e3b406d.jpg?1400757731";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2615-is-r', 'DS-2CD2615-IS(R)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/234/original/d326b9f0a72286e4275c80b80358890d.jpg?1400757379";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/235/original/6e722a43cc473f3b348ae6daa09455c7.jpg?1400757379";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/236/original/2eb808fc03fbfcd6906131ad78b3b7e5.jpg?1400757381";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2715f-is-r', 'DS-2CD2715F-IS(R)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/231/original/52752596034fbca07ea4dc3cf590a06b.jpg?1400757081";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/232/original/b2587688a647ce84f706daadb613b4cb.jpg?1400757083";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/233/original/48d8be83c8373782c4900adc26823fec.jpg?1400757084";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2515-iws-r', 'DS-2CD2515-IWS(R)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/228/original/cc653cb8f5b5414e2b2387f2b543b771.jpg?1400756666";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/229/original/74293e68752677b38a22ab1784001615.jpg?1400756667";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/230/original/74c709528e9da3de0fbc085e76fe2522.jpg?1400756668";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2415f-iw-r', 'DS-2CD2415F-IW(R)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/225/original/fa3a0c64434e56a68c7df91c15711861.jpg?1400756314";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/226/original/7998e816973c04347aa32c6794297ad3.jpg?1400756315";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/227/original/87b844158e4ed79c1645dc189e5e378c.jpg?1400756315";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd6412fwd', 'DS-2CD6412FWD', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/222/original/543fdae772ebd9578bf54905b94b216c.jpg?1400756062";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/223/original/3c621ae2d6b4e6d52c3e2789dd785856.jpg?1400756064";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/224/original/fae2f09436f8420abbb195e1eac786c0.jpg?1400756065";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8464f-eiw', 'DS-2CD8464F-EIW', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/219/original/1fadad8f0c0ec3c05942e6037127dda5.jpg?1400755629";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/220/original/07e6cdffd4379e534b0a4ea8620a3cb4.jpg?1400755631";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/221/original/fc499a85a25f4fd64056f2e9795576f8.jpg?1400755632";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2412f-i-w', 'DS-2CD2412F-I(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/216/original/583cf4dc71ba4b87d37e29a26ca896a2.jpg?1400755021";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/217/original/4776a3b41dc320c5608a0faf86f95f25.jpg?1400755022";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/218/original/1177151d35a55a8fc8b773023fdc18b6.jpg?1400755025";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2432f-i-w', 'DS-2CD2432F-I(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Box', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/213/original/bd84b479d6e0f649ab77cf636ef343f0.jpg?1400754602";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/214/original/16f287b5a39e46edf324951cf8250bf3.jpg?1400754605";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/215/original/c94853e24fe67d489645589584df6647.jpg?1400754607";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8233f-ei-z', 'DS-2CD8233F-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Bullet', 'admin', '12345', '', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/208/original/96cf7d7e16387e95ce802ee6e47e723d.jpg?1399905653";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/209/original/85e50d34f9ef90660d710ede55bb001e.jpg?1399905654";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/210/original/4c05dae4b6741d011c12c660c3ba5b93.jpg?1399905655";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8264f-ei-z', 'DS-2CD8264F-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/202/original/bf1af63024025c3c2cbafb0fa94e77f1.jpg?1399905760";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/203/original/84d704ae98c2dd8d850e19ccc497e297.jpg?1399905762";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/204/original/015b4bf501e6c98c652cd731f1dd43c1.jpg?1399905764";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8255f-ei-z', 'DS-2CD8255F-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/199/original/706525f2894e84a9ce467a7b2529b482.jpg?1399905935";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/200/original/44ee568c24db01d5fce6fdb2b0ced179.jpg?1399905937";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/201/original/0afc29dd91970e76cfdd86e766f00549.jpg?1399905938";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8254fwd-ei-z', 'DS-2CD8254FWD-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/196/original/04ed9f8f86416ee11a7cc0f4f60e3b9a.jpg?1399905980";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/197/original/64528be2fbf4c279d3c200ec573ef51e.jpg?1399905981";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/198/original/316a67e995b20d463e804f0436f0343b.jpg?1399905983";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8254f-ei-z', 'DS-2CD8254F-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '', 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/193/original/370c32020ae1d0413ef399ee05bc5332.jpg?1399906022";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/194/original/3fc461ed68754d3dd1418b4697ff8042.jpg?1399906023";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/195/original/07d6983a19421c07c16a47351045b0b4.jpg?1399906024";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd855-ei3-ei5', 'DS-2CD855-EI3(EI5)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/190/original/593b802a85fa956ab55b59aba6ce89b4.jpg?1399906343";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/191/original/fcaa38393e423021f3dfcfb3a22ca06c.jpg?1399906344";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/192/original/e423b086558853582c03652732d95403.jpg?1399906345";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd8253f-ei-z', 'DS-2CD8253F-EI(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Bullet', 'admin', '12345', '', 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/187/original/610991a54beae98aba5ab12814e3c417.jpg?1399906162";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/188/original/23a34071dc43094e95dc1f56cf362cf8.jpg?1399906163";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/189/original/fd86e0608eb434e6f8ebbc8528e0babd.jpg?1399906164";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd864-ei3', 'DS-2CD864-EI3', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/184/original/371caaef94e4b3deb5f08cb5a5302afa.jpg?1399906503";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/185/original/8ebe0f29edc4597ecb1ed726d36b5ab4.jpg?1399906504";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/186/original/71d5c62a61f0f18acae38312f8abf083.jpg?1399906505";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2012-i', 'DS-2CD2012-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/181/original/5ee7a59d46601645ced2052abe85cb2d.jpg?1399906706";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/182/original/cc978867440f322eb2a280d4ed0a6d2e.jpg?1399906707";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/183/original/9939e5d7ba214f85500665f7085226e1.jpg?1399906708";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2032-i', 'DS-2CD2032-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/178/original/64488d69e5fc8851cbe3a140c40aed11.jpg?1399906772";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/179/original/24f67fa6fc4b13397e07b84a07e69f40.jpg?1399906773";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/180/original/18ff715624e530047917a06c4765b028.jpg?1399906774";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2612f-i-s', 'DS-2CD2612F-I(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/175/original/75c07559c09a6c95469fc16a7ae345a8.jpg?1399907020";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/176/original/29412e8b941175508b78e09e26ab0b7d.jpg?1399907022";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/177/original/854cfa3343e6c83d1ab90f0625c9edc8.jpg?1399907023";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2632f-i-s', 'DS-2CD2632F-I(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/172/original/67d57743afef3ee307db455c850884b0.jpg?1399907092";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/173/original/24f170ddd5e9490eec66c28fa917e432.jpg?1399907093";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/174/original/4b74ffce8a4343c8bc73f7ade74f6c70.jpg?1399907094";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2212-i5', 'DS-2CD2212-I5', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/169/original/583362ad4abf153da69dbbfbcc2c4730.jpg?1399923614";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/170/original/dc4e109c97175495753249bcd374b875.jpg?1399923615";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/171/original/95fe0a2feb9c0e4e1686b8fd32553f85.jpg?1399923616";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2232-i5', 'DS-2CD2232-I5', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/166/original/2c28eb8b3f735e01cc586569ca1c93ee.jpg?1399923652";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/167/original/d3f3b14906d7f7b9c3d7d032de6856d7.jpg?1399923653";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/168/original/4e13faf8da7c065ba1621620bfd9a302.jpg?1399923654";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7164-e', 'DS-2CD7164-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/163/original/de92569f09c9187dfa161af7b302971b.jpg?1399924877";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/165/original/fe2b3e61e30751533288599cb4333f68.jpg?1399924878";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/164/original/aa4b20c3f41b72d81b78629a213db8c4.jpg?1399924879";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7153-e', 'DS-2CD7153-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/160/original/11f2cb68dd5ff40641c754eba8f71cdb.jpg?1399924937";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/162/original/1f25190e56e10ebc0a55563dda9f8366.jpg?1399924937";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/161/original/9f282be5f77d9322d715cc5c6bf8a9fe.jpg?1399924939";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2512-i-s-w', 'DS-2CD2512-I(S)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/157/original/924fd32d1957fca89e3034de63c6c449.jpg?1399925153";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/158/original/b2348b3aa052e8867ebf97b271b06365.jpg?1399925155";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/159/original/cad971a9670b474d676e97243455517d.jpg?1399925156";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2532-i-s-w', 'DS-2CD2532-I(S)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/154/original/7bca088af3d37503823ca8cc036249bf.jpg?1399925194";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/155/original/279c6c09abb06e6d7234e66899b28dbd.jpg?1399925195";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/156/original/46c2f017c4ca2211beaa4b1e15a7720f.jpg?1399925196";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7233f-e-i-z-h', 'DS-2CD7233F-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/151/original/6765d6c0a9c4b86d78a541aaffb7772d.jpg?1399925498";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/152/original/afca6d23dde581218b886e98335f0ff9.jpg?1399925500";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/153/original/4ecaf9e0fc5e47bf617d04bcbb7c6db3.jpg?1399925502";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7264fwd-e-i-z-h', 'DS-2CD7264FWD-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/148/original/973a14c74969783c3eb1765f5d86f97a.jpg?1399925534";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/149/original/26a512ec7da749bfebe0d3f2c3eee647.jpg?1399925536";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/150/original/06e99002375ad2b363fe0082d006c4c5.jpg?1399925537";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7255f-e-i-z-h', 'DS-2CD7255F-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/145/original/205eb43b6a19644221ee2e59595fe4f3.jpg?1399925568";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/146/original/d4cc7a0e9f0b105cbcde838edd7f9681.jpg?1399925569";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/147/original/ed1a62125039d450543ffdbae421a22b.jpg?1399925571";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7253f-e-i-z-h', 'DS-2CD7253F-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/142/original/5641c234e1e43a5e0ec554092faa682d.jpg?1399925604";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/143/original/8c7fc934d48c40e1fa1b757cc49e8d57.jpg?1399925606";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/144/original/b622d4028d02ef82d19dac3b2193c908.jpg?1399925607";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7254fwd-e-i-z-h', 'DS-2CD7254FWD-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/139/original/1fa95c5ffc0744aa6fd21b79d95a88a0.jpg?1399925720";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/140/original/e027a09d4e2e3b930be3e775eff4532c.jpg?1399925722";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/141/original/9efd1edeb0ada928f76218e61e1a8fc5.jpg?1399925723";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7283f-e-i-z-h', 'DS-2CD7283F-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2560x1920', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/136/original/20d5298316460fb6c8c9771367cdc280.jpg?1399925759";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/137/original/089985cad9bd9ced93c921088c11893e.jpg?1399925760";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/138/original/4a8bfdbf80eefe5453466483dc081cd0.jpg?1399925761";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd7254f-e-i-z-h', 'DS-2CD7254F-E(I)Z(H)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/133/original/52e2469862ebe3b81d754ee77caf751c.jpg?1399925794";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/134/original/a73252726ed6ded396a54d25f377c140.jpg?1399925795";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/135/original/3474004e44fb00f0861108dd9435663e.jpg?1399925797";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2312-i', 'DS-2CD2312-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/130/original/5dc87da0a1467d8bd6182d56ebab181f.jpg?1399925974";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/131/original/796dc4bc14e8d837bcbd01ec0cac8e26.jpg?1399925975";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/132/original/d9fb9f1c4d2fe74483e21b84ce2f87df.jpg?1399925976";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2332-i', 'DS-2CD2332-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/127/original/da63d42470e9fccc83a3e3a86796d3ea.jpg?1399926025";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/128/original/2e1b927eb3432fe8ef61515131efd010.jpg?1399926028";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/129/original/7528c7c40e297a9bd499275df4e3f973.jpg?1399926030";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2132-i', 'DS-2CD2132-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/124/original/869adfeafd4c07bec995cac34345d6c1.jpg?1399926235";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/125/original/470b62f84fe89945591cfcc63bfe4bbd.jpg?1399926236";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/126/original/3abf69fcf071467c6c553d9b0be47d43.jpg?1399926237";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2112-i', 'DS-2CD2112-I', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/121/original/4b95885ad45e86c999aef74220c9f40a.jpg?1399926275";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/122/original/614e07958149acbba0aac1c985c36431.jpg?1399926276";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/123/original/13408ec6e94486f7cb8bd00cfa9c4dc2.jpg?1399926277";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2732f-i-s', 'DS-2CD2732F-I(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/118/original/e364e6c990180345f741c431d4843e79.jpg?1399926521";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/119/original/c2eb66a39b1b58abce1d29c6bb0fbb94.jpg?1399926522";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/120/original/71e88959082c165e8b6cc9710b98899e.jpg?1399926523";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd2712f-i-s', 'DS-2CD2712F-I(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/115/original/f0237faca3d73991135d05e6002751ea.jpg?1399926563";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/116/original/2a6ca9b6f6813d131cfb40c635ce5af8.jpg?1399926564";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/117/original/27878c01658397ae3a95fcedbcd16955.jpg?1399926565";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd733f-e-i-z', 'DS-2CD733F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/112/original/b2af196daf27800760ad539a20dc53f0.jpg?1399926840";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/113/original/d71569365df5a39cc6dd0eedeadd2132.jpg?1399926841";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/114/original/64508f61078ef35563fb100b21839b5a.jpg?1399926843";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd793p-n-f-e-i-z', 'DS-2CD793P(N)F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/109/original/9137fecb4905a77ce71cde792f34c4c6.jpg?1399926917";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/110/original/0a31d8f4bfb554bb3edcbbc32484edd7.jpg?1399926918";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/111/original/45bcf1abaa840d5aa48fcb0a924f7ecc.jpg?1399926919";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd793p-n-fwd-e-i-z', 'DS-2CD793P(N)FWD-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/106/original/5aa56d79554281f62591b1eb8d391777.jpg?1399926957";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/107/original/0f3abb837942ed932a117e36fbd76d14.jpg?1399926958";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/108/original/72c058d0116d628eaaea7c3d19c319cc.jpg?1399926959";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd764f-e-i-z', 'DS-2CD764F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/103/original/21a5de6b89e20ce47c036d8587948900.jpg?1399927015";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/104/original/e24408c31e784df349e062adaf48fe11.jpg?1399927016";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/105/original/0a95be49171fa8f7c1b72142ce0b581c.jpg?1399927017";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd764fwd-e-i-z', 'DS-2CD764FWD-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/100/original/c27255fd5f6e58b387c1a995d4368ccc.jpg?1399927048";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/101/original/3915d57994a9127e9125b599eae0acff.jpg?1399927049";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/102/original/f7b3a401e076105c374e4a1e6f146a20.jpg?1399927051";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd753f-e-i-z', 'DS-2CD753F-E(I)(Z', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/097/original/652153bae5b7f82aba613ad6c69af89f.jpg?1399927082";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/098/original/90282f2a8cbdb07e63d72f358fc88d23.jpg?1399927083";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/099/original/1521ef907a6bcd791cde49152abbdce0.jpg?1399927084";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd755f-e-i-z', 'DS-2CD755F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/094/original/6f2adf3d94a1a8bd8779c9ef370a898d.jpg?1399927125";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/095/original/4f251bf16098d267f5bdca00ccc37563.jpg?1399927126";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/096/original/7d04079fc13902780320fbac549fe653.jpg?1399927127";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd754fwd-e-i-z', 'DS-2CD754FWD-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/091/original/87c2fdf14e0a9eda0fba026912820eeb.jpg?1399927332";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/092/original/7c54c4ef131454164db817dc4ac10f0c.jpg?1399927334";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/093/original/516eb459dd0373a4e9c1a92b7d06ec92.jpg?1399927337";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd754f-e-i-z', 'DS-2CD754F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/088/original/c33c1667dee8d9a9258cb25200d52e13.jpg?1399927370";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/089/original/4190e3c8b17d07737bace2b7f2605982.jpg?1399927371";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/090/original/23c5ab4c3e7040cd70f42e7509bfd2e4.jpg?1399927372";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd783f-e-i-z', 'DS-2CD783F-E(I)(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2560x1920', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/085/original/350d5e9047527695f45bdf9b41d7e786.jpg?1399927416";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/086/original/431ca82024eb10992a2508c9b62c01b5.jpg?1399927417";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/087/original/8cd29be85cf426cf0dfb18b0e9a61cfa.jpg?1399927418";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd833f-e-w', 'DS-2CD833F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '640x480', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/079/original/e6839f8536d200336ae7d53c50505441.jpg?1399927792";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/081/original/614a1f8ea156303f9ab9bd2760c6ec7a.jpg?1399927793";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/080/original/6865957b55603013906b4f8e65863fee.jpg?1399927794";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd893pf-nf-e-w', 'DS-2CD893PF(NF)-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/076/original/18c896664fd3a08580c4b0e047154d19.jpg?1399927847";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/077/original/78c8aa0bbe0b993856d32ad01d47229b.jpg?1399927848";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/078/original/92e32c7c4e1b0150ef72cbec51ac1a30.jpg?1399927849";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd893pf-nf-wd-e-w', 'DS-2CD893PF(NF)WD-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', 'pal:704x576;ntsc:704x480', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/073/original/843949b28b20a7ac1693f50378642639.jpg?1399927899";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/074/original/9f3d7b537c522c88d96cc9e4cb331237.jpg?1399927900";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/075/original/de6f4835a25edff31cc28035b87ac5e6.jpg?1399927901";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd864f-e-w', 'DS-2CD864F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/083/original/2518438b04f5e2623e2eeb36918f2d9a.jpg?1399928195";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/082/original/61adbac30378c9cd0412dddcde562a38.jpg?1399928196";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/070/original/b3ad390f16ff2e470caddbf8662070b7.jpg?1399928197";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd864fwd-e', 'DS-2CD864FWD-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/068/original/1fa40eac46bda6f06dceb740eb6bb6ee.jpg?1399928243";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/069/original/cc77cf6e54e377734347018a1d89761c.jpg?1399928244";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/067/original/6e010752ae61650c6edc7204eaecbd6d.jpg?1399928245";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd853f-e-w', 'DS-2CD853F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1600x1200', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/064/original/f4472e40169b92a2b594e52b66822bca.jpg?1399928277";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/065/original/255ebc0e8f4d0ac5f7edf71c6c671032.jpg?1399928278";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/066/original/b135c593cf148b5101d8c72864c690e6.jpg?1399928279";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd855f-e', 'DS-2CD855F-E', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/062/original/7ef142e588f6eefc3ce2a36e2c544b3b.jpg?1399928318";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/061/original/501c7c8d9ab45373c88d900ebdde51c8.jpg?1399928317";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/063/original/6501da183f2004f112a7cf8eace00d0b.jpg?1399928319";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd854f-e-w', 'DS-2CD854F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/059/original/d12db0a0320b8c64ad9225a175e4502c.jpg?1399459603";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/060/original/67a030187b4532d931a6f704609bd5ac.jpg?1399459604";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/058/original/abbdfd8efdf4817de2dbd320c26b972b.jpg?1399891560";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd883f-e-w', 'DS-2CD883F-E(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2560x1920', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/056/original/2247e2147f7b37b611f33fcc0a2d4b96.jpg?1399928457";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/057/original/d3bc75b4532f77ee52959ec0651ddec3.jpg?1399928458";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/211/original/c71dd537cf7e3a2cf76abe85d20a8ae9.jpg?1399928459";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4324f-i-z-h-s', 'DS-2CD4324F-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/049/original/56830016e49c7573e6ce75cf4d700b28.jpg?1399928679";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/050/original/f38bd2a1ed581fa4de329ca8bb60d706.jpg?1399928680";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/051/original/9e53b53ef5e3dab13edc90a6e2f54cab.jpg?1399928681";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4312fwd-i-z-h-s', 'DS-2CD4312FWD-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/046/original/42ade7680071930d93a79f8447d59425.jpg?1399928720";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/047/original/306766bc28699357a31a4f64cf5fb9f8.jpg?1399928721";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/048/original/e9f8f8c016a622e1ee95605915e3dd97.jpg?1399928722";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4312f-i-z-h-s', 'DS-2CD4312F-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x1024', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/043/original/62e374cd18a65aaeb522fb74b9f0d648.jpg?1399928759";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/044/original/c1951161976d97169de31a39e1ad3018.jpg?1399928760";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/045/original/03b5f302c64dabff48c8682cfa0dc6a6.jpg?1399928761";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4232fwd-i-z-h-s', 'DS-2CD4232FWD-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/039/original/f2365a8121a5ec2be94f8a3b459f5266.jpg?1399929089";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/041/original/393c7b9245977e943b387f09577be269.jpg?1399929090";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/042/original/48f84d1bef0f4251052fbfc01a6d4d5e.jpg?1399929092";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4224f-i-z-h-s', 'DS-2CD4224F-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/035/original/5476ab7403d5f0b052429ac0a2442ac8.jpg?1399929161";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/037/original/c9c8ffabe63cc30f2e4d6f0d6c158973.jpg?1399929162";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/038/original/018e9cd7bff1f457b6f8ad35fc07d156.jpg?1399929163";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4212fwd-i-z-h-s', 'DS-2CD4212FWD-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/031/original/9382d30ad11d59189b0ffc69bc26ee1f.jpg?1399929203";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/032/original/89741e330d702f2fa5ef5a81d347f6ed.jpg?1399929204";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/033/original/406525f0bb284443e0cd394e4ec58f11.jpg?1399929205";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4212f-i-z-h-s', 'DS-2CD4212F-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x1024', 'Bullet', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/028/original/1f4912ad1b9719f7e6d5880ab5c5c0f9.jpg?1399929238";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/029/original/29a41e9e8fafe27d538a111aa8820274.jpg?1399929239";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/030/original/0888c5d78a6acc0c94e251e05651cef7.jpg?1399929240";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4332fwd-i-z-h-s', 'DS-2CD4332FWD-I(Z)(H)(S)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/025/original/da1021fa9dc3ce4481df18cff29fe6f9.jpg?1399928936";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/027/original/b9715d9a515e0298a6ee3c5c49fd00dc.jpg?1399928937";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/084/original/5e1741d1f4f2ed35056944a427b431f1.jpg?1399928938";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4132fwd-i-z', 'DS-2CD4132FWD-I(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Dome', 'admin', '12345', '1', 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/022/original/f47e167c3f279beed46e8c97033c89b8.jpg?1399929544";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/023/original/e6c0d567161c4da6e3d610144dd4ea53.jpg?1399929545";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/024/original/943ebbbeec2b3c6e58ba0f5da416597f.jpg?1399929546";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd6026fhwd-a', 'DS-2CD6026FHWD-(A)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/019/original/86236e90bcb04bb8b1b4481c2d0b1c6a.jpg?1399929949";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/020/original/4d412918460faa5b3300357f916183c9.jpg?1399929950";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/021/original/962670641ab8d8820404ea805467a047.jpg?1399929951";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4124f-i-z', 'DS-2CD4124F-I(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/015/original/4c094201fc40f7826abaf7b8a4354aba.jpg?1399929603";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/016/original/c263fa116e7f1845eb88c6c4fdf7cf11.jpg?1399929604";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/017/original/1566e3b48f14ceb6ecaea3a4b5a44747.jpg?1399929604";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4112f-i-z', 'DS-2CD4112F-I(Z)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x1024', 'Dome', 'admin', '12345', '1', 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/012/original/1674447cbb0fa91dfd4c788665b4c76d.jpg?1399929643";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/013/original/27094910c19642c062e50555bbafd051.jpg?1399929644";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/014/original/e8e0329ec8cdba4a1eb89fbf1cacd778.jpg?1399929645";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4032fwd-a-p-w', 'DS-2CD4032FWD-(A)(P)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '2048x1536', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/010/original/cedb1b336020c37140d521c3560c1cd2.jpg?1399929740";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/011/original/753efaef77b2ff17c3d99bd860c55ace.jpg?1399929742";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/055/original/83b3ef750fd8fc04d5bfd97544ab13f1.jpg?1399929745";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4024f-a-p-w', 'DS-2CD4024F-(A)(P)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1920x1080', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/008/original/72e5ca6557f936e43f292f1c40ccb351.jpg?1399929791";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/009/original/770aa622af1b70de3727c89d3ea0db3e.jpg?1399929792";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/212/original/3719fb570837f2290256705541a989bf.jpg?1399929793";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4012fwd-a-p-w', 'DS-2CD4012FWD-(A)(P)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x960', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/005/original/be65f6f1e553424e01e6aad76f96e586.jpg?1399929841";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/006/original/0d4c1b5f99dab20fb5510b3173deb0aa.jpg?1399929842";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/007/original/b1bd327bdb9dbe6a43140b1b179a4bda.jpg?1399929843";}}', '');
INSERT INTO "cameras" 
VALUES (NULL, 'ds-2cd4012f-a-p-w', 'DS-2CD4012F-(A)(P)(W)', '37', 'Streaming/Channels/1/picture', '', 'h264/ch1/maiv_stream', '1280x1024', 'Box', 'admin', '12345', '1', 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 'a:3:{i:0;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/002/original/6cf04184a4de06d1c746e1f9137f39a5.jpg?1399929880";}i:1;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/003/original/fe681fc166d93de008c232b5bda20203.jpg?1399929881";}i:2;a:1:{s:3:"url";s:115:"http://s3.amazonaws.com/cambaseio/images/files/000/000/004/original/23c9b7642376e042e30fbff4cac947cb.jpg?1399929882";}}', '');
