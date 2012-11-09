<?php
## if the cookie value language is not set
## use the default option of language from the db
require_once "TextMessage.php";

class TextMessage_en_US extends TextMessage {

	var $lang;

	function TextMessage_en_US() {
		$this->lang = "English [en]";
	}/*constructor*/

	function message(	$message,
						$args	) {

		$messages = array (
			'Change Due' => 'Change Due',
			'Order Id' => 'Order Id',
			'Order Date' => 'Order Date',
			'Store Id' => 'Store Id',
			'SubTotal' => 'SubTotal',
			'Total' => 'Total',
			'Welcome' => 'Welcome',
			'Tax' => 'Tax',
			'date' => date("m/d/y"),
			'language' => $this->lang,
			'i am X years old' => 'I am '.  $args[0] . ' years old'
                        );
		return $messages[$message];
	}
		
}/*class*/
?>
