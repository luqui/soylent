<?
define('IN_EBB', true);
/*
Filename: Profile.php
Last Modified: 11/3/2006

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
  "PAGETITLE" => "$userinfo[title]",
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
//display profile
  switch ( $_GET['mode'] )
  {
  case 'view':
$user = anti_injection($_GET['user']);
$db->run = "SELECT * FROM ebb_users where Username='$user'";
$profile = $db->result();
$userchk = $db->num_results();
$db->close();
//check to see if this user exist.
if ($userchk == 0){
$error = $userinfo['usernotexist'];
echo error($error, "general");
}
$total = $profile['Post_Count'];
//see if the user set an avatar
if ($profile['Avatar'] == ""){
$avatar = "images/noavatar.gif";
}
else{
$avatar = $profile['Avatar'];
}
//see if user wish to have their email display.
if($profile['Hide_Email'] == 0){
$show_email = $userinfo['hideemail'];
}else{
$show_email = $profile['Email'];
}
//get status
if ($profile['Status'] == "groupmember"){
//find what usergroup this user belongs to.
$db->run = "SELECT * FROM ebb_group_users where Username='$user'";
$groupchk = $db->result();
$db->close();
//get the access level of this group.
$db->run = "SELECT * FROM ebb_groups where id='$groupchk[gid]'";
$level_r = $db->result();
$db->close();
$db->close();
if($level_r['Level'] == 1){
$rank = "<i><b>$level_r[Name]</b></i>";
}
if($level_r['Level'] == 2){
$rank = "<b>$level_r[Name]</b>";
}
if($level_r['Level'] == 3){
$rank = "<i>$level_r[Name]</i>";
}
}elseif($profile['Status'] == "Banned"){
$rank = "Banned";
}else{
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $total ORDER BY Post_req DESC";
$rank2 = $db->result();
$rank = "<i>$rank2[Name]</i>";
$db->close();
}
$gmttime = gmdate ($time_format, $profile['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
//output the html.
if(($total < $profile_rule) and ($profile['Status'] == "Member") or ($groupuser['Limitation'] == 4)){
$page = new template($template_path ."/viewprofile.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "USERNAME" => "$user",
  "LANG-FINDPOSTS" => "$userinfo[findposts]",
  "LANG-FINDTOPICS" => "$userinfo[findtopics]",
  "LANG-RANK-AVATAR" => "$userinfo[avatarrank]",
  "AVATAR" => "$avatar",
  "LANG-RANK" => "$userinfo[rank]",
  "RANK" => "$rank",
  "LANG-POSTCOUNT" => "$userinfo[postcount]",
  "POSTCOUNT" => "$total",
  "LANG-EMAIL" => "$form[email]",
  "EMAIL" => "$userinfo[hideemail]",
  "LANG-MSN" => "$form[msn]",
  "MSN" => "$userinfo[hideemail]",
  "LANG-AOL" => "$form[aol]",
  "AOL" => "$userinfo[hideemail]",
  "LANG-ICQ" => "$form[icq]",
  "ICQ" => "$userinfo[hideemail]",
  "LANG-YAHOO" => "$form[yim]",
  "YAHOO" => "$userinfo[hideemail]",
  "LANG-WWW" => "$form[www]",
  "WWW" => "$userinfo[hideemail]",
  "LANG-LOCATION" => "$form[location]",
  "LOCATION" => "$userinfo[hideemail]",
  "LANG-JOINED" => "$userinfo[joindate]",
  "JOINED" => "$join_date"));
$page->output();
}else{
$page = new template($template_path ."/viewprofile.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "USERNAME" => "$user",
  "LANG-FINDPOSTS" => "$userinfo[findposts]",
  "LANG-FINDTOPICS" => "$userinfo[findtopics]",
  "LANG-RANK-AVATAR" => "$userinfo[avatarrank]",
  "AVATAR" => "$avatar",
  "LANG-RANK" => "$userinfo[rank]",
  "RANK" => "$rank",
  "LANG-POSTCOUNT" => "$userinfo[postcount]",
  "POSTCOUNT" => "$total",
  "LANG-EMAIL" => "$form[email]",
  "EMAIL" => "$show_email",
  "LANG-MSN" => "$form[msn]",
  "MSN" => "$profile[MSN]",
  "LANG-AOL" => "$form[aol]",
  "AOL" => "$profile[AOL]",
  "LANG-ICQ" => "$form[icq]",
  "ICQ" => "$profile[ICQ]",
  "LANG-YAHOO" => "$form[yim]",
  "YAHOO" => "$profile[Yahoo]",
  "LANG-WWW" => "$form[www]",
  "WWW" => "$profile[WWW]",
  "LANG-LOCATION" => "$form[location]",
  "LOCATION" => "$profile[Location]",
  "LANG-JOINED" => "$userinfo[joindate]",
  "JOINED" => "$join_date"));
$page->output();
}
  break;
  case 'edit_profile':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$profile_result = $db->result();
$db->close();
if ($profile_result['PM_Notify'] == "Yes"){
$pmnotice_status = "<input type=\"radio\" name=\"pm_notice\" value=\"Yes\" class=\"text\" checked>$txt[yes] <input type=\"radio\" name=\"pm_notice\" value=\"No\" class=\"text\">$txt[no]";
}else{
$pmnotice_status = "<input type=\"radio\" name=\"pm_notice\" value=\"Yes\" class=\"text\">$txt[yes] <input type=\"radio\" name=\"pm_notice\" value=\"No\" class=\"text\" checked>$txt[no]";
}
if($profile_result['Hide_Email'] == 0){
$showemail_status = "<input type=\"radio\" name=\"show_email\" value=\"0\" class=\"text\" checked>$txt[yes] <input type=\"radio\" name=\"show_email\" value=\"1\" class=\"text\">$txt[no]";
}else{
$showemail_status = "<input type=\"radio\" name=\"show_email\" value=\"0\" class=\"text\">$txt[yes] <input type=\"radio\" name=\"show_email\" value=\"1\" class=\"text\" checked>$txt[no]";
}
//output
$timezone = timezone_select();
$style = style_select();
$language = lang_select();
$page = new template($template_path ."/editprofile.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITPROFILE" => "$userinfo[editprofile]",
  "LANG-TEXT" => "$userinfo[editprofiletxt]",
  "LANG-ENTERPASS" => "$userinfo[enterpass]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CURRPASS" => "$userinfo[currentpass]",
  "LANG-PMNOTIFY" => "$form[pm_notify]",
  "PMNOTIFY" => "$pmnotice_status",
  "LANG-SHOWEMAIL" => "$form[showemail]",
  "SHOWEMAIL" => "$showemail_status",
  "LANG-MSN" => "$form[msn]",
  "MSN" => "$profile_result[MSN]",
  "LANG-AOL" => "$form[aol]",
  "AOL" => "$profile_result[AOL]",
  "LANG-ICQ" => "$form[icq]",
  "ICQ" => "$profile_result[ICQ]",
  "LANG-YAHOO" => "$form[yim]",
  "YAHOO" => "$profile_result[Yahoo]",
  "LANG-WWW" => "$form[www]",
  "WWW" => "$profile_result[WWW]",
  "LANG-TIME" => "$form[timezone]",
  "TIME" => "$timezone",
  "LANG-TIMEFORMAT" => "$form[timeformat]",
  "LANG-TIMEINFO" => "$form[timeinfo]",
  "TIMEFORMAT" => "$profile_result[Time_format]",
  "LANG-STYLE" => "$form[style]",
  "STYLE" => "$style",
  "LANG-LANGUAGE" => "$form[defaultlang]",
  "LANGUAGE" => "$language",
  "LANG-LOCATION" => "$form[location]",
  "LOCATION" => "$profile_result[Location]",
  "SUBMIT" => "$userinfo[saveprofile]"));
$page->output();
  break;
  case 'profile_process':
$conpass = anti_injection($_POST['conpass']);
$pm_notice = anti_injection($_POST['pm_notice']);
$show_email = anti_injection($_POST['show_email']);
$msn = anti_injection($_POST['msn']);
$aol = anti_injection($_POST['aol']);
$yim = anti_injection($_POST['yim']);
$icq = anti_injection($_POST['icq']);
$www = anti_injection($_POST['www']);
$location = anti_injection($_POST['location']);
$time_zone = anti_injection($_POST['time_zone']);
$time_format = anti_injection($_POST['time_format']);
$style = anti_injection($_POST['style']);
$lang = anti_injection($_POST['default_lang']);
//error check.
if ($time_format == ""){
$error = $reg['notimeformat'];
echo error($error, "error");
}
if(strlen($conpass) > 35){
$error = $reg['longpassword'];
echo error($error, "error");
}
$db->run = "SELECT Password FROM ebb_users WHERE Username='$logged_user'";
$profileupdate = $db->result();
$db->close();
$pass = md5($conpass);
//see if password matches.
if ($profileupdate['Password'] !== $pass){
$error = $userinfo['curpassnomatch'];
echo error($error, "error");
}
//process query
$db->run = "UPDATE ebb_users SET PM_Notify='$pm_notice', Hide_Email='$show_email', MSN='$msn', AOL='$aol', Yahoo='$yim', ICQ='$icq', WWW='$www', Location='$location', Time_Zone='$time_zone', Time_format='$time_format', Style='$style', Language='$lang' WHERE Username='$logged_user'";
$db->query();
$db->close();
//bring user back
header("Location: Profile.php?mode=edit_profile");
  break;
  case 'edit_sig':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$sigupdate = $db->result();
$db->close();
$sig = $sigupdate['Sig'];
$displaysig = nl2br($sig);
$page = new template($template_path ."/editsig.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITSIG" => "$userinfo[editsig]",
  "LANG-TEXT" => "$userinfo[sigtxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CURRENTSIG" => "$userinfo[cursig]",
  "CURRENTSIG" => "$displaysig",
  "SIGNATURE" => "$sig",
  "LANG-SAVESIG" => "$userinfo[savesig]"));
$page->output();
  break;
  case 'sig_process':
$signature = stripslashes($_POST['signature']);
//error check
if(strlen($signature) > 255){
$error = $reg['longsig'];
echo error($error, "error");
}
//process query
$db->run = "Update ebb_users SET Sig='$signature' Where Username='$logged_user'";
$db->query();
$db->close();
//bring user back
header("Location: Profile.php?mode=edit_sig");
  break;
  case 'groupmanager':
//see if user can join a group.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $group_rule) and ($stat == "Member")){
$error = $userinfo['group_access'];
echo error($error, "error");
}
//output html.
$joined_group = groups_joined();
$page = new template($template_path ."/editgrouplist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-GROUPMANAGE" => "$userinfo[managegroups]",
  "LANG-TEXT" => "$userinfo[grouptxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-GROUPNAME" => "$groups[name]",
  "GROUPLIST" => "$joined_group"));
$page->output();
  break;
  case 'join_group':
//see if user can join a group.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $group_rule) and ($stat == "Member")){
$error = $userinfo['group_access'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$db->run = "select * from ebb_groups where id='$id'";
$status_check = $db->result;
$num_chk = $db->num_results();
$db->close();
//see if user is already a member of this group.
$db->run = "select * from ebb_group_users where Username='$logged_user' AND gid='$id'";
$membership_chk = $db->num_results();
$db->close();
if ($membership_chk == 1){
$error = $userinfo['alreadyjoined'];
echo error($error, "error");
}
//see if the group exist.
if ($num_chk == 1){
if ($status_check['Enrollment'] == 0){
$error = $userinfo['locked'];
echo error($error, "error");
}else{
$db->run = "insert into ebb_group_users (Username, gid, Status) values('$logged_user', '$id', 'Pending')";
$db->query();
$db->close();
header("Location: Profile.php");
}
}else{
$error = $groups['notexist'];
echo error($error, "error");
}
  break;
  case 'unjoin_group':
//see if user can join a group.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $group_rule) and ($stat == "Member")){
$error = $userinfo['group_access'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$db->run = "delete from ebb_group_users where Username='$logged_user' and gid='$id'";
$db->query();
$db->close();
header("Location: Profile.php");
  break;
  case 'avatar':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
//see if user can add an avatar to their profile.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $avatar_rule) and ($stat == "Member") or ($groupuser['Limitation'] == 3)){
$error = $userinfo['avatar_access'];
echo error($error, "error");
}
$allowed = "$userinfo[allowed] <b>.gif .jpeg .jpg .png</b>";
$db->run = "SELECT * FROM ebb_users WHERE Username='$logged_user'";
$avatar_result = $db->result();
$db->close();
if ($avatar_result['Avatar'] == ""){
$avatar = "images/noavatar.gif";
}else{
$avatar = $avatar_result['Avatar'];
}
$page = new template($template_path ."/editavatar.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITAVATAR" => "$userinfo[avatarsetting]",
  "LANG-TEXT" => "$userinfo[avatartxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CURRENTAVATAR" => "$userinfo[currentavatar]",
  "CURRENTAVATAR" => "$avatar",
  "ALLOWEDTYPES" => "$allowed",
  "LANG-AVATARLINK" => "$userinfo[avatarlink]",
  "LANG-SAVEAVATAR" => "$userinfo[saveavatar]",
  "LANG-SELAVATAR" => "$userinfo[selavatar]",
  "LANG-GALLERY" => "$avatargallery[title]"));
$page->output();
  break;
  case 'avatar_process':
//see if user can add an avatar to their profile.
$db->run = "select * from ebb_users where Username='$logged_user'";
$get_num = $db->result();
$db->close();
//check settings.
if(($get_num['Post_Count'] < $avatar_rule) and ($stat == "Member")){
$error = $userinfo['avatar_access'];
echo error($error, "error");
}
$avatar_img = anti_injection($_POST['avatar_img']);
if($avatar_img == ""){
$db->run = "Update ebb_users SET Avatar='' Where Username='$logged_user'";
$db->query();
$db->close(); 
header("Location: Profile.php?mode=avatar");
}else{
$type = getimagesize($avatar_img);
#compile a list of allow mime types.
$allowed = array("image/gif", "image/jpeg", "image/jpg", "image/png");
if (!in_array($type['mime'], $allowed)){
$error = $userinfo['wrongtype'];
echo error($error, "error");
}
//process query
$db->run = "Update ebb_users SET Avatar='$avatar_img' Where Username='$logged_user'";
$db->query();
$db->close();
//bring user back
header("Location: Profile.php?mode=avatar");
}
  break;
  case 'digest':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$sub = digest_list();
$page = new template($template_path ."/editsubscription.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITSUBSCRIPTION" => "$userinfo[subscriptionsetting]",
  "LANG-TEXT" => "$userinfo[digesttxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SUBSCRIBED" => "$userinfo[scription]",
  "LANG-DELETE" => "$userinfo[delsubscription]",
  "SUBSCRIPTIONS" => "$sub",
  "LANG-SAVEAVATAR" => "$userinfo[saveavatar]",
  "LANG-SELAVATAR" => "$userinfo[selavatar]",
  "LANG-GALLERY" => "$gallery[title]"));
$page->output();
  break;
  case 'digest_process':
$del = anti_injection($_GET['del']);
//process query
$db->run = "DELETE FROM ebb_topic_watch where username='$logged_user' and tid='$del'";
$db->query();
$db->close();
//bring user back
header("Location: Profile.php?mode=digest");
  break;
  case 'new_email':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$page = new template($template_path ."/editemail.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITEMAIL" => "$userinfo[emailupdate]",
  "LANG-TEXT" => "$userinfo[emailtxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CURREMAIL" => "$userinfo[currentemail]",
  "LANG-NEWEMAIL" => "$userinfo[newemail]",
  "LANG-CONFIRMEMAIL" => "$userinfo[confirmemail]",
  "LANG-UPDATEEMAIL" => "$userinfo[updateemail]"));
$page->output();
  break;
  case 'new_email_process':
$conemail = anti_injection($_POST['conemail']);
$newemail = anti_injection($_POST['newemail']);
if (($conemail == "") OR ($newemail !== $conemail)){
$error = $userinfo['noemailmatch'];
echo error($error, "error");
}
if((strlen($newemail) > 255) or (strlen($conemail) > 255)){
$error = $reg['longemail'];
echo error($error, "error");
}
$db->run = "SELECT Email FROM ebb_users where Username='$logged_user'";
$email = $db->result();
$db->close();
if ($curemail !== $email['Email']){
$error = $userinfo['nocuremailmatch'];
echo error($error, "error");
}
//process query
$db->run = "UPDATE ebb_users SET Email='$newemail' where Username='$logged_user'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?mode=new_email");
  break;
  case 'new_password':
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$page = new template($template_path ."/editpassword.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "LANG-EDITPASS" => "$userinfo[changepass]",
  "LANG-TEXT" => "$userinfo[passtxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CURRPASS" => "$userinfo[currentpass]",
  "LANG-NEWPASS" => "$userinfo[newpass]",
  "LANG-CONFIRMPASS" => "$userinfo[connewpass]",
  "LANG-UPDATEPASS" => "$userinfo[updatepass]"));
$page->output();
  break;
  case 'new_password_process':
$curpass = md5($_POST['curpass']);
$confirmpass = anti_injection($_POST['confirmpass']);
$newpass = $_POST['newpass'];
if (($confirmpass == "") OR ($newpass !== $confirmpass)){
$error = $userinfo['nopassmatch'];
echo error($error, "error");
}
if((strlen($newpass) > 35) or (strlen($confirmpass) > 35)){
$error = $reg['longemail'];
echo error($error, "error");
}
$db->run = "SELECT Password FROM ebb_users where Username='$logged_user'";
$password = $db->result();
$db->close();
if ($curpass !== $password['Password']){
$error = $userinfo['curpassnomatch'];
echo error($error, "error");
}
//process query
$passchange = md5($newpass);
$db->run = "UPDATE ebb_users SET Password='$passchange' where Username='$logged_user'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?mode=new_password");
  break;
  default:
//make sure guests can't access this part.
if ($stat == "guest"){
header("Location: index.php");
}
$db->run = "SELECT * FROM ebb_users where Username='$logged_user'";
$profile = $db->result();
$db->close();
$total = $profile['Post_Count'];
//see if the user set an avatar
if ($profile['Avatar'] == ""){
$avatar = "images/noavatar.gif";
}
else{
$avatar = $profile['Avatar'];
}
//get status
if ($profile['Status'] == "groupmember"){
//find what usergroup this user belongs to.
$db->run = "SELECT * FROM ebb_group_users where Username='$logged_user'";
$groupchk = $db->result();
$db->close();
//get the access level of this group.
$db->run = "SELECT * FROM ebb_groups where id='$groupchk[gid]'";
$level_r = $db->result();
$db->close();
$db->close();
if($level_r['Level'] == 1){
$rank = "<i><b>$level_r[Name]</b></i>";
}
if($level_r['Level'] == 2){
$rank = "<b>$level_r[Name]</b>";
}
if($level_r['Level'] == 3){
$rank = "<i>$level_r[Name]</i>";
}
}else{
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $total ORDER BY Post_req DESC";
$rank2 = $db->result();
$db->close();
$rank = "<i>$rank2[Name]</i>";
}
$gmttime = gmdate ($time_format, $profile['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
//output the html.
$page = new template($template_path ."/profile.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$userinfo[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "USERNAME" => "$logged_user",
  "LANG-RANK-AVATAR" => "$userinfo[avatarrank]",
  "AVATAR" => "$avatar",
  "LANG-RANK" => "$userinfo[rank]",
  "RANK" => "$rank",
  "LANG-EMAIL" => "$form[email]",
  "EMAIL" => "$profile[Email]",
  "LANG-MSN" => "$form[msn]",
  "MSN" => "$profile[MSN]",
  "LANG-AOL" => "$form[aol]",
  "AOL" => "$profile[AOL]",
  "LANG-ICQ" => "$form[icq]",
  "ICQ" => "$profile[ICQ]",
  "LANG-YAHOO" => "$form[yim]",
  "YAHOO" => "$profile[Yahoo]",
  "LANG-WWW" => "$form[www]",
  "WWW" => "$profile[WWW]",
  "LANG-LOCATION" => "$form[location]",
  "LOCATION" => "$profile[Location]",
  "LANG-JOINED" => "$userinfo[joindate]",
  "JOINED" => "$join_date",
  "LANG-OPTION" => "$userinfo[profilemenu]",
  "LANG-EDITPROFILE" => "$userinfo[editinfo]",
  "LANG-SIGNATURE" => "$userinfo[editsig]",
  "LANG-GROUPMANAGE" => "$userinfo[managegroups]",
  "LANG-AVATAR" => "$userinfo[avatarsetting]",
  "LANG-SUBSCRIPTION" => "$userinfo[subscriptionsetting]",
  "LANG-EMAILSETTING" => "$userinfo[emailupdate]",
  "LANG-PASSWORD" => "$userinfo[changepass]"));
$page->output();
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
