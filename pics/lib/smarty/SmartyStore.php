<?php

incSmarty();
require_once 'Smarty.class.php';
incRoot();

class SmartyStore extends Smarty {
	
	function SmartyStore($module) {
		
		switch ($module) {
		
			case 'users' :
				$this->Smarty();
                $this->template_dir = "../templates/users";
                //$this.config_dir   = '../../smarty_config';
                $this->compile_dir  = "../templates_c/users";
                $this->caching = 0;
                $this->cache_dir    = "../templates_c/users/smarty_cache";
				break;

			case 'admin' :
				$this->Smarty();
                $this->template_dir = "../templates/admin";
                //$this.config_dir   = '../../smarty_config';
                $this->compile_dir  = "../templates_c/admin";
                $this->caching = 0;
                $this->cache_dir    = "../templates_c/admin/smarty_cache";	
				break;

			default :
				$this->Smarty();
                $this->template_dir = "../templates/users";
                //$this.config_dir   = '../../smarty_config';
                $this->compile_dir  = "../templates_c/users";
                $this->caching = 0;
                $this->cache_dir    = "../templates_c/users/smarty_cache";
				break;

		}/*end switch*/
	}

	function templates_dir($property) {
		return $property; 
	}
}

?>
