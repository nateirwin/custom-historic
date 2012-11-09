<?php
## if the cookie value language is not set
## use the default option of language from the db
require_once "TextMessage.php";

class TextMessage_tr_TR extends TextMessage {

	var $lang;

	function TextMessage_tr_TR() {
		$this->lang = "Turkish [tr]";
	}/*constructor*/

	function message($message,$args) {
		$messages = array (
			'language' => $this->lang,
			'Order Id' => 'Alis Num',
			'Store Id' => 'Dukkan Num',
			'Order Date' => 'Alis Tarihi',
			'SubTotal' => 'Toplam',
			'i am X years old' => 'Ben '.  $args[0] . ' yasindayim',
			'date' => date("d/m/y")
                        );
		return $messages[$message];
	}
		
}/*class*/
?>
