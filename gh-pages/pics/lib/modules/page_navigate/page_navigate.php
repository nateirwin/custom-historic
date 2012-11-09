<?php

require_once "Preferences.php";
require_once "Orders.php";

$prefs = new Preferences();
$maxOrdersPP = $prefs->searchResultMaxPP();
                                                                                                                                               
$orders = new Orders();
/*Find out all the Orders exist in the Orders table and Calculate the Number of Pages needed to display them*/
$allOrders = $orders->max();
$totalPages = ceil ( $allOrders / $maxOrdersPP );

$smarty->assign("totalPages",$totalPages);

?>
