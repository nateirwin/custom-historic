<?php

require_once "Connect.php";

class OrderToFinances extends Connect{

	var $cache;
	var $cacheTime;

	function OrderToFinances(	$cache = 0, 
								$cacheTime = 0,
								$persistent = 0 ) {
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	/*
		TABLE OrderToFinances (
		orderId int unsigned NOT NULL,
		currency varchar(128) NOT NULL,
		subTotal decimal (15,2) NOT NULL,
		taxAmt decimal (15,2) NOT NULL,
		total decimal (15,2) NOT NULL,
		changeDue decimal (15,2) NOT NULL,
		PRIMARY KEY (orderId)
	*/
	function insertToOTF (	$orderId,
							$currency,
							$subTotal,
							$taxAmt,
							$total,
							$changeDue	) {
			$insertQuery = "INSERT INTO OrderToFinances	(orderId,currency,subTotal,taxAmt,total,changeDue)
												VALUES	($orderId,'$currency',$subTotal,$taxAmt,$total,$changeDue)";
			parent::executeQuery($insertQuery);

	} 

	function finByOid ($orderId ) {
		$query = "SELECT subTotal,taxAmt,total,changeDue FROM OrderToFinances WHERE orderId='$orderId'";
		return parent::query($query);
    }
	
	function currByOid ($orderId) {
		$query = "SELECT currency FROM OrderToFinances WHERE orderId='$orderId'";
		$currency = parent::query($query);
		return $currency[0]['currency'];
	}

}/*end class*/
?>
