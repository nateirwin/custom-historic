<?php

require_once "Tax.php";

class ReceiptCalcs {
	
	function calcSingleEntry($quantity = 1,
							$itemPrice = -1) {
		return number_format( (floatval($quantity) * floatval($itemPrice)) , 2, '.', ' ');
		//return money_format ('%i' , (floatval($amount) * floatval($itemPrice)) );
	}

	function calcTax($subTotal,
					$storeId) {
		$tax = new Tax();
        $rs = $tax->taxRate($storeId);
        if( isset($rs[0]) ) {
            $taxRate = $rs[0]['rate'];
			return number_format( (floatval($subTotal) * floatval($taxRate / 100)) , 2, '.', ' ');
		} else {
            return number_format( 0 , 2, '.', ' ');
        }
	}

	function calcTotal($subTotal,
						$tax ) {
		return number_format( (floatval($subTotal)) + (floatval($tax)), 2, '.', ' ');
	}
	
	function calcPaymentAmt($paymentAmount) {
		if (!is_numeric($paymentAmount)) { $paymentAmount = (float)substr($paymentAmount,1); }
		return $paymentAmount;
	}
	
	function calcChangeDue($paymentAmount,
						$total)	{
		if (!is_numeric($paymentAmount)) { $paymentAmount = (float)substr($paymentAmount,1); }
		return number_format(( floatval($paymentAmount) - floatval($total) ), 2, '.', ' ');
	}

	function calcDiscount(	$subTotal,
							$discountAmount
							) {
		if ( ! is_numeric($discountAmount) )	{ $discountAmount = (float)substr($discountAmount,1); }
	    if($discountAmount > 0)					{ $subTotal = $subTotal - $discountAmount; }
		return $subTotal;
	}

}/*end class*/
?>
