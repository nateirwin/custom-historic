<?

require_once "Pics.php";

$user = StoreSession::user();
if ( isset($user) ) {
    $userPics = new Pics();
	$userPictures = $userPics->commByName($user);
    $smarty->assign("userPics",$userPictures);
} 
if ( isset($_POST) ) {
	$userComm = new Pics();
    foreach ($_POST as $key => $comment) {
		if ( isset($key) && isset($comment) ) {
			$userComm->updateUC($key,$comment);
		}
		header('Location: view.php');
    }
} 

?>
