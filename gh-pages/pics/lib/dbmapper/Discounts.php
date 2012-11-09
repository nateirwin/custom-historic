<?php

require_once "Connect.php";

class Discounts extends Connect{

	var	$discountId;
	var $cache;
	var $cacheTime;

	function Discounts ($discountId = 0, 
						$cache = 0, 
						$cacheTime = 0, 
						$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->discountId	= $discountId;
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	function insertToOD ($name,
						$classification,        
						$methodInitialCost,    
						$methodPercentageCost, 
						$status) {

		if ($this->discountId == 0) {
			$insertQuery = "INSERT INTO Discounts (name,classification,,methodInitialCost,methodPercentageCost,status)
								         VALUES ('$name','$description','$methodInitialCost','$status','$methodPercentageCost')";				
		} else {
			$insertQuery = "INSERT INTO Discounts (name,classification,,methodInitialCost,methodPercentageCost,status,discountId) 
										 VALUES ('$name','$description','$methodInitialCost','$methodPercentageCost','$status',$this->discountId)";
		}
		
		parent::executeQuery($insertQuery);	
	
	} /*end function insertToOD()*/


	function deleteFromOD ($key,
							$equalTo) {

		if($this->discountId == 0) {
			print "discountId field cannot be ZERO for Deletion. Initialize the discountId in Constructor \n";
			return false;
		}
		$delQuery = "DELETE FROM Discounts WHERE $key = $equalTo"; 			
		 $this->executeQuery($delQuery);
	}/*end function deleteFromOD()*/
	

	function discountMethods() {
		$query = "SELECT discountId,name FROM Discounts WHERE status='1'";
		return parent::query($query);
	}

	function discountName($discountId) {
		$query = "SELECT name FROM Discounts WHERE discountId='$discountId'";
		$discountName = parent::query($query);
		return  $discountName[0]['name'];
	}
	
	function discountAmount($subtotal,
							$discountId) {
		if(!isset($discountId)) {
			return 0;
		} 
		$query = "SELECT discountAmount,discountPercentage FROM Discounts WHERE discountId ='$discountId'";
		$dadp = parent::query($query);
		//print $dadp[0]['discountAmount'] . "<br>";
		//print $dadp[0]['discountPercentage'] . "<br>";
		if(isset( $dadp[0]['discountAmount'] ) && isset( $dadp[0]['discountPercentage'] ) ) {
			if ( ($dadp[0]['discountAmount'] == 0) && ($dadp[0]['discountPercentage'] == 0) ) {
				return number_format( 0 , 2, '.', ' ');
			} elseif ( ($dadp[0]['discountAmount'] != 0) && ($dadp[0]['discountPercentage'] == 0) ) {
				return $dadp[0]['discountAmount'];
			} elseif ( ($dadp[0]['discountAmount'] == 0) && ($dadp[0]['discountPercentage'] != 0) ) {
				return number_format( $subtotal * ($dadp[0]['discountPercentage'] / 100) , 2, '.', ' ');
			} elseif ( ($dadp[0]['discountAmount'] != 0) && ($dadp[0]['discountPercentage'] != 0) ) {
				return number_format( ( $dadp[0]['discountAmount'] + ($subtotal * ($dadp[0]['discountPercentage'] / 100)) ) , 2, '.', ' ');
			} else {
				return 0;
			}
		}
	}


}/*end class*/
?>
