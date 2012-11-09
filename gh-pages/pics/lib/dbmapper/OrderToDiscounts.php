<?php

require_once "Connect.php";

class OrderToDiscounts extends Connect{

	var $cache;
	var $cacheTime;

	function OrderToDiscounts(	$cache = 0, 
								$cacheTime = 0,
								$persistent = 0 ) {
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	/*OTD stands for OrderToDiscounts*/
	/*
		orderId int unsigned NOT NULL,
		discountId  bigint unsigned NOT NULL,
		discountAmount decimal(8,4) NOT NULL default 0.00,
		INDEX findOrders(orderId),
		INDEX findDiscounts (discountId)
	*/

	function insertToOTD (	$orderId,
							$discountId,
							$discountAmount	) {
		if( isset($discountId) && ($discountId != 0) ) {
			$insertQuery = "INSERT INTO OrderToDiscounts (orderId,discountId,discountAmount)
													VALUES ($orderId,$discountId,$discountAmount)";
			parent::executeQuery($insertQuery);
		}

	} /*end function insertToOTD()*/

	function discountInfoByOid($orderId) {
		$query = "SELECT name, OrderToDiscounts.discountAmount FROM OrderToDiscounts, Discounts
					WHERE orderId='$orderId' AND OrderToDiscounts.discountId = Discounts.discountId";
		return parent::query($query);
	}

}/*end class*/
?>
