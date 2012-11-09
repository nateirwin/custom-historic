<?php
ob_start();

require_once "../lib/init/StoreSetup.php";
incUsers();
require_once "index.php";

ob_end_flush();
?>
