<?php

require_once "TextMessage.php";

/*$language = textMessage::setLang("tr_TR");
print $language->msg('cow');
print "\n";
print $language->msg('date');
print "\n";
*/

$args = array('32');
$lang = new TextMessage("en_US");
print $lang->getLangCode() . "\n";
print $lang->msg('i am X years old',$args) . "\n";
print $lang->msg('cow'). "\n" ;
print $lang->msg('date') . "\n";

print "\n \n";

$lang = new TextMessage("tr_TR");
print $lang->getLangCode() . "\n";
print $lang->msg('language',$args) . "\n";
print $lang->msg('i am X years old',$args) . "\n";
print $lang->msg('cow'). "\n" ;
print $lang->msg('date') . "\n";

setlocale(LC_ALL, 'tr_TR');
$locale_info = localeconv();
print_r($locale_info);

setlocale(LC_ALL, 'en_US');
$locale_info = localeconv();
print_r($locale_info);


?>
