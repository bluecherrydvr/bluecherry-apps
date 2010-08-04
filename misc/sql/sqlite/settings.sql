DROP TABLE IF EXISTS GlobalSettings;
CREATE TABLE "GlobalSettings" (
    id integer PRIMARY KEY NOT NULL,
    parameter character(60),
    value character(255)
);
