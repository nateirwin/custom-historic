<?php
	
	require_once "../lib/init/Setup.php";
	require_once "Categoriess.php";	
	require_once "tohtml.inc.php";

	$success = 0;
	$failure = 0;

	//print ini_get("include_path")."\n";
	function testInsert() {
	
		global $success, $failure;
	
		$category = new Categoriess();	
		if( !isset($category) ) {print "Category is not set"; $failure++;}	
		if ( ( $category->insertToCategories(1,"hello","hello",0) ) === false) {
			$failure++;
		} else {
			$success++;
		}

		$category = new Categories(3);
        if( !isset($category) ) {print "Category is not set"; $failure++;}
        if ( ( $category->insertToCats(0,"hello","hello",0) ) === false) {
            $failure++;
        } else {
            $success++;
        }		

	}
		
	function testDelete() {

		global $success, $failure;
		
		$category = new Categories(1);
		if( !isset($category) ) { print "Category is not set"; $failure++; }
        if ( ($category->deleteFromCats()) === false) {
			$failure++;
		} else {
			$success++;
		}

		$category = new Categories(3);
        if( !isset($category) ) { print "Category is not set"; $failure++; }
        if ( ($category->deleteFromCats()) === false) {
            $failure++;
        } else {
            $success++;
        }
        
	}

	function testFindAll() {

		global $success, $failure;
	
		$category = new Categories();
		if( !isset($category) ) { print "Category is not set"; $failure++; }
		if ( ( $rs = $category->findAll() ) === false ) {
			$failure++;
		} else {
			rs2html($rs);
			$success++; 
		}
				
	}

	function testFindByParentId() {
                                                                                                                                        
        global $success, $failure;
                                                                                                                                        
        $category = new Categories();
        if( !isset($category) ) { print "Category is not set"; $failure++; }
        if ( ( $rs = $category->findFindByParentId(0) ) === false ) {
            $failure++;
        } else {
            rs2html($rs);
            $success++;
        }
                                                                                                                                        
    }
	testInsert();
	testFindAll();
	//testDelete();

	print "CategoryTest.php     SUCCESS=".$success."   FAILURE=".$failure."\n <br>";
	print "Correct Results  SUCCESS=7   FAILURE=2 \n";

?>
