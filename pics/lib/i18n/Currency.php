<?php
## if the cookie value language is not set
## use the default option of language from the db

class Currency {

	var $currencyCode;

	/*
	*	Currency decision
    *   1-  Initialize language from the parameter
    *   2-  Query Preferences Table
	*/
	function Currency( $currCode = 0 ) {
		if ( $currCode != 0 ) {
			$this->currencyCode = $currCode;
		} else {
			$this->currencyCode = "USD";		
		}
	}

	function crcyLocale() {
		switch ($this->currencyCode) {
			case "USD":
				require_once "Currency_".$this->currencyCode.".php";
				return new Currency_USD();
			break;
			case "TL":
				require_once "Currency_".$this->currencyCode.".php";
                return new Currency_TL();
			default:
				print "No supported currency is selected";
		}//end switch
	}

	function crcy($amount) {
		$currLocale = $this->crcyLocale();
		return $currLocale->currencyFormat($amount);
	}

	/*
	*	returns Currency Code
	*/
	function getCurrencyCode() {
        return $this->currencyCode;
    }

}

?>
