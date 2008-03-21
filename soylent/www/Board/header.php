<?
session_start();
if (!defined('IN_EBB') ) {
die("<B>!!ACCESS DENIED HACKER!!</B>");
}
/*
Filename: header.php
Last Modified: 12/28/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
#function loading
require "function.php";
#call up the db class
$db = new db;
#GZIP compression
$db->run = "SELECT * FROM ebb_settings";
$row = $db->result();
$gzip = $row["GZIP"];
if ($gzip == 1){
ob_start('ob_gzhandler');
}
else{
ob_start();
}
$db->close();
#user check
if ((isset($_COOKIE['ebbuser']) && ($_COOKIE['ebbpass'])) OR (isset($_SESSION['ebb_user'])) && ($_SESSION['ebb_pass'])){
#get username value.
if(isset($_SESSION['ebb_user'])){
$user_check = anti_injection($_SESSION['ebb_user']);
}else{
$user_check = anti_injection($_COOKIE['ebbuser']);
}
$db->run = "SELECT * FROM ebb_users WHERE Username='$user_check'";
$row = $db->result();
$chk_user = $db->num_results();
$db->close();
if($chk_user == 1){
#set-up vars
$logged_user = $row['Username'];
$stat = $row["Status"];
$template = $row["Style"];
$time_format = $row["Time_format"];
$lang = $row["Language"];
$gmt = $row["Time_Zone"];
$last_visit = $row["last_visit"];
//check to see if user is part of a group.
if ($stat == "groupmember"){
$db->run = "SELECT * FROM ebb_group_users where Username='$logged_user'";
$groupuser = $db->result();
$group_auth_chk = $db->num_results();
$db->close();
if ($group_auth_chk == 1){
$db->run = "SELECT * FROM ebb_groups where id='$groupuser[gid]'";
$level_result = $db->result();
$db->close();
#set-up vars
$access_level = $level_result['Level'];
}
}
}else{
$error = "INVALID COOKIE OR SESSION!";
echo error($error, "error");
}
}
else{
$stat = "guest";
$db->run = "SELECT * FROM ebb_settings";
$row = $db->result();
$db->close();
#set-up vars
$template = $row["Default_Style"];
$lang = $row["Default_Language"];
$time_format = $row["Default_Time"];
$gmt = $row["Default_Zone"];
$active_type = $row['activation'];
}
#settings
$db->run = "SELECT * FROM ebb_settings";
$row = $db->result();
$db->close();
#set-up vars
$title = $row['Site_Title'];
$address = $row['Site_Address'];
$board_address = $row['Board_Address'];
$board_status = $row['Board_Status'];
$board_email = $row["Board_Email"];
$off_msg = $row["Off_Message"];
$announcements_stat = $row["Announcement_Status"];
$announcements = $row["Announcements"];
$tos = $row["TOS_Status"];
$board_rules = $row["TOS_Rules"];
$security_image = $row["Image_Verify"];
$spell_check = $row["spell_checker"];
$pm_quota = $row["PM_Quota"];
$cookie_path = $row["cookie_path"];
$cookie_domain = $row["cookie_domain"];
$cookie_secure = $row["cookie_secure"];
$pm_rule = $row['PM_Access'];
$profile_rule = $row['Profile_View'];
$group_rule = $row['Group_Membership'];
$avatar_rule = $row['Avatar_Usage'];
$mail_rule = $row['mail_type'];
#template
$db->run = "SELECT * FROM ebb_style where id='$template'";
$row = $db->result();
$db->close();
#set-up vars
$bg_color = $row["Background"];
$text_color = $row["Text"];
$link = $row["Link"];
$vlink = $row["VLink"];
$table1 = $row["Table1"];
$table2 = $row["Table2"];
$font = $row["Font"];
$border = $row["Border"];
$template_path = $row["Temp_Path"];
#template loading
require "template.php";
#language loading
require "lang/".$lang.".lang.php";
?>
