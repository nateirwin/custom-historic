<?php

require_once "Connect.php";

class Admins extends Connect{
	
	var $email;
	var $cache;
	var $cacheTime;

	function Admins($email,
					$cache = 0, 
					$cacheTime = 0, 
					$persistent	= 0 ) {
		parent::__construct($persistent);
		$this->email		= $email;
		$this->cache		= $cache;
		$this->cacheTime	= $cacheTime;
	}

	/*TODO check for the email before trying to insert it*/
	function insertToAdmins($title,
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
							$contactTel ) {
			$insertQuery = "INSERT INTO Admins (title,fname,mname,lname,address,city,state,zip,homeTel,workTel,cellTel,contactPerson,contactTel,passwd,email)
				            VALUES ('$title','$fname','$mname','$lname','$address','$city','$state','$zip','$homeTel','$workTel','$cellTel','$contactPerson','$contactTel',MD5('$this->email'),'$this->email')";
			parent::executeQuery($insertQuery);
			$query = "SELECT LAST_INSERT_ID()";
			$rs = parent::query($query);
			return parent::validateIndexes($rs,0,'last_insert_id()');
	}

	function emailPasswd ( $passwd ) {
        $query = "SELECT email,passwd from Admins WHERE email='$this->email' AND passwd=MD5('$passwd')";
        $info = parent::query($query);
        return parent::validateIndex($info,0);
    }


	function getUser($adminId) {
		$query = "SELECT * FROM Admins WHERE adminId='$adminId'";
		$adminArr = parent::query($query);
		return parent::validateIndex($adminArr,0); 
	}
	
	function adminId() {
		$query = "SELECT adminId FROM Admins WHERE email='$this->email'";
		$adminIdArr = parent::query($query);
		$size = sizeof($adminIdArr);
		return parent::validateIndexes($adminArr,0,'adminId');
	}

}/*end class*/
?>
