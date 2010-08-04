DROP TABLE IF EXISTS Users;
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
