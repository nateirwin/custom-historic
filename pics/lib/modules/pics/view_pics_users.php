<?

require_once "Pics.php";

if ( isset($_POST['uid']) ) {
    $smarty->assign("uid",$_POST['uid']);
}

$user = StoreSession::user();
if ( isset($user) ) {
    $userPics = new Pics();
    $userPictures = $userPics->picsByName($user);
	//print_r($userPictures);
	$smarty->assign("userPics",$userPictures);
}

?>
