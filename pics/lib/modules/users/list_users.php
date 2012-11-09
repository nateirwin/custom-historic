<?php

require_once "Users.php";

$users = new Users();

$userList = $users->nameIdAll();
$smarty->assign("userList",$userList);

?>
