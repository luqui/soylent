<?
define('IN_EBB', true);
/*
Filename: edit.php
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
  "PAGETITLE" => "$edit[edittopic]",
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
//send user to index page.
header("Location: index.php");
}
//display edit form.
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
switch ($_GET['mode']){
case 'edit_topic':
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$board_rules = $db->result();
$db->close();
//set var for board rules.
$html = $board_rules['HTML'];

$topic = anti_injection($_POST['topic']);
$topic = removeEvilAttributes($_POST['topic']);
$topic = htmlentities($_POST['topic'], ENT_QUOTES);
$post = anti_injection($_POST['post']);
$post = removeEvilAttributes($_POST['post']);
$post = htmlentities($_POST['post'], ENT_QUOTES);
$subscribe = anti_injection($_POST['subscribe']);
//do some error-checking.
if ($topic == ""){
$error = $process['nosubject'];
echo error($error, "error");
}
if ($post == ""){
$error = $process['nopost'];
echo error($error, "error");
}
if(strlen($topic) > 50){
$error = $process['longsubject'];
echo error($error, "error");
}
$no_smile = anti_injection($_POST['no_smile']);
$no_bbcode = anti_injection($_POST['no_bbcode']);
//set the disable variables to 0 if not selected.
if($no_smile == ""){
$no_smile = 0;
}
if($no_bbcode == ""){
$no_bbcode = 0;
}
//see if this user already subscribed to this topic.
$db->run = "Select * from ebb_topic_watch where username='$logged_user' and tid='$tid'";
$check_subscription = $db->num_results();
$db->close();
if (($check_subscription == 0) AND ($subscribe == "yes")){
//add user to list
$db->run = "insert into ebb_topic_watch (username, tid, status) values ('$logged_user', '$tid', 'Unread')";
$db->query();
$db->close();
}
//see if user can change topic type.
if(($board_rule['B_Important'] == 1) AND ($access_level == 1)){
$set_important = 1;
}elseif(($board_rule['B_Important'] == 2) AND ($access_level == 1) OR ($access_level == 2)){
$set_important = 1;
}elseif(($board_rule['B_Important'] == 3) AND ($access_level == 1) OR ($access_level == 2) OR ($stat == "Member")){
$set_important = 1;
}else{
$set_important = 0;
}
//see current post type stats.
if ($set_important == 1){
//check for topic type.
if ($topic['important'] == 1){
$post_type = $_POST['post_type'];
//update the topic.
$db->run = "UPDATE ebb_topics SET Topic='$topic', Body='$post', important='$post_type', disable_smiles='$no_smile', disable_bbcode='$no_bbcode' WHERE tid='$tid'";
$db->query();
$db->close();
}else{
//update the topic.
$db->run = "UPDATE ebb_topics SET Topic='$topic', Body='$post', disable_smiles='$no_smile', disable_bbcode='$no_bbcode' WHERE tid='$tid'";
$db->query();
$db->close();
}
}
//bring user back
header("Location: viewtopic.php?bid=$bid&tid=$tid");
break;
case 'edit_post':
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$board_rules = $db->result();
$db->close();
//set var for board rules.
$html = $board_rules['HTML'];
$reply_post = anti_injection($_POST['reply_post']);
$reply_post = removeEvilAttributes($_POST['reply_post']);
$reply_post = htmlentities($_POST['reply_post'], ENT_QUOTES);
$subscribe = anti_injection($_POST['subscribe']);
//error-checking.
if ($reply_post == ""){
$error = $process['nopost'];
echo error($error, "error");
}
$no_smile = anti_injection($_POST['no_smile']);
$no_bbcode = anti_injection($_POST['no_bbcode']);
//set the disable variables to 0 if not selected.
if($no_smile == ""){
$no_smile = 0;
}
if($no_bbcode == ""){
$no_bbcode = 0;
}
//see if this user already subscribed to this topic.
$db->run = "Select * from ebb_topic_watch where username='$logged_user' and tid='$tid'";
$check_subscription = $db->num_results();
$db->close();
if (($check_subscription == 0) AND ($subscribe == "yes")){
//add user to list
$db->run = "insert into ebb_topic_watch (username, tid, status) values ('$logged_user', '$tid', 'Unread')";
$db->query();
$db->close();
}
//update post
$db->run = "UPDATE ebb_posts SET Body='$reply_post', disable_smiles='$no_smile', disable_bbcode='$no_bbcode' WHERE pid='$pid'";
$db->query();
$db->close();
//bring user back
header("Location: viewtopic.php?bid=$bid&tid=$tid");
break;
case 'editpost':
//see if topic exist
$db->run = "select * from ebb_posts WHERE pid='$pid'";
$checkboard = $db->num_results();
$post_r = $db->result();
$db->close();
if ($checkboard == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
//check to see if this user is the author of this post.otherwise this action will be canceled.
if ($checkmod == 1){
$canedit = 1;
}else{
if (($logged_user == $post_r['re_author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1)){
$canedit = 1;
}elseif (($logged_user == $post_r['re_author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2)){
$canedit = 1;
}elseif (($logged_user == $post_r['re_author']) AND ($board_rule['B_Edit'] == 3)){
$canedit = 1;
}else{
$canedit = 0;
}
}
//output edit form.
if ($canedit == 1){
$bbcode = bbcode_form();
$smile = form_smiles();
//get subscription status.
$db->run = "Select * from ebb_topic_watch where username='$logged_user' and tid='$tid'";
$check_subscription = $db->num_results();
$db->close();
//check for subscription status.
if($check_subscription == 1){
$post_type = "<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\" checked>$post[notify]";
}else{
$post_type = "<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]";
}
//check for smile status.
if($post_r['disablesmiles'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\" checked>$post[disablesmiles]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]";
}
//check for bbcode status
if($post_r['disablebbcode'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\" checked>$post[disablebbcode]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}
if ($spell_check == 1){
$spell_body = $post_r['Body'];
$page = new template($template_path ."/editpost-spell.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$edit[editpost]",
  "BID" => "$bid",
  "TID" => "$tid",
  "PID" => "$pid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "BODY" => "$spell_body",
  "LANG-OPTIONS" => "$post[options]",
  "OPTIONS" => "$post_type",
  "LANG-EDITTOPIC" => "$edit[editpost]"));
$page->output();
}else{
$page = new template($template_path ."/editpost.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$edit[editpost]",
  "BID" => "$bid",
  "TID" => "$tid",
  "PID" => "$pid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "BODY" => "$post_r[Body]",
  "LANG-OPTIONS" => "$post[options]",
  "OPTIONS" => "$post_type",
  "LANG-EDITTOPIC" => "$edit[editpost]"));
$page->output();
}
}
else{
$error = $edit['denied'];
echo error($error, "error");
}
break;
case 'edittopic';
$db->run = "select * from ebb_topics WHERE tid='$tid'";
$checkboard = $db->num_results();
$topic = $db->result();
$db->close();
if ($checkboard == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
//get subscription status.
$db->run = "Select * from ebb_topic_watch where username='$logged_user' and tid='$tid'";
$check_subscription = $db->num_results();
$db->close();
//check to see if this user is the author of this post.otherwise this action will be canceled.
if ($checkmod == 1){
$canedit = 1;
}else{
if (($logged_user == $topic['author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1)){
$canedit = 1;
}elseif (($logged_user == $topic['author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2)){
$canedit = 1;
}elseif (($logged_user == $topic['author']) AND ($board_rule['B_Edit'] == 3)){
$canedit = 1;
}else{
$canedit = 0;
}
}
//output edit form.
if ($canedit == 1){
$bbcode = bbcode_form();
$smile = form_smiles();
//see who can set important topics.
if(($board_rule['B_Important'] == 1) AND ($access_level == 1)){
$set_important = 1;
}elseif(($board_rule['B_Important'] == 2) AND ($access_level == 1) OR ($access_level == 2)){
$set_important = 1;
}elseif(($board_rule['B_Important'] == 3) AND ($access_level == 1) OR ($access_level == 2) OR ($stat == "Member")){
$set_important = 1;
}else{
$set_important = 0;
}
//see current post type stats.
if ($set_important == 1){
//check for topic type.
if ($topic['important'] == 1){
$post_type = "$post[type]:&nbsp;<input type=\"radio\" name=\"post_type\" value=\"1\" class=\"text\" checked>$post[important] <input type=\"radio\" name=\"post_type\" value=\"0\">$post[normal]";
}else{
$post_type = "$post[type]:&nbsp;<input type=\"radio\" name=\"post_type\" value=\"1\" class=\"text\">$post[important] <input type=\"radio\" name=\"post_type\" value=\"0\" checked>$post[normal]";
}
//check for subscription status.
if($check_subscription == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\" checked>$post[notify]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]";
}
//check for smile status.
if($topic['disablesmiles'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\" checked>$post[disablesmiles]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]";
}
//check for bbcode status
if($topic['disablebbcode'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\" checked>$post[disablebbcode]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}
}else{
//check for subscription status.
if($check_subscription == 1){
$post_type = "<br><input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\" checked>$post[notify]";
}else{
$post_type = "<br><input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]";
}
//check for smile status.
if($topic['disablesmiles'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\" checked>$post[disablesmiles]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]";
}
//check for bbcode status
if($topic['disablebbcode'] == 1){
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\" checked>$post[disablebbcode]";
}else{
$post_type .= "<br><input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}
}
if ($spell_check == 1){
$spell_body = rawurlencode($topic['Body']);
$page = new template($template_path ."/edittopic-spell.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$edit[edittopic]",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "TOPIC" => "$topic[Topic]",
  "BODY" => "$spell_body",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-EDITTOPIC" => "$edit[edittopic]"));
$page->output();
}else{
$page = new template($template_path ."/edittopic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$edit[edittopic]",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "TOPIC" => "$topic[Topic]",
  "BODY" => "$topic[Body]",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-EDITTOPIC" => "$edit[edittopic]"));
$page->output();
}
}
else{
$error = $edit['denied'];
echo error($error, "error");
}
break;
default:
//user went here directly, aka incorrectly. direct them to the index page asap!
header("Location: index.php");
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
