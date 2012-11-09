<?php

require_once "Admins.php";
require_once "Users.php";

class Authentication {

	function credentials (	$email,
							$passwd, 
							$table ) {
		if ( !strcmp($table,"Users") ) {
			$users	= new Users($email);
			$arr = $users->emailPasswd($passwd);
			return $this->verifyCreds($arr,$arr['email'],$arr['passwd']);
		} else {
			$admin = new Admins($email);
			$arr = $admin->emailPasswd($passwd);
			return $this->verifyCreds($arr,$arr['email'],$arr['passwd']);
		}
	}

	/*TODO make sure to check for passwd*/
	function verifyCreds (	$queryResult,
							$email,
							$passwd	) {
		if ( isset($queryResult) ) {
			if ($queryResult['email'] == $email) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
	}

}/*end class*/

?>
