<?php

require_once "Users.php";

/*set error messages String to null*/
$error = "";
$insert = 0;

/*first entry to page ignore all the logic*/
if ( sizeof($_POST) <= 0 ){
} else {
	/*First Name Start*/
	$firstName = null;
	if ( !isset($_POST["fname"]) ) {
        $error = "*First Name field can NOT be Null <br>";
    } else {
        /*see if any empty strings involved*/
        $firstName = $_POST["fname"];
        $fsize = strlen(trim($firstName));
        if ( $fsize < 1 ) {
            $error = $error . "*First Name field Must be longer than zero characters <br>";
        } else {
            /*First Name is good*/
            $firstName = rtrim($_POST["fname"]); $firstName = ltrim($firstName);
			$smarty->assign("fname",$firstName);
        }
    }
	
	/*Last Name Start*/	
	$lastName = null;
	if ( !isset($_POST["lname"]) ) {
		$error = $error . "*Last Name field can NOT be Null <br>";
	} else {
	    /*see if any empty strings involved*/
	    $lastName = $_POST["lname"];
	    $lsize = strlen(trim($lastName));
	    if ( $lsize < 1 ) {
	        $error = $error . "*Last Name field Must be longer than zero characters <br>";
	    } else {
	        /*Last Name is good*/
		    $lastName = rtrim($_POST["lname"]); $lastName = ltrim($lastName);
			$smarty->assign("lname",$lastName);
		}
	}

	/*Password Start*/
    $passWord = null;
    if ( !isset($_POST["passwd"]) ) {
        $error = $error . "*Password field can NOT be Null <br>";
    } else {
        /*see if any empty strings involved*/
        $passWord = $_POST["passwd"];
        $psize = strlen(trim($passWord));
        if ( $psize < 6 ) {
            $error = $error . "*Password field Must be longer than 6 characters <br>";
        } else {
            /*Password is good*/
            $passWord = rtrim($_POST["passwd"]); $passWord = ltrim($passWord);
            $smarty->assign("passwd",$passWord);
        }
    }

	/*Email Start*/
	$email = null;
	if ( !isset($_POST["email"]) ) {
		$error = $error . "*Email field can NOT be Null <br>";
	} else {
	    /*see if any empty strings involved*/
	    $email = $_POST["email"];
	    $esize = strlen(trim($email));
	    if ( $esize < 1 ) {
	        $error = $error . "*Email field Must be longer than zero characters <br>";
	    } else {
	        /*email is good*/
	        $email = rtrim($_POST["email"]); $email = ltrim($email);
	        $smarty->assign("email",$email);
	    }
	}
	
	/*Email vs Verify Start*/
	if ( isset($_POST["verify"]) ) {
		$verify = rtrim($_POST["verify"]); $verify = ltrim($verify);	
		if ( !strcmp($email,$verify) ) {
			$smarty->assign("verify",$verify);
		} else {
			$error = $error . "*Email fields do NOT match <br>";
		}
	} else {
	    $error = $error . "*Second Email can NOT be Null <br>";
	}

	//print "fname=".$firstName." lname=".$lastName." email=".$email." verify=".$verify."<br>";
	function inputValue($name) {
		if ( !isset($_POST[$name]) ) { return ""; }
		else { $temp = rtrim($_POST[$name]); return ltrim($temp); }
	}

	$title = inputValue("title");
	$smarty->assign("title",$title);
	$mname = inputValue("mname");
	$smarty->assign("mname",$mname);
	$address = inputValue("address");
	$smarty->assign("address",$address);
	$city = inputValue("city");
	$smarty->assign("city",$city);
	$state = inputValue("state");
	$smarty->assign("state",$state);
	$zip = inputValue("zip");
	$smarty->assign("zip",$zip);
	$hTel = inputValue("htel");
	$smarty->assign("htel",$hTel);
	$wTel = inputValue("wtel");
	$smarty->assign("wtel",$wTel);
	$cTel = inputValue("ctel");
	$smarty->assign("ctel",$cTel);
	$ct = inputValue("ct");
	$smarty->assign("ct",$ct);
	$ctTel = inputValue("ctt");
	$smarty->assign("ctt",$ctTel);

	/*if no error occured insert to db and verify that insert was successful*/
    if (!strcmp($error,"")) { 
		$user = new Users($email);
		$insert	= $user->insertToUsers($title,$firstName,$mname,$lastName,$address,$city,$state,$zip,$hTel,$wTel,$cTel,$ct,$ctTel,$passWord);
		if ($insert > 0) {
			$inserted = $user->getUser($insert);
			if ( isset($inserted) ) {
				$smarty->assign("userPass",$passWord);
				$smarty->assign("inserted",$inserted);
				$smarty->assign("success","*Successfully inserted the new Customer's information");	
				$smarty->assign("inFlag","true");
			} else {
				$error = "*email/userId MUST be unique, Please choose another email/userId";
			}
		} else {
			$error = "*An Error is occured while inserting to the Database <br>
						Please consult your Systems Administrator";
		}
	}

}/*else $_POST variable is not null*/	
	$smarty->assign("error",$error);
	
?>
