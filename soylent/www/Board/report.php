<?
define('IN_EBB', true);
/*
Filename: report.php
Last Modified: 6/21/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
#get post and topic ids.
$tid = anti_injection($_GET['tid']);
$pid = anti_injection($_GET['pid']);

$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$report[title]",
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
#guest should not view this page.
header("Location: index.php");
}
//display search
switch($_GET['mode']){
case 'topic':
//check to see if topic exists or not and if it doesn't kill the program
$db->run = "select * FROM ebb_topics WHERE tid='$tid'";
$checktopic = $db->num_results();
$db->close();
if ($checktopic == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
$page = new template($template_path ."/report-topic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$report[title]",
  "LANG-TEXT" => "$report[text]",
  "TID" => "$tid",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BORDER" => "$border",
  "LANG-REPORTEDBY" => "$report[Reportedby]",
  "USERNAME" => "$logged_user",
  "LANG-REASON" => "$report[reason]",
  "LANG-SPAMPOST" => "$report[spampost]",
  "LANG-FIGHTPOST" => "$report[fightpost]",
  "LANG-ADVERT" => "$report[advert]",
  "LANG-USERPROBLEMS" => "$report[userproblems]",
  "LANG-OTHER" => "$report[other]",
  "LANG-MESSAGE" => "$report[message]",
  "LANG-SUBMIT" => "$report[submit]"));
$page->output();
break;
case 'report_topic':
$tid = anti_injection($_POST['tid']);
$reason = anti_injection($_POST['reason']);
$msg = anti_injection($_POST['msg']);
#load mail language file.
require "lang/".$lang.".email.php";
#error check.
if($reason == ""){
$error = $report['noreason'];
echo error($error, "error");
}
if($msg == ""){
$error = $report['nomsg'];
echo error($error, "error");
}
#get info on moderators to send out email.
$db->run = "select * FROM ebb_topics WHERE tid='$tid'";
$t = $db->result();
$db->close();
$db->run = "select * FROM ebb_grouplist WHERE board_id='$t[bid]'";
$b = $db->query();
$db->close();
#find moderators for the gorpu controling this board..
while($r = mysql_fetch_assoc($b)){
$db->run = "select * FROM ebb_group_users WHERE gid='$r[group_id]'";
$g = $db->query();
$db->close();
#get user's profile
while($r2 = mysql_fetch_assoc($g)){
$db->run = "select * FROM ebb_users WHERE Username='$r2[Username]'";
$u = $db->result();
$db->close();
#mail moderators.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $report_subject;
//get body of newsletter.
$mailer->Body = $report_topic_msg;
//add users to the email list
$mailer->AddBCC($u['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
set_time_limit(2500); //set the time higher if you need to.
@mail($u['Email'], $report_subject, $report_topic_msg, $headers);
}
}
}
#display thank you message.
echo "<p class=\"td\" align=\"center\">". $report['reportsent']. "</p>";
break;
case 'post':
//check to see if topic exists or not and if it doesn't kill the program
$db->run = "select * FROM ebb_posts WHERE pid='$pid'";
$checktopic = $db->num_results();
$db->close();
if ($checktopic == 0){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
$page = new template($template_path ."/report-post.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$report[title]",
  "LANG-TEXT" => "$report[text]",
  "TID" => "$tid",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BORDER" => "$border",
  "LANG-REPORTEDBY" => "$report[Reportedby]",
  "USERNAME" => "$logged_user",
  "LANG-REASON" => "$report[reason]",
  "LANG-SPAMPOST" => "$report[spampost]",
  "LANG-FIGHTPOST" => "$report[fightpost]",
  "LANG-ADVERT" => "$report[advert]",
  "LANG-USERPROBLEMS" => "$report[userproblems]",
  "LANG-OTHER" => "$report[other]",
  "LANG-MESSAGE" => "$report[message]",
  "LANG-SUBMIT" => "$report[submit]"));
$page->output();
break;
case 'report_post':
$pid = anti_injection($_POST['pid']);
$reason = anti_injection($_POST['reason']);
$msg = anti_injection($_POST['msg']);
#load mail language file.
require "lang/".$lang.".email.php";
#error check.
if($reason == ""){
$error = $report['noreason'];
echo error($error, "error");
}
if($msg == ""){
$error = $report['nomsg'];
echo error($error, "error");
}
#get info on moderators to send out email.
$db->run = "select * FROM ebb_posts WHERE pid='$pid'";
$p = $db->result();
$db->close();
$db->run = "select * FROM ebb_topics WHERE tid='$p[tid]'";
$t = $db->result();
$db->close();
$db->run = "select * FROM ebb_grouplist WHERE board_id='$t[bid]'";
$b = $db->query();
$db->close();
#find moderators for the gorpu controling this board..
while($r = mysql_fetch_assoc($b)){
$db->run = "select * FROM ebb_group_users WHERE gid='$r[group_id]'";
$g = $db->query();
$db->close();
#get user's profile
while($r2 = mysql_fetch_assoc($g)){
$db->run = "select * FROM ebb_users WHERE Username='$r2[Username]'";
$u = $db->result();
$db->close();
#mail moderators.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $report_subject;
//get body of newsletter.
$mailer->Body = $report_post_msg;
//add users to the email list
$mailer->AddBCC($u['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
set_time_limit(2500); //set the time higher if you need to.
@mail($u['Email'], $report_subject, $report_post_msg, $headers);
}
}
}
#display thank you message.
echo "<p class=\"td\" align=\"center\">". $report['reportsent']. "</p>";
break;
default:
#default.
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
