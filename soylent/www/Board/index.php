<?
define('IN_EBB', true);
/*
Filename: index.php
Last Modified: 6/2/2005

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
  "PAGETITLE" => "$index[title]",
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
}else{
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
//update the user's last active status.
$time = time();
$db->run = "update ebb_users SET last_visit='$time' where Username='$logged_user'";
$db->query();
$db->close();
//check to see if user is marked as online, if not mark them as online.
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
//update the user's last active status.
$time = time();
$db->run = "update ebb_users SET last_visit='$time' where Username='$logged_user'";
$db->query();
$db->close();
//check to see if user is marked as online, if not mark them as online.
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
//show announcement, if admins wants them on.
if ($announcements_stat == 1){

$string = $announcements;
$string = BBCode($string);
$string = smiles($string);
$string = nl2p($string);
//load template
$page = new template($template_path ."/announcement.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-ANNOUNCEMENT" => "$index[announcements]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TICKER" => "$index[ticker_txt]",
  "ANNOUNCEMENT" => "$string"));

$page->output();
}
//display show new post link.
if($stat !== "guest"){
echo "<p align=\"right\" class=\"td\"><a href=\"Search.php?action=newposts\">$index[newposts]</a></p>";
}
//display board.
$sql = "select * from ebb_category ORDER BY C_Order";
$errorq = $sql;
$category_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($category_query)) {

$page = new template($template_path ."/board-top.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "CAT-NAME" => "$row[Name]",
  "LANG-BOARD" => "$index[boards]",
  "LANG-TOPIC" => "$index[topics]",
  "LANG-POST" => "$index[posts]",
  "LANG-LASTPOSTDATE" => "$index[lastposteddate]"));

$page->output();
$sql = "SELECT * FROM ebb_boards WHERE Category='$row[id]' ORDER BY B_Order";
$errorq = $sql;
$board_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row2 = mysql_fetch_assoc ($board_query)) {

$id = $row2['id'];
$board = $row2['Board'];
$description = $row2['Description'];
$last_post = $row2['last_update'];
//get moderator detail.
$board_moderator = moderator_boardlist();

if ($last_post == ""){
$board_date = $index['noposts'];
$last_post_link = "";
}else{
$gmttime = gmdate ($time_format, $last_post);
$board_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
$posted_by = $row2['Posted_User'];
$post_link = $row2['Post_Link'];
$last_post_link = "<A HREF=\"viewtopic.php?$post_link\">$index[Postedby]</A>: $posted_by";
}
$db->run = "select * from ebb_topics WHERE bid='$id'";
$topic_num = $db->num_results();
$db->close();
$db->run = "select * from ebb_posts WHERE bid='$id'";
$post_num = $db->num_results();
$db->close();
//check for topic stat.
$db->run = "select * from ebb_read WHERE Board='$id' and user='$logged_user'";
$read_ct = $db->num_results();
$db->close();
if (($read_ct == 1) OR ($last_post == "") OR ($stat == "guest")){
$icon = "<img src=\"$template_path/images/old.gif\" alt=\"$index[newpost]\" title=\"$index[newpost]\">";
}else{
$icon = "<img src=\"$template_path/images/new.gif\" alt=\"$index[oldpost]\" title=\"$index[oldpost]\">";
}
//get permission rules.
$db->run = "select * from ebb_board_access WHERE B_id='$id'";
$board_rule = $db->result();
$db->close();
//decide view rule.
if(($board_rule['B_Read'] == 1) AND ($access_level == 1)){
$page = new template($template_path ."/board-middle.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "POST-ICON" => "$icon",
  "ID" => "$id",
  "BOARD" => "$board",
  "DESCRIPTION" => "$description",
  "TOPIC-COUNT" => "$topic_num",
  "POST-COUNT" => "$post_num",
  "LANG-MODERATOR" => "$index[moderators]",
  "MODERATOR" => "$board_moderator",
  "DATE" => "$board_date",
  "LAST-POST" => "$last_post_link"));

$page->output();
}elseif(($board_rule['B_Read'] == 2) AND ($access_level == 1) or ($access_level == 2)){
$page = new template($template_path ."/board-middle.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "POST-ICON" => "$icon",
  "ID" => "$id",
  "BOARD" => "$board",
  "DESCRIPTION" => "$description",
  "TOPIC-COUNT" => "$topic_num",
  "POST-COUNT" => "$post_num",
  "LANG-MODERATOR" => "$index[moderators]",
  "MODERATOR" => "$board_moderator",
  "DATE" => "$board_date",
  "LAST-POST" => "$last_post_link"));

$page->output();
}elseif(($board_rule['B_Read'] == 3) AND ($access_level == 1) or ($access_level == 2) or ($stat == "Member") OR ($access_level == 3)){
$page = new template($template_path ."/board-middle.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "POST-ICON" => "$icon",
  "ID" => "$id",
  "BOARD" => "$board",
  "DESCRIPTION" => "$description",
  "TOPIC-COUNT" => "$topic_num",
  "POST-COUNT" => "$post_num",
  "LANG-MODERATOR" => "$index[moderators]",
  "MODERATOR" => "$board_moderator",
  "DATE" => "$board_date",
  "LAST-POST" => "$last_post_link"));

$page->output();
}elseif($board_rule['B_Read'] == 0){
$page = new template($template_path ."/board-middle.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "POST-ICON" => "$icon",
  "ID" => "$id",
  "BOARD" => "$board",
  "DESCRIPTION" => "$description",
  "TOPIC-COUNT" => "$topic_num",
  "POST-COUNT" => "$post_num",
  "LANG-MODERATOR" => "$index[moderators]",
  "MODERATOR" => "$board_moderator",
  "DATE" => "$board_date",
  "LAST-POST" => "$last_post_link"));

$page->output();
}
}
echo "</table><br>";
}
//get member count.
$db->run = "select * from ebb_users";
$user_num = $db->num_results();
$db->close();
//get topic count.
$db->run = "select * from ebb_topics";
$topic_num = $db->num_results();
$db->close();
//get post count.
$db->run = "select * from ebb_posts";
$post_num = $db->num_results();
$db->close();
//get new user.
$db->run = "select * from ebb_users ORDER BY Date_Joined DESC LIMIT 1";
$new_user = $db->result();
$db->close();
//call the whos online function
$online = whosonline();
//load board stat-icon
$page = new template($template_path ."/boardstat.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-BOARDSTAT" => "$index[boardstatus]",
  "LANG-ICONGUIDE" => "$index[iconguide]",
  "LANG-NEWESTMEMBER" => "$index[newestmember]",
  "NEWESTMEMBER" => "$new_user[Username]",
  "TOTAL-TOPIC" => "$topic_num",
  "LANG-TOTALTOPIC" => "$index[topics]",
  "TOTAL-POST" => "$post_num",
  "LANG-TOTALPOST" => "$index[posts]",
  "TOTAL-USER" => "$user_num",
  "LANG-TOTALUSER" => "$index[membernum]",
  "LANG-NEWPOST" => "$index[newpost]",
  "LANG-OLDPOST" => "$index[oldpost]"));

$page->output();
//update online data.
$time = time();
$timeout = $time - 120;
//delete any old entries
$db->run = "delete from ebb_online where time<$timeout";
$db->query();
$db->close();
//grab total online currently
$db->run = "select DISTINCT Username from ebb_online where ip=''";
$online_logged_count = $db->num_results();
$db->close();
$db->run = "select DISTINCT ip from ebb_online where Username=''";
$online_guest_count = $db->num_results();
$db->close();
//output who's online.
$page = new template($template_path ."/whosonline.htm");
$page->replace_tags(array(
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-WHOSONLINE" => "$index[whosonline]",
  "LANG-ONLINEKEY" => "$index[onlinekey]",
  "LOGGED-ONLINE" => "$online_logged_count",
  "LANG-LOGGED-ONLINE" => "$index[membernum]",
  "GUEST-ONLINE" => "$online_guest_count",
  "LANG-GUEST-ONLINE" => "$index[guestonline]",
  "WHOSONLINE"=> "$online"));

$page->output();
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
