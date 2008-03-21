<?
define('IN_EBB', true);
/*
Filename: Process.php
Last Modified: 12/27/2006

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
  "PAGETITLE" => "&nbsp;",
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
//check to see if this user is a registered or not.
if ($stat == "guest"){
header ("Location: login.php");
exit();
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
//process form request.
  switch ( $_GET['mode'] )
  {
case 'New_Topic':
$bid = anti_injection($_GET['bid']);
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

$post_type = anti_injection($_POST['post_type']);
$no_smile = anti_injection($_POST['no_smile']);
$no_bbcode = anti_injection($_POST['no_bbcode']);
//set the disable variables to 0 if not selected.
if($no_smile == ""){
$no_smile = 0;
}
if($no_bbcode == ""){
$no_bbcode = 0;
}
//do some error checking
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
//flood check.
$currtime = time() - 30;
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$get_time_r = $db->result();
$db->close();
$lastposttime = $get_time_r['last_post'];
if ($lastposttime > $currtime){
$error = $process['flood'];
echo error($error, "error");
}
//process request.
$ip = $_SERVER['REMOTE_ADDR'];
$time = time();
$db->run = "insert into ebb_topics (author, Topic, Body, Type, important, IP, Original_Date, bid, last_update, disable_smiles, disable_bbcode) values ('$logged_user', '$topic', '$post', 'Topic', '$post_type', '$ip', '$time', '$bid', '$time', '$no_smile', '$no_bbcode')";
$db->query();
$db->close();
//update last_update col in db for board.
$db->run = "update ebb_boards SET last_update ='$time' WHERE id='$bid'";
$db->query();
$db->close();
//update user's last post.
$db->run = "Update ebb_users SET last_post='$time' WHERE Username='$logged_user'";
$db->query();
$db->close();
//get current post count then add on to it.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
$currnum = $get_num['Post_Count'];
$increase_count = $currnum + 1;
$db->run = "UPDATE ebb_users SET Post_Count='$increase_count' WHERE Username='$logged_user'";
$db->query();
$db->close();
//get tid.
$db->run = "SELECT * FROM ebb_topics ORDER BY tid DESC limit 1";
$tid_result = $db->result();
$db->close();
$tid = $tid_result['tid'];
//update post link.
$newlink = "bid=". $bid . "&tid=". $tid;
//update post link for board.
$db->run = "Update ebb_boards SET Post_Link='$newlink' WHERE id='$bid'";
$db->query();
$db->close();
//update post link for topic.
$db->run = "Update ebb_topics SET Post_Link='$newlink' WHERE tid='$tid'";
$db->query();
$db->close();
//update last poster for board.
$db->run = "Update ebb_boards SET Posted_User='$logged_user' WHERE id='$bid'";
$db->query();
$db->close();
//update last poster for topic.
$db->run = "Update ebb_topics SET Posted_User='$logged_user' WHERE tid='$tid'";
$db->query();
$db->close();
//delete the read table.
$db->run = "DELETE FROM ebb_read WHERE Board='$bid'";
$db->query();
$db->close();
//direct user to topic.
header("Location: viewtopic.php?$newlink");
//check to see if the author wishes to recieve a email when a reply is added.
if ($_POST['subscribe'] == "yes"){
$db->run = "insert into ebb_topic_watch (username, tid, status) values ('$logged_user', '$tid', 'Unread')";
$db->query();
$db->close();
}
break;
case 'New_Poll':
$bid = anti_injection($_GET['bid']);
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$board_rules = $db->result();
$db->close();
//set var for board rules.
$html = $board_rules['HTML'];

$poll_topic = anti_injection($_POST['topic']);
$poll_topic = removeEvilAttributes($_POST['topic']);
$poll_topic = htmlentities($_POST['topic'], ENT_QUOTES);
$poll_post = anti_injection($_POST['post']);
$poll_post = removeEvilAttributes($_POST['post']);
$poll_post = htmlentities($_POST['post'], ENT_QUOTES);

$post_type = anti_injection($_POST['post_type']);
$question = anti_injection($_POST['question']);
$poll_otp1 = anti_injection($_POST['$poll_otp1']);
$poll_otp2 = anti_injection($_POST['$poll_otp2']);
$poll_otp3 = anti_injection($_POST['$poll_otp3']);
$poll_otp4 = anti_injection($_POST['$poll_otp4']);
$poll_otp5 = anti_injection($_POST['$poll_otp5']);
$poll_otp6 = anti_injection($_POST['$poll_otp6']);
$poll_otp7 = anti_injection($_POST['$poll_otp7']);
$poll_otp8 = anti_injection($_POST['$poll_otp8']);
$poll_otp9 = anti_injection($_POST['$poll_otp9']);
$poll_otp10 = anti_injection($_POST['$poll_otp10']);
$no_smile = anti_injection($_POST['no_smile']);
$no_bbcode = anti_injection($_POST['no_bbcode']);
//set the disable variables to 0 if not selected.
if($no_smile == ""){
$no_smile = 0;
}
if($no_bbcode == ""){
$no_bbcode = 0;
}
//error check.
if ($poll_topic == ""){
$error = $process['nosubject'];
echo error($error, "error");
}
if ($poll_post == ""){
$error = $process['nopost'];
echo error($error, "error");
}
if ($question == ""){
$error = $process['noquestion'];
echo error($error, "error");
}
if (($poll_otp1 == "") AND ($poll_otp2 == "")){
$error = $process['moreoption'];
echo error($error, "error");
}
if(strlen($poll_topic) > 50){
$error = $process['longsubject'];
echo error($error, "error");
}
if(strlen($poll_otp1) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp2) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp3) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp4) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp5) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp6) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp7) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp8) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp9) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
if(strlen($poll_otp10) > 50){
$error = $process['longpoll'];
echo error($error, "error");
}
//flood check.
$currtime = time() - 30;
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$get_time_r = $db->result();
$db->close();
$lastposttime = $get_time_r['last_post'];
if ($lastposttime > $currtime){
$error = $process['flood'];
echo error($error, "error");
}
//process this
$ip = $_SERVER['REMOTE_ADDR'];
$time = time();
$db->run = "insert into ebb_topics (author, Topic, Body, Type, important, IP, Original_Date, last_update, bid, Question, disable_smiles, disable_bbcode) values ('$logged_user', '$poll_topic', '$poll_post', 'Poll', '$post_type', '$ip', '$time', '$time', '$bid', '$question', '$no_smile', '$no_bbcode')";
$db->query();
$db->close();
//update last_update col in db.
$db->run = "update ebb_boards SET last_update ='$time' WHERE id='$bid'";
$db->query();
$db->close();
//get current post count then add on to it.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
$currnum = $get_num['Post_Count'];
$increase_count = $currnum + 1;
$db->run = "UPDATE ebb_users SET Post_Count='$increase_count' WHERE Username='$logged_user'";
$db->query();
$db->close();
//update last post time.
$db->run = "Update ebb_users SET last_post='$time' WHERE Username='$logged_user'";
$db->query();
$db->close();
//get tid.
$db->run = "SELECT tid FROM ebb_topics ORDER BY tid DESC limit 1";
$tid_result = $db->result();
$db->close();
$tid = $tid_result['tid'];
//update post link.
$newlink = "bid=". $bid . "&tid=". $tid;
//update post link for board.
$db->run = "Update ebb_boards SET Post_Link='$newlink' WHERE id='$bid'";
$db->query();
$db->close();
//update post link for topic.
$db->run = "Update ebb_topics SET Post_Link='$newlink' WHERE tid='$tid'";
$db->query();
$db->close();
//update last poster for board.
$db->run = "Update ebb_boards SET Posted_User='$logged_user' WHERE id='$bid'";
$db->query();
$db->close();
//update last poster for topic.
$db->run = "Update ebb_topics SET Posted_User='$logged_user' WHERE tid='$tid'";
$db->query();
$db->close();
//delete the read table.
$db->run = "DELETE FROM ebb_read WHERE Board='$bid'";
$db->query();
$db->close();
//add poll options
for($i=1;$i<=10;$i++)
{
if ($_POST['poll_otp'.$i] == ""){
}
else{
$db->run = "INSERT INTO `ebb_poll` (Poll_Option, tid) values('".$_POST['poll_otp'.$i]."', '$tid')";
$db->query();
$db->close();
}
}
//direct user to topic.
header("Location: viewtopic.php?$newlink");
//check to see if the author wishes to recieve a email when a reply is added.
if ($_POST['subscribe'] == "yes"){
$db->run = "insert into ebb_topic_watch (username, tid, status) values ('$logged_user', '$tid', 'Unread')";
$db->query();
$db->close();
}
break;
case 'Reply':
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$page = anti_injection($_POST['page']);
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$board_rules = $db->result();
$db->close();
//set var for board rules.
$html = $board_rules['HTML'];

$reply_post = anti_injection($_POST['reply_post']);
$reply_post = removeEvilAttributes($_POST['reply_post']);
$reply_post = htmlentities($_POST['reply_post'], ENT_QUOTES);

$no_smile = anti_injection($_POST['no_smile']);
$no_bbcode = anti_injection($_POST['no_bbcode']);
//set the disable variables to 0 if not selected.
if($no_smile == ""){
$no_smile = 0;
}
if($no_bbcode == ""){
$no_bbcode = 0;
}
//error check
if ($reply_post == ""){
$error = $process['nopost'];
echo error($error, "error");
}
//flood check.
$currtime = time() - 30;
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$get_time_r = $db->result();
$db->close();
$lastposttime = $get_time_r['last_post'];
if ($lastposttime > $currtime){
$error = $process['flood'];
echo error($error, "error");
}
//process this
$ip = $_SERVER['REMOTE_ADDR'];
$time = time();
$db->run = "insert into ebb_posts (re_author, tid, bid, Body, IP, Original_Date, disable_smiles, disable_bbcode) values ('$logged_user', '$tid', '$bid', '$reply_post', '$ip', '$time', '$no_smile', '$no_bbcode')";
$db->query();
$db->close();
//update last_update col in db.
$db->run = "update ebb_boards SET last_update ='$time' WHERE id='$bid'";
$db->query();
$db->close();
//update last_update in topic
$db->run = "update ebb_topics SET last_update = '$time' WHERE tid='$tid'";
$db->query();
$db->close();
//get current post count then add on to it.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
$currnum = $get_num['Post_Count'];
$increase_count = $currnum + 1;
$db->run = "UPDATE ebb_users SET Post_Count='$increase_count' WHERE Username='$logged_user'";
$db->query();
$db->close();
//update last post time.
$db->run = "Update ebb_users SET last_post='$time' WHERE Username='$logged_user'";
$db->query();
$db->close();
//get pid.
$db->run = "SELECT pid FROM ebb_posts ORDER BY pid DESC limit 1";
$pid_result = $db->result();
$db->close();
$pid = $pid_result['pid'];
//get reply number
$db->run = "select * from ebb_posts WHERE tid='$tid'";
$reply_num = $db->num_results();
$db->close();
$total_pages = ceil($reply_num / 10);
//update post link.
if ($page < $total_pages){
$next = ($page + 1);
$newlink = "pg=". $next . "&bid=". $bid . "&tid=". $tid . "&pid=". $pid . "#". $pid;
}else{
$newlink = "bid=". $bid . "&tid=". $tid . "&pid=". $pid . "#". $pid;
}
//update post link for board.
$db->run = "Update ebb_boards SET Post_Link='$newlink' WHERE id='$bid'";
$db->query();
$db->close();
//update post link for topic.
$db->run = "Update ebb_topics SET Post_Link='$newlink' WHERE tid='$tid'";
$db->query();
$db->close();
//update last poster for board.
$db->run = "Update ebb_boards SET Posted_User='$logged_user' WHERE id='$bid'";
$db->query();
$db->close();
//update last poster for topic.
$db->run = "Update ebb_topics SET Posted_User='$logged_user' WHERE tid='$tid'";
$db->query();
$db->close();
//delete the read table.
$db->run = "DELETE FROM ebb_read WHERE Board='$bid'";
$db->query();
$db->close();
$db->run = "DELETE FROM ebb_read WHERE Topic='$tid'";
$db->query();
$db->close();
//direct user to topic.
header("Location: viewtopic.php?$newlink");
//check to see if the author wishes to recieve a email when a reply is added.
if ($_POST['subscribe'] == "yes"){
$db->run = "insert into ebb_topic_watch (username, tid, status) values ('$logged_user', '$tid', 'Unread')";
$db->query();
$db->close();
}
//update topic watch table to set post as new again.
$sql = "Select * FROM ebb_topic_watch WHERE tid='$tid'";
$errorq = $sql;
$usubscription = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_array($usubscription)) {
$db->run = "update ebb_topic_watch SET status='Unread' where tid='$tid'";
$db->query();
$db->close();
}
//gather info for email.
$db->run = "Select * FROM ebb_topic_watch WHERE tid='$tid'";
$digest = $db->result();
$db->close();
//grab topic info.
$db->run = "Select * FROM ebb_posts WHERE pid='$pid'";
$topic = $db->result();
$db->close();
//get topic name
$db->run = "Select * FROM ebb_topics Where tid='$tid'";
$name = $db->result();
$db->close();
//set values for email.
$digest_subject = "RE:".$name['Topic'];
//get message part of email.
require "lang/".$lang.".email.php";
$sql = "Select * FROM ebb_topic_watch WHERE tid='$tid'";
$errorq = $sql;
$notify = mysql_query ($sql) or die(error($error, "mysql", $errorq));
if (($topic['re_author'] != $logged_user) OR ($digest['status'] == "Unread")){
while ($row = mysql_fetch_array($notify)) {
//get email addresses
$db->run = "Select * FROM ebb_users WHERE Username='$row[username]'";
$email_r = $db->result();
$db->close();
//send out email to users.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $digest_subject;
//get body of newsletter.
$mailer->Body = $digest_message;
//add users to the email list
$mailer->AddBCC($email_r['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
set_time_limit(2500);
@mail($email_r['Email'], $digest_subject, $digest_message, $headers);
}
}
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
