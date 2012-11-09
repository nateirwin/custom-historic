<?php

incCommServs();
require_once "StoreAuth.php";
incSmarty();
require_once "SmartyStore.php";
require_once "Preferences.php";
require_once "Defaults.php";
require_once "TextMessage.php";

$smarty = new SmartyStore('users');
$preferences = new Preferences();

$styleSheet = $preferences->styleSheet();
$styleSheetPath = "css/" . $styleSheet;
$smarty->assign("stylesheet", $styleSheetPath );
/*i18n*/
$txtm = new TextMessage();

$smarty->assign("forwardUrl",$_SERVER['PHP_SELF']);
$authPage = Defaults::authPage();
$mAuth = new StoreAuth('Users');
$message = $mAuth->authMessage();
$smarty->assign("authmessage",$message);

$smarty->display('index.tpl');
?>
