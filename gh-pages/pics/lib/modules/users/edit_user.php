<?php

require_once "Users.php";

/*set error messages String to null*/
$error = "";
$success = "";
$action = "";

/*first entry to page ignore all the logic*/
if ( !isset($_GET['uid']) ) {
	$firstPage = "true";
	$users = new Users();
	$info = $users->userInfo();
	$smarty->assign("users",$info);
} else {
	
	if ( isset($_GET['db']) && !strcmp($_GET['db'],"true") ) {
		$firstPage = "false";
		$action="edit.php?uid=".$_GET['uid']."&db=false";
		$users = new Users();
		$info = $users->allById($_GET['uid']);
		$smarty->assign("title",$info['title']);
		$smarty->assign("fname",$info['fname']);
		$smarty->assign("mname",$info['mname']);
		$smarty->assign("lname",$info['lname']);
		$smarty->assign("address",$info['address']);
		$smarty->assign("city",$info['city']);
		$smarty->assign("state",$info['state']);
		$smarty->assign("zip",$info['zip']);
		$smarty->assign("htel",$info['homeTel']);
		$smarty->assign("wtel",$info['workTel']);
		$smarty->assign("ctel",$info['cellTel']);
		$smarty->assign("ct",$info['contactPerson']);
		$smarty->assign("ctt",$info['contactTel']);	
		$smarty->assign("email",$info['email']);
		$smarty->assign("verify",$info['email']);
	} else {
		$firstPage = "false";
		$action="edit.php?uid=".$_GET['uid']."&db=false";
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
		$email = inputValue("email");
        $smarty->assign("email",$email);
		/*if no error occured insert to db and verify that insert was successful*/
	    if (!strcmp($error,"")) { 
			$user = new Users();
			$uid = $_GET['uid'];
			$update	= $user->updateUsers($title,$firstName,$mname,$lastName,$address,$city,$state,$zip,$hTel,$wTel,$cTel,$ct,$ctTel,$passWord,$email,$uid);
			$updated = $user->getUser($uid);
			if ( !isset($update) ) {
				$error = "*Please choose a unique email/userId <br>";
			} else if ( !isset($update) ) {
				$error = "*An Error is occured while inserting to the Database <br>
                            Please consult your Systems Administrator";
			} else {
				$smarty->assign("userPass",$passWord);
				$smarty->assign("updated",$updated);
				$success = "*Successfully updated Customer's information";
			}
		}
	}/*else db=false*/

}/*else $_GET['uid'] variable is not null*/	
	$smarty->assign("firstPage",$firstPage);
	$smarty->assign("error",$error);
	$smarty->assign("action",$action);
	$smarty->assign("success",$success);

?>
