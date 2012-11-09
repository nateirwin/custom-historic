<?php

require_once "Users.php";

$error = "";
$success = "";
$user = StoreSession::user();

if ( !isset($user) ) {
	$error = $error . "*UserId/Email cannot be retrieved. Please try to update password later <br>";
} else {
	
	if (	!isset($_POST['oldpasswd']) ||
			!isset($_POST['passwd']) || 
			!isset($_POST['verify']) ||	
			( strlen(trim($_POST['oldpasswd'])) < 1 ) ||
			( strlen(trim($_POST['passwd'])) < 1 ) ||
			( strlen(trim($_POST['verify'])) < 1 )
		) {
		$error = $error . "*Please fill out all the fields <br>";	
	} else {
		$oldPasswd	=	trim($_POST['oldpasswd']);
		$passwd		=	trim($_POST['passwd']);
		$verify		=	trim($_POST['verify']);
		$userPass = new Users();
		$oldPasswd = $userPass->oldPasswd($user,$oldPasswd);
		if ( isset($oldPasswd) ) {
			if ( !strcmp($passwd,$verify) ) {
				$updated = $userPass->updatePasswd($user,$passwd);
				if ( !isset($updated) ) {
					$error = $error . "*An Error is occured while inserting to the Database <br>
										Please consult your Systems Administrator <br>";	
				} else {
					$success = "*Successfully updated password <br>";	
				}		
			} else {
				$error = $error . "*Password fields do NOT match <br>";
			}
		} else {
			$error = $error . "*Wrong password provided <br>";
		}
	}
	$smarty->assign("error",$error);
	$smarty->assign("success",$success);	
} 

?>
