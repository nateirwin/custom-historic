#
#
# Users table (SalesPerson,Waiter,Waitress)
# datecreated 08-17-04
# FIELDS
# userId 256 maximum Users per restaurant's' payroll 
# name 255 max characters email address of the user
# address 255 max characters address of the server
# phoneNo 255 max characters phone number of the server
# contactPerson 255 max characters contact information of the server
#
# TODO: How to find efficiency of a server (order per server type of deal)
# Answer: Join Orders table and Users table with userId
#
#

DROP TABLE IF EXISTS Users;
CREATE TABLE Users (
  title varchar (64),
  fname varchar (64) NOT NULL,
  mname varchar (64),
  lname varchar (64) NOT NULL,
  address tinytext,
  city tinytext,
  state varchar(2),
  zip varchar(16),
  homeTel varchar(32),
  workTel varchar (32),
  cellTel varchar (32),
  contactPerson tinytext,
  contactTel varchar(32),
  passwd varchar(255) NOT NULL,
  email varchar (64) NOT NULL UNIQUE,
  userId smallint unsigned NOT NULL auto_increment,
  PRIMARY KEY (userId),
  INDEX auth(email,passwd)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO Users (title,fname,mname,lname,address,city,state,zip,homeTel,workTel,cellTel,contactPerson,contactTel,passwd,email) 
			 VALUES ('Mr','Hakan','I','Zorlu','6371 E Ohio ave','Denver','CO','80224','(720) 524-4444','','','Rachel Ostraat','303-223-2322',MD5('dude'),'h_izan@yahoo.com');

##############################################################################################################################
##############################################################################################################################
#
#
#
DROP TABLE IF EXISTS Pics;
CREATE TABLE Pics (
	userId smallint unsigned NOT NULL,
	pic varchar(255) NOT NULL,
	picDate datetime NOT NULL,
	adminComment text,
	userComment text,
	PRIMARY KEY (userId,pic)
) TYPE=MyISAM;

##############################################################################################################################
##############################################################################################################################

DROP TABLE IF EXISTS Admins;
CREATE TABLE Admins (
  title varchar (64),
  fname varchar (64) NOT NULL,
  mname varchar (64),
  lname varchar (64) NOT NULL,
  address tinytext,
  city tinytext,
  state varchar(2),
  zip varchar(16),
  homeTel varchar(32),
  workTel varchar (32),
  cellTel varchar (32),
  contactPerson tinytext,
  contactTel varchar(32),
  passwd varchar(255) NOT NULL,
  email varchar (64) NOT NULL UNIQUE,
  adminId smallint unsigned NOT NULL auto_increment,
  PRIMARY KEY (adminId),
  INDEX auth(adminId,passwd)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO Admins (title,fname,mname,lname,address,city,state,zip,homeTel,workTel,cellTel,contactPerson,contactTel,passwd,email)
             VALUES ('Mr','Laurie','L','Laurie','','Denver','CO','80224','(720) 524-45555','','','Laurie','303-553-2322',MD5('umitkaya'),'laurie@customhistoric.com');


##############################################################################################################################
##############################################################################################################################
#
#	
#	 
#

DROP TABLE IF EXISTS Sessions;
CREATE TABLE Sessions (
	sessId char(32) NOT NULL,
	data text NOT NULL,
	lastAccess int(11) unsigned NOT NULL,
	PRIMARY KEY (sessId)
) TYPE=MyISAM;


##############################################################################################################################
##############################################################################################################################

#
#
# max 65535 different configuration/preferences variables.
# name name of the Preference
# value value of the preference that will be used by the user
#
#

DROP TABLE IF EXISTS Preferences;
CREATE TABLE Preferences (
  name varchar(64) NOT NULL,
  description tinytext,
  value varchar(128) NOT NULL,
  prefId smallint unsigned NOT NULL auto_increment,
  PRIMARY KEY (prefId),
  INDEX findByName(name)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO Preferences (name,description,value,prefId) VALUES ('Idle Time','The time (in seconds) before a user can stay Idle before They are required to Login again','3000',2);
INSERT INTO Preferences (name,description,value,prefId) VALUES ('CSS Style Sheet','CSS Style Sheet that is chosen','pages.css',3);
INSERT INTO Preferences (name,description,value,prefId) VALUES ('language','Default language to be used on this site','en',4);
INSERT INTO Preferences (name,description,value,prefId) VALUES ('currency','Default currency to be used on this site','en',5);
