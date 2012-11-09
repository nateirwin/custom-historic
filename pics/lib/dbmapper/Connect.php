<?php
/*Make this a class soon*/
incAdodb();
incHelp();
require_once "DBErrorCodes.php";

class Connect {

	var $storeDbConnection = null;

	function __construct ($persistent = 0 ) {

		$this->findSetDbDriver($persistent);

	}/*end constructor*/

	function query($query) {
		$rs = $this->executeQuery($query);
		return $rs->GetArray();
	}
                                                                                                                                           
	function executeQuery ($query) {
		if ($this->cache == 0) {
			if ( ($rs = $this->storeDbConnection->Execute($query) ) === false) {
				print $query . "<br>";
				print 'Error Retrieving All Fields: '.$this->storeDbConnection->ErrorMsg().'<BR>';
				return null;
			}
			return $rs;
		} else {
			/*get the current dir / cachedir for adodb queries*/
			global $ADODB_CACHE_DIR;
			$ADODB_CACHE_DIR  = dirname(__FILE__).'/ADOdbcache';
			if ( !$rs = $this->storeDbConnection->CacheExecute($this->cacheTime,$query) ) {
				print $query . "<br>";
				print 'Error Retrieving All Fields: '.$this->storeDbConnection->ErrorMsg().'<BR>';
				return null;
			}
			return $rs;
		}/*else*/
                                                                                                                                          
	}/*function */

	function validateIndex($queryResult,$index) {
		if ( isset($queryResult[$index]) ) {
	        return $queryResult[$index];
		} else {
		    return null;
		}
	}/*validate*/

	function validateIndexes($queryResult,$indexA,$indexB) {
		if ( isset($queryResult[$indexA][$indexB]) ) {
			return $queryResult[$indexA][$indexB];
		} else {
			return null;
		}
	}

/***********************************************************************************************************/
/*  functions below are not for external calls they are only for internal calls                            */
/***********************************************************************************************************/

	function findSetDbDriver($persistent) {
		switch ( DB_API ) {
			case "adodb" :	
				error_reporting (E_ALL);                /*show all the error messages*/
				require_once ('adodb.inc.php');
				require_once ('adodb-pear.inc.php');
				global $ADODB_FETCH_MODE;
				$ADODB_FETCH_MODE = ADODB_FETCH_ASSOC;
				if ($persistent == 0) {
					$this->storeDbConnection = &ADONewConnection(DB_TYPE);
						if (!$this->storeDbConnection->Connect(DB_HOST, DB_USER, DB_PASS, DB_NAME)) {
							print DBErrorCodes::code1();
							print $this->storeDConnection->ErrorMsg();
						}
				} else {
					$this->storeDbConnection = &ADONewConnection(DB_TYPE);
						if (!isset( $this->storeDbConnection ) ) {
							if (!$this->storeDbConnection->PConnect(DB_HOST, DB_USER, DB_PASS, DB_NAME)) {
								print ErrorCodes::code1();
								print $this->storeDbConnection->ErrorMsg();
							}
						}
               	}/*else*/
				break;
			default :
				print "Can't find the appropriate DB Abstraction Layer \n <BR>";
				break;
		}/*end switch*/

	}/*findSetDbDriver()*/


}/*end class*/
?>
