<?php
/*
Module dpendencies:
1- Categories.php
2- $_GET['cid'] variable
*/

require_once "Categories.php";

$categId = $_GET['cid'];

$categs = new Categories();
$category = $categs->categById($categId);
$smarty->assign("ctSize",sizeof($category));
if ( (isset($category)) && (sizeof($category) > 0) ) {
	$topNavigate[] = $category[0];
	while ( $category[0]['parentId'] > 0 ) {
		$category = $categs->categById($category[0]['parentId']);
		$topNavigate[] = $category[0];
	}
	$topNavigate = array_reverse($topNavigate);
	//print_r($topNavigate);
	$smarty->assign("topNavigate",$topNavigate);

	$childCategs = new Categories();
	$subNavigate = $childCategs->categsByPId($categId);
	$smarty->assign("snSize",sizeof($subNavigate));
	$smarty->assign("subNavigate",$subNavigate);
}
?>
