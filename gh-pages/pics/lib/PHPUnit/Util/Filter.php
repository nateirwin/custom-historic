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
// $Id: Filter.php,v 1.6.2.1 2004/07/12 17:31:39 sebastian Exp $
//

/**
 * Utility class for code filtering.
 *
 * @author      Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @copyright   Copyright &copy; 2002-2004 Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @license     http://www.php.net/license/3_0.txt The PHP License, Version 3.0
 * @category    PHP
 * @package     PHPUnit2
 * @subpackage  Util
 */
class PHPUnit2_Util_Filter {
    // {{{ Members

    /**
    * Source files that are to be filtered.
    *
    * @var    array
    * @access protected
    * @static
    */
    protected static $filteredFiles = array(
      'PHPUnit2/Extensions/ExceptionTestCase.php',
      'PHPUnit2/Extensions/RepeatedTest.php',
      'PHPUnit2/Extensions/TestDecorator.php',
      'PHPUnit2/Extensions/TestSetup.php',
      'PHPUnit2/Framework/Assert.php',
      'PHPUnit2/Framework/AssertionFailedError.php',
      'PHPUnit2/Framework/ComparisonFailure.php',
      'PHPUnit2/Framework/IncompleteTest.php',
      'PHPUnit2/Framework/IncompleteTestError.php',
      'PHPUnit2/Framework/Test.php',
      'PHPUnit2/Framework/TestCase.php',
      'PHPUnit2/Framework/TestFailure.php',
      'PHPUnit2/Framework/TestListener.php',
      'PHPUnit2/Framework/TestResult.php',
      'PHPUnit2/Framework/TestSuite.php',
      'PHPUnit2/Framework/Warning.php',
      'PHPUnit2/Runner/BaseTestRunner.php',
      'PHPUnit2/Runner/StandardTestSuiteLoader.php',
      'PHPUnit2/Runner/TestCollector.php',
      'PHPUnit2/Runner/TestRunListener.php',
      'PHPUnit2/Runner/TestSuiteLoader.php',
      'PHPUnit2/Runner/Version.php',
      'PHPUnit2/TextUI/ResultPrinter.php',
      'PHPUnit2/TextUI/TestRunner.php',
      'PHPUnit2/Util/Filter.php',
      'PHPUnit2/Util/Printer.php'
    );

    // }}}
    // {{{ public static function getFilteredCodeCoverage($codeCoverageInformation)

    /**
    * Filters source lines from PHPUnit classes.
    *
    * @param  array
    * @return array
    * @access public
    * @static
    */
    public static function getFilteredCodeCoverage($codeCoverageInformation) {
        $files = array_keys($codeCoverageInformation);

        foreach ($files as $file) {
            if (in_array(self::getCanonicalFilename($file), self::$filteredFiles)) {
                unset($codeCoverageInformation[$file]);
            }
        }

        return $codeCoverageInformation;
    }

    // }}}
    // {{{ public static function getFilteredStacktrace(Exception $e)

    /**
    * Filters stack frames from PHPUnit classes.
    *
    * @param  Exception $e
    * @return string
    * @access public
    * @static
    */
    public static function getFilteredStacktrace(Exception $e) {
        $filteredStacktrace = '';
        $stacktrace         = $e->getTrace();

        foreach ($stacktrace as $frame) {
            $filtered = false;

            if (isset($frame['file']) &&
                !in_array(self::getCanonicalFilename($frame['file']), self::$filteredFiles)) {
                $filteredStacktrace .= sprintf(
                  "%s:%s\n",

                  $frame['file'],
                  isset($frame['line']) ? $frame['line'] : '?'
                );
            }
        }

        return $filteredStacktrace;
    }

    // }}}
    // {{{ protected static function getCanonicalFilename($filename)

    /**
    * Canonicalizes a source file name.
    *
    * @param  string
    * @return string
    * @access public
    * @static
    */
    protected static function getCanonicalFilename($filename) {
        return str_replace(
          '\\',
          '/',
          substr($filename, strpos($filename, 'PHPUnit2'))
        );
    }

    // }}}
}

/*
 * vim600:  et sw=2 ts=2 fdm=marker
 * vim<600: et sw=2 ts=2
 */
?>
