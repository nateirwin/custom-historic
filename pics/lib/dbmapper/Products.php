<?php

require_once "Connect.php";

class Products extends Connect{

	var $productId;
	var $cache;
	var $cacheTime;

	function Products($productId = 0, 
						$cache = 0, 
						$cacheTime = 0,
						$persistent = 0 ) {
		parent::__construct($persistent);
		$this->productId     = $productId;
		$this->cache      = $cache;
        $this->cacheTime  = $cacheTime;
	}

	function insertToProducts($categId,
								$name,        
								$description,    
								$price, 
								$status) {

	     /*	if (productId == 0)
				Use Auto_Increment to assign productId field
			else
				Assign the value passed in Constructor */
		if ($this->productId == 0) {
			$insertQuery = "INSERT INTO Products (categId,name,description,price,status)
									        VALUES ($categId,'$name','$description',$price,$status)";				
		} else {
			$insertQuery = "INSERT INTO Products (categId,name,description,price,status,productId) 
											VALUES ($categId,'$name','$description',$price,$status,$this->productId)";
		}
	
		parent::executeQuery($insertQuery);
	
	} /*end function*/

}/*end class*/


?>
