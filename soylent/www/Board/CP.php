<?
define('IN_EBB', true);
/*
Filename: CP.php
Last Modified: 12/28/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
require "admin_function.php";
$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$cp[title]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
//check to see if the install file is stil on the user's server.
if (file_exists("install.php")){
$error = $txt['installadmin'];
echo error($error, "error");
}
//check to see if the user can access this board.
echo check_ban();
//check to see if this user is able to access this area.
if (($access_level == 2) or ($stat == "Member") or ($stat == "guest") or ($access_level == 3)){
header("Location: index.php");
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
  "LANG-GROUPLIST" => "$menu[groups]",
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
//display admin CP
  switch ( $_GET['action'] )
  {
case 'boards':
$boardmanager = admin_boardmanager();
$page = new template($template_path ."/cp-boards.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-BOARDS" => "$cp[boardsetup]",
  "LANG-TEXT" => "$cp[boardtext]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "LANG-ADDBOARD" => "$cp[addnewboard]",
  "LANG-ADDCATEGORY" => "$cp[newcat]",
  "BOARD" => "$boardmanager"));

$page->output();
break;
case 'board_order':
$id = anti_injection($_GET['id']);
$o = anti_injection($_GET['o']);
$cat = anti_injection($_GET['cat']);
$db->run = "SELECT * FROM ebb_boards where id='$id'";
$order_r = $db->result();
$db->close();
if ($o == "up"){
if ($order_r['B_Order'] == 1){
$error = $cp['ontop'];
echo error($error, "error");
}else{
$neworder = $order_r['B_Order'] - 1;
//move the old order number.
$move_dwn = $neworder + 1;
//perform query.
$db->run = "UPDATE ebb_boards SET B_Order='$move_dwn' WHERE B_Order='$neworder' and Category='$cat' and id!='$id'";
$db->query();
$db->close();
}
}
if ($o == "down"){
$db->run = "SELECT * FROM ebb_boards where Category='$cat'";
$ct = $db->num_results();
$db->close();
//see if user is trying to go lower than they can.
if($order_r['B_Order'] == $ct){
$error = $cp['onbottom'];
echo error($error, "error");
}else{
$neworder = $order_r['B_Order'] + 1;
//move the old order number.
$move_up = $neworder - 1;
//perform query.
$db->run = "UPDATE ebb_boards SET B_Order='$move_up' WHERE B_Order='$neworder' and Category='$cat' and id!='$id'";
$db->query();
$db->close();
}
}
//process the query.
$db->run = "UPDATE ebb_boards SET B_Order='$neworder' WHERE id='$id'";
$db->query();
$db->close();
//bring user back to board section
header("Location: CP.php?action=boards");
break;
case 'cat_order':
$id = anti_injection($_GET['id']);
$o = anti_injection($_GET['o']);
$db->run = "SELECT * FROM ebb_category where id='$id'";
$order_r = $db->result();
$db->close();
if ($o == "up"){
if ($order_r['C_Order'] == 1){
$error = $cp['ontop'];
echo error($error, "error");
}else{
$neworder = $order_r['C_Order'] - 1;
//move the old order number.
$move_dwn = $neworder + 1;
//perform query.
$db->run = "UPDATE ebb_category SET C_Order='$move_dwn' WHERE C_Order='$neworder' and id!='$id'";
$db->query();
$db->close();
}
}
if ($o == "down"){
$db->run = "SELECT * FROM ebb_category";
$ct = $db->num_results();
$db->close();
//see if user is trying to go lower than they can.
if($order_r['C_Order'] == $ct){
$error = $cp['onbottom'];
echo error($error, "error");
}else{
$neworder = $order_r['C_Order'] + 1;
$move_up = $neworder - 1;
//perform query.
$db->run = "UPDATE ebb_category SET C_Order='$move_up' WHERE C_Order='$neworder' and id!='$id'";
$db->query();
$db->close();
}
}
//process the query.
$db->run = "UPDATE ebb_category SET C_Order='$neworder' WHERE id='$id'";
$db->query();
$db->close();
//bring user back to board section
header("Location: CP.php?action=boards");
break;
case 'board_add':
$cat_select = category_select();
$page = new template($template_path ."/cp-newboard.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ADDBOARD" => "$cp[addnewboard]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TEXT" => "$cp[addboardtxt]",
  "LANG-BOARDNAME" => "$cp[boardname]",
  "LANG-DESCRIPTION" => "$cp[description]",
  "LANG-BOARDPERMISSION" => "$cp[boardpermissions]",
  "LANG-BOARDSETTINGS" => "$cp[boardsettings]",
  "LANG-ON" => "$cp[on]",
  "LANG-OFF" => "$cp[off]",
  "LANG-READACCESS" => "$cp[boardread]",
  "LANG-WRITEACCESS" => "$cp[boardwrite]",
  "LANG-REPLYACCESS" => "$cp[boardreply]",
  "LANG-VOTEACCESS" => "$cp[boardvote]",
  "LANG-POLLACCESS" => "$cp[boardpoll]",
  "LANG-EDITACCESS" => "$cp[boardedit]",
  "LANG-DELETEACCESS" => "$cp[boarddelete]",
  "LANG-IMPORTANTACCESS" => "$cp[boardimportant]",
  "LANG-ACCESS-ADMINS" => "$cp[access_admin]",
  "LANG-ACCESS-ADMINSMODS" => "$cp[access_admin_mod]",
  "LANG-ACCESS-ALL" => "$cp[access_all]",
  "LANG-ACCESS-NONE" => "$cp[access_none]",
  "LANG-ACCESS-REG" => "$cp[access_users]",
  "LANG-CATEGORY" => "$cp[category]",
  "CATEGORY" => "$cat_select",
  "LANG-HTML" => "$cp[html]",
  "LANG-BBCODE" => "$cp[bbcode]",
  "LANG-SMILE" => "$cp[smile]",
  "LANG-IMG" => "$cp[img]",
  "LANG-SUBMIT" => "$cp[addboard]"));

$page->output();
break;
case 'board_add_process':
$board_name = anti_injection($_POST['board_name']);
$description = anti_injection($_POST['description']);
$readaccess = anti_injection($_POST['readaccess']);
$writeaccess = anti_injection($_POST['writeaccess']);
$replyaccess = anti_injection($_POST['replyaccess']);
$voteaccess = anti_injection($_POST['voteaccess']);
$pollaccess = anti_injection($_POST['pollaccess']);
$editaccess = anti_injection($_POST['editaccess']);
$deleteaccess = anti_injection($_POST['deleteaccess']);
$importantaccess = anti_injection($_POST['importantaccess']);
$catsel = anti_injection($_POST['catsel']);
$html = anti_injection($_POST['html']);
$bbcode = anti_injection($_POST['bbcode']);
$smiles = anti_injection($_POST['smiles']);
$img = anti_injection($_POST['img']);
//do some error checking.
if ($board_name == ""){
$error = $cp['boardnameerror'];
echo error($error, "error");
}
if ($description == ""){
$error = $cp['descriptionerror'];
echo error($error, "error");
}
if ($readaccess == ""){
$error = $cp['noreadsetting'];
echo error($error, "error");
}
if ($writeaccess == ""){
$error = $cp['nowritesetting'];
echo error($error, "error");
}
if ($replyaccess == ""){
$error = $cp['noreplysetting'];
echo error($error, "error");
}
if ($voteaccess == ""){
$error = $cp['novotesetting'];
echo error($error, "error");
}
if ($pollaccess == ""){
$error = $cp['nopollsetting'];
echo error($error, "error");
}
if ($editaccess == ""){
$error = $cp['noeditsetting'];
echo error($error, "error");
}
if ($deleteaccess == ""){
$error = $cp['nodeletesetting'];
echo error($error, "error");
}
if ($importantaccess == ""){
$error = $cp['noimportantsetting'];
echo error($error, "error");
}
if ($catsel == ""){
$error = $cp['categoryerror'];
echo error($error, "error");
}
if ($html == ""){
$error = $cp['htmlerror'];
echo error($error, "error");
}
if ($bbcode == ""){
$error = $cp['bbcodeerror'];
echo error($error, "error");
}
if ($smiles == ""){
$error = $cp['smileserror'];
echo error($error, "error");
}
if ($img == ""){
$error = $cp['imgerror'];
echo error($error, "error");
}
if(strlen($board_name) > 50){
$error = $cp['longboardname'];
echo error($error, "error");
}
//get board order
$db->run = "SELECT * FROM ebb_boards where Category='$catsel'";
$ct = $db->num_results();
$db->close();
$board_order = $ct + 1;
//process the query.
$db->run = "insert into ebb_boards (Board, Description, Category, HTML, Smiles, BBcode, Image, B_Order) values ('$board_name', '$description', '$catsel', '$html', '$smiles', '$bbcode', '$img', '$board_order')";
$db->query();
$db->close();
//insert the permission rules into the permission table.
$db->run = "SELECT id FROM ebb_boards order by id DESC LIMIT 1";
$r_id = $db->result();
$db->close();
// process query.
$db->run = "insert into ebb_board_access (B_Read, B_Post, B_Reply, B_Vote, B_Poll, B_Delete, B_Edit, B_Important, B_id) values ('$readaccess', '$writeaccess', '$replyaccess', '$voteaccess', '$pollaccess', '$deleteaccess', '$editaccess', '$importantaccess', '$r_id[id]')";
$db->query();
$db->close();
//bring the user to the board section
header("Location: CP.php?action=boards");
break;
case 'board_modify':
$id = $_GET['id'];
$db->run = "SELECT * FROM ebb_boards where id='$id'";
$modify = $db->result();
$db->close();
//get permission values.
$db->run = "SELECT * FROM ebb_board_access where B_id='$id'";
$permission = $db->result();
$db->close();
//read detect.
if ($permission['B_Read'] == 1){
$read_status = "<select name=\"readaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"0\">$cp[access_all]
</select>";
}
if ($permission['B_Read'] == 2){
$read_status = "<select name=\"readaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"0\">$cp[access_all]
</select>";
}
if ($permission['B_Read'] == 3){
$read_status = "<select name=\"readaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"0\">$cp[access_all]
</select>";
}
if ($permission['B_Read'] == 0){
$read_status = "<select name=\"readaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"0\" selected>$cp[access_all]
</select>";
}
//write detect.
if ($permission['B_Post'] == 1){
$write_status = "<select name=\"writeaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Post'] == 2){
$write_status = "<select name=\"writeaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Post'] == 3){
$write_status = "<select name=\"writeaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Post'] == 4){
$reply_status = "<select name=\"writeaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//reply detect
if ($permission['B_Reply'] == 1){
$reply_status = "<select name=\"replyaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Reply'] == 2){
$reply_status = "<select name=\"replyaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Reply'] == 3){
$reply_status = "<select name=\"replyaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Reply'] == 4){
$reply_status = "<select name=\"replyaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//vote detect
if ($permission['B_Vote'] == 1){
$vote_status = "<select name=\"voteaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Vote'] == 2){
$vote_status = "<select name=\"voteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Vote'] == 3){
$vote_status = "<select name=\"voteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Vote'] == 4){
$vote_status = "<select name=\"voteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//poll detect
if ($permission['B_Poll'] == 1){
$poll_status = "<select name=\"pollaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Poll'] == 2){
$poll_status = "<select name=\"pollaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Poll'] == 3){
$poll_status = "<select name=\"pollaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"0\">$cp[access_all]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Poll'] == 4){
$poll_status = "<select name=\"pollaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//delete detect
if ($permission['B_Delete'] == 1){
$delete_status = "<select name=\"deleteaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Delete'] == 2){
$delete_status = "<select name=\"deleteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Delete'] == 3){
$delete_status = "<select name=\"deleteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"0\">$cp[access_all]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Delete'] == 4){
$delete_status = "<select name=\"deleteaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//edit detect
if ($permission['B_Edit'] == 1){
$edit_status = "<select name=\"editaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Edit'] == 2){
$edit_status = "<select name=\"editaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Edit'] == 3){
$edit_status = "<select name=\"editaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"0\">$cp[access_all]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Edit'] == 4){
$edit_status = "<select name=\"editaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//important detect
if ($permission['B_Important'] == 1){
$important_status = "<select name=\"importantaccess\" class=\"text\">
<option value=\"1\" selected>$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Important'] == 2){
$important_status = "<select name=\"importantaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\" selected>$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Important'] == 3){
$important_status = "<select name=\"importantaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\" selected>$cp[access_users]
<option value=\"0\">$cp[access_all]
<option value=\"4\">$cp[access_none]
</select>";
}
if ($permission['B_Important'] == 4){
$important_status = "<select name=\"importantaccess\" class=\"text\">
<option value=\"1\">$cp[access_admin]
<option value=\"2\">$cp[access_admin_mod]
<option value=\"3\">$cp[access_users]
<option value=\"4\" selected>$cp[access_none]
</select>";
}
//html detect.
if ($modify['HTML'] == 1){
$html = "<input type=\"radio\" name=\"html\" value=\"1\" checked>$cp[on] <input type=\"radio\" name=\"html\" value=\"0\">$cp[off]";
}else{
$html = "<input type=\"radio\" name=\"html\" value=\"1\">$cp[on] <input type=\"radio\" name=\"html\" value=\"0\" checked>$cp[off]";
}
//bbcode detect.
if ($modify['BBcode'] == 1){
$bbcode = "<input type=\"radio\" name=\"bbcode\" value=\"1\" checked>$cp[on] <input type=\"radio\" name=\"bbcode\" value=\"0\">$cp[off]";
}else{
$bbcode = "<input type=\"radio\" name=\"bbcode\" value=\"1\">$cp[on] <input type=\"radio\" name=\"bbcode\" value=\"0\" checked>$cp[off]";
}
//smiles detect.
if ($modify['Smiles'] == 1){
$smile = "<input type=\"radio\" name=\"smiles\" value=\"1\" checked>$cp[on] <input type=\"radio\" name=\"smiles\" value=\"0\">$cp[off]";
}else{
$smile = "<input type=\"radio\" name=\"smiles\" value=\"1\">$cp[on] <input type=\"radio\" name=\"smiles\" value=\"0\" checked>$cp[off]";
}
//image detect.
if ($modify['Image'] == 1){
$img = "<input type=\"radio\" name=\"img\" value=\"1\" checked>$cp[on] <input type=\"radio\" name=\"img\" value=\"0\">$cp[off]";
}else{
$img = "<input type=\"radio\" name=\"img\" value=\"1\">$cp[on] <input type=\"radio\" name=\"img\" value=\"0\" checked>$cp[off]";
}
$cat_select = category_select();
$page = new template($template_path ."/cp-modifyboard.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYBOARD" => "$cp[modifyboard]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "ID" => "$id",
  "LANG-BOARDNAME" => "$cp[boardname]",
  "BOARDNAME" => "$modify[Board]",
  "LANG-DESCRIPTION" => "$cp[description]",
  "DESCRIPTION" => "$modify[Description]",
  "LANG-BOARDPERMISSION" => "$cp[boardpermissions]",
  "LANG-BOARDSETTINGS" => "$cp[boardsettings]",
  "LANG-ON" => "$cp[on]",
  "LANG-OFF" => "$cp[off]",
  "LANG-READACCESS" => "$cp[boardread]",
  "READACCESS" => "$read_status",
  "LANG-WRITEACCESS" => "$cp[boardwrite]",
  "WRITEACCESS" => "$write_status",
  "LANG-REPLYACCESS" => "$cp[boardreply]",
  "REPLYACCESS" => "$reply_status",
  "LANG-VOTEACCESS" => "$cp[boardvote]",
  "VOTEACCESS" => "$vote_status",
  "LANG-POLLACCESS" => "$cp[boardpoll]",
  "POLLACCESS" => "$poll_status",
  "LANG-EDITACCESS" => "$cp[boardedit]",
  "EDITACCESS" => "$edit_status",
  "LANG-DELETEACCESS" => "$cp[boarddelete]",
  "DELETEACCESS" => "$delete_status",
  "LANG-IMPORTANTACCESS" => "$cp[boardimportant]",
  "IMPORTANTACCESS" => "$important_status",
  "LANG-CATEGORY" => "$cp[category]",
  "CATEGORY" => "$cat_select",
  "LANG-HTML" => "$cp[html]",
  "HTML" => "$html",
  "LANG-BBCODE" => "$cp[bbcode]",
  "BBCODE" => "$bbcode",
  "LANG-SMILE" => "$cp[smile]",
  "SMILE" => "$smile",
  "LANG-IMG" => "$cp[img]",
  "IMG" => "$img"));

$page->output();
break;
case 'board_modify_process':
$id = anti_injection($_GET['id']);
$modify_board_name = anti_injection($_POST['board_name']);
$modify_description = anti_injection($_POST['description']);
$modify_readaccess = anti_injection($_POST['readaccess']);
$modify_writeaccess = anti_injection($_POST['writeaccess']);
$modify_replyaccess = anti_injection($_POST['replyaccess']);
$modify_voteaccess = anti_injection($_POST['voteaccess']);
$modify_pollaccess = anti_injection($_POST['pollaccess']);
$modify_editaccess = anti_injection($_POST['editaccess']);
$modify_deleteaccess = anti_injection($_POST['deleteaccess']);
$modify_importantaccess = anti_injection($_POST['importantaccess']);
$modify_catsel = anti_injection($_POST['catsel']);
$modify_html = anti_injection($_POST['html']);
$modify_bbcode = anti_injection($_POST['bbcode']);
$modify_smiles = anti_injection($_POST['smiles']);
$modify_img = anti_injection($_POST['img']);
//do some error checking.
if ($modify_board_name == ""){
$error = $cp['boardnameerror'];
echo error($error, "error");
}
if ($modify_description == ""){
$error = $cp['descriptionerror'];
echo error($error, "error");
}
if ($modify_readaccess == ""){
$error = $cp['noreadsetting'];
echo error($error, "error");
}
if ($modify_writeaccess == ""){
$error = $cp['nowritesetting'];
echo error($error, "error");
}
if ($modify_replyaccess == ""){
$error = $cp['noreplysetting'];
echo error($error, "error");
}
if ($modify_voteaccess == ""){
$error = $cp['novotesetting'];
echo error($error, "error");
}
if ($modify_pollaccess == ""){
$error = $cp['nopollsetting'];
echo error($error, "error");
}
if ($modify_editaccess == ""){
$error = $cp['noeditsetting'];
echo error($error, "error");
}
if ($modify_deleteaccess == ""){
$error = $cp['nodeletesetting'];
echo error($error, "error");
}
if ($modify_importantaccess == ""){
$error = $cp['noimportantsetting'];
echo error($error, "error");
}
if ($modify_catsel == ""){
$error = $cp['categoryerror'];
echo error($error, "error");
}
if ($modify_html == ""){
$error = $cp['htmlerror'];
echo error($error, "error");
}
if ($modify_bbcode == ""){
$error = $cp['bbcodeerror'];
echo error($error, "error");
}
if ($modify_smiles == ""){
$error = $cp['smileserror'];
echo error($error, "error");
}
if ($modify_img == ""){
$error = $cp['imgerror'];
echo error($error, "error");
}
if(strlen($modify_board_name) > 50){
$error = $cp['longboardname'];
echo error($error, "error");
}
//process the query.
$db->run = "UPDATE ebb_boards SET Board='$modify_board_name', Description='$modify_description', Category='$modify_catsel', HTML='$modify_html', Smiles='$modify_smiles', BBcode='$modify_bbcode', Image='$modify_img' WHERE id='$id'";
$db->query();
$db->close();
//modify the permission table.
$db->run = "UPDATE ebb_board_access SET B_Read='$modify_readaccess', B_Post='$modify_writeaccess', B_Reply='$modify_replyaccess', B_Vote='$modify_voteaccess', B_Poll='$modify_pollaccess', B_Delete='$modify_deleteaccess', B_Edit='$modify_editaccess', B_Important='$modify_importantaccess' WHERE B_id='$id'";
$db->query();
$db->close();
//bring user back to board section
header("Location: CP.php?action=boards");
break;
case 'board_delete':
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/cp-deleteboard.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-DELETEBOARD" => "$cp[delboard]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$cp[delsure]",
  "ID" => "$id",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
break;
case 'board_delete_process':
$id = anti_injection($_GET['id']);
//get topic details for deleting other items.
$sql = "Select bid FROM ebb_topics WHERE bid='$id'";
$errorq = $sql;
$boarddel_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while($r = mysql_fetch_assoc($boarddel_query)){
//delete polls made by topics in this board.
$db->run = "DELETE FROM ebb_poll WHERE tid='$r[tid]'";
$db->query();
$db->close();
//delete read status from topics made in this board.
$db->run = "DELETE FROM ebb_read WHERE Topic='$r[tid]'";
$db->query();
$db->close();
//delete any user subscriptions for topics made in this board.
$db->run = "DELETE FROM ebb_topic_watch WHERE tid='$r[tid]'";
$db->query();
$db->close();
}
//delete board.
$db->run = "DELETE FROM ebb_boards WHERE id='$id'";
$db->query();
$db->close();
//delete topics made in that board.
$db->run = "DELETE FROM ebb_topics WHERE bid='$id'";
$db->query();
$db->close();
//delete read status from the db.
$db->run = "DELETE FROM ebb_read WHERE Board='$id'";
$db->query();
$db->close();
//delete posts made in that board.
$db->run = "DELETE FROM ebb_posts WHERE bid='$id'";
$db->query();
$db->close();
//delete the permission rules set for this board.
$db->run = "DELETE FROM ebb_board_access WHERE B_id='$id'";
$db->query();
$db->close();
//delete the moderator list for this board.
$db->run = "DELETE FROM ebb_grouplist WHERE board_id='$id'";
$db->query();
$db->close();
//bring user back to board section
header("Location: CP.php?action=boards");
break;
case 'category_add':
$page = new template($template_path ."/cp-newcategory.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ADDCATEGORY" => "$cp[addcat]",
  "LANG-TEXT" => "$cp[addcattxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CATEGORYNAME" => "$cp[catname]"));

$page->output();
break;
case 'category_add_process':
$category_name = anti_injection($_POST['category_name']);
if($category_name == ""){
$error = $cp['addcaterror'];
echo error($error, "error");
}
if(strlen($category_name) > 50){
$error = $cp['longcatname'];
echo error($error, "error");
}
//get board order
$db->run = "SELECT * FROM ebb_category";
$ct = $db->num_results();
$db->close();
$catorder = $ct + 1;
//process query.
$db->run = "insert into ebb_category (Name, C_Order) values ('$category_name', '$catorder')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=boards");
break;
case 'category_modify':
$id = anti_injection($_GET['id']);
$db->run = "SELECT * FROM ebb_category WHERE id='$id'";
$modify_cat_r = $db->result();
$db->close();
$page = new template($template_path ."/cp-modifycategory.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYCATEGORY" => "$cp[modcat]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-CATEGORYNAME" => "$cp[catname]",
  "CATEGORYNAME" => "$modify_cat_r[Name]",
  "CATEGORY" => "$admin_category"));

$page->output();
break;
case 'category_modify_process':
$id = anti_injection($_GET['id']);
$modify_category_name = anti_injection($_POST['category_name']);
if ($modify_category_name == ""){
$error = $cp['modcaterror'];
echo error($error, "error");
}
if(strlen($modify_category_name) > 50){
$error = $cp['longcatname'];
echo error($error, "error");
}
//process query
$db->run = "UPDATE ebb_category SET Name='$modify_category_name' WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=boards");
break;
case 'category_delete':
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/cp-deletecategory.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-DELETECATEGORY" => "$cp[delcat]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$cp[delsure]",
  "ID" => "$id",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]",
  "LANG-DELETEWARNING" => "$cp[catdelwarning]"));

$page->output();
break;
case 'category_delete_process':
$id = anti_injection($_GET['id']);
//get needed details for deleting other items.
$sql = "Select Category FROM ebb_boards WHERE Category='$id'";
$errorq = $sql;
$catdel_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while($r = mysql_fetch_assoc($catdel_query)){
$sql2 = "Select bid FROM ebb_topics WHERE bid='$r[id]'";
$errorq = $sql2;
$topicdel_query = mysql_query ($sql2) or die(error($error, "mysql", $errorq));
while($r2 = mysql_fetch_assoc($topicdel_query)){
//delete polls made by topics in this board.
$db->run = "DELETE FROM ebb_poll WHERE tid='$r2[tid]'";
$db->query();
$db->close();
//delete read status from topics made in this board.
$db->run = "DELETE FROM ebb_read WHERE Topic='$r2[tid]'";
$db->query();
$db->close();
//delete any user subscriptions for topics made in this board.
$db->run = "DELETE FROM ebb_topic_watch WHERE tid='$r2[tid]'";
$db->query();
$db->close();
}
//delete topics made in that board.
$db->run = "DELETE FROM ebb_topics WHERE bid='$r[id]'";
$db->query();
$db->close();
//delete read status from the db.
$db->run = "DELETE FROM ebb_read WHERE Board='$r[id]'";
$db->query();
$db->close();
//delete posts made in that board.
$db->run = "DELETE FROM ebb_posts WHERE bid='$r[id]'";
$db->query();
$db->close();
//delete the permission rules set for this board.
$db->run = "DELETE FROM ebb_board_access WHERE B_id='$r[id]'";
$db->query();
$db->close();
//delete the moderator list for this board.
$db->run = "DELETE FROM ebb_grouplist WHERE board_id='$r[id]'";
$db->query();
$db->close();
}
//delete board.
$db->run = "DELETE FROM ebb_boards WHERE Category='$id'";
$db->query();
$db->close();
//delete category.
$db->run = "DELETE FROM ebb_category WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=boards");
break;
case 'groupsetup':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$grouplist = admin_grouplist();
$page = new template($template_path ."/cp-groupsetup.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPSETUP" => "$cp[groupsetup]",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BORDER" => "$border",
  "LANG-TEXT" => "$cp[grouptxt]",
  "LANG-NEWGROUP" => "$cp[addgroup]",
  "LANG-GROUPNAME" => "$groups[name]",
  "LANG-GROUPDESCRIPTION" => "$groups[description]",
  "LANG-GROUPLEADER" => "$groups[leader]",
  "LANG-GROUPSTATUS" => "$groups[groupstat]",
  "LANG-GROUPACCESS" => "$cp[groupaccess]",
  "LANG-GROUPMEMBERLIST" => "$groups[groupmembers]",
  "GROUPS" => "$grouplist"));

$page->output();
break;
case 'new_group':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$page = new template($template_path ."/cp-newgroup.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-CREATEGROUP" => "$cp[addgroup]",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BORDER" => "$border",
  "LANG-GROUPNAME" => "$groups[name]",
  "LANG-GROUPDESCRIPTION" => "$groups[description]",
  "LANG-GROUPLEADER" => "$groups[leader]",
  "LANG-GROUPSTATUS" => "$groups[groupstat]",
  "LANG-OPEN" => "$groups[open]",
  "LANG-CLOSE" => "$groups[closed]",
  "LANG-HIDDEN" => "$cp[grouphidden]",
  "LANG-GROUPACCESS" => "$cp[groupaccess]",
  "LANG-SEL-LEVEL" => "$cp[sel_level]",
  "LANG-LEVEL-1" => "$cp[level1]",
  "LANG-LEVEL-2" => "$cp[level2]",
  "LANG-LEVEL-3" => "$cp[level3]",
  "LANG-LIMITATIONS" => "$cp[limitation]",
  "LANG-NONE" => "$cp[none]",
  "LANG-LIMITEDCP" => "$cp[limitedcp]",
  "LANG-BANPM" => "$cp[banpm]",
  "LANG-BANAVATAR" => "$cp[banavatar]",
  "LANG-BANPROFILE" => "$cp[banprofile]",
  "LANG-ADDGROUP" => "$cp[addgroup]"));

$page->output();
break;
case 'new_group_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
//get data from form.
$group = anti_injection($_POST['group']);
$description = anti_injection($_POST['description']);
$leader = anti_injection($_POST['leader']);
$status = anti_injection($_POST['status']);
$groupaccess = anti_injection($_POST['groupaccess']);
$limitations = anti_injection($_POST['limitations']);
//do some error checking.
if ($group == ""){
$error = $cp['nogroupname'];
echo error($error, "error");
}
if ($description == ""){
$error = $cp['nogroupdescription'];
echo error($error, "error");
}
if ($leader == ""){
$error = $cp['nogroupleader'];
echo error($error, "error");
}
if ($status == ""){
$error = $cp['statusnotset'];
echo error($error, "error");
}
if ($groupaccess == ""){
$error = $cp['noaccessset'];
echo error($error, "error");
}
if($limitations == ""){
$error = $cp['nolimitationset'];
echo error($error, "error");
}
if(strlen($group) > 30){
$error = $cp['longgroupname'];
echo error($error, "error");
}
if(strlen($leader) > 50){
$error = $cp['longleadername'];
echo error($error, "error");
}
if(($limitations == 1) AND ($groupaccess != 1)){
$error = $cp['wrongrule'];
echo error($error, "error");
}
//process query.
$db->run = "insert into ebb_groups (Name, Description, Leader, Enrollment, Level, Limitation) values ('$group', '$description', '$leader', '$status', '$groupaccess', '$limitations')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=groupsetup");
break;
case 'group_modify':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$db->run = "SELECT * FROM ebb_groups WHERE id='$id'";
$group_r = $db->result();
$db->close();
#get membership type setting.
if ($group_r['Enrollment'] == 0){
$group_status = "<input type=\"radio\" name=\"status\" value=\"0\" checked>$groups[closed] <input type=\"radio\" name=\"status\" value=\"1\">$groups[open]  <input type=\"radio\" name=\"status\" value=\"2\">$cp[grouphidden]";
}elseif ($group_r['Enrollment'] == 1){
$group_status = "<input type=\"radio\" name=\"status\" value=\"0\">$groups[closed] <input type=\"radio\" name=\"status\" value=\"1\" checked>$groups[open] <input type=\"radio\" name=\"status\" value=\"2\">$cp[grouphidden]";
}else{
$group_status = "<input type=\"radio\" name=\"status\" value=\"0\">$groups[closed] <input type=\"radio\" name=\"status\" value=\"1\">$groups[open] <input type=\"radio\" name=\"status\" value=\"2\" checked>$cp[grouphidden]";
}
#get level setting
if ($group_r['Level'] == 1){
$access_status = "<select name=\"groupaccess\" class=\"text\">
<option value=\"1\" selected>$cp[level1]
<option value=\"2\">$cp[level2]
<option value=\"3\">$cp[level3]
</select>";
}elseif($group_r['Level'] == 2){
$access_status = "<select name=\"groupaccess\" class=\"text\">
<option value=\"1\">$cp[level1]
<option value=\"2\" selected>$cp[level2]
<option value=\"3\">$cp[level3]
</select>";
}else{
$access_status = "<select name=\"groupaccess\" class=\"text\">
<option value=\"1\">$cp[level1]
<option value=\"2\">$cp[level2]
<option value=\"3\" selected>$cp[level3]
</select>";
}
#get limitation setting.
if($group_r['Limitation'] == 0){
$limitation_status = "<select name=\"limitations\" class=\"text\">
<option value=\"0\" selected>$cp[none]
<option value=\"1\">$cp[limitedcp]
<option value=\"2\">$cp[banpm]
<option value=\"3\">$cp[banavatar]
<option value=\"4\">$cp[banprofile]";
}elseif($group_r['Limitation'] == 1){
$limitation_status = "<select name=\"limitations\" class=\"text\">
<option value=\"0\">$cp[none]
<option value=\"1\" selected>$cp[limitedcp]
<option value=\"2\">$cp[banpm]
<option value=\"3\">$cp[banavatar]
<option value=\"4\">$cp[banprofile]";
}elseif($group_r['Limitation'] == 2){
$limitation_status = "<select name=\"limitations\" class=\"text\">
<option value=\"0\">$cp[none]
<option value=\"1\">$cp[limitedcp]
<option value=\"2\" selected>$cp[banpm]
<option value=\"3\">$cp[banavatar]
<option value=\"4\">$cp[banprofile]";
}elseif($group_r['Limitation'] == 3){
$limitation_status = "<select name=\"limitations\" class=\"text\">
<option value=\"0\">$cp[none]
<option value=\"1\">$cp[limitedcp]
<option value=\"2\">$cp[banpm]
<option value=\"3\" selected>$cp[banavatar]
<option value=\"4\">$cp[banprofile]";
}else{
$limitation_status = "<select name=\"limitations\" class=\"text\">
<option value=\"0\">$cp[none]
<option value=\"1\">$cp[limitedcp]
<option value=\"2\">$cp[banpm]
<option value=\"3\">$cp[banavatar]
<option value=\"4\" selected>$cp[banprofile]";
}
#output it.
$page = new template($template_path ."/cp-modifygroup.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYGROUP" => "$cp[modifygroup]",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "BORDER" => "$border",
  "ID" => "$id",
  "LANG-GROUPNAME" => "$groups[name]",
  "GROUPNAME" => "$group_r[Name]",
  "LANG-GROUPDESCRIPTION" => "$groups[description]",
  "GROUPDESCRIPTION" => "$group_r[Description]",
  "LANG-GROUPLEADER" => "$groups[leader]",
  "GROUPLEADER" => "$group_r[Leader]",
  "LANG-GROUPSTATUS" => "$groups[groupstat]",
  "GROUPSTATUS" => "$group_status",
  "LANG-GROUPACCESS" => "$cp[groupaccess]",
  "GROUPACCESS" => "$access_status",
  "LANG-LIMITATIONS" => "$cp[limitation]",
  "LIMITATIONS" => "$limitation_status"));

$page->output();
break;
case 'modify_group_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
//get data from form.
$id = anti_injection($_GET['id']);
$group = anti_injection($_POST['group']);
$description = anti_injection($_POST['description']);
$leader = anti_injection($_POST['leader']);
$status = anti_injection($_POST['status']);
$groupaccess = anti_injection($_POST['groupaccess']);
$limitations = anti_injection($_POST['limitations']);
//do some error checking.
if ($group == ""){
$error = $cp['nogroupname'];
echo error($error, "error");
}
if ($description == ""){
$error = $cp['nogroupdescription'];
echo error($error, "error");
}
if ($leader == ""){
$error = $cp['nogroupleader'];
echo error($error, "error");
}
if ($status == ""){
$error = $cp['statusnotset'];
echo error($error, "error");
}
if ($groupaccess == ""){
$error = $cp['noaccessset'];
echo error($error, "error");
}
if($limitations == ""){
$error = $cp['nolimitationset'];
echo error($error, "error");
}
if(strlen($group) > 30){
$error = $cp['longgroupname'];
echo error($error, "error");
}
if(strlen($leader) > 50){
$error = $cp['longleadername'];
echo error($error, "error");
}
if(($limitations == 1) AND ($groupaccess != 1)){
$error = $cp['wrongrule'];
echo error($error, "error");
}
//process query.
$db->run = "update ebb_groups SET Name='$group', Description='$description', Leader='$leader', Enrollment='$status', Level='$groupaccess', Limitation='$limitations' where id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=groupsetup");
break;
case 'group_delete':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
//see if user is trying to remove the default groups, if so cancel that action!
if (($id == 1) OR ($id == 2)){
$error = $cp['nodelgroup'];
echo error($error, "error");
}
//proces query.
$db->run = "delete from ebb_groups where id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=groupsetup");
break;
case 'group_memberlist':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$groupmemberlist = admin_view_group();
$page = new template($template_path ."/cp-groupmemberlist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPMEMBERLIST" => "$cp[viewlist]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TEXT" => "$cp[groupmemberlist]",
  "LANG-GROUPMEMBERS" => "$groups[groupmembers]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-POSTCOUNT" => "$members[posts]",
  "LANG-REGISTRATIONDATE" => "$members[joindate]",
  "GROUPMEMBERS" => "$groupmemberlist"));
$page->output();
break;
case 'groupmember_remove':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$gid = anti_injection($_GET['gid']);
$u = anti_injection($_GET['u']);
//check to see if this move will remove all level 1 users.
$db->run = "select * from ebb_group_users where gid='$gid'";
$admin_num_chk = $db->num_results();
$db->close();
if (($gid == 1) AND ($admin_num_chk == 1)){
$error = $cp['nouserdelete'];
echo error($error, "error");
}
//proces query.
$db->run = "delete from ebb_group_users where gid='$gid' AND Username='$u'";
$db->query();
$db->close();
//change user back to regular status.
$db->run = "update ebb_users SET Status='Member' where Username='$u'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=group_memberlist&id=$gid");
break;
case 'grouppermission':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$admin_grouplist = permission_display_group();
$page = new template($template_path ."/cp-permissionchoice.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPPERMISSION" => "$cp[grouppermission]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-GROUPLIST" => "$groups[title]",
  "GROUPLIST" => "$admin_grouplist"));
$page->output();
break;
case 'grouprights':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$group_rights = group_permission();
$page = new template($template_path ."/cp-grouppermission.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPPERMISSION" => "$cp[grouppermission]",
  "LANG-TEXT" => "$cp[grouppermissiontxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-GROUPPERMISSION" => "$cp[grouppermission]",
  "LANG-BOARDNAME" => "$cp[boardname]",
  "LANG-GROUPRIGHTS" => "$cp[grouprights]",
  "GROUPRIGHTS" => "$group_rights"));
$page->output();
break;
case 'grouprights_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$stat = anti_injection($_GET['stat']);
$gid = anti_injection($_GET['gid']);
$bid = anti_injection($_GET['bid']);
if($stat == "grant"){
$db->run = "insert into ebb_grouplist (group_id, board_id) values('$gid', '$bid')";
$db->query();
$db->close();
}else{
$db->run = "delete from ebb_grouplist where group_id='$gid' and board_id='$bid'";
$db->query();
$db->close();
}
//bring user back
header("Location: CP.php?action=grouprights&id=$gid");
break;
case 'pendinglist':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$admin_grouplist = admin_display_group();
$page = new template($template_path ."/cp-pendingchoice.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPPENDINGLIST" => "$cp[pendinglist]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-GROUPLIST" => "$groups[title]",
  "GROUPLIST" => "$admin_grouplist"));
$page->output();
break;
case 'pendingview':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$pendingusers = admin_grouppending();
$page = new template($template_path ."/cp-pendinglist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-GROUPPENDINGLIST" => "$cp[pendinglist]",
  "LANG-TEXT" => "$cp[pendinglisttxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-PENDINGLIST" => "$cp[pendinglist]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-POSTCOUNT" => "$members[posts]",
  "LANG-REGISTRATIONDATE" => "$members[joindate]",
  "PENDINGLIST" => "$pendingusers",
  "LANG-ADDUSERTOGROUP" => "$cp[addtogroup]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-SUBMIT" => "$cp[addusergroup]",
  "ID" => "$id"));
$page->output();
break;
case 'pending_stat':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$accept = anti_injection($_GET['accept']);
$gid = anti_injection($_GET['gid']);
$u = anti_injection($_GET['u']);
if($accept == 1){
//proces query.
$db->run = "update ebb_group_users SET Status='Active' where gid='$gid' AND Username='$u'";
$db->query();
$db->close();
//change user to group status.
$db->run = "update ebb_users SET Status='groupmember' where Username='$u'";
$db->query();
$db->close();
}else{
//proces query.
$db->run = "delete from ebb_group_users where gid='$gid' AND Username='$u'";
$db->query();
$db->close();
}
//bring user back
header("Location: CP.php?action=groupsetup");
break;
case 'group_adduser':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$user = anti_injection($_POST['user']);
$id = anti_injection($_POST['id']);
if($user == ""){
$error = $cp['nousername_group'];
echo error($error, "error");
}
if($id == ""){
$error = $cp['noiderror'];
echo error($error, "error");
}
//proces query.
$db->run = "insert into ebb_group_users (Username, gid, Status) values('$user', '$id', 'Active')";
$db->query();
$db->close();
//change user to group status.
$db->run = "update ebb_users SET Status='groupmember' where Username='$user'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=groupsetup");
break;
case 'smiles':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$admin_smiles = admin_smilelisting();
$page = new template($template_path ."/cp-smiles.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-SMILES" => "$cp[smiles]",
  "LANG-ADDSMILES" => "$cp[addsmiles]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SMILE" => "$cp[smiletbl]",
  "LANG-CODE" => "$cp[codetbl]",
  "LANG-FILENAME" => "$cp[filetbl]",
  "SMILES" => "$admin_smiles"));

$page->output();
break;
case 'add_smiles':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$page = new template($template_path ."/cp-newsmiles.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ADDSMILES" => "$cp[addsmiles]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SMILECODE" => "$cp[smilecode]",
  "LANG-SMILEFILE" => "$cp[smilefile]"));

$page->output();
break;
case 'add_smiles_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$smile_code = anti_injection($_POST['smile_code']);
$smile_file = anti_injection($_POST['smile_file']);
if ($smile_code == ""){
$error = $cp['nosmilecodeerror'];
echo error($error, "error");
}
if ($smile_file == ""){
$error = $cp['nosmilefileerror'];
echo error($error, "error");
}
if(strlen($smile_code) > 30){
$error = $cp['longsmilecode'];
echo error($error, "error");
}
if(strlen($smile_file) > 80){
$error = $cp['longsmilepath'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_smiles (code, img_name) values('$smile_code', '$smile_file')";
$db->query();
$db->close();
//bring user back.
header("Location: CP.php?action=smiles");
break;
case 'modify_smiles':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$db->run = "select * from ebb_smiles where id='$id'";
$modify_smiles_r = $db->result();
$db->close();
$page = new template($template_path ."/cp-modifysmiles.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYSMILES" => "$cp[modifysmiles]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SMILECODE" => "$cp[smilecode]",
  "SMILECODE" => "$modify_smiles_r[code]",
  "LANG-SMILEFILE" => "$cp[smilefile]",
  "SMILEFILE" => "$modify_smiles_r[img_name]"));

$page->output();
break;
case 'modify_smiles_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$mod_smile_code = anti_injection($_POST['smile_code']);
$mod_smile_file = anti_injection($_POST['smile_file']);
if ($mod_smile_code == ""){
$error = $cp['nosmilecodeerror'];
echo error($error, "error");
}
if ($mod_smile_file == ""){
$error = $cp['nosmilefileerror'];
echo error($error, "error");
}
if(strlen($mod_smile_code) > 30){
$error = $cp['longsmilecode'];
echo error($error, "error");
}
if(strlen($mod_smile_file) > 80){
$error = $cp['longsmilepath'];
echo error($error, "error");
}
//process query
$db->run = "update ebb_smiles set code='$mod_smile_code', img_name='$mod_smile_file' where id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=smiles");
break;
case 'delete_smiles':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/cp-deletesmiles.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-DELETESMILES" => "$cp[delsmiles]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$cp[delsure]",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
break;
case 'delete_smiles_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
//process query
$db->run = "delete from ebb_smiles where id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=smiles");
break;
case 'newsletter':
$page = new template($template_path ."/cp-newsletter.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-NEWSLETTER" => "$cp[newsletter]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TEXT" => "$cp[newslettertxt]",
  "LANG-SUBJECT" => "$cp[subject]",
  "LANG-MESSAGE" => "$cp[msg]",
  "LANG-SENDTO" => "$cp[sendopt]",
  "LANG-ADMINSEND" => "$cp[sendadmin]",
  "LANG-MODERATORSEND" => "$cp[sendmod]",
  "LANG-ALLSEND" => "$cp[sendall]",
  "LANG-SENDNEWSLETTER" => "$cp[sendnewsletter]"));

$page->output();
break;
case 'mail_send':
$subject = stripslashes($_POST['subject']);
$mail_message = stripslashes($_POST['mail_message']);
//error checking.
if($subject == ""){
$error = $cp['nosubject'];
echo error($error, "error");
}
if ($mail_message == ""){
$error = $cp['nomailmsg'];
echo error($error, "error");
}
//query.
$sql = "SELECT * FROM ebb_users";
$errorq = $sql;
$newsletter_q = mysql_query($sql) or die(error($error, "mysql", $errorq));
//see what form of email will be used to send out the emails.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $subject;
//get body of newsletter.
$mailer->Body = $mail_message;
//add users to the email list
while ($row = mysql_fetch_array($newsletter_q)) {
$mailer->AddBCC($row['Email']);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
while ($row = mysql_fetch_array($newsletter_q)) {
set_time_limit(1200); //set the time higher if you need to.
@mail($row['Email'], $subject, $mail_message, $headers);
}
}
//bring user back.
header("Location: CP.php");
break;
case 'ranks':
$admin_rank = admin_ranklisting();
$page = new template($template_path ."/cp-rank.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-RANK" => "$cp[ranks]",
  "LANG-ADDRANK" => "$cp[addrank]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-RANKNAME" => "$cp[rankname]",
  "LANG-POSTRULE" => "$cp[postrule]",
  "RANKS" => "$admin_rank"));

$page->output();
break;
case 'add_rank':
$page = new template($template_path ."/cp-newrank.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ADDRANK" => "$cp[addrank]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-RANKNAME" => "$cp[rankname]",
  "LANG-STAR" => "$cp[stars]",
  "LANG-POSTRULE" => "$cp[postrule]"));

$page->output();
break;
case 'rank_add_process':
$rank_name = anti_injection($_POST['rank_name']);
$rank_star = anti_injection($_POST['rank_star']);
$rank_post = anti_injection($_POST['rank_post']);
if ($rank_name == ""){
$error = $cp['norankname'];
echo error($error, "error");
}
if ($rank_star == ""){
$error = $cp['nostarfile'];
echo error($error, "error");
}
if ($rank_post == ""){
$error = $cp['nopostrule'];
echo error($error, "error");
}
if(strlen($rank_name) > 50){
$error = $cp['longrankname'];
echo error($error, "error");
}
if(strlen($rank_star) > 60){
$error = $cp['longrankstar'];
echo error($error, "error");
}
if(strlen($rank_post) > 5){
$error = $cp['longrankpost'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_ranks (Name, Star_Image, Post_req) values ('$rank_name', '$rank_star', '$rank_post')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=ranks");
break;
case 'rank_edit':
$id = anti_injection($_GET['id']);
$db->run = "Select * FROM ebb_ranks WHERE id='$id'";
$edit_rank = $db->result();
$db->close();
$page = new template($template_path ."/cp-modifyrank.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYRANK" => "$cp[editrank]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-RANKNAME" => "$cp[rankname]",
  "RANKNAME" => "$edit_rank[Name]",
  "LANG-STAR" => "$cp[stars]",
  "STARIMAGE" => "$edit_rank[Star_Image]",
  "LANG-POSTRULE" => "$cp[postrule]",
  "POSTRULE" => "$edit_rank[Post_req]"));

$page->output();
break;
case 'rank_edit_process':
$id = anti_injection($_GET['id']);
$modify_rank_name = anti_injection($_POST['rank_name']);
$modify_rank_star = anti_injection($_POST['rank_star']);
$modify_post = anti_injection($_POST['rank_post']);
if ($modify_rank_name == ""){
$error = $cp['norankname'];
echo error($error, "error");
}
if ($modify_rank_star == ""){
$error = $cp['nostarfile'];
echo error($error, "error");
}
if ($modify_post == ""){
$error = $cp['nopostrule'];
echo error($error, "error");
}
if(strlen($modify_rank_name) > 50){
$error = $cp['longrankname'];
echo error($error, "error");
}
if(strlen($modify_rank_star) > 60){
$error = $cp['longrankstar'];
echo error($error, "error");
}
if(strlen($modify_post) > 5){
$error = $cp['longrankpost'];
echo error($error, "error");
}
//process query
$db->run = "UPDATE ebb_ranks SET Name='$modify_rank_name', Star_Image='$modify_rank_star', Post_req='$modify_post' WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=ranks");
break;
case 'rank_delete':
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/cp-deleterank.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-DELETERANK" => "$cp[delrank]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$cp[delsure]",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
break;
case 'rank_delete_process':
$id = anti_injection($_GET['id']);
//process query
$db->run = "DELETE FROM ebb_ranks WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=ranks");
break;
case 'settings':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$db->run = "SELECT * FROM ebb_settings";
$settings = $db->result();
$db->close();
$timezone = timezone_select();
$style_select = style_select();
$lang = lang_select();
//board status detection
if($settings['Board_Status'] == 1){
$board_status = "<input type=\"radio\" name=\"board_stat\" value=\"1\" class=\"text\" checked>$cp[on] <input type=\"radio\" name=\"board_stat\" value=\"0\" class=\"text\">$cp[off]";
}
else{
$board_status = "<input type=\"radio\" name=\"board_stat\" value=\"1\" class=\"text\">$cp[on] <input type=\"radio\" name=\"board_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//announcement status detection
if($settings['Announcement_Status'] == 1){
$announce_status = "<input type=\"radio\" name=\"announce_stat\" value=\"1\" class=\"text\" checked>$cp[on] <input type=\"radio\" name=\"announce_stat\" value=\"0\" class=\"text\">$cp[off]";
}
else{
$announce_status = "<input type=\"radio\" name=\"announce_stat\" value=\"1\" class=\"text\">$cp[on] <input type=\"radio\" name=\"announce_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//tos status detection
if ($settings['TOS_Status'] == 1){
$tos_stat = "<input type=\"radio\" name=\"term_stat\" value=\"1\" class=\"text\" checked>$cp[on] <input type=\"radio\" name=\"term_stat\" value=\"0\" class=\"text\">$cp[off]";
}
else{
$tos_stat = "<input type=\"radio\" name=\"term_stat\" value=\"1\" class=\"text\">$cp[on] <input type=\"radio\" name=\"term_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//gzip status detection
if ($settings['GZIP'] == 1){
$gzip_status = "<input type=\"radio\" name=\"gzip_stat\" value=\"1\" class=\"text\" checked>$cp[on] <input type=\"radio\" name=\"gzip_stat\" value=\"0\" class=\"text\">$cp[off]";
}
else{
$gzip_status = "<input type=\"radio\" name=\"gzip_stat\" value=\"1\" class=\"text\">$cp[on] <input type=\"radio\" name=\"gzip_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//security image detection
if ($settings['Image_Verify'] == 1){
$imagevert_status = "<input type=\"radio\" name=\"imagevert_stat\" value=\"1\" class=\"text\" checked>$cp[lowset] <input type=\"radio\" name=\"imagevert_stat\" value=\"2\" class=\"text\">$cp[highset] <input type=\"radio\" name=\"imagevert_stat\" value=\"0\" class=\"text\">$cp[off]";
}elseif($settings['Image_Verify'] == 2){
$imagevert_status = "<input type=\"radio\" name=\"imagevert_stat\" value=\"1\" class=\"text\">$cp[lowset] <input type=\"radio\" name=\"imagevert_stat\" value=\"2\" class=\"text\" checked>$cp[highset] <input type=\"radio\" name=\"imagevert_stat\" value=\"0\" class=\"text\">$cp[off]";
}else{
$imagevert_status = "<input type=\"radio\" name=\"imagevert_stat\" value=\"1\" class=\"text\">$cp[lowset] <input type=\"radio\" name=\"imagevert_stat\" value=\"2\" class=\"text\">$cp[highset] <input type=\"radio\" name=\"imagevert_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//spell checker detection
if ($settings['spell_checker'] == 1){
$spell_status = "<input type=\"radio\" name=\"spell_stat\" value=\"1\" class=\"text\" checked>$cp[on] <input type=\"radio\" name=\"spell_stat\" value=\"0\" class=\"text\">$cp[off]";
}else{
$spell_status = "<input type=\"radio\" name=\"spell_stat\" value=\"1\" class=\"text\">$cp[on] <input type=\"radio\" name=\"spell_stat\" value=\"0\" class=\"text\" checked>$cp[off]";
}
//activation detection
if($settings['activation'] == "User"){
$activate_status = "<input type=\"radio\" name=\"active_stat\" value=\"None\" class=\"text\">$cp[activenone] <input type=\"radio\" name=\"active_stat\" value=\"User\" class=\"text\" checked>$cp[activeusers] <input type=\"radio\" name=\"active_stat\" value=\"Admin\" class=\"text\">$cp[activeadmin]";
}elseif($settings['activation'] == "Admin"){
$activate_status = "<input type=\"radio\" name=\"active_stat\" value=\"None\" class=\"text\">$cp[activenone] <input type=\"radio\" name=\"active_stat\" value=\"User\" class=\"text\">$cp[activeusers] <input type=\"radio\" name=\"active_stat\" value=\"Admin\" class=\"text\" checked>$cp[activeadmin]";
}else{
$activate_status = "<input type=\"radio\" name=\"active_stat\" value=\"None\" class=\"text\" checked>$cp[activenone] <input type=\"radio\" name=\"active_stat\" value=\"User\" class=\"text\">$cp[activeusers] <input type=\"radio\" name=\"active_stat\" value=\"Admin\" class=\"text\">$cp[activeadmin]";
}
//ssl detection status
if ($settings['cookie_secure'] == 1){
$secure_status = "<input type=\"radio\" name=\"secure_stat\" value=\"1\" class=\"text\" checked>$cp[enable] <input type=\"radio\" name=\"secure_stat\" value=\"0\" class=\"text\">$cp[disable]";
}
else{
$secure_status = "<input type=\"radio\" name=\"secure_stat\" value=\"1\" class=\"text\">$cp[enable] <input type=\"radio\" name=\"secure_stat\" value=\"0\" class=\"text\" checked>$cp[disable]";
}
//mail type detection.
if($settings['mail_type'] == 0){
$mail_status = "<input type=\"radio\" name=\"mail_type\" value=\"0\" class=\"text\" checked>$cp[mailsmtp] <input type=\"radio\" name=\"mail_type\" value=\"1\" class=\"text\">$cp[mailreg]";
}else{
$mail_status = "<input type=\"radio\" name=\"mail_type\" value=\"0\" class=\"text\">$cp[mailsmtp] <input type=\"radio\" name=\"mail_type\" value=\"1\" class=\"text\" checked>$cp[mailreg]";
}
$page = new template($template_path ."/cp-settings.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-SETTINGS" => "$cp[settings]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-BOARDNAME" => "$cp[boardname]",
  "BOARDNAME" => "$settings[Site_Title]",
  "LANG-SITEADDRESS" => "$cp[sitelink]",
  "LANG-SITEADDESSTXT" => "$cp[sitelink_txt]",
  "SITEADDRESS" => "$settings[Site_Address]",
  "LANG-BOARDSTATUS" => "$cp[boardstat]",
  "BOARDSTATUS" => "$board_status",
  "LANG-OFFMSG" => "$cp[boardoffmsg]",
  "OFFMSG" => "$settings[Off_Message]",
  "LANG-BOARDADDRESS" => "$cp[boardlink]",
  "LANG-BOARDADDRESSTXT" => "$cp[boardlink_txt]",
  "BOARDADDRESS" => "$settings[Board_Address]",
  "LANG-BOARDEMAIL" => "$cp[boardemail]",
  "BOARDEMAIL" => "$settings[Board_Email]",
  "LANG-ANNOUNCEMENTSTATUS" => "$cp[announcestat]",
  "ANNOUNCEMENTSTATUS" => "$announce_status",
  "LANG-ANNOUNCEMENT" => "$cp[announce]",
  "ANNOUNCEMENT" => "$settings[Announcements]",
  "LANG-ANNOUNCERULE" => "$cp[onelineannounce]",
  "LANG-DEFAULTSTYLE" => "$cp[defaultstyle]",
  "DEFAULTSTYLE" => "$style_select",
  "LANG-DEFAULTLANGUAGE" => "$cp[defaultlang]",
  "DEFAULTLANGUAGE" => "$lang",
  "LANG-TOSSTAT" => "$cp[tosstat]",
  "TOSSTAT" => "$tos_stat",
  "LANG-TOS" => "$cp[tos]",
  "TOS" => "$settings[TOS_Rules]",
  "LANG-GZIP" => "$cp[gzip]",
  "LANG-GZIPREQ" => "$cp[gzipreq]",
  "GZIP" => "$gzip_status",
  "LANG-IMGVERT" => "$cp[securityimage]",
  "LANG-GDREQ" => "$cp[gdreq]",
  "IMGVERT" => "$imagevert_status",
  "LANG-SPELLCHECKER" => "$cp[spellchecker]",
  "LANG-PSPELL" => "$cp[pspell]",
  "SPELLCHECKER" => "$spell_status",
  "LANG-PMQUOTA" => "$cp[pmquota]",
  "PMQUOTA" => "$settings[PM_Quota]",
  "LANG-ACTIVE-TYPE" => "$cp[activation]",
  "ACTIVE-TYPE" => "$activate_status",
  "LANG-TIMEZONE" => "$cp[defaulttimezone]",
  "TIMEZONE" => "$timezone",
  "LANG-TIMEFORMAT" => "$cp[defaultimtformat]",
  "LANG-TIMERULE" => "$cp[timeformat]",
  "TIMEFORMAT" => "$settings[Default_Time]",
  "LANG-COOKIEOPTIONS" => "$cp[cookie_options]",
  "LANG-COOKIEDOMAIN" => "$cp[cookiedomain]",
  "LANG-COOKIEDOMAINTXT" => "$cp[cookiedomain_txt]",
  "COOKIEDOMAIN" => "$settings[cookie_domain]",
  "LANG-COOKIEPATH" => "$cp[cookiepath]",
  "LANG-COOKIEPATHTXT" => "$cp[cookiepath_txt]",
  "COOKIEPATH" => "$settings[cookie_path]",
  "LANG-COOKIESECURE" => "$cp[cookiesecure]",
  "LANG-SSL" => "$cp[ssl]",
  "COOKIESECURE" => "$secure_status",
  "LANG-USEROPTIONS" => "$cp[access_options]",
  "LANG-PMACCESS" => "$cp[pm_access]",
  "PMACCESS" => "$settings[PM_Access]",
  "LANG-PROFILEVIEW" => "$cp[profile_view]",
  "PROFILEVIEW" => "$settings[Profile_View]",
  "LANG-GROUPMEMBERSHIP" => "$cp[group_membership]",
  "GROUPMEMBERSHIP" => "$settings[Group_Membership]",
  "LANG-AVATARUSAGE" => "$cp[avatar_usage]",
  "AVATARUSAGE" => "$settings[Avatar_Usage]",
  "LANG-MAILOPTIONS" => "$cp[mailsettings]",
  "LANG-MAILTYPE" => "$cp[mailtype]",
  "MAILTYPE" => "$mail_status",
  "LANG-HOST" => "$cp[smtphost]",
  "HOST" => "$settings[smtp_server]",
  "LANG-PORT" => "$cp[smtpport]",
  "PORT" => "$settings[smtp_port]",
  "LANG-USERNAME" => "$cp[smtpuser]",
  "USERNAME" => "$settings[smtp_user]",
  "LANG-PASSWORD" => "$cp[smtppass]",
  "PASSWORD" => "$settings[smtp_pass]",
  "LANG-SAVESETTINGS" => "$cp[savesettings]"));

$page->output();
break;
case 'settings_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
//define form results.
$board_name = anti_injection($_POST['board_name']);
$site_address = anti_injection($_POST['site_address']);
$board_stat = anti_injection($_POST['board_stat']);
$off_msg = anti_injection($_POST['off_msg']);
$board_address = anti_injection($_POST['board_address']);
$board_email = anti_injection($_POST['board_email']);
$announce_stat = anti_injection($_POST['announce_stat']);
$announce_msg = anti_injection($_POST['announce_msg']);
$dstyle = anti_injection($_POST['style']);
$default_lang = anti_injection($_POST['default_lang']);
$term_stat = anti_injection($_POST['term_stat']);
$term_msg = anti_injection($_POST['term_msg']);
$gzip_stat = anti_injection($_POST['gzip_stat']);
$imagevert_stat = anti_injection($_POST['imagevert_stat']);
$spell_stat = anti_injection($_POST['spell_stat']);
$pm_quota = anti_injection($_POST['pm_quota']);
$active_stat = anti_injection($_POST['active_stat']);
$default_zone = anti_injection($_POST['time_zone']);
$default_time = anti_injection($_POST['default_time']);
$cookie_domain = anti_injection($_POST['cookie_domain']);
$cookie_path = anti_injection($_POST['cookie_path']);
$secure_stat = anti_injection($_POST['secure_stat']);
$pm_access = anti_injection($_POST['pm_access']);
$profile_view = anti_injection($_POST['profile_view']);
$group_membership = anti_injection($_POST['group_membership']);
$avatar_usage = anti_injection($_POST['avatar_usage']);
$mail_type = anti_injection($_POST['mail_type']);
$smtp_host = anti_injection($_POST['smtp_host']);
$smtp_port = anti_injection($_POST['smtp_port']);
$smtp_user = anti_injection($_POST['smtp_user']);
$smtp_pass = anti_injection($_POST['smtp_pass']);
//error checking.
if ($board_name == ""){
$error = $cp['noboardname'];
echo error($error, "error");
}
if ($site_address == ""){
$error = $cp['nositeaddress'];
echo error($error, "error");
}
if (($board_stat == 0) AND ($off_msg == "")){
$error = $cp['noclosemsg'];
echo error($error, "error");
}
if ($board_address == ""){
$error = $cp['noboardaddress'];
echo error($error, "error");
}
if ($board_email == ""){
$error = $cp['noemail'];
echo error($error, "error");
}
if (($announce_stat == 1) AND ($announce_msg == "")){
$error = $cp['noannounce'];
echo error($error, "error");
}
if ($dstyle == ""){
$error = $cp['nostyle'];
echo error($error, "error");
}
if ($default_lang == ""){
$error = $cp['nolang'];
echo error($error, "error");
}
if (($term_stat == 1) AND ($term_msg == "")){
$error = $cp['notos'];
echo error($error, "error");
}
if ($gzip_stat == ""){
$error = $cp['nogzip'];
echo error($error, "error");
}
if ($imagevert_stat == ""){
$error = $cp['noimgagevert'];
echo error($error, "error");
}
if ($spell_stat == ""){
$error = $cp['nospellchecker'];
echo error($error, "error");
}
if (($pm_quota == "") OR (!is_numeric($pm_quota))){
$error = $cp['nopmquota'];
echo error($error, "error");
}
if ($default_zone == ""){
$error = $cp['notimezone'];
echo error($error, "error");
}
if ($default_time == ""){
$error = $cp['notimeformat'];
echo error($error, "error");
}
if($cookie_domain == ""){
$error = $cp['nocookiedomain'];
echo error($error, "error");
}
if ($cookie_path == ""){
$error = $cp['nocookiepath'];
echo error($error, "error");
}
if ($secure_stat == ""){
$error = $cp['nocookiesecure'];
echo error($error, "error");
}
if(strlen($board_name) > 50){
$error = $cp['longboardname'];
echo error($error, "error");
}
if(strlen($site_address) > 50){
$error = $cp['longsiteaddress'];
echo error($error, "error");
}
if(strlen($board_address) > 50){
$error = $cp['longboardaddress'];
echo error($error, "error");
}
if(strlen($board_email) > 255){
$error = $cp['longboardemail'];
echo error($error, "error");
}
if(strlen($off_msg) > 255){
$error = $cp['longoffmsg'];
echo error($error, "error");
}
if(strlen($announce_msg) > 255){
$error = $cp['longannouce'];
echo error($error, "error");
}
if(strlen($pm_quota) > 4){
$error = $cp['longpmquota'];
echo error($error, "error");
}
if(strlen($cookie_domain) > 50){
$error = $cp['longcookiedomain'];
echo error($error, "error");
}
if(strlen($cookie_path) > 50){
$error = $cp['longcookiepath'];
echo error($error, "error");
}
if($pm_access == ""){
$error = $cp['nopmrule'];
echo error($error, "error");
}
if($profile_view == ""){
$error = $cp['noprofileaccess'];
echo error($error, "error");
}
if($group_membership == ""){
$error = $cp['nogrouprule'];
echo error($error, "error");
}
if($avatar_usage == ""){
$error = $cp['noavatarrule'];
echo error($error, "error");
}
if(strlen($pm_access) > 4){
$error = $cp['longpmrule'];
echo error($error, "error");
}
if(strlen($profile_view) > 4){
$error = $cp['longprofileview'];
echo error($error, "error");
}
if(strlen($group_membership) > 4){
$error = $cp['longgroupmembership'];
echo error($error, "error");
}
if(strlen($avatar_usage) > 4){
$error = $cp['longavatar'];
echo error($error, "error");
}
if($mail_type == ""){
$error = $cp['nomailrule'];
echo error($error, "error");
}
#check for smtp values.
if($mail_type == 0){
if($smtp_host == ""){
$error = $cp['nosmtphost'];
echo error($error, "error");
}
if($smtp_port == ""){
$error = $cp['nosmtpport'];
echo error($error, "error");
}
if($smtp_user == ""){
$error = $cp['nosmtpuser'];
echo error($error, "error");
}
if($smtp_pass == ""){
$error = $cp['nosmtppass'];
echo error($error, "error");
}
if(strlen($smtp_host) > 255){
$error = $cp['longsmtphost'];
echo error($error, "error");
}
if(strlen($smtp_port) > 5){
$error = $cp['longsmtpport'];
echo error($error, "error");
}
if(strlen($smtp_user) > 255){
$error = $cp['longsmtpuser'];
echo error($error, "error");
}
if(strlen($smtp_pass) > 255){
$error = $cp['longsmtppass'];
echo error($error, "error");
}
}
//process query
$db->run = "UPDATE ebb_settings SET Site_Title='$board_name', Site_Address='$site_address', Board_Status='$board_stat', Board_Address='$board_address', Board_Email='$board_email', Off_Message='$off_msg', Announcement_Status='$announce_stat', Announcements='$announce_msg', Default_Style='$dstyle', Default_Language='$default_lang', TOS_Status='$term_stat', TOS_Rules='$term_msg', GZIP='$gzip_stat', Image_Verify='$imagevert_stat', spell_checker='$spell_stat', PM_Quota='$pm_quota', activation='$active_stat', Default_Zone='$default_zone', Default_Time='$default_time', cookie_domain='$cookie_domain', cookie_path='$cookie_path', cookie_secure='$secure_stat', PM_Access='$pm_access', Profile_View='$profile_view', Group_Membership='$group_membership', Avatar_Usage='$avatar_usage', mail_type='$mail_type', smtp_server='$smtp_host', smtp_port='$smtp_port', smtp_user='$smtp_user', smtp_pass='$smtp_pass'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=settings");
break;
case 'user_manage':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$page = new template($template_path ."/cp-usermanage.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-SELECTUSER" => "$cp[seluser]",
  "LANG-TEXT" => "$cp[usertxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-USERNAME" => "$txt[username]",));

$page->output();
break;
case 'user_manage2':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$user = anti_injection($_POST['username']);
if($user == ""){
header("Location: CP.php?action=user_manage");
}
$db->run = "SELECT * FROM ebb_users where Username='$user'";
$profile_result = $db->result();
$user_chk = $db->num_results();
$db->close();
if($user_chk == 0){
$error = $userinfo['usernotexist'];
echo error($error, "error");
}else{
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
if($profile_result['Status'] == "Banned"){
$banuser_status = "<input type=\"checkbox\" name=\"banuser\" class=\"text\" value=\"yes\" checked>$cp[tickban]";
}else{
$banuser_status = "<input type=\"checkbox\" name=\"banuser\" class=\"text\" value=\"yes\">$cp[tickban]";
}
if($profile_result['active'] == 1){
$activeuser_status = "<input type=\"radio\" name=\"active_user\" value=\"1\" class=\"text\" checked>$txt[yes] <input type=\"radio\" name=\"active_user\" value=\"0\" class=\"text\">$txt[no]";
}else{
$activeuser_status = "<input type=\"radio\" name=\"active_user\" value=\"1\" class=\"text\">$txt[yes] <input type=\"radio\" name=\"active_user\" value=\"0\" class=\"text\" checked>$txt[no]";
}
$timezone = timezone_select();
$style = style_select();
$language = lang_select();
$page = new template($template_path ."/cp-usermanage2.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MANAGEUSER" => "$cp[manageuser]",
  "LANG-TEXT" => "$cp[usertext]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-EMAIL" => "$form[email]",
  "EMAIL" => "$profile_result[Email]",
  "LANG-USERNAME" => "$txt[username]",
  "USERNAME" => "$profile_result[Username]",
  "LANG-TIME" => "$form[timezone]",
  "TIME" => "$timezone",
  "LANG-TIMEFORMAT" => "$form[timeformat]",
  "LANG-TIMEINFO" => "$form[timeinfo]",
  "TIMEFORMAT" => "$time_format",
  "LANG-PMNOTIFY" => "$form[pm_notify]",
  "PMNOTIFY" => "$pmnotice_status",
  "LANG-SHOWEMAIL" => "$form[showemail]",
  "SHOWEMAIL" => "$showemail_status",
  "LANG-STYLE" => "$form[style]",
  "STYLE" => "$style",
  "LANG-LANGUAGE" => "$form[defaultlang]",
  "LANGUAGE" => "$language",
  "LANG-OPTIONAL" => "$form[optional]",
  "LANG-MSN" => "$form[msn]",
  "MSN" => "$profile_result[MSN]",
  "LANG-AOL" => "$form[aol]",
  "AOL" => "$profile_result[AOL]",
  "LANG-YIM" => "$form[yim]",
  "YIM" => "$profile_result[Yahoo]",
  "LANG-ICQ" => "$form[icq]",
  "ICQ" => "$profile_result[ICQ]",
  "LANG-SIG" => "$form[sig]",
  "SIG" => "$profile_result[Sig]",
  "LANG-WWW" => "$form[www]",
  "WWW" => "$profile_result[WWW]",
  "LANG-LOCATION" => "$form[location]",
  "LOCATION" => "$profile_result[Location]",
  "LANG-ADMINTOOLS" => "$cp[admintools]",
  "LANG-ACTIVEUSER" => "$cp[activeuser]",
  "ACTIVEUSER" => "$activeuser_status",
  "LANG-BANUSER" => "$cp[banuser]",
  "BANUSER" => "$banuser_status",
  "LANG-DELUSER" => "$cp[deluser]",
  "LANG-TICKDELETE" => "$cp[tickdel]",
  "SUBMIT" => "$cp[submit]"));
$page->output();
}
break;
case 'user_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
//get form details.
$username = anti_injection($_POST['user']);
$email = anti_injection($_POST['email']);
$time_zone = anti_injection($_POST['time_zone']);
$time_format = anti_injection($_POST['time_format']);
$pm_notice = anti_injection($_POST['pm_notice']);
$show_email = anti_injection($_POST['show_email']);
$banuser = anti_injection($_POST['banuser']);
$style = anti_injection($_POST['style']);
$default_lang = anti_injection($_POST['default_lang']);
$msn_messenger = anti_injection($_POST['msn_messenger']);
$aol_messenger = anti_injection($_POST['aol_messenger']);
$yim = anti_injection($_POST['yim']);
$icq = anti_injection($_POST['icq']);
$location = anti_injection($_POST['location']);
$sig = anti_injection($_POST['sig']);
$site = anti_injection($_POST['site']);
$active_user = anti_injection($_POST['active_user']);
$deluser = anti_injection($_POST['deluser']);
//do some error checking.
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
if ($email == ""){
$error = $reg['noemail'];
echo error($error, "error");
}
if(strlen($email) > 50){
$error = $reg['longemail'];
echo error($error, "error");
}
//see if admin wants to ban user.
if($banuser == "yes"){
$db->run = "update ebb_users set Status='Banned' where Username='$username'";
$db->query();
$db->close();
//remove any group access if the user belongs to any.
$db->run = "delete from ebb_group_users where Username='$username'";
$db->query();
$db->close();
}
//see if admin wants to delete user.
if($deluser == "yes"){
$db->run = "delete from ebb_users where Username='$username'";
$db->query();
$db->close();
//delete topics made by user.
$db->run = "delete from ebb_posts where re_author='$username'";
$db->query();
$db->close();
//get topic details to delete replies,then delete the topics.
$sql = "SELECT * FROM ebb_topics WHERE author='$username'";
$errorq = $sql;
$board_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while($row = mysql_fetch_assoc($sql)){
$db->run = "delete from ebb_posts where tid='$row[tid]'";
$db->query();
$db->close();
}
$db->run = "delete from ebb_topics where author='$username'";
$db->query();
$db->close();
//delete any subscriptions to topics this user belongs to..
$db->run = "DELETE FROM ebb_topic_watch WHERE username='$username'";
$db->query();
$db->close();
}
//update user info.
$db->run = "update ebb_users set Email='$email', MSN='$msn_messenger', AOL='$aol_messenger', Yahoo='$yim', ICQ='$icq', Location='$location', Sig='$sig', WWW='$site', Time_format='$time_format', Time_Zone='$time_zone', PM_Notify='$pm_notice', Hide_Email='$show_email', Style='$style', Language='$default_lang', active='$active_user' where Username='$username'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=user_manage");
break;
case 'activate':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
//get list of inactive users.
$db->run = "SELECT * FROM ebb_users WHERE active='0'";
$inactive_q = $db->query();
$user_ct = $db->num_results();
$db->close();
//output the html.
$inactive_list = inactive_users();
$page = new template($template_path ."/cp-activateusers.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ACTIVATEUSER" => "$cp[activateacct]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-STYLENAME" => "$cp[stylename]",
  "LANG-USERNAME" => "$txt[username]",
  "LANG-JOINDATE" => "$members[joindate]",
  "USERLIST" => "$inactive_list"));

$page->output();
break;
case 'activate_user';
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$stat = anti_injection($_GET['stat']);
$id = anti_injection($_GET['id']);
//load email language file.
require "lang/".$lang.".email.php";
if($stat == "accept"){
//check for correct user id.
$db->run = "select * from ebb_users where id='$id'";
$acct_chk = $db->num_results();
$db->close();
if($acct_chk == 1){
//set user as active.
$db->run = "update ebb_users set active='1' where id='$id'";
$db->query();
$db->close();
echo $cp['useractivated'];
//send out email to user.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $acct_pass_subject;
//get body of newsletter.
$mailer->Body = $acct_pass_msg;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $acct_pass_subject, $acct_pass_msg, $headers);
}
}else{
$error = $cp['useractivateerror'];
echo error($error, "error");
}
}else{
//check for correct user id.
$db->run = "select * from ebb_users where id='$id'";
$acct_chk = $db->num_results();
$db->close();
if($acct_chk == 1){
//set user as active.
$db->run = "delete from ebb_users where id='$id'";
$db->query();
$db->close();
echo $cp['userdeny'];
//send out email to user.
if($mail_rule == 0){
//call up the smtp class.
$mailer = new ebbmail();
//setup the subject of this newsletter.
$mailer->Subject = $acct_fail_subject;
//get body of newsletter.
$mailer->Body = $acct_fail_msg;
//add users to the email list
$mailer->AddBCC($email);
//send out the email.
$mailer->Send();
//clear the list to prevent any double emails.
$mailer->ClearAllRecipients();
}else{
//create a From: mailheader
$headers = "From: $title <$board_email>";
@mail($email, $acct_fail_subject, $acct_fail_msg, $headers);
}
}else{
$error = $cp['useractivateerror'];
echo error($error, "error");
}
}
break;
case 'style':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$admin_style = admin_stylelisting();
$page = new template($template_path ."/cp-style.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-STYLES" => "$cp[managestyle]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-STYLENAME" => "$cp[stylename]",
  "STYLES" => "$admin_style"));

$page->output();
break;
case 'add_style':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$page = new template($template_path ."/cp-newstyle.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-ADDSTYLE" => "$cp[addstyle]",
  "LANG-TEXT" => "$cp[addstyletxt]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-STYLENAME" => "$cp[stylename]",
  "LANG-BACKGROUND" => "$cp[background]",
  "LANG-BGCOLORRULE" => "$cp[bgcolorrule]",
  "LANG-TEXTCOLOR" => "$cp[txtcolor]",
  "LANG-LINKCOLOR" => "$cp[linkcolor]",
  "LANG-VLINKCOLOR" => "$cp[vlinkcolor]",
  "LANG-TABLE1COLOR" => "$cp[tab1]",
  "LANG-TABLE2COLOR" => "$cp[tab2]",
  "LANG-FONT" => "$cp[font]",
  "LANG-BORDERCOLOR" => "$cp[border]",
  "LANG-IMAGEPATH" => "$cp[imgpath]",
  "LANG-IMAGERULE" => "$cp[imgrule]",
  "LANG-TEMPLATEPATH" => "$cp[temppath]",
  "LANG-TEMPLATERULE" => "$cp[temprule]"));

$page->output();
break;
case 'style_add_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$style_name = anti_injection($_POST['style_name']);
$bgcolor = anti_injection($_POST['background']);
$txt_color = anti_injection($_POST['text_color']);
$link_color = anti_injection($_POST['link_color']);
$vlink_color = anti_injection($_POST['vlink_color']);
$table_1 = anti_injection($_POST['table_1']);
$table_2 = anti_injection($_POST['table_2']);
$font_face = anti_injection($_POST['font_face']);
$border_color = anti_injection($_POST['border_color']);
$image_path = anti_injection($_POST['image_path']);
$template_path = anti_injection($_POST['template_path']);
//error check
if ($style_name == ""){
$error = $cp['nostylename'];
echo error($error, "error");
}
if ($bgcolor == ""){
$error = $cp['nobgcolor'];
echo error($error, "error");
}
if ($txt_color == ""){
$error = $cp['notxtcolor'];
echo error($error, "error");
}
if ($link_color == ""){
$error = $cp['nolinkcolor'];
echo error($error, "error");
}
if ($vlink_color == ""){
$error = $cp['novlinkcolor'];
echo error($error, "error");
}
if ($table_1 == ""){
$error = $cp['notab1color'];
echo error($error, "error");
}
if ($table_2 == ""){
$error = $cp['notab2color'];
echo error($error, "error");
}
if ($font_face == ""){
$error = $cp['nofont'];
echo error($error, "error");
}
if ($border_color == ""){
$error = $cp['nobordercolor'];
echo error($error, "error");
}
if ($image_path == ""){
$error = $cp['noimgpath'];
echo error($error, "error");
}
if ($template_path == ""){
$error = $cp['notemppath'];
echo error($error, "error");
}
if(strlen($style_name) > 50){
$error = $cp['longstylename'];
echo error($error, "error");
}
if(strlen($bgcolor) > 7){
$error = $cp['longbgcolor'];
echo error($error, "error");
}
if(strlen($txt_color) > 7){
$error = $cp['longtxtcolor'];
echo error($error, "error");
}
if(strlen($link_color) > 7){
$error = $cp['longlinkcolor'];
echo error($error, "error");
}
if(strlen($vlink_color) > 7){
$error = $cp['longvlinkcolor'];
echo error($error, "error");
}
if(strlen($table_1) > 7){
$error = $cp['longtable1color'];
echo error($error, "error");
}
if(strlen($table_2) > 7){
$error = $cp['longtable2color'];
echo error($error, "error");
}
if(strlen($border_color) > 7){
$error = $cp['longbordercolor'];
echo error($error, "error");
}
if(strlen($image_path) > 80){
$error = $cp['longimgpath'];
echo error($error, "error");
}
if(strlen($template_path) > 80){
$error = $cp['longtemppath'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_style (Name, Background, Text, Link, VLink, Table1, Table2, Font, Border, Images_Path, Temp_Path) values ('$style_name', '$bgcolor', '$txt_color', '$link_color', '$vlink_color', '$table_1', '$table_2', '$font_face', '$border', '$image_path ', '$template_path')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=style");
break;
case 'style_edit':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$db->run = "SELECT * FROM ebb_style WHERE id='$id'";
$modify_style = $db->result();
$db->close();
$page = new template($template_path ."/cp-modifystyle.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-MODIFYSTYLE" => "$cp[updatestyle]",
  "LANG-TEXT" => "$cp[addstyletxt]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-STYLENAME" => "$cp[stylename]",
  "STYLENAME" => "$modify_style[Name]",
  "LANG-BACKGROUND" => "$cp[background]",
  "LANG-BGCOLORRULE" => "$cp[bgcolorrule]",
  "BACKGROUND" => "$modify_style[Background]",
  "LANG-TEXTCOLOR" => "$cp[txtcolor]",
  "TEXTCOLOR" => "$modify_style[Text]",
  "LANG-LINKCOLOR" => "$cp[linkcolor]",
  "LINKCOLOR" => "$modify_style[Link]",
  "LANG-VLINKCOLOR" => "$cp[vlinkcolor]",
  "VLINKCOLOR" => "$modify_style[VLink]",
  "LANG-TABLE1COLOR" => "$cp[tab1]",
  "TABLE1COLOR" => "$modify_style[Table1]",
  "LANG-TABLE2COLOR" => "$cp[tab2]",
  "TABLE2COLOR" => "$modify_style[Table2]",
  "LANG-FONT" => "$cp[font]",
  "FONT" => "$modify_style[Font]",
  "LANG-BORDERCOLOR" => "$cp[border]",
  "BORDERCOLOR" => "$modify_style[Border]",
  "LANG-IMAGEPATH" => "$cp[imgpath]",
  "LANG-IMAGERULE" => "$cp[imgrule]",
  "IMAGESPATH" => "$modify_style[Images_Path]",
  "LANG-TEMPLATEPATH" => "$cp[temppath]",
  "LANG-TEMPLATERULE" => "$cp[temprule]",
  "TEMPLATEPATH" => "$modify_style[Temp_Path]"));

$page->output();
break;
case 'style_modify':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$style_name = anti_injection($_POST['style_name']);
$bgcolor = anti_injection($_POST['background']);
$txt_color = anti_injection($_POST['text_color']);
$link_color = anti_injection($_POST['link_color']);
$vlink_color = anti_injection($_POST['vlink_color']);
$table_1 = anti_injection($_POST['table_1']);
$table_2 = anti_injection($_POST['table_2']);
$font_face = anti_injection($_POST['font_face']);
$border_color = anti_injection($_POST['border_color']);
$image_path = anti_injection($_POST['image_path']);
$template_path = anti_injection($_POST['template_path']);
//error check
if ($style_name == ""){
$error = $cp['nostylename'];
echo error($error, "error");
}
if ($bgcolor == ""){
$error = $cp['nobgcolor'];
echo error($error, "error");
}
if ($txt_color == ""){
$error = $cp['notxtcolor'];
echo error($error, "error");
}
if ($link_color == ""){
$error = $cp['nolinkcolor'];
echo error($error, "error");
}
if ($vlink_color == ""){
$error = $cp['novlinkcolor'];
echo error($error, "error");
}
if ($table_1 == ""){
$error = $cp['notab1color'];
echo error($error, "error");
}
if ($table_2 == ""){
$error = $cp['notab2color'];
echo error($error, "error");
}
if ($font_face == ""){
$error = $cp['nofont'];
echo error($error, "error");
}
if ($border_color == ""){
$error = $cp['nobordercolor'];
echo error($error, "error");
}
if ($image_path == ""){
$error = $cp['noimgpath'];
echo error($error, "error");
}
if ($template_path == ""){
$error = $cp['notemppath'];
echo error($error, "error");
}
if(strlen($style_name) > 50){
$error = $cp['longstylename'];
echo error($error, "error");
}
if(strlen($bgcolor) > 7){
$error = $cp['longbgcolor'];
echo error($error, "error");
}
if(strlen($txt_color) > 7){
$error = $cp['longtxtcolor'];
echo error($error, "error");
}
if(strlen($link_color) > 7){
$error = $cp['longlinkcolor'];
echo error($error, "error");
}
if(strlen($vlink_color) > 7){
$error = $cp['longvlinkcolor'];
echo error($error, "error");
}
if(strlen($table_1) > 7){
$error = $cp['longtable1color'];
echo error($error, "error");
}
if(strlen($table_2) > 7){
$error = $cp['longtable2color'];
echo error($error, "error");
}
if(strlen($border_color) > 7){
$error = $cp['longbordercolor'];
echo error($error, "error");
}
if(strlen($image_path) > 80){
$error = $cp['longimgpath'];
echo error($error, "error");
}
if(strlen($template_path) > 80){
$error = $cp['longtemppath'];
echo error($error, "error");
}
//process query
$db->run = "Update ebb_style SET Name='$style_name', Background='$bgcolor', Text='$txt_color', Link='$link_color', VLink='$vlink_color', Table1='$table_1', Table2='$table_2', Font='$font_face', Border='$border_color', Images_Path='$image_path', Temp_Path='$template_path' WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=style");
break;
case 'style_delete':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
$page = new template($template_path ."/cp-deletestyle.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-DELETESTYLE" => "$cp[delstyle]",
  "LANG-DELETEWARNING" => "$cp[delstylewarning]",
  "ID" => "$id",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-DELSURE" => "$cp[delsure]",
  "LANG-YES" => "$txt[yes]",
  "LANG-NO" => "$txt[no]"));

$page->output();
break;
case 'delete_style_process':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
$id = anti_injection($_GET['id']);
//process query
$db->run = "DELETE FROM ebb_style WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=style");
break;
case 'banlist':
$admin_banlist_ip = admin_banlist_ip();
$admin_banlist_email = admin_banlist_email();
$page = new template($template_path ."/cp-banlist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-BANLIST" => "$cp[banlist]",
  "LANG-TEXT" => "$cp[banlisttext]",
  "LANG-EMAILBAN" => "$cp[emailban]",
  "LANG-BANEMAILTXT" => "$cp[emailbantxt]",
  "LANG-MATCHTYPETXT" => "$cp[matchtypetxt]",
  "LANG-EXACTMATCH" => "$cp[exactmatch]",
  "LANG-WILDCARDMATCH" => "$cp[wildcardmatch]",
  "LANG-UNBANEMAIL" => "$cp[emailunban]",
  "LANG-UNBANEMAILTXT" => "$cp[emailunbantxt]",
  "BANLIST-EMAIL" => "$admin_banlist_email",
  "LANG-BANIP" => "$cp[ipban]",
  "LANG-BANIPTXT" => "$cp[ipbantxt]",
  "LANG-UNBANIP" => "$cp[ipunban]",
  "LANG-UNBANIPTXT" => "$cp[ipunbantxt]",
  "BANLIST-IP" => "$admin_banlist_ip",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SUBMIT" => "$cp[submit]"));

$page->output();
break;
case 'ban_add':
//form values.
$emailbanning = anti_injection($_POST['emailbanning']);
$ipbanning = anti_injection($_POST['ipbanning']);
$match_type = anti_injection($_POST['match_type']);
$type = anti_injection($_POST['type']);
//error checking.
if($match_type == ""){
$error = $cp['nomatchtype'];
echo error($error, "error");
}
//if emailbanning is blank.
if(($emailbanning == "") AND ($type="Email")){
$error = $cp['noemailban'];
echo error($error, "error");
}
if (($ipbanning == "") AND ($type == "IP")){
$error = $cp['noip'];
echo error($error, "error");
}
if(strlen($emailbanning) > 255){
$error = $cp['longemailban'];
echo error($error, "error");
}
if(strlen($ipbanning) > 15){
$error = $cp['longipban'];
echo error($error, "error");
}
if($type == ""){
header("Location: CP.php?action=banlist");
}
//process query
if($type == "IP"){
$db->run = "insert into ebb_banlist (ban_item, ban_type, match_type) values('$ipbanning', 'IP', 'Exact')";
$db->query();
$db->close();
}
if($type == "Email"){
$db->run = "insert into ebb_banlist (ban_item, ban_type, match_type) values('$emailbanning', 'Email', '$match_type')";
$db->query();
$db->close();
}
//bring user back
header("Location: CP.php?action=banlist");
break;
case 'ban_remove':
//get form values
$ipsel = anti_injection($_POST['ipsel']);
$emailsel = anti_injection($_POST['emailsel']);
$type = anti_injection($_POST['type']);
//error check.
if($type == ""){
header("Location: CP.php?action=banlist");
}
if(($ipsel == "") and ($type == "IP") or ($emailsel == "") and ($type == "Email")){
$error = $cp['noselectban'];
echo error($error, "error");
}
//process query
if($type == "IP"){
$db->run = "DELETE FROM ebb_banlist WHERE id='$ipsel'";
$db->query();
$db->close();
}
if($type == "Email"){
$db->run = "DELETE FROM ebb_banlist WHERE id='$emailsel'";
$db->query();
$db->close();
}
//bring user back
header("Location: CP.php?action=banlist");
break;
case 'blacklistuser':
$username_blacklist = admin_blacklist();
$page = new template($template_path ."/cp-blacklist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-BLACKLISTEDUSERS" => "$cp[blacklist]",
  "LANG-TEXT" => "$cp[usernameblacklisttxt]",
  "LANG-BLACKEDUSERNAME" => "$cp[blacklistusername]",
  "LANG-USERNAMETOBAN" => "$cp[blackedusername]",
  "LANG-BLACKLISTTYPE" => "$cp[blacklisttype]",
  "LANG-EXACTMATCH" => "$cp[exactmatch]",
  "LANG-WILDCARDMATCH" => "$cp[wildcardmatch]",
  "LANG-UNBLACKLISTUSER" => "$cp[whitelistingusername]",
  "LANG-UNBLACKLISTUSERTXT" => "$cp[whitelistingusernametxt]",
  "BLACKLIST" => "$username_blacklist",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-SUBMIT" => "$cp[submit]"));

$page->output();
break;
case 'blacklist_add':
//get form data.
$blacklistuser = anti_injection($_POST['blacklistuser']);
$match_type = anti_injection($_POST['match_type']);
//error checking.
if($blacklistuser == ""){
$error = $cp['nousernameentered'];
echo error($error, "error");
}
if($match_type == ""){
$error = $cp['nomatchtype'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_blacklist (blacklisted_username, match_type) values('$blacklistuser', '$match_type')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=blacklistuser");
break;
case 'blacklist_remove':
//get form data.
$blkusersel = anti_injection($_POST['blkusersel']);
if($blkusersel == ""){
$error = $cp['nousernameselected'];
echo error($error, "error");
}
//process query
$db->run = "delete from ebb_blacklist where id='$blkusersel'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=blacklistuser");
break;
case 'censor':
$admin_censor = admin_censorlist();
$page = new template($template_path ."/cp-censorlist.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-CENSORLIST" => "$cp[censor]",
  "LANG-ORIGINALWORD" => "$cp[originalword]",
  "CENSORLIST" => "$admin_censor",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-ADDCENSOR" => "$cp[addcensor]",
  "LANG-SUBMIT" => "$cp[submit]"));

$page->output();
break;
case 'censor_add';
$addcensor = anti_injection($_POST['addcensor']);
if ($addcensor = ""){
$error = $cp['nocensor'];
echo error($error, "error");
}
if(strlen($addcensor) > 50){
$error = $cp['longcensor'];
echo error($error, "error");
}
//process query
$db->run = "insert into ebb_censor (Original_Word) values('$addcensor')";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=censor");
break;
case 'censor_modify':
$id = anti_injection($_GET['id']);
//process query
$db->run = "DELETE FROM ebb_censor WHERE id='$id'";
$db->query();
$db->close();
//bring user back
header("Location: CP.php?action=censor");
break;
case 'prune':
$board_select = prune_boardlist();
$page = new template($template_path ."/cp-prune.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-PRUNE" => "$cp[prune]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-TEXT" => "$cp[prunetxt]",
  "LANG-PRUNERULE" => "$cp[prunerule]",
  "LANG-BOARDLIST" => "$cp[pruneboard]",
  "BOARDLIST" => "$board_select",
  "LANG-SUBMIT" => "$cp[pruneboards]"));

$page->output();
break;
case 'prune_process':
$prune_age = anti_injection($_POST['prune_age']);
$boardsel = anti_injection($_POST['boardsel']);
//error check
if($prune_age == ""){
$error = $cp['noprunedate'];
echo error($error, "error");
}
if($boardsel == ""){
$error = $cp['noboardselect'];
echo error($error, "error");
}
//perform prune.
$time_math = 3600*24*$prune_age;
$remove_eq = time() - $time_math;
//process query
$sql = "select * from ebb_topics WHERE Original_Date>='$remove_eq' and bid='$boardsel'";
$errorq = $sql;
$prune_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while($r = mysql_fetch_assoc($prune_query)){
//process query
$db->run = "DELETE FROM ebb_posts WHERE Original_Date>='$r[tid]' and bid='$boardsel'";
$db->query();
$db->close();
}
//process query
$db->run = "DELETE FROM ebb_topics WHERE Original_Date>='$remove_eq' and bid='$boardsel'";
$db->query();
$db->close();
//bring user back.
header("Location: CP.php?action=prune");
break;
case 'user_prune':
$page = new template($template_path ."/cp-userprune.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "LANG-USERPRUNE" => "$cp[userprune]",
  "LANG-TEXT" => "$cp[userprunetext]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-PRUNEWARNING" => "$cp[userprunewarning]",
  "LANG-BEGINPRUNE" => "$cp[beginuserprune]"));

$page->output();
break;
case 'process_user_pruning':
$date_math = 3600*24*7;
$time_eq = time() - $date_math;
//process query
$db->run = "DELETE FROM ebb_posts WHERE Original_Date>='$remove_eq'";
$db->query();
$db->close();
//bring user back.
header("Location: CP.php");
break;
case 'analyze':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
mysql_query("ANALYZE TABLE ebb_banlist");
mysql_query("ANALYZE TABLE ebb_blacklist");
mysql_query("ANALYZE TABLE ebb_boards");
mysql_query("ANALYZE TABLE ebb_category");
mysql_query("ANALYZE TABLE ebb_censor");
mysql_query("ANALYZE TABLE ebb_group_users");
mysql_query("ANALYZE TABLE ebb_grouplist");
mysql_query("ANALYZE TABLE ebb_groups");
mysql_query("ANALYZE TABLE ebb_online");
mysql_query("ANALYZE TABLE ebb_pm");
mysql_query("ANALYZE TABLE ebb_pm_banlist");
mysql_query("ANALYZE TABLE ebb_poll");
mysql_query("ANALYZE TABLE ebb_posts");
mysql_query("ANALYZE TABLE ebb_ranks");
mysql_query("ANALYZE TABLE ebb_read");
mysql_query("ANALYZE TABLE ebb_settings");
mysql_query("ANALYZE TABLE ebb_smiles");
mysql_query("ANALYZE TABLE ebb_style");
mysql_query("ANALYZE TABLE ebb_topic_watch");
mysql_query("ANALYZE TABLE ebb_topics");
mysql_query("ANALYZE TABLE ebb_users");
mysql_query("ANALYZE TABLE ebb_votes");
break;
case 'optimize':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
mysql_query("OPTIMIZE TABLE ebb_banlist");
mysql_query("OPTIMIZE TABLE ebb_blacklist");
mysql_query("OPTIMIZE TABLE ebb_boards");
mysql_query("OPTIMIZE TABLE ebb_category");
mysql_query("OPTIMIZE TABLE ebb_censor");
mysql_query("OPTIMIZE TABLE ebb_group_users");
mysql_query("OPTIMIZE TABLE ebb_grouplist");
mysql_query("OPTIMIZE TABLE ebb_groups");
mysql_query("OPTIMIZE TABLE ebb_online");
mysql_query("OPTIMIZE TABLE ebb_pm");
mysql_query("OPTIMIZE TABLE ebb_pm_banlist");
mysql_query("OPTIMIZE TABLE ebb_poll");
mysql_query("OPTIMIZE TABLE ebb_posts");
mysql_query("OPTIMIZE TABLE ebb_ranks");
mysql_query("OPTIMIZE TABLE ebb_read");
mysql_query("OPTIMIZE TABLE ebb_settings");
mysql_query("OPTIMIZE TABLE ebb_smiles");
mysql_query("OPTIMIZE TABLE ebb_style");
mysql_query("OPTIMIZE TABLE ebb_topic_watch");
mysql_query("OPTIMIZE TABLE ebb_topics");
mysql_query("OPTIMIZE TABLE ebb_users");
mysql_query("OPTIMIZE TABLE ebb_votes");
break;
case 'repair':
//see if a jr. admin is trying to access this section.
if($groupuser['Limitation'] == 1){
$error = $cp['noaccess'];
echo error($error, "error");
}
mysql_query("REPAIR TABLE ebb_banlist");
mysql_query("REPAIR TABLE ebb_blacklist");
mysql_query("REPAIR TABLE ebb_boards");
mysql_query("REPAIR TABLE ebb_category");
mysql_query("REPAIR TABLE ebb_censor");
mysql_query("REPAIR TABLE ebb_group_users");
mysql_query("REPAIR TABLE ebb_grouplist");
mysql_query("REPAIR TABLE ebb_groups");
mysql_query("REPAIR TABLE ebb_online");
mysql_query("REPAIR TABLE ebb_pm");
mysql_query("REPAIR TABLE ebb_pm_banlist");
mysql_query("REPAIR TABLE ebb_poll");
mysql_query("REPAIR TABLE ebb_posts");
mysql_query("REPAIR TABLE ebb_ranks");
mysql_query("REPAIR TABLE ebb_read");
mysql_query("REPAIR TABLE ebb_settings");
mysql_query("REPAIR TABLE ebb_smiles");
mysql_query("REPAIR TABLE ebb_style");
mysql_query("REPAIR TABLE ebb_topic_watch");
mysql_query("REPAIR TABLE ebb_topics");
mysql_query("REPAIR TABLE ebb_users");
mysql_query("REPAIR TABLE ebb_votes");
break;
default:
$page = new template($template_path ."/cp-mainmenu.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$cp[title]",
  "BORDER" => "$border",
  "TABLE1" => "$table1",
  "TABLE2" => "$table2",
  "LANG-WELCOME" => "$cp[welcome]",
  "LANG-MAINMENU" => "$cp[mainmenu]",
  "LANG-BOARDMENU" => "$cp[boardmenu]",
  "LANG-USERMENU" => "$cp[usermenu]",
  "LANG-GENERALMENU" => "$cp[generalmenu]",
  "LANG-GROUPMENU" => "$cp[groupmenu]",
  "LANG-STYLEMENU" => "$cp[stylemenu]",
  "LANG-UTILITIES" => "$cp[utilities]",
  "LANG-SETTINGS" => "$cp[settings]",
  "LANG-CHECKVERSION" => "$cp[checkver]",
  "VERSION" => "1.0.9",
  "LANG-MANAGE" => "$cp[manage]",
  "LANG-BOARDSETUP" => "$cp[boardsetup]",
  "LANG-SMILES" => "$cp[smiles]",
  "LANG-NEWSLETTER" => "$cp[newsletter]",
  "LANG-GROUPSETUP" => "$cp[groupsetup]",
  "LANG-GROUPPERMISSION" => "$cp[grouppermission]",
  "LANG-PENDINGLIST" => "$cp[pendinglist]",
  "LANG-RANKS" => "$cp[ranks]",
  "LANG-CREATESTYLE" => "$cp[createstyle]",
  "LANG-BAN" => "$cp[banlist]",
  "LANG-BLACKLISTUSERS" => "$cp[blacklist]",
  "LANG-ACTIVATE" => "$cp[activateacct]",
  "LANG-CENSOR" => "$cp[censor]",
  "LANG-PRUNE" => "$cp[prune]",
  "LANG-USERPRUNE" => "$cp[userprune]",
  "LANG-ANALYZE" => "$cp[analyze]",
  "LANG-OPTIMIZE" => "$cp[optimize]",
  "LANG-REPAIR" => "$cp[repair]"));
$page->output();
}
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
ob_end_flush();
?>
