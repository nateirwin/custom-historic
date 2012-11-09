<?php

require_once "Connect.php";

class AdminComments extends Connect{
	
	var $cache;
	var $cacheTime;

	function AdminComments(	$cache = 0, 
							$cacheTime = 0, 
							$persistent	= 0 ) {
		parent::__construct($persistent);
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	function insertToAC($pics,
						$comment ) {
		$insertQuery = "INSERT INTO AdminComments (pics,comments)
										VALUES ('$pics','$comment')";
		parent::executeQuery($insertQuery);
	}	

	function commentByPic($pic) {
		$query = "SELECT comments FROM AdminComments WHERE pic = '$pic'";
		return parent::query($query);
	}

}/*end class*/
?>
