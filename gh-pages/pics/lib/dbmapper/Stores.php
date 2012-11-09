<?php

require_once "Connect.php";

class Stores extends Connect{
	
	var $storeName;
	var $cache;
	var $cacheTime;

	function Stores($cache = 0, 
					$cacheTime = 0, 
					$persistent	= 0 ) {
		parent::__construct($persistent);
		$this->storeName	= STORE_NAME;
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}
	
	/*check for the storeName before Trying to Enter it*/
	function insertToStores($address,    
							$city,
							$state,
							$zip,
							$telephone) {
		
		$insertQuery = "INSERT INTO Stores (name,address,city,state,zip,telephone)
		                 VALUES ('$this->storeName','$address','$city','$state','$zip','$telephone')";
		parent::executeQuery($insertQuery);	
	} /*end function insertToStores()*/


	function storeId(){
		$query = "SELECT storeId FROM Stores WHERE name='$this->storeName'";
		$storeIdArr = parent::query($query);
		$size = sizeof($storeIdArr);
		if ($size == 0) {
			return 0;
		} else {
			return $storeIdArr[0]['storeId'];
		}
	}

}/*end class*/
?>
