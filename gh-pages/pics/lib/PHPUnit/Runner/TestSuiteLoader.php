<?php
//
// +------------------------------------------------------------------------+
// | PEAR :: PHPUnit2                                                       |
// +------------------------------------------------------------------------+
// | Copyright (c) 2002-2004 Sebastian Bergmann <sb@sebastian-bergmann.de>. |
// +------------------------------------------------------------------------+
// | This source file is subject to version 3.00 of the PHP License,        |
// | that is available at http://www.php.net/license/3_0.txt.               |
// | If you did not receive a copy of the PHP license and are unable to     |
// | obtain it through the world-wide-web, please send a note to            |
// | license@php.net so we can mail you a copy immediately.                 |
// +------------------------------------------------------------------------+
//
// $Id: TestSuiteLoader.php,v 1.3 2004/07/11 09:18:12 sebastian Exp $
//

/**
 * An interface to define how a test suite should be loaded.
 *
 * @author      Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @copyright   Copyright &copy; 2002-2004 Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @license     http://www.php.net/license/3_0.txt The PHP License, Version 3.0
 * @category    PHP
 * @package     PHPUnit2
 * @subpackage  Runner
 */
interface PHPUnit2_Runner_TestSuiteLoader {
    // {{{ public function load($suiteClassName)

    /**
    * @param  string  $suiteClassName
    * @return ReflectionClass
    * @access public
    */
    public function load($suiteClassName);

    // }}}
    // {{{ public function reload(ReflectionClass $aClass)

    /**
    * @param  ReflectionClass  $aClass
    * @return ReflectionClass
    * @access public
    */
    public function reload(ReflectionClass $aClass);

    // }}}
}

/*
 * vim600:  et sw=2 ts=2 fdm=marker
 * vim<600: et sw=2 ts=2
 */
?>
