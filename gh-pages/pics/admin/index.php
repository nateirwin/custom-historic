<?php
ob_start();

require_once "../lib/init/StoreSetup.php";
incAdmin();
require_once "index.php";

ob_end_flush();
?>
