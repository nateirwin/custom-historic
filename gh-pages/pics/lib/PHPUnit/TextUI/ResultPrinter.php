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
// $Id: ResultPrinter.php,v 1.5 2004/04/24 06:23:57 sebastian Exp $
//

require_once 'PHPUnit2/Framework/TestFailure.php';
require_once 'PHPUnit2/Framework/TestListener.php';
require_once 'PHPUnit2/Framework/TestResult.php';
require_once 'PHPUnit2/Util/Filter.php';
require_once 'PHPUnit2/Util/Printer.php';

/**
 * @author      Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @copyright   Copyright &copy; 2002-2004 Sebastian Bergmann <sb@sebastian-bergmann.de>
 * @license     http://www.php.net/license/3_0.txt The PHP License, Version 3.0
 * @category    PHP
 * @package     PHPUnit2
 * @subpackage  TextUI
 */
class PHPUnit2_TextUI_ResultPrinter extends PHPUnit2_Util_Printer implements PHPUnit2_Framework_TestListener {
    // {{{ Members

    /**
    * @var    integer
    * @access private
    */
    private $column = 0;

    // }}}
    // {{{ public function printResult(PHPUnit2_Framework_TestResult $result, $timeElapsed)

    /**
    * @param  PHPUnit2_Framework_TestResult $result
    * @param  float                         $runTime
    * @access public
    */
    public function printResult(PHPUnit2_Framework_TestResult $result, $timeElapsed) {
        $this->printHeader($timeElapsed);
        $this->printErrors($result);
        $this->printFailures($result);
        $this->printIncompletes($result);
        $this->printFooter($result);
    }

    // }}}
    // {{{ protected function printDefects($defects, $count, $type)

    /**
    * @param  array   $defects
    * @param  integer $count
    * @param  string  $type
    * @access protected
    */
    protected function printDefects($defects, $count, $type) {
        if ($count == 0) {
            return;
        }

        $this->write(
          sprintf(
            "There %s %d %s%s:\n",

            ($count == 1) ? 'was' : 'were',
            $count,
            $type,
            ($count == 1) ? '' : 's'
          )
        );

        $i = 1;

        foreach ($defects as $defect) {
            $this->printDefect($defect, $i++);
        }
    }

    // }}}
    // {{{ protected function printDefect(PHPUnit2_Framework_TestFailure $defect, $count)

    /**
    * @param  PHPUnit2_Framework_TestFailure $defect
    * @param  integer                        $count
    * @access protected
    */
    protected function printDefect(PHPUnit2_Framework_TestFailure $defect, $count) {
        $this->printDefectHeader($defect, $count);
        $this->printDefectTrace($defect);
    }

    // }}}
    // {{{ protected function printDefectHeader(PHPUnit2_Framework_TestFailure $defect, $count)

    /**
    * @param  PHPUnit2_Framework_TestFailure $defect
    * @param  integer                        $count
    * @access protected
    */
    protected function printDefectHeader(PHPUnit2_Framework_TestFailure $defect, $count) {
        $name = $defect->failedTest()->getName();

        if ($name == null) {
            $class = new ReflectionClass($defect->failedTest());
            $name  = $class->name;
        }

        $this->write(
          sprintf(
            "%d) %s\n",

            $count,
            $name
          )
        );
    }

    // }}}
    // {{{ protected function printDefectTrace(PHPUnit2_Framework_TestFailure $defect)

    /**
    * @param  PHPUnit2_Framework_TestFailure $defect
    * @access protected
    */
    protected function printDefectTrace(PHPUnit2_Framework_TestFailure $defect) {
        $e       = $defect->thrownException();
        $message = method_exists($e, 'toString') ? $e->toString() : $e->getMessage();

        $this->write($message . "\n");

        $this->write(
          PHPUnit2_Util_Filter::getFilteredStacktrace(
            $defect->thrownException()
          )
        );
    }

    // }}}
    // {{{ protected function printErrors(PHPUnit2_Framework_TestResult $result)

    /**
    * @param  PHPUnit2_Framework_TestResult  $result
    * @access protected
    */
    protected function printErrors(PHPUnit2_Framework_TestResult $result) {
        $this->printDefects($result->errors(), $result->errorCount(), 'error');
    }

    // }}}
    // {{{ protected function printFailures(PHPUnit2_Framework_TestResult $result)

    /**
    * @param  PHPUnit2_Framework_TestResult  $result
    * @access protected
    */
    protected function printFailures(PHPUnit2_Framework_TestResult $result) {
        $this->printDefects($result->failures(), $result->failureCount(), 'failure');
    }

