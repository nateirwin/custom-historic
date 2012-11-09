<?php
## if the cookie value language is not set
## use the default option of language from the db

incUtil();

require_once "Currency.php";
require_once "Converter.php";

class Currency_USD extends Currency {

	var $lang;
	var $locale;

	function Currency_USD() {
		$this->lang = "English [en]";
		if (	(setlocale(LC_MONETARY, 'en_US') !== false) &&
				(setlocale(LC_NUMERIC, 'en_US')  !== false) )  {
			$this->locale = localeconv();
		}
	}/*constructor*/

	function currencyFormat( $amount ) {

		$amount = Converter::currToNum($amount);
		$formatted = number_format($amount,$this->locale['frac_digits'],
											$this->locale['mon_decimal_point'],
											$this->locale['mon_thousands_sep']);
		return $this->sign($formatted);	
	}
	
	function sign ( $formatted ) {

		if ($this->locale['p_sep_by_space']) {
			$formatted = ' '.$formatted;
		}
		
		if ( $formatted >= 0 ) {
			$formatted = $this->signLocation(	$this->locale['positive_sign'],
												$this->locale['currency_symbol'],
												$formatted	);
		} else {
			$formatted = $this->signLocation(	$this->locale['negative_sign'],
												$this->locale['currency_symbol'],
												$formatted	);
		}
		return $formatted;
	}
		
	function signLocation(	$sign,
							$crSymbol,
							$formatted ) {
		$locValue = $this->locale['p_sign_posn']; 
		switch ($locValue) {	
		case 0:
			$formatted = "(".$sign.$crSymbol.$formatted.")";
			break;
		case 1:
			$formatted = $sign.$crSymbol.$formatted;
			break;
		case 2:
			$formatted = $crSymbol.$formatted.$sign;
			break;
		case 3:
			$formatted = $sign.$crSymbol.$formatted;
			break;
		case 4:
			$formatted = $crSymbol.$sign.$formatted;
			break;
		default:
			print "wrong value passed in Currency converter";		
		}/*end switch*/

		return $formatted;
	}

}/*class*/
	//$curr = new Currency_en_US();
	//print $curr->currencyFormat(20) . "\n";
?>
