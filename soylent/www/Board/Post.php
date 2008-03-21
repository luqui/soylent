<?
define('IN_EBB', true);
/*
Filename: Post.php
Last Modified: 6/2/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
$mode = $_GET['mode'];
//determine the page title.
if ($mode == "New_Topic"){
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$post[newtopic]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
}
elseif ($mode == "New_Poll"){
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$post[newpoll]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
}
elseif ($mode == "Reply"){
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$post[reply]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
}
//see if this is a guest trying to post.
if ($stat == "guest"){
header ("Location: login.php");
}
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
//display post
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$db->run = "SELECT * FROM ebb_boards WHERE id='$bid'";
$post_rules_r = $db->result();
$db->close();
//get posting rules.
$db->run = "select * from ebb_board_access WHERE B_id='$bid'";
$board_rule = $db->result();
$db->close();
//set var to posting rules.
if($post_rules_r['HTML'] == 1){
$allowhtml = $cp['on'];
}else{
$allowhtml = $cp['off'];
}
if($post_rules_r['Smiles'] == 1){
$allowsmiley = $cp['on'];
}else{
$allowsmiley = $cp['off'];
}
if($post_rules_r['BBcode'] == 1){
$allowbbcode = $cp['on'];
}else{
$allowbbcode = $cp['off'];
}
if($post_rules_r['Image'] == 1){
$allowimg = $cp['on'];
}else{
$allowimg = $cp['off'];
}
  switch ( $mode )
{
case 'New_Topic':
//see if user can post.
if (($board_rule['B_Post'] == 1) AND ($access_level != 1)){
$error = $post['nowrite'];
echo error($error, "error");
}elseif (($board_rule['B_Post'] == 2) AND ($stat == "Member") or ($access_level == 3)){
$error = $post['nowrite'];
echo error($error, "error");
}elseif ($board_rule['B_Post'] == 4){
$error = $post['nowrite'];
echo error($error, "error");
}
//see if user can post important topics
if(($board_rule['B_Important'] == 1) AND ($access_level != 1)){
$post_important = 0;
}elseif(($board_rule['B_Important'] == 2) AND ($stat == "Member") or ($access_level == 3)){
$post_important = 0;
}elseif($board_rule['B_Important'] == 4){
$post_important = 0;
}else{
$post_important = 1;
}
//output form options
if($post_important == 1){
$post_type = "$post[type]:&nbsp;<input type=\"radio\" name=\"post_type\" value=\"1\">$post[important] <input type=\"radio\" name=\"post_type\" value=\"0\" checked>$post[normal]<br>
<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]<br>
<input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]<br>
<input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}else{
$post_type = "<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]<br>
<input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]<br>
<input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}
$bbcode = bbcode_form();
$smile = form_smiles();
if ($spell_check == 1){
$page = new template($template_path ."/postnewtopic-spell.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[newtopic]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-POSTTOPIC" => "$post[posttopic]"));
$page->output();
}else{
$page = new template($template_path ."/postnewtopic.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[newtopic]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-POSTTOPIC" => "$post[posttopic]"));
$page->output();
}
break;
case 'New_Poll':
//see if this uyser can post a poll on this board.
if (($board_rule['B_Poll'] == 1) AND ($access_level != 1)){
$error = $post['nopoll'];
echo error($error, "error");
}elseif (($board_rule['B_Poll'] == 2) AND ($stat == "Member") or ($access_level == 3)){
$error = $post['nopoll'];
echo error($error, "error");
}elseif ($board_rule['B_Poll'] == 4){
$error = $post['nopoll'];
echo error($error, "error");
}
//see if user can post important topics
if(($board_rule['B_Important'] == 1) AND ($access_level != 1)){
$post_important = 0;
}elseif(($board_rule['B_Important'] == 2) AND ($stat == "Member") or ($access_level == 3)){
$post_important = 0;
}elseif($board_rule['B_Important'] == 4){
$post_important = 0;
}else{
$post_important = 1;
}
//output form options
if($post_important == 1){
$post_type = "$post[type]:&nbsp;<input type=\"radio\" name=\"post_type\" value=\"1\">$post[important] <input type=\"radio\" name=\"post_type\" value=\"0\" checked>$post[normal]<br>
<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]<br>
<input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]<br>
<input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}else{
$post_type = "<input type=\"checkbox\" name=\"subscribe\" value=\"yes\" class=\"text\">$post[notify]<br>
<input type=\"checkbox\" name=\"no_smile\" value=\"1\" class=\"text\">$post[disablesmiles]<br>
<input type=\"checkbox\" name=\"no_bbcode\" value=\"1\" class=\"text\">$post[disablebbcode]";
}
$bbcode = bbcode_form();
$smile = form_smiles();
if ($spell_check == 1){
$page = new template($template_path ."/postnewpoll-spell.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[newpoll]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-POLL" => "$post[polltext]",
  "LANG-QUESTION" => "$post[question]",
  "LANG-OPTION1" => "$post[pollopt1]",
  "LANG-OPTION2" => "$post[pollopt2]",
  "LANG-OPTION3" => "$post[pollopt3]",
  "LANG-OPTION4" => "$post[pollopt4]",
  "LANG-OPTION5" => "$post[pollopt5]",
  "LANG-OPTION6" => "$post[pollopt6]",
  "LANG-OPTION7" => "$post[pollopt7]",
  "LANG-OPTION8" => "$post[pollopt8]",
  "LANG-OPTION9" => "$post[pollopt9]",
  "LANG-OPTION10" => "$post[pollopt10]",
  "LANG-POSTTOPIC" => "$post[posttopic]"));
$page->output();
}else{
$page = new template($template_path ."/postnewpoll.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[newpoll]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-TOPIC" => "$post[topic]",
  "LANG-OPTIONS" => "$post[options]",
  "POST-TYPE" => "$post_type",
  "LANG-POLL" => "$post[polltext]",
  "LANG-QUESTION" => "$post[question]",
  "LANG-OPTION1" => "$post[pollopt1]",
  "LANG-OPTION2" => "$post[pollopt2]",
  "LANG-OPTION3" => "$post[pollopt3]",
  "LANG-OPTION4" => "$post[pollopt4]",
  "LANG-OPTION5" => "$post[pollopt5]",
  "LANG-OPTION6" => "$post[pollopt6]",
  "LANG-OPTION7" => "$post[pollopt7]",
  "LANG-OPTION8" => "$post[pollopt8]",
  "LANG-OPTION9" => "$post[pollopt9]",
  "LANG-OPTION10" => "$post[pollopt10]",
  "LANG-POSTTOPIC" => "$post[posttopic]"));
$page->output();
}
break;
case 'Reply':
//see if user can post a reply.
if (($board_rule['B_Reply'] == 1) AND ($access_level != 1)){
$error = $post['nowrite'];
echo error($error, "error");
}elseif (($board_rule['B_Reply'] == 2) AND ($stat == "Member") or ($access_level == 3)){
$error = $post['nowrite'];
echo error($error, "error");
}elseif ($board_rule['B_Reply'] == 4){
$error = $post['nowrite'];
echo error($error, "error");
}
//output form.
$bbcode = bbcode_form();
$smile = form_smiles();
if ($spell_check == 1){
$page = new template($template_path ."/postreply-spell.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[reply]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-OPTIONS" => "$post[options]",
  "NOTIFY" => "$post[notify]",
  "LANG-DISABLESMILES" => "$post[disablesmiles]",
  "LANG-DISABLEBBCODE" => "$post[disablebbcode]",
  "LANG-REPLY" => "$post[reply]",
  "PAGE" => "$pg"));
$page->output();
}else{
$page = new template($template_path ."/postreply.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[reply]",
  "LANG-ALLOWHTML" => "$post[html]",
  "ALLOWHTML" => "$allowhtml",
  "LANG-ALLOWSMILES" => "$post[smiles]",
  "ALLOWSMILES" => "$allowsmiley",
  "LANG-ALLOWBBCODE" => "$post[bbcode]",
  "ALLOWBBCODE" => "$allowbbcode",
  "LANG-ALLOWIMG" => "$post[img]",
  "ALLOWIMG" => "$allowimg",
  "BID" => "$bid",
  "TID" => "$tid",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-POSTINGRULES" => "$post[postingrules]",
  "BBCODE" => "$bbcode",
  "LANG-POSTINGIN" => "$post[postin]",
  "POSTINGIN" => "$post_rules_r[Board]",
  "LANG-SMILES" => "$post[moresmiles]",
  "SMILES" => "$smile",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$logged_user",
  "LANG-OPTIONS" => "$post[options]",
  "NOTIFY" => "$post[notify]",
  "LANG-DISABLESMILES" => "$post[disablesmiles]",
  "LANG-DISABLEBBCODE" => "$post[disablebbcode]",
  "LANG-REPLY" => "$post[reply]",
  "PAGE" => "$pg"));
$page->output();
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
