<?php

require_once "Connect.php";
require_once "Preferences.php";

class StoreSessionDB extends Connect{

	var $cache;
	var $cacheTime;

	function StoreSessionDB($cache = 0, 
							$cacheTime = 0,
							$persistent = 0  ) {
		
		parent::__construct($persistent);
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;

		if ( ! session_set_save_handler (	array(&$this,'open'),
											array(&$this,'close'),
											array(&$this,'read'),
											array(&$this,'write'),
											array(&$this,'destroy'),
											array(&$this,'gc')
										) ) {
			print "Session Handler Failed <br>";
		}

	}/*Constructor*/

	/*Set Session Cookie*/
	function open ( ) {
		//print "open";	
		$this->gc(86400);
		return true;
	}/*open*/

	function close() {
		//print "close";
		return true;
	}/*close*/

	/* Write To Session Table*/	
	function write(	$sessId,
					$data ) {
		//print "write";
		$currentTime = time();
		$data = addslashes($data);
		$query = "REPLACE INTO Sessions (sessId,data,lastAccess) VALUES ('$sessId','$data',$currentTime)";
		if ( ($result = parent::executeQuery($query)) === false) {
			return false;
		} else {
			return true;
		}

	}/*write*/	

	function read($sessId) {
		$prefs = new Preferences();
        $idle = $prefs->idleTime();

		$expiry = time() - $idle;
		$query = "SELECT data FROM Sessions WHERE sessId='$sessId' AND lastAccess >= $expiry";
		$data = parent::query($query);
		
		if( isset($data[0]) ) {
				return $data[0]['data'];
			} else {
				return '';
			}
	
	}/*read*/

	function destroy($sessId) {
		//print "destroy";
		$query = "DELETE FROM Sessions WHERE sessId='$sessId'";
		if ( ($result = parent::executeQuery($query)) === false ) {
			return false;
		} else {
			setcookie ("storez" ,'',(time()-86400), "/",'', 1);
			return true;
		}
		
	}/*destroy*/

	function gc($maxLife) {
		
		//print "gc";
		$gcTime = time() - $maxLife;
		$query = "DELETE FROM Sessions WHERE lastAccess < $gcTime";	

	}/*gc*/

}/*end class*/
?>
