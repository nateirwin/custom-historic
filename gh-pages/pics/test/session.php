<?php
ob_start();
require_once "../lib/init/StoreSetup.php";
incCommServs();
require_once "StoreSession.php";
print "Login page <br>";
StoreSession::setUser('zorlu');
StoreSession::loginPage();
ob_end_flush();
?>    
