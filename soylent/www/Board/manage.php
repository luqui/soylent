<?
define('IN_EBB', true);
/*
Filename: manage.php
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
  "PAGETITLE" => "$mod[title]",
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
if ($access_level == 2){
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
//diaply moderator cp
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$pid = anti_injection($_GET['pid']);
//check to see if this user is able to access this area.
$db->run = "select * from ebb_grouplist WHERE board_id='$bid' and group_id='$level_result[id]'";
$checkmod = $db->num_results();
$db->close();
//if user has no right to be here, send them to the index page.
if (($stat == "Member") OR ($access_level == 3) OR ($stat == "guest") or ($checkmod == 0)){
header("Location: index.php");
}
  switch ( $_GET['mode'] )
  {
  case 'viewip':
$ip = $_GET['ip'];
$u = $_GET['u'];
//get number of users this ip matches.
$iplist = ip_checker();
//get other ips the poster used before.
$ipcheck = other_ip_check();
//output html.
$page = new template($template_path ."/mod-viewip.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$mod[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-IPINFO" => "$mod[ipinfo]",
  "LANG-IP" => "$mod[topicip]",
  "IP" => "$ip",
  "LANG-USERNAME" => "$mod[ipusermatch]",
  "USERNAME" => "$iplist",
  "LANG-TOTALCOUNT" => "$mod[totalcount]",
  "TOTALCOUNT" => "$ipcheck"));

$page->output();
  break;
  case 'move':
$boardlist = board_select();
$page = new template($template_path ."/mod-movetopic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$mod[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TEXT" => "$mod[move]",
  "TID" => "$tid",
  "BID" => "$bid",
  "BOARDLIST" => "$boardlist",
  "LANG-SUBMIT" => "$mod[movetopic]"));

$page->output();
  break;
  case 'move_process':
//process query
$board = anti_injection($_POST['board']);
if($board == ""){
$error = $mod['noboard'];
echo error($error, "error");
}
#change board info on tables.
$sql = "Select bid FROM ebb_topics WHERE tid='$tid'";
$errorq = $sql;
$chpost_q = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while($r = mysql_fetch_assoc($chpost_q)){
$db->run = "Update ebb_posts SET bid='$board' WHERE tid='$r[tid]'";
$db->query();
$db->close();
}
$db->run = "Update ebb_topics SET bid='$board' WHERE tid='$tid'";
$db->query();
$db->close();
//update last posted section of the old topic location.
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
//update board of new location, if the topic is newer.
$db->run = "SELECT * FROM ebb_boards WHERE id='$board'";
$board_chk = $db->result();
$db->close();
$db->run = "SELECT * FROM ebb_topics WHERE tid='$tid'";
$topic_chk = $db->result();
$db->close();
if($board_chk['last_update'] < $topic_chk['last_update']){
//update the last_update colume for ebb_boards.
$db->run = "UPDATE ebb_boards SET last_update='$topic_chk[last_update]', Posted_User='$topic_chk[Posted_User]', Post_Link='$topic_chk[Post_Link]'  WHERE id='$board'";
$db->query();
$db->close(); 
}
//bring user back
header("Location: viewtopic.php?bid=$board&tid=$tid");
  break;
  case 'delete':
$page = new template($template_path ."/mod-delete.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$mod[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$mod[condel]",
  "TID" => "$tid",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
  break;
  case 'delete_process':
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
$board_r = $db->num_results();
$db->close();
//update the last_update colume for ebb_boards.
$db->run = "UPDATE ebb_boards SET last_update='$board_r[last_update]', Posted_User='$board_r[Posted_User]', Post_Link='$board_r[Post_Link]'  WHERE id='$bid'";
$db->query();
$db->close();
}
//bring user back
header("Location: index.php");
  break;
  case 'lock':
//process query
$db->run = "Update ebb_topics SET Locked='1' WHERE tid='$tid'";
$db->query();
$db->close();
//bring user back
header("Location: viewtopic.php?bid=$bid&tid=$tid");
  break;
  case 'unlock':
//process query
$db->run = "Update ebb_topics SET Locked='0' WHERE tid='$tid'";
$db->query();
$db->close();
//bring user back
header("Location: viewtopic.php?bid=$bid&tid=$tid");
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
