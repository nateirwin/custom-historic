<?php

class Converter {

	/*TODO rewrite this function*/
	function currToNum ( $currency ) {
		if ( ! is_numeric($currency) )	{ 
			return (float)substr($currency,1); 
		} else {
			return $currency;
		}
	}

}/*end class*/
?>
