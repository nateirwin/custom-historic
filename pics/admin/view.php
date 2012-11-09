<?php
ob_start();

require_once "../lib/init/StoreSetup.php";
incCommServs();
require_once "StoreSession.php";
StoreSession::adminPage();

incAdmin();
require_once "view.php";

ob_end_flush();
?>
