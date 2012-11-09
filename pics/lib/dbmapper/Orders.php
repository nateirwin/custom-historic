<?php

require_once "Connect.php";

class Orders extends Connect{

	var	$orderId;
	var $cache;
	var $cacheTime;

	function Orders($orderId = 0, 
					$cache = 0, 
					$cacheTime = 0,
					$persistent = 0 ) {
		parent::__construct($persistent);
		$this->orderId     = $orderId;
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}
	/*
	DROP TABLE IF EXISTS Orders;
	CREATE TABLE Orders (
		storeId smallint unsigned NOT NULL,
		orderDate datetime NOT NULL,
		orderId int unsigned NOT NULL auto_increment,
	) TYPE=MyISAM AUTO_INCREMENT=1;
	*/
	function insertToOrders( $storeId,
                            $orderDate ) {
		$sqlOrderDate="FROM_UNIXTIME(unix_timestamp('$orderDate'))";
		if ($this->orderId == 0) {
            $insertQuery = "INSERT INTO Orders (storeId,orderDate) 
										VALUES ($storeId,$sqlOrderDate)";
        } else {
			$insertQuery = "INSERT INTO Orders (storeId,orderDate,orderId) 
										VALUES ($storeId,$sqlOrderDate,$this->orderId)";
		}
		parent::executeQuery($insertQuery);
		$query = "SELECT LAST_INSERT_ID()";
		$rs = parent::query($query);
		return $rs[0]["last_insert_id()"];
		
	} /*end function insertToOrders()*/


	function ordersByDate(	$min,
							$max,
							$maxOrdersPP) {
		$query = "SELECT Orders.orderId,storeId,orderDate,currency,subTotal from Orders,OrderToFinances 
					WHERE Orders.orderId=OrderToFinances.orderId BETWEEN $min AND $max ORDER BY orderDate DESC LIMIT $maxOrdersPP";
		return parent::query($query);
	}

	function max() {
		$maxQuery = "SELECT  max(orderId) FROM Orders";
		$rs = parent::executeQuery($maxQuery);
		$maxInArray = $rs->fields;
		if ( !isset($maxInArray['max(orderId)']) ) { return 0; } 
		else {return $maxInArray['max(orderId)']; }		
	}

}/*end class*/
?>
