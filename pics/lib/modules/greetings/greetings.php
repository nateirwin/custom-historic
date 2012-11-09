<?php

$smarty->assign("welcome",$txtm->msg("Welcome"));
$user = StoreSession::user();
$smarty->assign("user",$user);

?>
