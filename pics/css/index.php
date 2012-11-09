<?php

//To prevent directory browsing
$hostname = $_SERVER["HTTP_HOST"];
header ("location: http://$hostname");
die;

?>
