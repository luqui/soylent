<?
define('IN_EBB', true);
/*
Filename: viewboard.php
Last Modified: 12/27/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
$bid = anti_injection($_GET['bid']);
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$viewboard[title]",
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
//record user comming in here
$db->run = "select * from ebb_read WHERE Board='$bid' and user='$logged_user'";
$read_ct = $db->num_results();
$db->close();
if (($read_ct == 0) AND ($stat !== "guest")){
$db->run = "insert into ebb_read (Board, user) values('$bid', '$logged_user')";
$db->query();
$db->close();
}
//check to see if board exists or not and if it doesn't kill the program
$db->run = "select * from ebb_boards WHERE id = '$bid'";
$checkboard = $db->num_results();
$db->close();
if ($checkboard == 0){
$error = $viewboard['doesntexist'];
echo error($error, "error");
}
//list boards
$db->run = "select * from ebb_boards WHERE id='$bid'";
$rules = $db->result();
$db->close();
//check for the posting rule.
$db->run = "select * from ebb_board_access WHERE B_id='$bid'";
$board_rule = $db->result();
$db->close();
if ($stat == "guest"){
//guest has no power to post anything at all.
$posting = '';
}else{
//determine rules.
if (($board_rule['B_Post'] == 1) AND ($access_level != 1)){
$posting = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}elseif (($board_rule['B_Post'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$posting = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}elseif ($board_rule['B_Post'] == 4){
$posting = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}else{
//determine poll rule.
if (($board_rule['B_Poll'] == 1) AND ($access_level == 1)){
//this board can't post polls.
$posting = "<a href=\"Post.php?mode=New_Topic&amp;bid=$bid\"><img src=\"$template_path/images/newtopic.gif\" border=\"0\" alt=\"$viewboard[addnewtopic]\"></a>&nbsp;<a href=\"Post.php?mode=New_Poll&amp;bid=$bid\"><img src=\"$template_path/images/newpoll.gif\" border=\"0\" alt=\"$viewboard[addnewpoll]\"></a><br><br>";
}elseif(($board_rule['B_Poll'] == 2) AND ($access_level == 1) or ($access_level == 2)){
$posting = "<a href=\"Post.php?mode=New_Topic&amp;bid=$bid\"><img src=\"$template_path/images/newtopic.gif\" border=\"0\" alt=\"$viewboard[addnewtopic]\"></a>&nbsp;<a href=\"Post.php?mode=New_Poll&amp;bid=$bid\"><img src=\"$template_path/images/newpoll.gif\" border=\"0\" alt=\"$viewboard[addnewpoll]\"></a><br><br>";
}elseif(($board_rule['B_Poll'] == 3) AND ($access_level == 1) or ($access_level == 2) or ($stat == "Member") OR ($access_level == 3)){
$posting = "<a href=\"Post.php?mode=New_Topic&amp;bid=$bid\"><img src=\"$template_path/images/newtopic.gif\" border=\"0\" alt=\"$viewboard[addnewtopic]\"></a>&nbsp;<a href=\"Post.php?mode=New_Poll&amp;bid=$bid\"><img src=\"$template_path/images/newpoll.gif\" border=\"0\" alt=\"$viewboard[addnewpoll]\"></a><br><br>";
}else{
//this user can't make a poll.
$posting = "<a href=\"Post.php?mode=New_Topic&amp;bid=$bid\"><img src=\"$template_path/images/newtopic.gif\" border=\"0\" alt=\"$viewboard[addnewtopic]\"></a><br><br>";
}
}
}
//output the rules of this board.
if($board_rule['B_Read'] == 1){
$board_policy = $viewtopic['read1']. "<br>";
}
if($board_rule['B_Read'] == 2){
$board_policy = $viewtopic['read2']. "<br>";
}
if($board_rule['B_Read'] == 3){
$board_policy = $viewtopic['read3']. "<br>";
}
if($board_rule['B_Read'] == 0){
$board_policy = $viewtopic['read0']. "<br>";
}
if($board_rule['B_Post'] == 1){
$board_policy .= $viewtopic['post1']. "<br>";
}
if($board_rule['B_Post'] == 2){
$board_policy .= $viewtopic['post2']. "<br>";
}
if($board_rule['B_Post'] == 3){
$board_policy .= $viewtopic['post3']. "<br>";
}
if($board_rule['B_Post'] == 4){
$board_policy .= $viewtopic['post4']. "<br>";
}
if($board_rule['B_Reply'] == 1){
$board_policy .= $viewtopic['reply1']. "<br>";
}
if($board_rule['B_Reply'] == 2){
$board_policy .= $viewtopic['reply2']. "<br>";
}
if($board_rule['B_Reply'] == 3){
$board_policy .= $viewtopic['reply3']. "<br>";
}
if($board_rule['B_Reply'] == 4){
$board_policy .= $viewtopic['reply4']. "<br>";
}
if($board_rule['B_Vote'] == 1){
$board_policy .= $viewtopic['vote1']. "<br>";
}
if($board_rule['B_Vote'] == 2){
$board_policy .= $viewtopic['vote2']. "<br>";
}
if($board_rule['B_Vote'] == 3){
$board_policy .= $viewtopic['vote3']. "<br>";
}
if($board_rule['B_Vote'] == 4){
$board_policy .= $viewtopic['vote4']. "<br>";
}
if($board_rule['B_Poll'] == 1){
$board_policy .= $viewtopic['poll1']. "<br>";
}
if($board_rule['B_Poll'] == 2){
$board_policy .= $viewtopic['poll2']. "<br>";
}
if($board_rule['B_Poll'] == 3){
$board_policy .= $viewtopic['poll3']. "<br>";
}
if($board_rule['B_Poll'] == 4){
$board_policy .= $viewtopic['poll4']. "<br>";
}
if($board_rule['B_Edit'] == 1){
$board_policy .= $viewtopic['edit1']. "<br>";
}
if($board_rule['B_Edit'] == 2){
$board_policy .= $viewtopic['edit2']. "<br>";
}
if($board_rule['B_Edit'] == 3){
$board_policy .= $viewtopic['edit3']. "<br>";
}
if($board_rule['B_Edit'] == 4){
$board_policy .= $viewtopic['edit4']. "<br>";
}
if($board_rule['B_Delete'] == 1){
$board_policy .= $viewtopic['del1']. "<br>";
}
if($board_rule['B_Delete'] == 2){
$board_policy .= $viewtopic['del2']. "<br>";
}
if($board_rule['B_Delete'] == 3){
$board_policy .= $viewtopic['del3']. "<br>";
}
if($board_rule['B_Delete'] == 4){
$board_policy .= $viewtopic['del4']. "<br>";
}
if($board_rule['B_Important'] == 1){
$board_policy .= $viewtopic['important1']. "<br>";
}
if($board_rule['B_Important'] == 2){
$board_policy .= $viewtopic['important2']. "<br>";
}
if($board_rule['B_Important'] == 3){
$board_policy .= $viewtopic['important3']. "<br>";
}
if($board_rule['B_Important'] == 4){
$board_policy .= $viewtopic['important4']. "<br>";
}
//see if this user is a moderator of this board.
$db->run = "select * from ebb_grouplist WHERE board_id='$bid' and group_id='$level_result[id]'";
$checkmod = $db->num_results();
$db->close();
if($checkmod == 1){
$board_policy .= $viewtopic['moderated'];
}
$pg = $_REQUEST['pg'];
$query = $_POST['query'];
//pagination
if(!isset($_GET['pg'])){
    $pg = 1;
} else {
    $pg = $_GET['pg'];
}
// Define the number of results per page
$max_results = 10;

// Figure out the limit for the query based
// on the current page number.
$from = (($pg * $max_results) - $max_results);
// Figure out the total number of results in DB:
$db->run = "select * from ebb_topics WHERE bid = '$bid' ORDER BY important DESC, last_update DESC LIMIT $from, $max_results";
$query = $db->query();
$db->close();
$db->run = "select * from ebb_topics WHERE bid = '$bid' ORDER BY important DESC, last_update DESC";
$num = $db->num_results();
$db->close();
// Figure out the total number of pages. Always round up using ceil()
$total_pages = ceil($num / $max_results);
$pagenation = "<div align=\"left\">$txt[pages]&nbsp;";
// Build page number
if($pg > 1){
$prev = ($pg - 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$prev&amp;bid=$bid\">$txt[prev]</a></small>&nbsp;";
}
//output numbers.
for($i = 1; $i <= $total_pages; $i++){
if(($pg) == $i){
$pagenation .= "<b>$i</b>&nbsp;";
} else {
$pagenation .= "<a href=\"$_SERVER[PHP_SELF]?pg=$i&amp;bid=$bid\">$i</a>&nbsp;";
}
}
// Build Next Link
if($pg < $total_pages){
$next = ($pg + 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$next&amp;bid=$bid\">$txt[next]</a></small>";
}
$pagenation .= "</div>";
$board = board_listing();
$page = new template($template_path ."/viewboard.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$rules[Board]",
  "PAGENATION" => "$pagenation",
  "POST-RULE" => "$posting",
  "BOARD-POLICY" => "$board_policy",
  "LANG-TOPIC" => "$viewboard[topic]",
  "LANG-POSTEDBY" => "$viewboard[postedby]",
  "LANG-REPLIES" => "$viewboard[replies]",
  "LANG-POSTVIEWS" => "$viewboard[views]",
  "LANG-LASTPOSTEDBY" => "$viewboard[lastpost]",
  "VIEWBOARD" => "$board",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-ICONGUIDE" => "$viewboard[guide]",
  "LANG-NEW" =>"$viewboard[newtopic]",
  "LANG-OLD" =>"$viewboard[oldtopic]",
  "LANG-POLL" =>"$viewboard[polltopic]",
  "LANG-LOCKED" =>"$viewboard[lockedtopic]",
  "LANG-IMPORTANT" => "$viewboard[importanttopic]",
  "LANG-HOTTOPIC" => "$viewboard[hottopic]"));
$page->output();
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
