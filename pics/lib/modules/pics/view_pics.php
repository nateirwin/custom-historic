<?

incImage();

require_once "ThumbNail.php";
require_once "Pics.php";

if ( isset($_POST['uid']) ) {
    $smarty->assign("uid",$_POST['uid']);
}

if ( isset($_POST['uid']) && $_POST['uid'] > 0 ) {
    $userPics = new Pics();
    $userPictures = $userPics->picsByUid($_POST['uid']);
	$smarty->assign("userPics",$userPictures);
}

?>
