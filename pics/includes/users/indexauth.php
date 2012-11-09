<?php

incSmarty();
incUtil();
require_once "SmartyStore.php";
require_once "Preferences.php";
require_once "Defaults.php";
require_once "TextMessage.php";

$smarty = new SmartyStore('users');
$preferences = new Preferences();
/*i18n*/
$txtm = new TextMessage();

$styleSheet = $preferences->styleSheet();
$styleSheetPath = "css/" . $styleSheet;
$smarty->assign("stylesheet", $styleSheetPath );

$smarty->display('indexauth.tpl');
?>
