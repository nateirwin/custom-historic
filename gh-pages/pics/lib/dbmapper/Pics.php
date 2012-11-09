<?php

require_once "Connect.php";

class Pics extends Connect{
	
	var $cache;
	var $cacheTime;

	function Pics(	$cache = 0, 
					$cacheTime = 0, 
					$persistent	= 0 ) {
		parent::__construct($persistent);
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	function insertToUP($userId,
						$pic ) {
		$insertQuery = "INSERT INTO Pics (userId,pic,picDate)
										VALUES ('$userId','$pic',now())";
		parent::executeQuery($insertQuery);
	}	

	function picsByUid($uid) {
		$query = "SELECT * FROM Pics WHERE userId = '$uid'";
		return parent::query($query);
	}
	
	function picsByName($email) {	
		$query = "SELECT * FROM Users,Pics WHERE email='$email' AND Pics.userId=Users.userId";
        return parent::query($query);
    }

	function admCommsByUid($uid) {
		$query = "SELECT pic,adminComment FROM Pics WHERE userId='$uid'";
		return parent::query($query);
	}

	function commByName($email) {
		$query = "SELECT pic,picDate,userComment FROM Users,Pics WHERE email='$email' AND Pics.userId=Users.userId";
		return parent::query($query);
	}
	
	function usrCommsByUid($uid) {
		$query = "SELECT pic,userComment FROM Pics WHERE userId='$uid'";
		return parent::query($query);
	}
	
	function deletePics($picId) {
        $query = "DELETE from Pics WHERE pic = '$picId'";
        parent::executeQuery($query);
	}	
	
	function updateAC(	$picId,
						$comment) {
		$picId = str_replace("_",".",$picId);
		$comment = "UPDATE Pics SET adminComment='$comment' WHERE pic='$picId'";
		parent::executeQuery($comment);
	}

	function updateUC(  $picId,
						$comment) {
		$picId = str_replace("_",".",$picId);
		$comment = "UPDATE Pics SET userComment='$comment' WHERE pic='$picId'";
		parent::executeQuery($comment);
	}

}/*end class*/
?>
