<?php

incSession();
require_once "Preferences.php";
require_once "StoreSessionDB.php";
require_once "Sessions.php";

class StoreSession {

	function startSession() {
		$stSession = new StoreSessionDB();
		session_set_cookie_params(0,'/','',1);
		session_name("mypics");
		session_start();
	}

	function sessionData($data) {
		$_SESSION['data'] = $data;
	}
	
	function setKey() {
		//$this->dataStr = gmp_strval(gmp_random(4)).','.$USER.','. gmp_strval(gmp_random(2));
		return true;
	}

	/*Call this function in the loginPage
	* $forwarUrl =	url that comes after the login page
	*/
	function loginPage($forwardUrl, $userName) {
		StoreSession::startSession();
		StoreSession::sessionData($userName);
		header("Location: $forwardUrl");
	}
	
	function page() {
		StoreSession::startSession();
		StoreSession::isUserSess();
	}

	function adminPage() {
		StoreSession::startSession();
		StoreSession::isAdminSess();
	}
	
	function logoutPage() {
		StoreSession::startSession();
		session_destroy();
	}
	
	function isUserSess() {
		$sessId = session_id();
		$session = new Sessions();
		$dbSessId = $session->sessionValid($sessId);
		$myUser = null;
		if ( ($dbData = $session->dataField($sessId)) != null ) {
			$myUser = $session->isUser($dbData);	
		}
		if (	!isset($dbSessId) || !isset($sessId) || !isset($myUser) ||
				($sessId != $dbSessId)
			) {
			print "forwarded to the logout.php <br>";
			header ("Location: logout.php");
		}
	}
	

	function isAdminSess() {
		$sessId = session_id();
        $session = new Sessions();
        $dbSessId = $session->sessionValid($sessId);
        $myUser = null;
        if ( ($dbData = $session->dataField($sessId)) != null ) {
            $myUser = $session->isAdmin($dbData);
        }
        if (    !isset($dbSessId) || !isset($sessId) || !isset($myUser) ||
                ($sessId != $dbSessId)
            ) {
            print "forwarded to the logout.php <br>";
            header ("Location: logout.php");
        }
	}

	function user() {
		if ( isset($_SESSION['data']) ) {
			return $_SESSION['data'];
		}
		return null;
	}

}/*class*/
?>
