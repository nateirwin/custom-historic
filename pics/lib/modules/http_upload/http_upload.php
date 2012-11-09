<?php

incImage();
incHttpUpload();
require_once "Upload.php";
require_once "Pics.php";
require_once "ThumbNail.php";

$upload = new Http_Upload('en');
$files = $upload->getFiles();
if (PEAR::isError($files)) {
    die ($files->getMessage());
}
$success = "";
$uid = $_POST['uid'];
$smarty->assign("action","upload.php?pre=".$_POST['uid']);
$userPic = new Pics();
foreach ($files as $file) {
    //print_r($file->getProp());
    if ($file->isValid()) {
		$smarty->assign("uid",$uid);
		$filePre = $_GET['pre'];
        $file->setName('uniq');
        $destDir = './images/';
        $destName = $file->moveTo($destDir);
        $realName = $file->getProp('real');
		$userPic->insertToUP($filePre,$destName);
		/*resize the pictures*/
		$resize = new ThumbNail($destDir.$destName);
		$resize->build($destName);
        $success = $success . "*Uploaded file $realName <br>";
    } 
}

$smarty->assign("success",$success);

/*
if ($files->isMissing()) {
    $smarty->assign("error","No file selected <br>");
} 
if ($files->isError()) {
    $smarty->assign("error",$file->errorMsg()."<br>");
}
*/


?>
