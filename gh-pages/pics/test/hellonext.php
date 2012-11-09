<?php

 if (!isset($auth)) {
        print "auth is not set <br>";
        exit();
    }
                                                                                                                                                            
    $auth->start();
    //print $auth->getUsername();
    if ($auth->getAuth()) {
        echo "<meta http-equiv=\"refresh\" content=\"0; url=index.php?productId=1&categId=1\">";
        //print "authenticated";
    } else {
        print "not authenticated";
    }

?>
