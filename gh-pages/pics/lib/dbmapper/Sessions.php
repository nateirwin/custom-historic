<?php

require_once "Connect.php";

class Sessions extends Connect{

	var $sessionId;
	var $cache;
	var $cacheTime;

	function Sessions(	$sessionId = 0, 
						$cache = 0, 
						$cacheTime = 0, 
						$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->sessionId	= $sessionId;
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	/*
	 *
	 */
	function sessionValid($sessId) {

		$prefs = new Preferences();
		$idle = $prefs->idleTime();
		$expiry = time() - $idle;

		$query = "SELECT sessId FROM Sessions WHERE sessId='$sessId' AND lastAccess >= $expiry";
		$session = parent::query($query);
		$validSess = parent::validateIndexes($session,0,'sessId');
		return $validSess;
	}/*end function*/
	
    function dataField($sessId) {
        $prefs = new Preferences();
        $idle = $prefs->idleTime();
        $expiry = time() - $idle;

        $query = "SELECT data FROM Sessions WHERE sessId='$sessId' AND lastAccess >= $expiry";
		$session = parent::query($query);
		$validData = parent::validateIndexes($session,0,'data');
		$vars=preg_split('/([a-z,A-Z]+)\|/',$validData,-1,PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE);
		if (isset($vars[1])) {
			$validData = unserialize($vars[1]);
			return $validData;
		} else { 
			return null;
		}
    }/*end function*/

	function isAdmin($email) {
		$query = "SELECT email FROM Admins WHERE email='$email'";
		$admin =  parent::query($query);
		return parent::validateIndexes($admin,0,'email');
	}

	function isUser($email) {
		$query = "SELECT email FROM Users WHERE email='$email'";
		$user =  parent::query($query);
		return parent::validateIndexes($user,0,'email');
	}

}/*end class*/


?>
