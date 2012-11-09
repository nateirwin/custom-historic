<?php

require_once "Connect.php";

class Tax extends Connect{

	var $cache;
	var $cacheTime;

	function Tax (	$cache = 0, 
					$cacheTime = 0,
					$persistent = 0 ) {
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}
	/*
	CREATE TABLE Tax (
		storeId smallint unsigned NOT NULL,
		taxZone varchar(128),
		rate decimal(3,4) NOT NULL,
		description tinytext,
		INDEX findStore(storeId)
	) TYPE=MyISAM;
	*/
	function insertToTax($storeId,
							$taxZone,
							$rate,
							$description ) {
		$insertQuery = "INSERT INTO Orders (storeId,taxZone,rate,description) 
									VALUES ($storeId,$taxZone,$rate,'$description')";
		parent::executeQuery($insertQuery);
		
	}


	function taxRate( $storeId ) {
		$query = "SELECT rate from Tax WHERE storeId='$storeId'";
		return parent::query($query);
	}

}/*end class*/
?>
