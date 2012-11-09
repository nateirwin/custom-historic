<?php

incSmarty();
incUtil();
incMods();
require_once "SmartyStore.php";
require_once "Preferences.php";
require_once "Defaults.php";
require_once "TextMessage.php";

$smarty = new SmartyStore('admin');
$preferences = new Preferences();
/*i18n*/
$txtm = new TextMessage();

$styleSheet = $preferences->styleSheet();
$styleSheetPath = "css/" . $styleSheet;
$smarty->assign("stylesheet", $styleSheetPath );

/*include modules*/
require_once "users/delete_user.php";

$smarty->display('deletecust.tpl');
?>
