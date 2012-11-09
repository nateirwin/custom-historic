<?php
ob_start();

require_once "../lib/init/StoreSetup.php";
incCommServs();
require_once "StoreSession.php";
print "Regular page <br>";
StoreSession::page();

ob_end_flush();
?>    
