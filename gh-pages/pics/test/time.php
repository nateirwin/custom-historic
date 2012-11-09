<?php

require_once "../lib/init/StoreSetup.php";
incAdodb();
require_once('adodb.inc.php');
ob_start();
function NotifyExpire($ref,$key) {
     print "<p><b>Notify Expiring=$ref, sessionkey=$key</b></p>";
}
                                                                                                                                                            
                                                                                                                                                            
$ADODB_SESSION_DRIVER='mysql';
$ADODB_SESSION_CONNECT='localhost';
$ADODB_SESSION_USER ='root';
$ADODB_SESSION_PWD ='';
$ADODB_SESSION_DB ='pos';
$ADODB_SESS_LIFE = 120;
                                                                                                                                                            
//$ADODB_SESS_DEBUG = true;
$USER = 'h_izan@yahoo.com';
$ADODB_SESSION_EXPIRE_NOTIFY = array('USER','NotifyExpire');
                                                                                                                                                            
error_reporting(E_ALL);
include('session/adodb-cryptsession.php');
session_start();
print "session id <br>";
print session_id()."<br>";
if (ADODB_Session::read(session_id()) == '') {
	ADODB_Session::destroy(session_id());
	//ADODB_Session::gc(160);
	//unset($_COOKIE['PHPSESSID']);
	//unset($_SESSION);
	//session_destroy();
	//session_unset();
	print "session info is empty <br>";
}

print ADODB_Session::read(session_id()) . "<br>";
print date("F j, Y, g:i a s",1095289294) . "<br>";
print date("F j, Y, g:i a s",1095289337);
ob_end_flush();
?>
