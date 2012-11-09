<?php
incRSS();
require_once "RSS.php";
$rssLink =& new XML_RSS("http://slashdot.org/slashdot.rdf");
$rssLink->parse();
$smarty->assign("rssTitle","Headlines from <a href=\"http://slashdot.org\">Slashdot</a></h1>\n");
$rss = $rssLink->getItems();
//print_r($rss);
$smarty->assign("rss",$rss);
//    echo "<li><a href=\"" . $item['link'] . "\">" . $item['title'] . "</a></li>\n";
//}
?>
