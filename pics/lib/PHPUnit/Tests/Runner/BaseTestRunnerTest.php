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
// $Id: BaseTestRunnerTest.php,v 1.6 2004/07/12 04:26:05 sebastian Exp $
//

require_once 'PHPUnit2/Framework/TestCase.php';

require_once 'PHPUnit2/Tests/MockRunner.php';
require_once 'PHPUnit2/Tests/NonStatic.php';

/**
 * @author      Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @copyright   Copyright &copy; 2002-2004 Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @license     http://www.php.net/license/3_0.txt The PHP License, Version 3.0
 * @category    PHP
 * @package     PHPUnit2
 * @subpackage  Tests
 */
class PHPUnit2_Tests_Runner_BaseTestRunnerTest extends PHPUnit2_Framework_TestCase {
    public function testInvokeNonStaticSuite() {
        $runner = new PHPUnit2_Tests_MockRunner;
        $runner->getTest('PHPUnit2_Tests_NonStatic');
    }
}

/*
 * vim600:  et sw=2 ts=2 fdm=marker
 * vim<600: et sw=2 ts=2
 */
?>
