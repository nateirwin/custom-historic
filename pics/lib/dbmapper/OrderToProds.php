<?php

require_once "Connect.php";

class OrderToProds extends Connect{

	var	$orderId;
	var $cache;
	var $cacheTime;

	function OrderToProds ( $orderId = 0, 
							$cache = 0, 
							$cacheTime = 0,
							$persistent = 0 ) {
		parent::__construct($persistent);
		$this->orderId     = $orderId;
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	/*OTP stands for OrderToProds*/
	function insertToOTP(	$orderId,
							$basket	) {

		$size = sizeof($basket);

		for ($i=0; $i<$size;$i++) {
			$productId = $basket[$i][0];
			$productQuantity = $basket[$i][1];
			if($this->orderId == 0) {
				$insertQuery = "INSERT INTO OrderToProds (orderId,productId,productQuantity)
												VALUES ($orderId,$productId,$productQuantity)";
			} else {
				$insertQuery = "INSERT INTO OrderToProds (orderId,productId,productQuantity)
                                                VALUES ($this->orderId,$productId,$productQuantity)";
			}
		
			parent::executeQuery($insertQuery);
		}
	} /*end function insertToOTP()*/
    
	function prodInfoByOid ( $orderId ) {
		$query = "SELECT name, price*productQuantity, productQuantity FROM OrderToProds, Products 
					WHERE OrderToProds.orderId='$orderId' AND OrderToProds.productId=Products.productId";
		return parent::query($query);
	}

}/*end class*/
?>
