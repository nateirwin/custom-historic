<?php

ob_start();

require_once "../lib/init/StoreSetup.php";
incCommServs();
require_once "ShoppingCart.php";

//setcookie ("cart","generic",0, "/",'', $secure);
ShoppingCart::putIntoCart(2,"Futomaki","5.95",1) ;
//ShoppingCart::putIntoCart(3,"Shake Maki","4.00",1) ;
//ShoppingCart::deleteCart();
ShoppingCart::printCart();

ob_end_flush();
?>    
