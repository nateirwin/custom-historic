<?php

require_once "Connect.php";

class OrderToUser extends Connect{

	var $cache;
	var $cacheTime;

	function OrderToUser (	$cache = 0, 
							$cacheTime = 0,
							$persistent = 0 ) {
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	/*OTU stands for OrderToUser*/
	/*
		orderId int unsigned NOT NULL,
		userId smallint unsigned NOT NULL,
		PRIMARY KEY (orderId,userId),
		INDEX findUsers (userId)
	*/

	function insertToOTU (	$orderId,
							$userId		) {
		$insertQuery = "INSERT INTO OrderToUser (orderId,userId)
										VALUES	($orderId,$userId)";
		parent::executeQuery($insertQuery);
	}

}/*end class*/
?>
