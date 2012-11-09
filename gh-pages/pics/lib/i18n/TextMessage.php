<?php
## if the cookie value language is not set
## use the default option of language from the db

class TextMessage {

	var $languageCode;

	/*
	*	Language decision
	*	1-	Initialize language from the parameter
	*	2-	Look for the $_COOKIE variable	
	*	3-	Query Preferences Table
	*/
	function TextMessage( $lang = 0 ) {
		if ( $lang != 0 ) {
			$this->languageCode = $lang;
		} else {
			$langCode = "en_US";
			$this->languageCode = $langCode;
		}
	}

	function langFile() {
		switch ($this->languageCode) {
			case "en_US":
				require_once "TextMessage_".$this->languageCode.".php";
				return new TextMessage_en_US();
			break;
			case "tr_TR":
				require_once "TextMessage_".$this->languageCode.".php";
                return new TextMessage_tr_TR();
			default:
				print "No supported language is selected";
		}//end switch
	}

	function getLangCode() {
		return $this->languageCode;
	}

	function msg (	$message,
					$args = 0	) {
		$currLang = $this->langFile();
		return $currLang->message($message,$args);
	}
	/*
	function warn (	$warning,
					$args = 0	) {
		$currWarn = $this->warnFile();
		return $currWarn->warning($message,$args);
	}
	*/

	/*Can implement a function which will decide the timezones*/
}

?>
