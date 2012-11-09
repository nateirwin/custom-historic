##   This file contains useful sql queries to be used in the future ##
############################################################################################
##
## ALTER scripts below can be used to reindex a primary key field with auto_increment in case of huge gaps are genrated
## as a result of delete queries
## These can be written as a crown job or something
##
ALTER TABLE menus 
DROP COLUMN menuId;
##
ALTER TABLE menus 
ADD menuId smallint unsigned NOT NULL auto_increment, 
ADD PRIMARY KEY (menuId);
##
############################################################################################
##
##show info
##
SHOW INDEX FROM menus;
##
############################################################################################
##
DESC menus;
##
############################################################################################
##
##what is the number of iterations to get the result of the script
##
EXPLAIN SELECT * FROM menus;
##
############################################################################################
##
SELECT  max(orderId) FROM ServersToOrders;
SELECT  min(orderId) FROM ServersToOrders;
SELECT count(*) FROM ServersToOrders;
##
