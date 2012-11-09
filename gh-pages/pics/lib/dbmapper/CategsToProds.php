<?php

require_once "Connect.php";

class CategsToProds extends Connect{

	var $prodId;
	var	$categId;
	var $cache;
	var $cacheTime;

	function CategsToProds ($categId = 0,
							$prodId = 0, 
							$cache = 0, 
							$cacheTime = 0, 
							$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->categId		=	$categId;
		$this->prodId       =   $prodId;
		$this->cache		=	$cache;
		$this->cacheTime	=	$cacheTime;
	}

	function insertToCTP(	$categId,	
							$prodId		) {

		if ( $this->categId == 0 || $this->prodId == 0 ) {
			$insertQuery = "INSERT INTO ProdsToCategs	(categId,prodId)
								         VALUES			($categId,$prodId)";				
		} else {
			$insertQuery = "INSERT INTO Categories	(categId,prodId) 
										 VALUES		($this->categId,$this->prodId)";
		}
		
		parent::executeQuery($insertQuery);	
	
	} 

	function prodsByCategId($categId) {
		$query = "SELECT CategsToProds.productId,name,price FROM Products,CategsToProds WHERE categId='$categId'  and Products.productId = CategsToProds.productId";
		$prods = parent::query($query);
		return $prods;
	}
	
}/*end class*/


?>
