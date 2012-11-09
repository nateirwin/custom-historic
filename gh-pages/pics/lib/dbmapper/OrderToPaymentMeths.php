<?php

require_once "Connect.php";

class OrderToPaymentMeths extends Connect{

	var $cache;
	var $cacheTime;

	function OrderToPaymentMeths(	$cache = 0, 
									$cacheTime = 0,
									$persistent = 0 ) {
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	/*OTPM stands for OrderToPaymentMeths*/
	/*
		orderId int unsigned NOT NULL,
		paymentId  bigint unsigned NOT NULL,
		paymentCost decimal(15,2) NOT NULL,
		paymentAmount decimal(15,2) NOT NULL,
		INDEX findOrders(orderId),
		INDEX findPayments (paymentId)
	*/

	function insertToOTPM (	$orderId,
							$paymentId,
							$paymentCost,
							$paymentAmount	) {
		$insertQuery = "INSERT INTO OrderToPaymentMeths (orderId,paymentId,paymentCost,paymentAmount)
											VALUES ($orderId,$paymentId,$paymentCost,$paymentAmount)";

        parent::executeQuery($insertQuery);

	} /*end function insertToOTPM()*/

	function paymentInfoByOid ($orderId ) {
		$query = "SELECT name, paymentCost, paymentAmount FROM OrderToPaymentMeths, PaymentMethods
					WHERE orderId='$orderId' AND OrderToPaymentMeths.paymentId = PaymentMethods.paymentMethId";
		return parent::query($query);	
	}

}/*end class*/
?>
