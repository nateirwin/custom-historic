<?php

incSmarty();
require_once "SmartyStore.php";
require_once "Preferences.php";
require_once "TextMessage.php";
require_once "Currency.php";                                                                                                                                                               
$smarty = new SmartyStore('admin');
$prefs = new Preferences();
                                                                                                                                                               
$styleSheet = $prefs->styleSheet();
$styleSheetPath = "css/" . $styleSheet;
$smarty->assign("stylesheet", $styleSheetPath );
/*i18n*/
$txtm = new TextMessage();
$crcy = new Currency();
$smarty->assign("crcy",$crcy);
                                                                                                                                                               
$smarty->display('logout.tpl');
?>
