<?php

if (!isset($quantity) || $quantity <= 0) {
	$quantity = 1;
}

if (isset($_COOKIE['cart']) || isset ($_REQUEST['addp']) ) {
                                                                                                                                                
    $basket = null;
    if (isset ($_REQUEST['addp'])) {
        $addp = $_REQUEST['addp'];
        $prInfo = explode('&',$addp);
        $prId=$prInfo[0];
        $prName=$prInfo[1];
        $prPrice=$prInfo[2];
        $basket = ShoppingCart::addToCart($prId,$quantity,$prName,$prPrice,1);
    } else {
        $basket = ShoppingCart::getCart();
    }
                                                                                                                                                
    $subtotal = ShoppingCart::subTotal($basket);
    $smarty->assign("basketItems",$basket);
    $smarty->assign("subtotal",$subtotal);
}
?>
