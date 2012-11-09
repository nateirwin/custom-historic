<?php
ob_start();

require_once "../lib/init/StoreSetup.php";
incCommServs();
require_once "StoreSession.php";
StoreSession::logoutPage();

/*include the graphics of the page*/
incAdmin();
require_once "logout.php";

ob_end_flush();
?>
