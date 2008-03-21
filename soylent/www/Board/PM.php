<?
define('IN_EBB', true);
/*
Filename: PM.php
Last Modified: 8/23/2006

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
  "PAGETITLE" => "$menu[pm]",
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
//see if a group has banned access here.
if($groupuser['Limitation'] == 2){
$error = $pm['pm_access'];
echo error($error, "error");
}
//get current post count to determine if user has enough post to access this section.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $pm_rule) and ($stat == "Member")){
$error = $pm['pm_access'];
echo error($error, "error");
}
//display pm.
  switch ( $_GET['action'] )
  {
  case 'write':
$user = anti_injection($_GET['user']);
$bbcode = bbcode_form();
$smile = form_smiles();
$page = new template($template_path ."/pm-postpm.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "LANG-POSTPM" => "$pm[PostPM]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TO" => "$pm[send]",
  "TO" => "$user",
  "LANG-SUBJECT" => "$pm[subject]",
  "LANG-SENDPM" => "$pm[sendpm]"));
$page->output();
  break;
  case 'write_process':
//get the values from the form.
$send = anti_injection($_POST['send']);
$subject = anti_injection($_POST['subject']);
$message = anti_injection($_POST['message']);
if ($send == ""){
$error = $pm['nosend'];
echo error($error, "error");
}
if ($subject == ""){
$error = $pm['nosubject'];
echo error($error, "error");
}
if ($message == ""){
$error = $pm['nomessage'];
echo error($error, "error");
}
if(strlen($send) > 25){
$error = $pm['longuser'];
echo error($error, "error");
}
if(strlen($subject) > 25){
$error = $pm['longsubject'];
echo error($error, "error");
}
//see if user has access to recieve PM's.
$db->run = "select * from ebb_users where Username='$send'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $pm_rule) or ($groupuser['Limitation'] == 2)){
$error = $pm['pm_access_user'];
echo error($error, "error");
}
//check to see if the from user's inbox is full.
$db->run = "SELECT * FROM ebb_pm WHERE Reciever='$send'";
$check_inbox = $db->num_results();
$db->close();
if ($check_inbox == $pm_quota){
$error = $pm['overquota'];
echo error($error, "error");
}
//check to see if this user is on the ban list.
$db->run = "SELECT * FROM ebb_pm_banlist WHERE Banned_User='$logged_user' and Ban_Creator='$send'";
$check_ban_r = $db->num_results();
$db->close();
if ($check_ban_r == 1){
$error = $pm['blocked'];
echo error($error, "general");
}else{
$time = time();
//process query
$db->run = "insert into ebb_pm (Sender, Reciever, Subject, Message, Date) values ('$logged_user', '$send', '$subject', '$message', '$time')";
$db->query();
$db->close();
//email user if they have decided
$db->run = "SELECT * FROM ebb_users WHERE Username='$send'";
$notify = $db->result();
$db->close();
if ($notify['PM_Notify'] == "Yes"){
//get pm id.
$db->run = "SELECT id FROM ebb_pm ORDER BY id DESC limit 1";
$pm_id_result = $db->result();
$db->close();
$pm_id = $pm_id_result['id'];
//grab values from PM message.
$db->run = "select * from ebb_pm where id='$pm_id'";
$pm_data = $db->result();
$db->close();
//pull-up language mail file.
require "lang/".$lang.".email.php";
//get ready to send mail.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $pm_subject;
//get body of newsletter.
$mailer->Body = $pm_message;
//add users to the email list
$mailer->AddBCC($notify['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($notify['Email'], $pm_subject, $pm_message, $headers);
}
}
//bring user back
header("Location: PM.php");
}
  break;
  case 'read':
$id = anti_injection($_GET['id']);
$replybutton = "<a href=\"PM.php?action=reply&id=$id\"><img src=\"$template_path/images/reply.gif\" border=\"0\" alt=\"$pm[replyalt]\"></a><br><br>";

$db->run = "select * from ebb_pm WHERE id='$id'";
$pm_r = $db->result();
$db->close();
//mark as read
if ($pm_r['Read_status'] == ""){
$db->run = "update ebb_pm SET Read_Status='old' where id='$id'";
$db->query();
$db->close();
}
//bbcode & other formating processes.
$string = $pm_r['Message'];
$string = smiles($string);
$string = BBCode($string);
$string = language_filter($string);
$string = nl2br($string);
//get the date
$gmttime = gmdate ($time_format, $pm_r['Date']);
$readpm_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
$db->run = "select * from ebb_users WHERE Username = '$pm_r[Sender]'";
$user = $db->result();
$db->close();
//get sig.
if($user['Sig'] == ""){
$sig = "";
}
else{
$pmsig = nl2br($user['Sig']);
$sig = "-----------<br>$pmsig";
}

$page = new template($template_path ."/pm-read.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "LANG-READPM" => "$pm[readpm]",
  "REPLYBUTTON" => "$replybutton",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-FROM" => "$pm[from]",
  "FROM" => "$pm_r[Sender]",
  "LANG-BANUSER" => "$pm[banuser]",
  "LANG-TO" => "$pm[to]",
  "TO" => "$pm_r[Reciever]",
  "LANG-DATE" => "$pm[date]",
  "DATE" => "$readpm_date",
  "LANG-SUBJECT" => "$pm[subject]",
  "SUBJECT" => "$pm_r[Subject]",
  "PM-MESSAGE" => "$string",
  "SIGNATURE" => "$sig"));
$page->output();
echo "<a href=\"PM.php?action=delete&id=$pm_r[id]\"><img src=\"$template_path/images/delete_pm.gif\" border=\"0\"></a>";
  break;
  case 'reply':
$id = anti_injection($_GET['id']);
$db->run = "Select * From ebb_pm WHERE id='$id'";
$reply = $db->result();
$db->close();
$reply_subject = "RE:&nbsp;".$reply['Subject'];
$bbcode = bbcode_form();
$smile = form_smiles();
$page = new template($template_path ."/pm-replypm.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "LANG-REPLYPM" => "$pm[replypm]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BBCODE" => "$bbcode",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TO" => "$pm[send]",
  "TO" => "$reply[Sender]",
  "LANG-SUBJECT" => "$pm[subject]",
  "SUBJECT" => "$reply_subject",
  "LANG-SENDPM" => "$pm[reply]"));
$page->output();
  break;
  case 'reply_process':
//get the value from the form.
$reply_send = anti_injection($_POST['send']);
$reply_message = anti_injection($_POST['message']);
$re_subject = anti_injection($_POST['subject']);
//error-check
if ($reply_send == ""){
$error = $pm['nosend'];
echo error($error, "error");
}
if ($reply_message == ""){
$error = $pm['nomessage'];
echo error($error, "error");
}
if(strlen($reply_send) > 25){
$error = $pm['longuser'];
echo error($error, "error");
}
//see if user has access to recieve PM's.
$db->run = "select * from ebb_users where Username='$reply_send'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $pm_rule) or ($groupuser['Limitation'] == 2)){
$error = $pm['pm_access_user'];
echo error($error, "error");
}
//check to see if the from user's inbox is full.
$db->run = "SELECT * FROM ebb_pm WHERE Reciever='$reply_send'";
$check_inbox = $db->num_results();
$db->close();
if ($check_inbox == $pm_quota){
$error = $pm['overquota'];
echo error($error, "error");
}
//check to see if this user is on the ban list.
$db->run = "SELECT * FROM ebb_pm_banlist WHERE Banned_User='$logged_user' and Ban_Creator='$reply_send'";
$check_ban_r = $db->num_results();
$db->close();
if ($check_ban_r == 1){
$error = $pm['blocked'];
echo error($error, "general");
}
//process query
$time = time();
$db->run = "insert into ebb_pm (Sender, Reciever, Subject, Message, Date) values ('$logged_user', '$reply_send', '$re_subject', '$reply_message', '$time')";
$db->query();
$db->close();
//email user if they have decided
$db->run = "SELECT * FROM ebb_users WHERE Username='$reply_send'";
$notify = $db->result();
$db->close();
if ($notify['PM_Notify'] == "Yes"){
//get pm id.
$db->run = "SELECT id FROM ebb_pm ORDER BY id DESC limit 1";
$pm_id_result = $db->result();
$db->close();
$pm_id = $pm_id_result['id'];
//grab values from PM message.
$db->run = "select * from ebb_pm where id='$pm_id'";
$pm_data = $db->result();
$db->close();
//pull-up language mail file.
require "lang/".$lang.".email.php";
//get ready to send mail.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $pm_subject;
//get body of newsletter.
$mailer->Body = $pm_message;
//add users to the email list
$mailer->AddBCC($email_r['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($notify['Email'], $pm_subject, $pm_message, $headers);
}
}
//bring user back
header("Location: PM.php");
  break;
  case 'delete':
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/pm-deletepm.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$mod[title]",
  "LANG-DELETEPM" => "$pm[delpm]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$pm[confirmdelete]",
  "ID" => "$id",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
  break;
  case 'delete_process':
$id = anti_injection($_GET['id']);
//process query
$db->run = "DELETE FROM ebb_pm Where id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: PM.php");
  break;
  case 'ban':
$page = new template($template_path ."/pm-banuser.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "LANG-BANUSER" => "$pm[banusertitle]",
  "TEXT" => "$pm[text]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-BAN" => "$pm[usertoban]",
  "BAN" => "$ban_user",
  "LANG-SUBMIT" => "$pm[banuser]"));
$page->output();
  break;
  case 'ban_process':
$banned_user = anti_injection($_POST['banned_user']);
if ($banned_user == ""){
$error = $pm['blankfield'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_pm_banlist (Banned_User, Ban_Creator) values('$banned_user', '$logged_user')";
$db->query();
$db->close();
//bring user back
header("Location: PM.php?action=banlist");
  break;
  case 'banlist':
$banlist = view_banlist();
$page = new template($template_path ."/pm-viewbanlist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "LANG-BANLIST" => "$pm[banlisttitle]",
  "TEXT" => "$pm[text2]",
  "LANG-BANUSER" => "$pm[banusertitle]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-BANNEDUSER" => "$pm[banneduser]",
  "LANG-DELETE" => "$pm[del]",
  "BANLIST" => "$banlist"));
$page->output();
  break;
  case 'del_ban':
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/pm-deletebanlist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$mod[title]",
  "LANG-DELETEBANLIST" => "$pm[delbanuser]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$pm[banlistconfirm]",
  "ID" => "$id",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
  break;
  case 'process_del_ban':
$id = anti_injection($_GET['id']);
//process query
$db->run = "DELETE FROM ebb_pm_banlist WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: PM.php?action=banlist");
  break;
  default:
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
$db->run = "select * from ebb_pm WHERE Reciever='$logged_user' LIMIT $from, $max_results";
$query = $db->query();
$db->close();
$db->run = "select * from ebb_pm WHERE Reciever='$logged_user'";
$num = $db->num_results();
$db->close();
// Figure out the total number of pages. Always round up using ceil()
$total_pages = ceil($num / $max_results);
$pagenation = "<div align=\"left\">$txt[pages]&nbsp;";
// Build page number
if($pg > 1){
$prev = ($pg - 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$prev\">$txt[prev]</a></small>&nbsp;";
}
//output numbers.
for($i = 1; $i <= $total_pages; $i++){
if(($pg) == $i){
$pagenation .= "<b>$i</b>&nbsp;";
} else {
$pagenation .= "<a href=\"$_SERVER[PHP_SELF]?pg=$i\">$i</a>&nbsp;";
}
}
// Build Next Link
if($pg < $total_pages){
$next = ($pg + 1);
$pagenation .= "<small><a href=\"$_SERVER[PHP_SELF]?pg=$next\">$txt[next]</a></small>";
}
$pagenation .= "</div>";

//output im inbox.
$inbox = pm_inbox();
$page = new template($template_path ."/pm-inbox.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$menu[pm]",
  "PAGENATION" => "$pagenation",
  "LANG-VIEWBANLIST" => "$pm[banlist]",
  "LANG-POSTPM" => "$pm[postpmalt]",
  "LANG-PMRULE" => "$pm[pmquota]",
  "PMRULE" => "$pm_quota",
  "LANG-CURRENTAMOUNT" => "$pm[curquota]",
  "CURRENTAMOUNT" => "$num",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SUBJECT" => "$pm[subject]",
  "LANG-SENDER" => "$pm[sender]",
  "LANG-PMDATE" => "$pm[date]",
  "PM-INBOX" => "$inbox"));
$page->output();
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
