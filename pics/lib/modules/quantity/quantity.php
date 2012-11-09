<?php

/*for multiple numbers*/
if ( isset($_REQUEST['display']) && $_REQUEST['display'] != 0 ){
    $quantity=$_REQUEST['display'];
    $smarty->assign("display",$quantity);
} else {
    $quantity=1;
    $smarty->assign("display","0");
}

?>
