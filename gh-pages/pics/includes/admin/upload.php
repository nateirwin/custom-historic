<?php

incSmarty();
incUtil();
incMods();
require_once "SmartyStore.php";
require_once "Preferences.php";
require_once "Defaults.php";
require_once "TextMessage.php";
require_once "Users.php";

$smarty = new SmartyStore('admin');
$preferences = new Preferences();
/*i18n*/
$txtm = new TextMessage();

$styleSheet = $preferences->styleSheet();
$styleSheetPath = "css/" . $styleSheet;
$smarty->assign("stylesheet", $styleSheetPath );
$smarty->assign("action",$_SERVER['PHP_SELF']);

//print_r($_POST);
/*first entry to page ignore all the logic*/
if ( !isset($_POST['uid']) ) {
    $users = new Users();
    $info = $users->userInfo();
	$smarty->assign("firstPage","true");
    $smarty->assign("users",$info);
} else {
	require_once "http_upload/http_upload.php";
}

$smarty->display('upload.tpl');

?>
