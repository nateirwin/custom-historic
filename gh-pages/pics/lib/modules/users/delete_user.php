<?php

require_once "Users.php";

$user = new Users();
$users = $user->userInfo();
$smarty->assign("users",$users);
foreach ($_POST as $uid) {
	$rs = $user->deleteUser($uid);
} 
if ( isset($rs) ) {
	header('Location: deletecust.php');
}

?>
