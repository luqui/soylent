<?
define('IN_EBB', true);
/*
Filename: login.php
Last Modified: 6/21/2006

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
  "PAGETITLE" => "$login[login]",
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
//user is logged in, so place them elsewhere.
if ($stat !== "guest"){
header("Location: index.php");
}
//display login system.
switch ($_GET['mode']){
case 'verify_acct':
$key = anti_injection($_GET['key']);
$u = anti_injection($_GET['u']);
//see if user added false info.
if(($key == "") or ($u == "")){
//bring user to index for fooling around.
header("Location: index.php");
}
//check for correct key code & username.
$db->run = "select * from ebb_users where Username='$u' AND act_key='$key'";
$acct_chk = $db->num_results();
$db->close();
if($acct_chk == 1){
//set user as active.
$db->run = "update ebb_users set active='1' where Username='$u'";
$db->query();
$db->close();
echo $login['correctinfo'];
}else{
$error = $login['incorrectinfo'];
echo error($error, "error");
}
break;
case 'lostpassword':
$page = new template($template_path ."/lostpassword.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$login[passwordrecovery]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-REGISTER" => "$login[reg]",
  "LANG-EMAIL" => "$form[email]",
  "LANG-GETPASS" => "$userinfo[getpass]"));
$page->output();
  break;
  case 'process_lostpassword':
$lost_user = anti_injection($_POST['lost_user']);
$lost_email = anti_injection($_POST['lost_email']);
$random_password = makeRandomPassword();
$db->run = "SELECT * FROM ebb_users WHERE Username='$lost_user'";
$email_r = $db->result();
$db->close();
$email = $email_r['Email'];
//see if the email matches the one on record.
if ($lost_email !== $email['Email']){
$error = $userinfo['noemailmatch'];
echo error($error, "error");
}else{
$db->run = "UPDATE ebb_users SET Password='$random_password' WHERE Username='$lost_user'";
$db->query();
$db->close();
}
require "lang/".$lang.".email.php";
//get ready to send mail.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $lost_subject;
//get body of newsletter.
$mailer->Body = $lost_message;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $lost_subject, $lost_message, $headers);
}
$error = $userinfo['emailsent'];
echo error($error, "general");
break;
default:
if ((!isset($_COOKIE['ebbuser'])) OR (!isset($_SESSION['ebb_user']))){
$redirect = $_SERVER['HTTP_REFERER'];
$page = new template($template_path ."/login.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$login[login]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "TEXT" => "$login[text]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-REGISTER" => "$login[reg]",
  "LANG-PASSWORD" => "$login[pass]",
  "LANG-FORGOT" => "$login[forgot]",
  "LANG-REMEMBER" => "$login[rememberlogin]",
  "LANG-REMEMBERTXT" => "$login[remembertxt]",
  "LANG-LOGIN" => "$login[login]",
  "PATH-REDIRECT" => "$redirect"));
$page->output();
}
else{
$error = $login['alreadylogged'];
echo error($error, "general");
}
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
