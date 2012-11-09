<?php

incSmarty();
incMods();
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

/*include modules*/
require_once "users/edit_passwd.php";

$smarty->display('account.tpl');
?>