    // }}}
    // {{{ protected function printIncompletes(PHPUnit2_Framework_TestResult $result)

    /**
    * @param  PHPUnit2_Framework_TestResult  $result
    * @access protected
    */
    protected function printIncompletes(PHPUnit2_Framework_TestResult $result) {
        $this->printDefects($result->notImplemented(), $result->notImplementedCount(), 'incomplete testcase');
    }

    // }}}
    // {{{ protected function printHeader($timeElapsed)

    /**
    * @param  float   $timeElapsed
    * @access protected
    */
    protected function printHeader($timeElapsed) {
        if ($timeElapsed) {
            $this->write(
              sprintf(
                "\n\nTime: %s\n",

                $timeElapsed
              )
            );
        }
    }

    // }}}
    // {{{ protected function printFooter(PHPUnit2_Framework_TestResult $result)

    /**
    * @param  PHPUnit2_Framework_TestResult  $result
    * @access protected
    */
    protected function printFooter(PHPUnit2_Framework_TestResult $result) {
        if ($result->allCompletlyImplemented() &&
            $result->wasSuccessful()) {
            $this->write(
              sprintf(
                "\nOK (%d test%s)\n",

                $result->runCount(),
                ($result->runCount() == 1) ? '' : 's'
              )
            );
        }
        
        else if (!$result->allCompletlyImplemented() &&
                 $result->wasSuccessful()) {
            $this->write(
              sprintf(
                "\nOK, but incomplete test cases!!!\nTests run: %d, incomplete test cases: %d.\n",

                $result->runCount(),
                $result->notImplementedCount()
              )
            );
        }        
        
        else {
            $this->write(
              sprintf(
                "\nFAILURES!!!\nTests run: %d, Failures: %d, Errors: %d, Incomplete Tests: %d.\n",

                $result->runCount(),
                $result->failureCount(),
                $result->errorCount(),
                $result->notImplementedCount()
              )
            );
        }
    }

    // }}}
    // {{{ public function printWaitPrompt()

    /**
    * @access public
    */
    public function printWaitPrompt() {
        $this->write("\n<RETURN> to continue\n");
    }

    // }}}
    // {{{ public function addError(PHPUnit2_Framework_Test $test, Exception $e)

    /**
    * An error occurred.
    *
    * @param  PHPUnit2_Framework_Test $test
    * @param  Exception               $e
    * @access public
    */
    public function addError(PHPUnit2_Framework_Test $test, Exception $e) {
        if ($e instanceof PHPUnit2_Framework_IncompleteTest) {
            $this->addIncompleteTest($test, $e);
        } else {
            $this->write('E');
        }
    }

    // }}}
    // {{{ public function addFailure(PHPUnit2_Framework_Test $test, PHPUnit2_Framework_AssertionFailedError $e)

    /**
    * A failure occurred.
    *
    * @param  PHPUnit2_Framework_Test                 $test
    * @param  PHPUnit2_Framework_AssertionFailedError $e
    * @access public
    */
    public function addFailure(PHPUnit2_Framework_Test $test, PHPUnit2_Framework_AssertionFailedError $e) {
        if ($e instanceof PHPUnit2_Framework_IncompleteTest) {
            $this->addIncompleteTest($test, $e);
        } else {
            $this->write('F');
        }
    }

    // }}}
    // {{{ public function addIncompleteTest(PHPUnit2_Framework_Test $test, Exception $e)

    /**
    * Incomplete test.
    *
    * @param  PHPUnit2_Framework_Test $test
    * @param  Exception               $e
    * @access public
    */
    public function addIncompleteTest(PHPUnit2_Framework_Test $test, Exception $e) {
        $this->write('I');
    }

    // }}}
    // {{{ public function endTest(PHPUnit2_Framework_Test $test)

    /**
    * A test ended.
    *
    * @param  PHPUnit2_Framework_Test $test
    * @access public
    */
    public function endTest(PHPUnit2_Framework_Test $test) {
    }

    // }}}
    // {{{ public function startTest(PHPUnit2_Framework_Test $test)

    /**
    * A test started.
    *
    * @param  PHPUnit2_Framework_Test $test
    * @access public
    */
    public function startTest(PHPUnit2_Framework_Test $test) {
        $this->write('.');

        if ($this->column++ >= 40) {
            $this->column = 0;
            $this->write("\n");
        }
    }

    // }}}
}

/*
 * vim600:  et sw=2 ts=2 fdm=marker
 * vim<600: et sw=2 ts=2
 */
?>
