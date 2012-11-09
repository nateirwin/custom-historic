<?php

require_once "Connect.php";

class Categories extends Connect{

	var	$categId;
	var $cache;
	var $cacheTime;

	function Categories( $categId = 0, 
					$cache = 0, 
					$cacheTime = 0, 
					$persistent	= 0 ) {

		parent::__construct($persistent);
		$this->categId     = $categId;
		$this->cache      = $cache;
		$this->cacheTime  = $cacheTime;
	}

	function insertToCategs( $parentId,
							$name,        
							$description,    
							$position ) {

	     /*	if (categid == 0)
				Use Auto_Increment to assign categId field
			else
				Assign the value passed in Constructor */
		if ($this->categId == 0) {
			$insertQuery = "INSERT INTO Categories (parentId,name,description,position)
								         VALUES ($parentId,'$name','$description',$position)";				
		} else {
			$insertQuery = "INSERT INTO Categories (parentId,name,description,position,categId) 
										 VALUES ($parentId,'$name','$description',$position,$this->categId)";
		}
		
		parent::executeQuery($insertQuery);	
	
	} /*end function insertToCats()*/

	function categsByPId($parentId) {
		$query="SELECT categId,parentId,name FROM Categories WHERE parentId='$parentId' ORDER BY categId";
		$categs = parent::query($query);
		return $categs;
	}

	function categById($categId) {
		$query="SELECT categId,parentId,name FROM Categories WHERE categId='$categId'";
		$categ = parent::query($query);
		return $categ;
	}	
}/*end class*/


?>
