<?php

require_once "Connect.php";

class PrefOptions extends Connect{

	var	$name;
	var $cache;
	var $cacheTime;

	function Preferences(	$cache = 0, 
							$cacheTime = 0, 
							$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	function prefValues ($prefId) {
		$query = "SELECT values FROM PrefOptions WHERE prefId='$prefId'";
		$rs = parent::query($query);
	}
	
	function languages() {
		return $this->prefValues(6);
	}

}/*end class*/
?>
