<?php

require_once "Connect.php";

class PaymentMethods extends Connect{

	var	$paymentMethId;
	var $cache;
	var $cacheTime;

	function PaymentMethods ($paymentMethId = 0, 
							$cache = 0, 
							$cacheTime = 0, 
							$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->paymentMethId	= $paymentMethId;
		$this->cache			= $cache;
		$this->cacheTime		= $cacheTime;
	}

	function insertToOPM (	$name,
							$classification,        
							$methodInitialCost,    
							$methodPercentageCost, 
							$status) {

		if ($this->paymentMethId == 0) {
			$insertQuery = "INSERT INTO PaymentMethods (name,classification,,methodInitialCost,methodPercentageCost,status)
								         VALUES ('$name','$classification','$methodInitialCost','$methodPercentageCost','$status')";				
		} else {
			$insertQuery = "INSERT INTO PaymentMethods (name,classification,,methodInitialCost,methodPercentageCost,status,paymentMethId) 
										 VALUES ('$name','$classification','$methodInitialCost','$methodPercentageCost','$status',$this->paymentMethId)";
		}
		
		parent::executeQuery($insertQuery);	
	
	}


	function paymentCost(	$paymentMethId,
							$total	) {
		$query = "SELECT methodInitialCost,methodPercentageCost FROM PaymentMethods WHERE paymentMethId='$paymentMethId'";
		$paymentCosts = parent::query($query);
		$initialCost = $paymentCosts[0]['methodInitialCost'];
		$percentageCost = $paymentCosts[0]['methodPercentageCost'];
		$paymentCosts = number_format( ( floatval($initialCost) + (floatval($percentageCost) * floatval($total / 100)) ), 2, '.', ' ');
		return $paymentCosts;	
	}	

	/*
	* return 'Cash' 'Gift Card' 'Credit Card' 'Check'
	*/
	function paymentType($paymentMethId) {
		$query = "SELECT classification FROM PaymentMethods WHERE paymentMethId='$paymentMethId'";
		$paymentType = parent::query($query);
		return  $paymentType[0]['classification'];
	}	

	function paymentName($paymentMethId) {
		$query = "SELECT name FROM PaymentMethods WHERE paymentMethId='$paymentMethId'";
		$paymentName = parent::query($query);
		return  $paymentName[0]['name'];
	}

	function paymentMeths() {
		$query = "SELECT paymentMethId,name FROM PaymentMethods WHERE status='1'";
		return parent::query($query);	
	}

}/*end class*/
?>
