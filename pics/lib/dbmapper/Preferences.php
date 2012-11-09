<?php

require_once "Connect.php";

class Preferences extends Connect{

	var	$name;
	var $cache;
	var $cacheTime;

	function Preferences(	$cache = 0, 
							$cacheTime = 0, 
							$persistent	= 0		) {

		parent::__construct($persistent);
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	/*TODO implement this function*/	
	function updatePrefsVal ($value) {
		$updateQuery = "UPDATE Preferences SET value=" . $value . "WHERE" . "prefId = " . '\''. $this->prefId .'\'';
	}

	function prefValue ($prefName) {
		$query = "SELECT value FROM Preferences WHERE name='$prefName'";
		$rs = parent::query($query);
		return $rs[0]['value'];
	}
	
	function searchResultMaxPP() {
		return $this->prefValue("");
	}

    function idleTime() {
        return $this->prefValue("Idle Time");
    }

	function styleSheet() {
		return $this->prefValue("CSS Style Sheet");
	}

	function language() {
		return $this->prefValues("language");
	}

}/*end class*/
?>
