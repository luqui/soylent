<?
define('IN_EBB', true);
/*
Filename: viewtopic.php
Last Modified: 11/9/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$pid = anti_injection($_GET['pid']);
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$viewtopic[title]",
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
$db->run = "select * from ebb_read WHERE Topic='$tid' and user='$logged_user'";
$read_ct = $db->num_results();
$db->close();

if (($read_ct == 0) AND ($stat !== "guest")){
$db->run = "insert into ebb_read (Topic, user) values('$tid', '$logged_user')";
$db->query();
$db->close();
}
//check to see if topic exists or not and if it doesn't kill the program
$db->run = "select * FROM ebb_topics WHERE tid='$tid'";
$checktopic = $db->num_results();
$t_name = $db->result();
$db->close();
$db->run = "select * FROM ebb_boards WHERE id='$bid'";
$checkboard = $db->num_results();
$b_name = $db->result();
$db->close();
if (($checkboard == 0) or ($checktopic == 0)){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
//increment the total view of the topic by one.
$addone = $t_name['Views'] + 1;
$db->run = "update ebb_topics SET Views='$addone' where tid='$tid'";
$db->query();
$db->close();
//update the status of the topic watch.
$db->run = "select * from ebb_topic_watch where username='$logged_user' and tid='$tid'";
$t_watch = $db->result();
$db->close();
if ($t_watch['status'] == "Unread"){
$db->run = "update ebb_topic_watch SET status='Read' where username='$logged_user' and tid='$tid'";
$db->query();
$db->close();
}
//check for the posting rule.
$db->run = "select * from ebb_board_access WHERE B_id='$bid'";
$board_rule = $db->result();
$db->close();
//see if the user can access this spot.
if (($board_rule['B_Read'] == 1) AND ($access_level !== 1)){
$error = $viewboard['noread'];
echo error($error, "error");
}elseif (($board_rule['B_Read'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$error = $viewboard['noread'];
echo error($error, "error");
}elseif (($board_rule['B_Read'] == 3) AND ($stat == "guest")){
$error = $viewboard['noread'];
echo error($error, "error");
}
//set some board vars.
$allowsmile = $b_name['Smiles'];
$allowbbcode = $b_name['BBcode'];
$allowhtml = $b_name['HTML'];
$allowimg = $b_name['Images'];
//begin pagenation
$pg = $_REQUEST['pg'];
$query = $_POST['query'];
if(!isset($_GET['pg'])){
$pg = 1;
} else {
$pg = $_GET['pg'];
}
//check to see if the user can post a reply.
if ($stat == "guest"){
$replylink = '';
}else{
if (($board_rule['B_Reply'] == 1) AND ($access_level != 1)){
$replylink = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}elseif (($board_rule['B_Reply'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$replylink = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}elseif ($board_rule['B_Reply'] == 4){
$replylink = "<img src=\"$template_path/images/locked.gif\"><br><br>";
}else{
if ($t_name['Locked'] == 0){
$replylink = "<a href=\"Post.php?mode=Reply&amp;tid=$tid&amp;bid=$bid&amp;pg=$pg\"><img src=\"$template_path/images/reply.gif\" border=\"0\" alt=\"$viewtopic[replytopicalt]\"></a><br><br>";
}else{
$replylink = "<img src=\"$template_path/images/locked.gif\"><br><br>";
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
// Define the number of results per page
$max_results = 10;

// Figure out the limit for the query based
// on the current page number.
$from = (($pg * $max_results) - $max_results);
// Figure out the total number of results in DB:
$db->run = "select * from ebb_posts WHERE tid='$tid' LIMIT $from, $max_results";
$query = $db->query();
$db->close();
$db->run = "select * from ebb_posts WHERE tid='$tid'";
$num = $db->num_results();
$db->close();
// Figure out the total number of pages. Always round up using ceil()
$total_pages = ceil($num / $max_results);
$pagenation = "<div align=\"left\">$txt[pages]&nbsp;";
// Build page number
if($pg > 1){
$prev = ($pg - 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$prev&amp;bid=$bid&amp;tid=$tid&amp;pid=$pid\">$txt[prev]</a></small>&nbsp;";
}
//output numbers.
if($num == 0){
$pagenation .= "<b>1</b>";
}else{
for($i = 1; $i <= $total_pages; $i++){
if(($pg) == $i){
$pagenation .= "<b>$i</b>&nbsp;";
} else {
$pagenation .= "<a href=\"$_SERVER[PHP_SELF]?pg=$i&amp;bid=$bid&amp;tid=$tid&amp;pid=$pid\">$i</a>&nbsp;";
}
}
}
// Build Next Link
if($page < $total_pages){
$next = ($pg + 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$next&amp;bid=$bid&amp;tid=$tid&amp;pid=$pid\">$txt[next]</a></small>";
}
$pagenation .= "</div>";
//end pagenation, get info.
$db->run = "select * from ebb_users WHERE Username='$t_name[author]'";
$user = $db->result();
$db->close();
$gmttime = gmdate ($time_format, $t_name['Original_Date']);
$topic_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
$total = $user['Post_Count'];
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $total ORDER BY Post_req DESC";
$rank = $db->result();
$db->close();
//rank star info.
if ($user['Status'] == "groupmember"){
//find what usergroup this user belongs to.
$db->run = "SELECT * FROM ebb_group_users where Username='$t_name[author]'";
$groupchk = $db->result();
$db->close();
//get the access level of this group.
$db->run = "SELECT * FROM ebb_groups where id='$groupchk[gid]'";
$level_r = $db->result();
$db->close();
$db->close();
if($level_r['Level'] == 1){
$rankicon = "<img src=\"$template_path/images/adminstar.gif\">";
$rank = $level_r['Name'];
}
if($level_r['Level'] == 2){
$rankicon = "<img src=\"$template_path/images/modstar.gif\">";
$rank = $level_r['Name'];
}
if($level_r['Level'] == 3){
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $total ORDER BY Post_req DESC";
$rank = $db->result();
$db->close();
$rankicon = "<img src=\"$template_path/images/$rank[Star_Image]\">";
$rank = $level_r['Name'];
}
}elseif($user['Status'] == "Banned"){
$rankicon = "";
$rank = "Banned";
}else{
$rankicon = "<img src=\"$template_path/images/$rank[Star_Image]\">";
$rank = $rank['Name'];
}
//avatar and sig info.
if($user['Avatar'] == ""){
$avatar = "images/noavatar.gif";
}else{
$avatar = $user['Avatar'];
}
if($user['Sig'] == ""){
$sig = "";
}else{
$tsig = nl2br($user['Sig']);
$sig = "-----------<br>$tsig";
}
//end info
$string = $t_name['Body'];
$msg = $string;
//see if user wished to disable smiles.
if($t_name['disable_smiles'] == 0){
if ($allowsmile == 1){
$msg = smiles($msg);
}
}
//see if user wished to disable bbcode
if($t_name['disable_bbcode'] == 0){
if ($allowbbcode == 1){
$msg = BBCode($msg);
}
if ($allowimg == 1){
$msg = BBCode($msg, true);
}
}
//censor convert.
$msg = language_filter($msg);
$msg = nl2br($msg);
if ($allowhtml == 1){
$msg = html_entity_decode($msg);
}
//display post
if ($checkmod == 1){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\" class=\"td\"><a href=\"manage.php?mode=viewip&amp;ip=$t_name[IP]&amp;u=$t_name[author]&amp;tid=$tid&amp;bid=$bid\"><img src=\"$template_path/images/ip_logged.gif\" alt=\"$viewtopic[iplogged]\" border=\"0\"></a>&nbsp;<a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}else{

if (($logged_user == $t_name['author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1) AND ($board_rule['B_Delete'] == 1)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($access_level == 1) AND ($board_rule['B_Delete'] == 1)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2) AND ($board_rule['B_Delete'] == 2)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Delete'] == 2)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($board_rule['B_Delete'] == 3)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($board_rule['B_Edit'] == 3)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $t_name['author']) AND ($board_rule['B_Delete'] == 3)){
$menu = "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=edittopic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_topic&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}else{
$menu = '';
}
}
//show replys, if any
$reply = reply_listing();
//admin & moderator options
if ($checkmod == 1){
if ($t_name['Locked'] == 1){
$modtool = "<a href=\"manage.php?mode=move&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/move.gif\" alt=\"\" border=\"0\"></a>&nbsp;<a href=\"manage.php?mode=delete&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/deletetopic.gif\" alt=\"\" border=\"0\"></a>&nbsp;<a href=\"manage.php?mode=unlock&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/unlock.gif\" alt=\"\" border=\"0\"></a><br>";
}else{
$modtool = "<a href=\"manage.php?mode=move&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/move.gif\" alt=\"\" border=\"0\"></a>&nbsp;<a href=\"manage.php?mode=delete&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/deletetopic.gif\" alt=\"\" border=\"0\"></a>&nbsp;<a href=\"manage.php?mode=lock&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/lock.gif\" alt=\"\" border=\"0\"></a><br>";
}
}
else{
$modtool = '<br>';
}
if ($t_name['Type'] == "Poll"){
//check to see if a user already voted.
$db->run = "SELECT * FROM ebb_votes WHERE Username='$logged_user' AND tid='$tid'";
$count = $db->num_results();
$db->close();
//see who can vote on the poll.
if(($board_rule['B_Vote'] == 1) AND ($access_level != 1)){
$canvote = 0;
}elseif(($board_rule['B_Vote'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$canvote = 0;
}elseif($board_rule['B_Vote'] == 4){
$canvote = 0;
}else{
$canvote = 1;
}
//display results
if (($count == 1) OR ($stat == "guest") or ($canvote == 0)){
$poll = view_results();
}else{
//display poll
$poll = view_poll();
}
}else{
//no poll exists so lets just make this equal nothing.
$poll = '';
}
//output viewtopic
$page = new template($template_path ."/viewtopic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$b_name[Board]",
  "BID" => "$bid",
  "TID" => "$tid",
  "LANG-TOPIC" => "$t_name[Topic]",
  "REPLYLINK" => "$replylink",
  "PAGENATION" => "$pagenation",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "MENU" => "$menu",
  "POLL" => "$poll",
  "LANG-PRINT" => "$viewtopic[ptitle]",
  "SUBJECT" => "$t_name[Topic]",
  "LANG-POSTED" => "$viewtopic[postedon]",
  "TOPIC-DATE" => "$topic_date",
  "AUTHOR" => "$t_name[author]",
  "RANK" => "$rank",
  "RANKICON" => "$rankicon",
  "AVATAR" => "$avatar",
  "LANG-POSTCOUNT" => "$viewtopic[posts]",
  "POSTCOUNT" => "$total",
  "TOPIC" => "$msg",
  "SIGNATURE" => "$sig",
  "REPLY" => "$reply",
  "MODERATORTOOL" => "$modtool",
  "BOARD-POLICY" => "$board_policy"));
$page->output();
//see if the user can post a reply in this topic.
if (($board_rule['B_Reply'] == 1) AND ($access_level != 1)){
$can_post = 0;
}elseif (($board_rule['B_Reply'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$can_post = 0;
}elseif ($board_rule['B_Reply'] == 4){
$can_post = 0;
}else{
$can_post = 1;
}
if (($stat == "guest") OR ($t_name['Locked'] == 1) OR ($can_post == 0)){
//display nothing.
}else{
//bbcode buttons
$bbcode = bbcode_form();
$smile = form_smiles();
//output it
if ($spell_check == 1){
$page = new template($template_path ."/instantreply-spell.htm");
$page->replace_tags(array(
  "LANG-INSTANTREPLY" => "$viewtopic[instantreply]",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-OPTIONS" => "$post[options]",
  "NOTIFY" => "$post[notify]",
  "LANG-DISABLESMILES" => "$post[disablesmiles]",
  "LANG-DISABLEBBCODE" => "$post[disablebbcode]",
  "LANG-REPLY" => "$post[reply]",
  "PAGE" => "$pg"));
$page->output();
}else{
$page = new template($template_path ."/instantreply.htm");
$page->replace_tags(array(
  "LANG-INSTANTREPLY" => "$viewtopic[instantreply]",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-OPTIONS" => "$post[options]",
  "NOTIFY" => "$post[notify]",
  "LANG-DISABLESMILES" => "$post[disablesmiles]",
  "LANG-DISABLEBBCODE" => "$post[disablebbcode]",
  "LANG-REPLY" => "$post[reply]",
  "PAGE" => "$pg"));
$page->output();
}
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
