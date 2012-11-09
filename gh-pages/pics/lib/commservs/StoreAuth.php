<?php

incAuth();
incUtil();
incCommServs();
require_once "StoreSession.php";
require_once "Authentication.php";

class StoreAuth {
	
	var $message;

	function StoreAuth ($table) {
		
		$auth= new Authentication();
		$this->message="NOT Authenticated";
			if ( isset($_REQUEST['username']) && isset($_REQUEST['password']) ) {
				$email = $_REQUEST['username'];
				$password = $_REQUEST['password'];
				if ($auth->credentials($email,$password,$table)) {
					$this->message="Authenticated";
					StoreSession::loginPage('indexauth.php',$email);
				} else {
					$this->message="NOT Authenticated";
				}
			} 
	
	}/*end storeAuth*/
		
	function authMessage() {
		return $this->message;
	}

}/*end class*/
?>
