<?php

incSmarty();
incUtil();
require_once "ReceiptCalcs.php";
require_once "SmartyStore.php";
require_once "ReceiptCalcs.php";

class ShoppingCart {

	function addToCart ($productId,
						$quantity,
						$productName,
						$price,
						$secure) {
		
		$basket = ShoppingCart::getCart();
		$cart = "";
		$changeProduct = "";
		$i = 0;
		$flag = 1;
		if( $basket != null ) {	
			$size = sizeof($basket);
			for ($i=0; $i<$size;$i++) {
				/*add the same item to the cart and modify cookie['cart'] string*/
			    if ($basket[$i][0] == $productId) {
					$basket[$i][1] = $basket[$i][1]+$quantity;
					$basket[$i][3] = ReceiptCalcs::calcSingleEntry($basket[$i][1],$price);
			        $changeProduct = "#".$basket[$i][0].",".$basket[$i][1].",".$basket[$i][2].",".$basket[$i][3];
			        $cart = $cart . $changeProduct;
					$flag = 0;
				} else {
					/*product has not changed just add to the cookie['cart'] string*/
					$product = "#".$basket[$i][0].",". $basket[$i][1].",".$basket[$i][2].",".$basket[$i][3];
					$cart = $cart . $product;
				}
			}/*for*/
			/*add new item to the cart and modify cookie['cart'] string*/
			if($flag) {
				$lineTotal = ReceiptCalcs::calcSingleEntry($quantity,$price);
				$newItem = array($productId,$quantity,$productName,$lineTotal);
				array_push($basket,$newItem);
				$addProduct = "#".$productId.",".$quantity.",".$productName.",".$lineTotal;
				$cart = $cart . $addProduct;
				//print "addProduct" . $addProduct . "<br>";
				//print "cart" . $cart . "<br>";
			}/*if*/
		} else {
			/*basket is null add first item to the basket cookie['cart']*/
			$addProduct = "#$productId,$quantity,$productName,$price";
			$basket[$i][0]=$productId;
            $basket[$i][1]=$quantity;
            $basket[$i][2]=$productName;
            $basket[$i][3]=ReceiptCalcs::calcSingleEntry($quantity,$price);
			$cart = $cart . $addProduct;
		}
		//print "setcookie = " . $cart ."<br>";
		//print "sizeof".sizeof($basket) ."<br>";
		setcookie ("cart",$cart,0, "/",'', $secure);
		return $basket;
    }/*end addToCart*/	
	

	function deleteFromCart($POST,
							$basket,
							$secure) {
	$cart = "";
	$size = sizeof($basket);
	for ($i=0; $i<$size;$i++) {
		if (isset($POST[$i])) {
			//print "$POST $i    " . $POST[$i];
			/*if delete checked just move the item from array don't add it to cookie string*/
			unset($basket[$i]);
		} else {
			$product = "#".$basket[$i][0].",". $basket[$i][1].",".$basket[$i][2].",".$basket[$i][3];
			$cart = $cart . $product;
		}
	}/*for*/
	setcookie ("cart",$cart,0, "/",'', $secure);
	$basket = array_values($basket);
	return $basket;
}/*deleteFromCart*/
		
		
	function getCartValue() {
		if(!isset($_COOKIE['cart'])) {
			return '';
		} else {
			return $_COOKIE['cart'];
		}
	}
	
    /* return the array representation of the string cookie value*/
    function getCart() {
        //$value = ShoppingCart::getCartValue();
        $value = null;
        $token = strtok(ShoppingCart::getCartValue(), "#");
        $i = 0;
        while ($token) {
            $value[$i] = explode(",",$token);
            $i++;
            $token = strtok("#");
        }
        return $value;
    } /*getCart*/

	function deleteCart() {
		if (isset($_COOKIE['cart'])) {
			setcookie("cart",'',(time()-86400), "/",'', 1);
		}
	}
	
	function subTotal($basket) {
		$size = sizeof($basket);
		$subTotal = 0;
		for($i=0; $i<$size; $i++) {
			if (isset($basket[$i][3])) {
				$subTotal = $subTotal + $basket[$i][3];
			}
		}
		/*TODO not internationalized*/
		return number_format( $subTotal , 2, '.', ' ');
	}

}/*end class*/
?>
