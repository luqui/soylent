<?
define('IN_EBB', true);
/*
Filename: delete.php
Last Modified: 11/9/2006

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
  "PAGETITLE" => "$delete[title]",
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
header("Location: index.php");
}
//define board ids.
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$pid = anti_injection($_GET['pid']);
//check for the posting rule.
$db->run = "select * from ebb_board_access WHERE B_id='$bid'";
$board_rule = $db->result();
$db->close();
//see if user is a moderator of this board.
$db->run = "select * from ebb_grouplist WHERE board_id='$bid' and group_id='$level_result[id]'";
$checkmod = $db->num_results();
$db->close();
//display delete prompt.
switch ($_GET['action']){
case 'delete_topic':
//delete topics.
$db->run = "DELETE FROM ebb_topics WHERE tid='$tid'";
$db->query();
$db->close();
//delete replies, if any.
$db->run = "DELETE FROM ebb_posts WHERE tid='$tid'";
$db->query();
$db->close();
//delete any subscriptions to this topic.
$db->run = "DELETE FROM ebb_topic_watch WHERE tid='$tid'";
$db->query();
$db->close();
//update last posted section.
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$board_num = $db->num_results();
$db->close();
if($board_num == 0){
$db->run = "UPDATE ebb_boards SET last_update='' WHERE tid='$tid'";
$db->query();
$db->close();
}else{
$db->run = "SELECT * FROM ebb_topics WHERE bid='$bid' ORDER BY last_update DESC LIMIT 1";
$board_r = $db->result();
$db->close();
//update the last_update colume for ebb_boards.
$db->run = "UPDATE ebb_boards SET last_update='$board_r[last_update]', Posted_User='$board_r[Posted_User]', Post_Link='$board_r[Post_Link]'  WHERE id='$bid'";
$db->query();
$db->close();
}
//bring user back
header("Location: index.php");
break;
case 'delete_post':
//delete topics.
$db->run = "DELETE FROM ebb_posts WHERE pid='$pid'";
$db->query();
$db->close();
//update last posted section.
$db->run = "SELECT * FROM ebb_posts WHERE tid='$tid'";
$post_num = $db->num_results();
$db->close();
if($post_num == 0){
$db->run = "SELECT * FROM ebb_topics WHERE tid='$tid'";
$post_r = $db->result();
$db->close();
#create link to original post.
$originalupdate = "bid=". $post_r['bid'] . "&tid=". $post_r['tid'];
//update topic last_update.
$db->run = "UPDATE ebb_topics SET last_update='$post_r[Original_Date]', Posted_User='$post_r[author]', Post_Link='$originalupdate' WHERE tid='$tid'";
$db->query();
$db->close();
//bring user back
header("Location: viewtopic.php?$originalupdate");
}else{
$db->run = "SELECT * FROM ebb_posts WHERE tid='$tid' ORDER BY Original_Date DESC LIMIT 1";
$topic_r = $db->result();
$db->close();
//create new post link.
$newlink = "bid=". $bid . "&tid=". $tid . "&pid=". $topic_r['pid'] . "#". $topic_r['pid'];
//update the last_update colume for ebb_boards.
$db->run = "UPDATE ebb_topics SET last_update='$topic_r[Original_Date]', Posted_User='$topic_r[re_author]', Post_Link='$newlink'  WHERE tid='$tid'";
$db->query();
$db->close();
//bring user back
header("Location: viewtopic.php?$newlink");
}
break;
case 'del_topic':
//see if topic exist
$db->run = "select * from ebb_topics WHERE tid='$tid'";
$checkboard = $db->num_results();
$db->close();
if ($checkboard == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
$db->run = "SELECT author FROM ebb_topics Where tid='$tid'";
$topic = $db->result();
$db->close();
//see if user has rights to do this.
if ($checkmod == 1){
$candel = 1;
}else{

if (($logged_user == $topic['author']) AND ($access_level == 1) AND ($board_rule['B_Delete'] == 1)){
$candel = 1;
}elseif (($logged_user == $topic['author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Delete'] == 2)){
$candel = 1;
}elseif (($logged_user == $topic['author']) AND ($board_rule['B_Delete'] == 3)){
$candel = 1;
}else{
$candel = 0;
}
}
//check to see if this user is the author of this post.otherwise this action will be canceled.
if ($candel == 1){
$page = new template($template_path ."/deletetopic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-DELETETOPIC" => "$delete[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$delete[topiccon]",
  "BID" => "$bid",
  "ID" => "$tid",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
}else{
$error = $delete['denied'];
echo error($error, "error");
}
break;
case 'del_post':
//see if the post exist.
$db->run = "select * from ebb_posts WHERE pid='$pid'";
$checkboard = $db->num_results();
$db->close();
if ($checkboard == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
$db->run = "SELECT re_author FROM ebb_posts Where pid='$pid'";
$post = $db->result();
$db->close();
//see if user has rights to do this.
if ($checkmod == 1){
$candel = 1;
}else{

if (($logged_user == $post['re_author']) AND ($access_level == 1) AND ($board_rule['B_Delete'] == 1)){
$candel = 1;
}elseif (($logged_user == $post['re_author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Delete'] == 2)){
$candel = 1;
}elseif (($logged_user == $post['re_author']) AND ($board_rule['B_Delete'] == 3)){
$candel = 1;
}else{
$candel = 0;
}
}
//check to see if this user is the author of this post.otherwise this action will be canceled.
if ($candel == 1){
$page = new template($template_path ."/deletepost.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-DELETEPOST" => "$delete[delpost]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$delete[postcon]",
  "BID" => "$bid",
  "TID" => "$tid",
  "ID" => "$pid",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
}else{
$error = $delete['denied'];
echo error($error, "error");
}
break;
default:
header("Location: index.php");
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
