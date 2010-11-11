CREATE TABLE GlobalSettings (
	parameter varchar(60) PRIMARY KEY NOT NULL,
	value varchar(255)
);

INSERT INTO 'GlobalSettings' VALUES ('G_DVR_NAME', 'Bluecherry DVR v2');
INSERT INTO 'GlobalSettings' VALUES ('G_DVR_EMAIL', 'root@localhost');
