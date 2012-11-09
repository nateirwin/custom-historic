<?php

	define('STORE_PATH', '/home/laurie/public_html/pics');

	ini_set("include_path",STORE_PATH."/lib/init".':'. ini_get("include_path"));
	ini_set("include_path",STORE_PATH."/lib/dbmapper".':'. ini_get("include_path"));
	ini_set("include_path",STORE_PATH."/lib/i18n".':'. ini_get("include_path"));
	require_once "local.php";
	
	function incRoot() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH.":" ) ) {
            ini_set("include_path", STORE_PATH .':'. ini_get("include_path"));
        }
    }

	function incLib() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib:" ) ) {
            ini_set("include_path", STORE_PATH."/lib".':'. ini_get("include_path"));
        }
    }
	
	function incPear() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/pear:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/pear".':'. ini_get("include_path"));
		} 
	}
	
	function incAdodb() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/adodb:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/adodb".':'. ini_get("include_path"));
		}
	}
	
	function incSmarty () {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/smarty:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/smarty".':'. ini_get("include_path"));
		}
	}

	function incHelp() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/help/action:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/help/action".':'. ini_get("include_path"));	
		}
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/help/error:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/help/error".':'. ini_get("include_path"));
		}
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/help/pages:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/help/pages".':'. ini_get("include_path"));
		}
	}

	function incPHPUnit() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/PHPUnit:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/PHPUnit".':'. ini_get("include_path"));
		}
	}

	function incUtil() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/util:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/util".':'. ini_get("include_path"));
		}
	}

	function incAuth() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/auth:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/auth".':'. ini_get("include_path"));
        }
	}
	
	function incSession() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/session:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/session".':'. ini_get("include_path"));
		}
	}
	
	function incCommServs() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/commservs:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/commservs".':'. ini_get("include_path"));
		}
	}
	
	function incUsers() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/includes/users:" ) ) {
            ini_set("include_path", STORE_PATH."/includes/users".':'. ini_get("include_path"));
        }
    }
	
	function incEcomm() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/includes/ecomm:" ) ) {
            ini_set("include_path", STORE_PATH."/includes/ecomm".':'. ini_get("include_path"));
        }
    }

	function incAdmin() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/includes/admin:" ) ) {
            ini_set("include_path", STORE_PATH."/includes/admin".':'. ini_get("include_path"));
        }
    }
	
	function incMods() {	
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/modules:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/modules".':'. ini_get("include_path"));
		}
	}

	function incRSS() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/xml/rss:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/xml/rss".':'. ini_get("include_path"));
		}
	}

	function incImage() {
        if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/image:" ) ) {
            ini_set("include_path", STORE_PATH."/lib/image".':'. ini_get("include_path"));
        }
    }

	function incHttpUpload() {
		if ( !strstr(ini_get("include_path"), ":".STORE_PATH."/lib/HTTP_Upload:" ) ) {
			ini_set("include_path", STORE_PATH."/lib/HTTP_Upload".':'. ini_get("include_path"));
		}
	}
	//print ini_get("include_path")."\n";
?>

