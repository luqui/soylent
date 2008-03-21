<?
define('IN_EBB', true);
/*
Filename: register.php
Last Modified: 10/8/2006

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
  "PAGETITLE" => "$reg[register]",
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
//display register form.
  switch ( $_GET['action'] )
  {
  case 'process':
//get values from form.
$email = anti_injection($_POST['email']);
$username = anti_injection($_POST['username']);
$password = anti_injection($_POST['password']);
$vert_password = anti_injection($_POST['vert_password']);
$time_zone = anti_injection($_POST['time_zone']);
$time_format = anti_injection($_POST['time_format']);
$pm_notice = anti_injection($_POST['pm_notice']);
$show_email = anti_injection($_POST['show_email']);
$style = anti_injection($_POST['style']);
$default_lang = anti_injection($_POST['default_lang']);
$msn_messenger = anti_injection($_POST['msn_messenger']);
$aol_messenger = anti_injection($_POST['aol_messenger']);
$yim = anti_injection($_POST['yim']);
$icq = anti_injection($_POST['icq']);
$location = anti_injection($_POST['location']);
$sig = anti_injection($_POST['sig']);
$site = anti_injection($_POST['site']);
$agreecheck = anti_injection($_POST['agreecheck']);
$number = anti_injection($_POST['img_vert']);
$emailvalidate = valid_email($email);
$IP = $_SERVER['REMOTE_ADDR'];
//error checking.
if (($security_image == 1) AND ($number == "")){
$error = $reg['noimgvert'];
echo error($error, "error");
}elseif (($security_image == 2) AND ($number == "")){
$error = $reg['noimgvert'];
echo error($error, "error");
}
if (($tos == 1) AND ($agreecheck == "")){
$error = $reg['disagreetos'];
echo error($error, "error");
}
if ($style == ""){
$error = $reg['nostyle'];
echo error($error, "error");
}
if ($default_lang == ""){
$error = $reg['nolang'];
echo error($error, "error");
}
if ($time_zone == ""){
$error = $reg['notimezone'];
echo error($error, "error");
}
if ($time_format == ""){
$error = $reg['notimeformat'];
echo error($error, "error");
}
if ($pm_notice == ""){
$error = $reg['nopmnotify'];
echo error($error, "error");
}
if ($show_email == ""){
$error = $reg['noshowemail'];
echo error($error, "error");
}
if ($username == ""){
$error = $reg['nouser'];
echo error($error, "error");
}
if ($email == ""){
$error = $reg['noemail'];
echo error($error, "error");
}
if ($password == ""){
$error = $reg['nopass'];
echo error($error, "error");
}
if ($vert_password == ""){
$error = $reg['novertpass'];
echo error($error, "error");
}
if (ereg('[^A-Za-z0-9]', $username)){
$error = $reg['invalidchar'];
echo error($error, "error");
}
if($emailvalidate == 1){
$error = $reg['invalidemail'];
echo error($error, "error");
}
if ($vert_password !== $password){
$error = $reg['nomatch'];
echo error($error, "error");
}
if(strlen($username) > 25){
$error = $reg['longusername'];
echo error($error, "error");
}
if(strlen($username) < 4){
$error = $reg['shortusername'];
echo error($error, "error");
}
if((strlen($vert_password) > 35) and (strlen($password) > 35)){
$error = $reg['longpassword'];
echo error($error, "error");
}
if(strlen($email) > 255){
$error = $reg['longemail'];
echo error($error, "error");
}
if(strlen($sig) > 255){
$error = $reg['longsig'];
echo error($error, "error");
}
//check to see if the user & email have already been used already.
$db->run = "SELECT Email FROM ebb_users WHERE Email='$email'";
$email_check = $db->num_results();
$db->close();
$db->run = "SELECT Username FROM ebb_users WHERE Username='$username'";
$username_check = $db->num_results();
$db->close();
if(($email_check == 1) || ($username_check == 1)){
if($email_check == 1){
$error = $reg['emailexist'];
echo error($error, "error");
}
if($username_check == 1){
$error = $reg['usernameexist'];
echo error($error, "error");
}
}
//see if username used is in blacklist.
$blklist = blacklisted_usernames($username);
if ($blklist == 1) {
$error = $txt['usernameblacklisted'];
echo error($error, "general");
}
//see if the email being used is on the banlist.
$emailban = check_email($email);
if ($emailban == 1){
$error = $txt['emailban'];
echo error($error, "general");
}
//see if a username/IP was banned.
echo check_ban();
//do image verify if enabled.
if (($security_image == 1) or ($security_image == 2)){
$match_check = md5($number);
//see if the security image and the user's text match.
if ($match_check !== $_SESSION['image_value']){
$error = $reg['imgvertnomatch'];
echo error($error, "error");
}else{
//number correct, remove the random value from session.
session_destroy();
}
}
//see if activation is set to either User or Admin.
if($active_type == "User"){
$active_stat = 0;
$act_key = makeRandomPassword();
}elseif($active_type == "Admin"){
$active_stat = 0;
$act_key = '';
}else{
$active_stat = 1;
$act_key = '';
}
//perfrom query.
$pass = md5($password);
$time = time();
$db->run = "INSERT INTO ebb_users (Email, Username, Password, Status, Date_Joined, IP, MSN, AOL, Yahoo, ICQ, Location, Sig, WWW, Time_format, Time_Zone, PM_Notify, Hide_Email, Style, Language, active, act_key) VALUES('$email', '$username', '$pass', 'Member', $time, '$IP', '$msn_messenger', '$aol_messenger', '$yim', '$icq', '$location', '$sig', '$site', '$time_format', '$time_zone', '$pm_notice', '$show_email', '$style', '$default_lang', '$active_stat', '$act_key')";
$db->query();
$db->close();
//send out email to remind user they created an account.
require "lang/".$lang.".email.php";
if($active_type == "User"){
//send out email to user.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $user_verify_subject;
//get body of newsletter.
$mailer->Body = $user_verify_msg;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $user_verify_subject, $user_verify_msg, $headers);
}
echo $reg['acctuser'];
}elseif($active_type == "Admin"){
//send out email to user.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $admin_verify_subject;
//get body of newsletter.
$mailer->Body = $admin_verify_msg;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $admin_verify_subject, $admin_verify_msg, $headers);
}
echo $reg['acctadmin'];
}else{
//send out email to user.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $register_subject;
//get body of newsletter.
$mailer->Body = $register_message;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $register_subject, $register_message, $headers);
}
echo $reg['acctmade'];
}
break;
default:
if (($access_level == 1) OR ($stat == "Member") OR ($access_level == 2) OR ($access_level == 3)){
$error = $reg['alreadyreg'];
echo error($error, "error");
}
if($tos == 1){

$tos_part = "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\">$form[tos]</td>
<td class=\"td\" width=\"50%\"><textarea name=\"tos_msg\" rows=\"5\" cols=\"40\" class=\"text\">$board_rules</textarea><br>
<input name=\"agreecheck\" type=\"checkbox\" value=\"yes\"><b>$form[agree]</b><br></td>
</tr>";
}else{
$tos_part = '';
}
if ($security_image == 1){
$security_img = "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\">$form[securityimage]<br><b>($form[securitynotice])</b></td>
<td class=\"td\" width=\"50%\"><input type=\"text\" name=\"img_vert\" size=\"30\" class=\"text\">&nbsp;<img src=\"security_image2.php\"></td>
</tr>";
}elseif($security_image == 2){
$security_img = "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\">$form[securityimage]<br><b>($form[securitynotice])</b></td>
<td class=\"td\" width=\"50%\"><input type=\"text\" name=\"img_vert\" size=\"30\" class=\"text\">&nbsp;<img src=\"security_image.php\"></td>
</tr>";
}else{
$security_img = '';
}
$timezone = timezone_select();
$style = style_select();
$language = lang_select();
$page = new template($template_path ."/register.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$reg[register]",
  "LANG-TEXT" => "$reg[text]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-EMAIL" => "$form[email]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-RULE" => "$reg[nospecialchar]",
  "LANG-PASSWORD" => "$login[pass]",
  "LANG-CORNFIRMPASSWORD" => "$form[confirmpass]",
  "LANG-TIME" => "$form[timezone]",
  "TIME" => "$timezone",
  "LANG-TIMEFORMAT" => "$form[timeformat]",
  "LANG-TIMEINFO" => "$form[timeinfo]",
  "TIMEFORMAT" => "$time_format",
  "LANG-PMNOTIFY" => "$form[pm_notify]",
  "LANG-SHOWEMAIL" => "$form[showemail]",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]",
  "LANG-STYLE" => "$form[style]",
  "STYLE" => "$style",
  "LANG-LANGUAGE" => "$form[defaultlang]",
  "LANGUAGE" => "$language",
  "SECURITY-IMAGE" => "$security_img",
  "LANG-OPTIONAL" => "$form[optional]",
  "LANG-MSN" => "$form[msn]",
  "LANG-AOL" => "$form[aol]",
  "LANG-YIM" => "$form[yim]",
  "LANG-ICQ" => "$form[icq]",
  "LANG-SIG" => "$form[sig]",
  "LANG-WWW" => "$form[www]",
  "LANG-LOCATION" => "$form[location]",
  "TOS" => "$tos_part",
  "SUBMIT" => "$reg[register]"));
$page->output();
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
