<?

require_once "Pics.php";

if ( isset($_POST['uid']) ) {
	$smarty->assign("uid",$_POST['uid']);
}

if ( isset($_POST['uid']) && $_POST['uid'] > 0 ) {
    $userPics = new Pics();
    $userPictures = $userPics-> admCommsByUid($_POST['uid']);
	$smarty->assign("userPics",$userPictures);
} else if ( isset($_POST) ) {
	$adminComm = new Pics();
    foreach ($_POST as $key => $comment) {
		if ( isset($key) && isset($comment) ) {
			$adminComm->updateAC($key,$comment);
		}
    }
}

?>
