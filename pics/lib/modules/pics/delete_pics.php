<?

require_once "Pics.php";

if ( isset($_POST['uid']) ) {
    $smarty->assign("uid",$_POST['uid']);
}

$userPics = new Pics();

if ( isset($_POST['uid']) && $_POST['uid'] > 0 ) {
	//print_r($_POST);
    $userPictures = $userPics->picsByUid($_POST['uid']);
	//print_r($userPictures);
	$smarty->assign("userPics",$userPictures);
} else if ( isset($_POST) ) {
	foreach ($_POST as $pic) {
		$rmOrig = "./images/$pic";
		$rmThumb = "./images/thb_$pic";
		$rmPop = "./images/pop_$pic";
		$out = unlink($rmOrig);
		$out = unlink($rmThumb);
		$out = unlink($rmPop);
		$userPics->deletePics($pic);
	}
}

?>
