--
-- Table structure for table: ActiveUsers
--
CREATE TABLE ActiveUsers (
	id integer NOT NULL PRIMARY KEY AUTO_INCREMENT,
	ip TEXT(20),
	from_client BOOLEAN ,
	time INT,
	kick BOOLEAN
);
