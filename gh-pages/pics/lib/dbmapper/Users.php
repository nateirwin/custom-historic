<?php

require_once "Connect.php";

class Users extends Connect{
	
	var $email;
	var $cache;
	var $cacheTime;

	function Users(	$email = null,
					$cache = 0, 
					$cacheTime = 0, 
					$persistent	= 0 ) {
		parent::__construct($persistent);
		$this->email		= $email;
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	/*TODO check for the email before trying to insert it*/
	/*Better security of creating a new account*/
	function insertToUsers(	$title,
							$fname,
							$mname,
							$lname,
							$address,
							$city,
							$state,
							$zip,
							$homeTel,
							$workTel,
							$cellTel,
							$contactPerson,
							$contactTel, 
							$passwd,
							$email = null ) {
			$insertQuery = null;
			if (isset($this->email)) {
				$insertQuery = "INSERT INTO Users (title,fname,mname,lname,address,city,state,zip,homeTel,workTel,cellTel,contactPerson,contactTel,passwd,email)
				            VALUES ('$title','$fname','$mname','$lname','$address','$city','$state','$zip','$homeTel','$workTel','$cellTel','$contactPerson','$contactTel',MD5('$passwd'),'$this->email')";
			} else {
				$insertQuery = "INSERT INTO Users (title,fname,mname,lname,address,city,state,zip,homeTel,workTel,cellTel,contactPerson,contactTel,passwd,email)
                            VALUES ('$title','$fname','$mname','$lname','$address','$city','$state','$zip','$homeTel','$workTel','$cellTel','$contactPerson','$contactTel',MD5('$passwd'),'$email')";
			}
			parent::executeQuery($insertQuery);
			$query = "SELECT LAST_INSERT_ID()";
			$rs = parent::query($query);
			return parent::validateIndexes($rs,0,'last_insert_id()');
	} /*end function insertToUsers()*/

	function updateUsers (	$title,
                            $fname,
                            $mname,
                            $lname,
                            $address,
                            $city,
                            $state,
                            $zip,
                            $homeTel,
                            $workTel,
                            $cellTel,
                            $contactPerson,
                            $contactTel,
							$passwd,
							$email,
							$userId) {
		$updateQuery = "UPDATE Users SET title='$title', fname='$fname', mname='$mname', lname='$lname', address = '$address', city = '$city', state = '$state', zip = '$zip', homeTel = '$homeTel', workTel = '$workTel', cellTel = '$cellTel', contactPerson = '$contactPerson', contactTel = '$contactTel', passwd=MD5('$passwd'),email='$email' WHERE userId='$userId'";
        return parent::executeQuery($updateQuery);
    } /*end function*/
	
	function updatePasswd (	$email,
							$passwd	) {
		$updateQuery = "UPDATE Users SET passwd=MD5('$passwd') WHERE email='$email'";
		return parent::executeQuery($updateQuery);
	}

	function oldPasswd	(	$email,
							$oldPasswd	) {
		$query = "SELECT email from Users WHERE email='$email' AND passwd=MD5('$oldPasswd')";
		$info = parent::query($query);
		return parent::validateIndexes($info,0,'email');
	}
							
	function deleteUser($uid) {
		$query = "SELECT pic FROM Pics WHERE userId='$uid'";
		$pics = parent::query($query);
		//print_r($pics);
		foreach ($pics as $outer) {
			foreach ($outer as $pic){
				$rmOrig = "./images/$pic";
				$rmThumb = "./images/thb_$pic";
				$rmPop = "./images/pop_$pic";
				$out = unlink($rmOrig);
				$out = unlink($rmThumb);
				$out = unlink($rmPop);
			}
		} 
		$query = "DELETE from Pics WHERE userId = '$uid'";
		parent::executeQuery($query);
		$query = "DELETE from Users WHERE userId = '$uid'";
		return parent::executeQuery($query);
	}	

    function emailPasswd ( $passwd ) {
        $query = "SELECT email,passwd from Users WHERE email='$this->email' AND passwd=MD5('$passwd')";
        $info = parent::query($query);
        return parent::validateIndex($info,0);
    }	

	function getUser($userId) {
		$query = "SELECT * FROM Users WHERE userId='$userId'";
		$userArr = parent::query($query);
		return parent::validateIndex($userArr,0); 
	}
	
	function userId() {
		$query = "SELECT userId FROM Users WHERE email='$this->email'";
		$userIdArr = parent::query($query);
		$size = sizeof($userIdArr);
		return parent::validateIndexes($userArr,0,'userId');
	}

	function userInfo() {
		$query = "SELECT lname,mname,fname,email,userId FROM Users ORDER BY 'fname'";
		return parent::query($query);
	}

	function nameIdAll() {
		$query = "SELECT fname,lname,mname,userId,email FROM Users ORDER BY 'fname'";
		return parent::query($query);
	}
	
	function allById($userId) {
		$query = "SELECT * FROM Users WHERE userId='$userId'";
		$user = parent::query($query);
		return parent::validateIndex($user,0);
	}
}/*end class*/
?>
