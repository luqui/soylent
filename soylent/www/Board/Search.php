<?
define('IN_EBB', true);
/*
Filename: Search.php
Last Modified: 12/28/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";

$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$search[search]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
//check to see if the install file is stil on the user's server.
if (file_exists("install.php")){
if ($access_level == 1){
$error = $txt['installadmin'];
echo error($error, "error");
}
else{
$error = $txt['install'];
echo error($error, "general");
}
}
//check to see if this user is able to access this board.
echo check_ban();
//check to see if the board is on or off.
if ($board_status == 0){
$offline_msg = nl2br($off_msg);
$error = $offline_msg;
if ($access_level == 1){
$error .= "[<a href=\"CP.php\">$menu[cp]</a>]";
}
echo error($error, "general");
}
//output top
if ($access_level == 1){
$page = new template($template_path ."/top-admin.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-WELCOME" => "$txt[welcome]",
  "LOGGEDUSER" => "$logged_user",
  "LANG-LOGOUT" => "$txt[logout]",
  "LANG-PM" => "$menu[launchpm]",
  "LANG-CP" => "$menu[cp]",
  "TABLE1" => "$table1",
  "ADDRESS" => "$address",
  "LANG-HOME" => "$menu[home]",
  "LANG-SEARCH" => "$menu[search]",
  "LANG-FAQ" => "$menu[faq]",
  "LANG-MEMBERLIST" => "$menu[members]",
  "LANG-GROUPLIST"=> "$menu[groups]",
  "LANG-PROFILE" => "$menu[profile]"));

$page->output();
//check to see if user is marked as online, if not mark them as online.
$time = time();
$db->run = "select * from ebb_online where Username='$logged_user'";
$count_member = $db->num_results();
$db->close();
if ($count_member == 0){
//user seems to be just getting on.
$db->run = "insert into ebb_online (Username, time, location) values('$logged_user', '$time', '$_SERVER[PHP_SELF]')";
$db->query();
$db->close();
}
else{
//user is still here so lets up their time to let the script know the user is still around.
$db->run = "update ebb_online Set time='$time', location='$_SERVER[PHP_SELF]' where Username='$logged_user'";
$db->query();
$db->close();
}
}
if (($stat == "Member") OR ($access_level == 2) OR ($access_level == 3)){
$page = new template($template_path ."/top-logged.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-WELCOME" => "$txt[welcome]",
  "LOGGEDUSER" => "$logged_user",
  "LANG-LOGOUT" => "$txt[logout]",
  "LANG-PM" => "$menu[launchpm]",
  "TABLE1" => "$table1",
  "ADDRESS" => "$address",
  "LANG-HOME" => "$menu[home]",
  "LANG-SEARCH" => "$menu[search]",
  "LANG-FAQ" => "$menu[faq]",
  "LANG-MEMBERLIST" => "$menu[members]",
  "LANG-GROUPLIST"=> "$menu[groups]",
  "LANG-PROFILE" => "$menu[profile]"));

$page->output();
//check to see if user is marked as online, if not mark them as online.
$time = time();
$db->run = "select * from ebb_online where Username='$logged_user'";
$count_member = $db->num_results();
$db->close();
if ($count_member == 0){
//user seems to be just getting on.
$db->run = "insert into ebb_online (Username, time, location) values('$logged_user', '$time', '$_SERVER[PHP_SELF]')";
$db->query();
$db->close();
}
else{
//user is still here so lets up their time to let the script know the user is still around.
$db->run = "update ebb_online Set time='$time', location='$_SERVER[PHP_SELF]' where Username='$logged_user'";
$db->query();
$db->close();
}
}
if ($stat == "guest"){
$page = new template($template_path ."/top-guest.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-WELCOME" => "$txt[welcomeguest]",
  "LANG-LOGIN" => "$txt[login]",
  "LANG-REGISTER" => "$txt[register]",
  "TABLE1" => "$table1",
  "ADDRESS" => "$address",
  "LANG-HOME" => "$menu[home]",
  "LANG-SEARCH" => "$menu[search]",
  "LANG-FAQ" => "$menu[faq]",
  "LANG-MEMBERLIST" => "$menu[members]",
  "LANG-GROUPLIST"=> "$menu[groups]",));

$page->output();
//check to see if guest is marked as online, if not mark them as online.
$time = time();
$ip = $_SERVER["REMOTE_ADDR"];
$db->run = "select * from ebb_online where ip='$ip'";
$count_guest = $db->num_results();
$db->close();
if ($count_guest == 0){
$db->run = "insert into ebb_online (ip, time, location) values('$ip', '$time', '$_SERVER[PHP_SELF]')";
$db->query();
$db->close();
}
else{
//user is still here so lets up their time to let the script know the user is still around.
$db->run = "update ebb_online Set time='$time', location='$_SERVER[PHP_SELF]' where ip='$ip'";
$db->query();
$db->close();
}
}
//display search
  switch ( $_GET['action'] )
  {
  case 'user_result';
//get query text to perform search.
$search_type = anti_injection($_GET['search_type']);
$search_type = removeEvilAttributes($_GET['search_type']);
$search_type = htmlentities($_GET['search_type'], ENT_QUOTES);
$poster = anti_injection($_GET['poster']);
$poster = removeEvilAttributes($_GET['poster']);
$poster = htmlentities($_GET['poster'], ENT_QUOTES);
//see if user added too many characters in query.
if ((strlen($keyword) > 50) or (strlen($poster) > 50)){
$error = $search['toolong'];
echo error($error, "error");
}
//get results
if ($search_type == "topic"){
$sql = "SELECT * FROM ebb_topics WHERE author LIKE '$poster'";
$errorq = $sql;
$search_result = mysql_query($sql) or die(error($error, "mysql", $errorq));
$searchresults = search_results_topic();
}
if ($search_type == "post"){
$sql = "SELECT * FROM ebb_posts WHERE re_author LIKE '$poster'";
$errorq = $sql;
$search_result = mysql_query($sql) or die(error($error, "mysql", $errorq));
$searchresults = search_results_post();
}
if (($poster == "") and ($keyword == "") AND ($search_type == "")){
$error = $search['nokeyword'];
echo error($error, "error");
}
$num = mysql_num_rows($search_result);
if ($num == 0){
$error = $search['noresults'];
echo error($error, "general");
}
$page = new template($template_path ."/searchresults.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$search[search]",
  "LANG-SEARCHRESULTS" => "$search[searchresults]",
  "NUM-RESULTS" => "$num",
  "LANG-RESULTS" => "$search[result]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "LANG-USERNAME" => "$search[author]",
  "LANG-TOPIC" => "$search[topic]",
  "RESULTS" => "$searchresults"));
$page->output();
break;
  case 'result';
//get query text to perform search.
$search_type = anti_injection($_POST['search_type']);
$search_type = removeEvilAttributes($_POST['search_type']);
$search_type = htmlentities($_POST['search_type'], ENT_QUOTES);
$keyword = anti_injection($_POST['keyword']);
$keyword = removeEvilAttributes($_POST['keyword']);
$keyword = htmlentities($_POST['keyword'], ENT_QUOTES);
$poster = anti_injection($_POST['poster']);
$poster = removeEvilAttributes($_POST['poster']);
$poster = htmlentities($_POST['poster'], ENT_QUOTES);
$board = anti_injection($_POST['board']);
$board = removeEvilAttributes($_POST['board']);
$board = htmlentities($_POST['board'], ENT_QUOTES);
//see if user added too many characters in query.
if ((strlen($keyword) > 50) or (strlen($poster) > 50)){
$error = $search['toolong'];
echo error($error, "error");
}
//get results
if (($search_type == "topic") or ($search_type == "")){
$sql = "SELECT * FROM ebb_topics WHERE author LIKE '$poster' OR Topic LIKE '$keyword' OR Body LIKE '$keyword' OR bid LIKE '$board'";
$errorq = $sql;
$search_result = mysql_query($sql) or die(error($error, "mysql", $errorq));
$searchresults = search_results_topic();
}
if ($search_type == "post"){
$sql = "SELECT * FROM ebb_posts WHERE re_author LIKE '$poster' OR Body LIKE '$keyword' OR bid LIKE '$board'";
$errorq = $sql;
$search_result = mysql_query($sql) or die(error($error, "mysql", $errorq));
$searchresults = search_results_post();
}
if (($poster == "") and ($keyword == "")){
$error = $search['nokeyword'];
echo error($error, "error");
}
$num = mysql_num_rows($search_result);
if ($num == 0){
$error = $search['noresults'];
echo error($error, "general");
}
$page = new template($template_path ."/searchresults.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$search[search]",
  "LANG-SEARCHRESULTS" => "$search[searchresults]",
  "NUM-RESULTS" => "$num",
  "LANG-RESULTS" => "$search[result]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "LANG-USERNAME" => "$search[author]",
  "LANG-TOPIC" => "$search[topic]",
  "RESULTS" => "$searchresults"));
$page->output();
break;
case 'newposts':
//find topics
$sql = "SELECT * FROM ebb_topics WHERE Original_Date<='$last_visit' or last_update='$last_visit'";
$errorq = $sql;
$search_result = mysql_query($sql) or die(error($error, "mysql", $errorq));
//find posts
$sql2 = "SELECT * FROM ebb_posts WHERE Original_Date<='$last_visit'";
$errorq = $sql2;
$search_result2 = mysql_query($sql2) or die(error($error, "mysql", $errorq));
$result_info = search_results_newposts();
$search_results = $result_info[0];
$count = $result_info[1];
//see if there's no results found.
if ($count == 0){
$error = $search['noresults'];
echo error($error, "general");
}
//output result.
$page = new template($template_path ."/searchresults.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$search[search]",
  "LANG-SEARCHRESULTS" => "$search[searchresults]",
  "NUM-RESULTS" => "$count",
  "LANG-RESULTS" => "$search[result]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "LANG-USERNAME" => "$search[author]",
  "LANG-TOPIC" => "$search[topic]",
  "RESULTS" => "$search_results"));
$page->output();
break;
default:
$boardlist = board_select();
$page = new template($template_path ."/search.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$search[search]",
  "LANG-TEXT" => "$search[text]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-KEYWORD" => "$search[keyword]",
  "LANG-USERNAME" => "$search[author]",
  "LANG-SELBOARD" => "$search[selboard]",
  "BOARDLIST" => "$boardlist",
  "LANG-TEXT2" => "$search[text2]",
  "LANG-TOPIC" => "$search[topic]",
  "LANG-POST" => "$search[post]",
  "LANG-SEARCH" => "$search[search]"));
$page->output();
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
