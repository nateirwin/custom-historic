<?php
/**
 * Smarty plugin
 * @package Smarty
 * @subpackage plugins
 */


/**
 * Smarty string_format modifier plugin
 *
 * Type:     modifier<br>
 * Name:     currency<br>
 * Purpose:  format currencies via internal lib i18n more info can be found in lib/i18n/Currency.php
 * @link http://smarty.php.net/manual/en/language.modifier.string.format.php
 * @param string
 * @return string
 */
function smarty_modifier_currency($string,$crcys)
{
    return $crcys->crcy($string); 
}

/* vim: set expandtab: */

?>
