<?
if (!defined('IN_EBB') ) {
die("<B>!!ACCESS DENIED HACKER!!</B>");
}
/*
Filename: admin_function.php
Last Modified: 10/8/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
#admin-board manager
function admin_boardmanager(){

global $table1, $table2, $border, $cp;

$sql = "select * from ebb_category ORDER BY C_Order";
$errorq = $sql;
$cat_q = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($cat_q)) {

$boardmanager .= "<tr bgcolor=\"$table1\">
<td class=\"td\" width=\"25%\" align=\"center\"><b>$row[Name]</b></td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=category_modify&id=$row[id]\">$cp[modify]</a></td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=category_delete&id=$row[id]\">$cp[delete]</a></td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=cat_order&amp;id=$row[id]&amp;o=up\">$cp[moveup]</a>&nbsp;/&nbsp;<a href=\"CP.php?action=cat_order&amp;id=$row[id]&amp;o=down\">$cp[movedown]</a></td>
</tr>";
$sql = "SELECT * FROM ebb_boards WHERE Category = '$row[id]' ORDER BY B_Order";
$errorq = $sql;
$board_q = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row2 = mysql_fetch_assoc ($board_q)) {

$boardmanager .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"25%\">$row2[Board]</td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=board_modify&id=$row2[id]\">$cp[modify]</a></td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=board_delete&id=$row2[id]\">$cp[delete]</a></td><td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=board_order&amp;id=$row2[id]&amp;cat=$row[Name]&amp;o=up\">$cp[moveup]</a>&nbsp;/&nbsp;<a href=\"CP.php?action=board_order&amp;id=$row2[id]&amp;cat=$row[Name]&amp;o=down\">$cp[movedown]</a></td>
</tr>";

}
}
return $boardmanager;
}
#
function prune_boardlist(){

$board_select = "<select name=\"boardsel\" class=\"text\">";
$sql = "SELECT * FROM ebb_boards";
$errorq = $sql;
$cat_q = mysql_query($sql) or die(error($error, "", $errorq));

while ($row = mysql_fetch_assoc ($cat_q)){

$board_select .= "<option value=\"$row[id]\">$row[Board]";
}
$board_select .= "</select>";

return $board_select;
}
#admin-group list
function admin_grouplist(){

global $table2, $cp, $groups;

$sql = "select * from ebb_groups";
$errorq = $sql;
$group_q = mysql_query ($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($group_q)) {

if ($row['Enrollment'] == 1){
$group_status = $groups['open'];
}elseif($row['Enrollment'] == 0){
$group_status = $groups['closed'];
}else{
$group_status = $cp['grouphidden'];
}
$grouplist .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=group_modify&id=$row[id]\">$cp[modify]</a>&nbsp;|&nbsp;<a href=\"CP.php?action=group_delete&id=$row[id]\">$cp[delete]</a></td>
<td class=\"td\">$row[Name]</td>
<td class=\"td\">$row[Leader]</td>
<td class=\"td\" align=\"center\">$group_status</td>
<td class=\"td\" align=\"center\">$row[Level]</td>
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=group_memberlist&amp;id=$row[id]\">$cp[viewlist]</a></td>
</tr>";
}
return $grouplist;
}
#admin-grouplist
function admin_view_group(){
global $groups, $table2, $id, $db, $gmt, $time_format, $template_path, $pm, $cp;

$sql = "select * from ebb_group_users where gid='$id' and Status='Active'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$gnum = mysql_num_rows($query);
//If no results are found.
if ($gnum == 0){
$groupmemberlist ="<tr bgcolor=\"$table2\">
<td class=\"td\" colspan=\"5\" align=\"center\">$groups[nomembers]</td>
</tr>";
}else{
while ($row = mysql_fetch_assoc ($query)){

$db->run = "select * from ebb_users where Username='$row[Username]'";
$r = $db->result();
$db->close();

$gmttime = gmdate ($time_format, $r['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));

$groupmemberlist .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=groupmember_remove&gid=$row[gid]&u=$row[Username]\">$cp[removefromgroup]</a></td><td class=\"td\" align=\"center\"><a href=\"PM.php?action=write&user=$r[Username]\"><img src=\"$template_path/images/pm.gif\" alt=\"$pm[postpmalt]\" border=\"0\"></a></td><td class=\"td\" align=\"center\"><a href=\"Profile.php?mode=view&amp;user=$r[Username]\">$r[Username]</a></td><td class=\"td\" align=\"center\">$r[Post_Count]</td><td class=\"td\" align=\"center\">$join_date</td>
</tr>";
}
}
return $groupmemberlist;
}
#admin group detail display
function admin_display_group(){
global $table2, $cp;
$sql = "select * from ebb_groups";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($query)){

$admin_grouplist .="<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=pendingview&amp;id=$row[id]\">$row[Name]</a></td></tr>";
}
return $admin_grouplist;
}
#admin-pending grouplist
function admin_grouppending(){
global $table2, $id, $db, $gmt, $time_format, $template_path, $pm, $cp;

$sql = "select * from ebb_group_users where gid='$id' and Status='Pending'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$gnum = mysql_num_rows($query);
//See if no results are found.
if ($gnum == 0){
$pendingusers ="<tr bgcolor=\"$table2\">
<td class=\"td\" colspan=\"5\" align=\"center\">$cp[nopending]</td>
</tr>";
}else{
while ($row = mysql_fetch_assoc ($query)){

$db->run = "select * from ebb_users where Username='$row[Username]'";
$r = $db->result();
$db->close();

$gmttime = gmdate ($time_format, $r['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));

$pendingusers .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=pending_stat&accept=1&gid=$row[gid]&u=$row[Username]\">$cp[pendingaccept]</a>|<a href=\"CP.php?action=pending_stat&accept=0&gid=$row[gid]&u=$row[Username]\">$cp[pendingdeny]</a></td><td class=\"td\" align=\"center\"><a href=\"PM.php?action=write&user=$r[Username]\"><img src=\"$template_path/images/pm.gif\" alt=\"$pm[postpmalt]\" border=\"0\"></a></td><td class=\"td\" align=\"center\"><a href=\"Profile.php?mode=view&amp;user=$r[Username]\">$r[Username]</a></td><td class=\"td\" align=\"center\">$r[Post_Count]</td><td class=\"td\" align=\"center\">$join_date</td>
</tr>";
}
}
return $pendingusers;
}
#admin- permission display group
function permission_display_group(){

global $groups, $table1, $table2, $border, $cp;
$sql = "select * from ebb_groups";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($query)){

$admin_grouplist .="<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"CP.php?action=grouprights&amp;id=$row[id]\">$row[Name]</a></td></tr>";
}
return $admin_grouplist;
}
#admin-group permission
function group_permission(){

global $table2, $id, $db, $cp;

$sql = "select * from ebb_boards";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($query)){

$db->run = "select * from ebb_grouplist where group_id='$id' and board_id='$row[id]'";
$n = $db->num_results();
$db->close();
if($n == 0){
$group_rights .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\">$row[Board]</td><td class=\"td\" align=\"center\"><a href=\"CP.php?action=grouprights_process&amp;stat=grant&gid=$id&bid=$row[id]\">$cp[grantaccess]</a></td>
</tr>";
}else{
$group_rights .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\">$row[Board]</td><td class=\"td\" align=\"center\"><a href=\"CP.php?action=grouprights_process&amp;stat=ungrant&gid=$id&bid=$row[id]\">$cp[ungrantaccess]</a></td>
</tr>";
}
}
return $group_rights;
}
#admin-smile listing
function admin_smilelisting(){

global $table2, $cp;

$sql = "SELECT * FROM ebb_smiles";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc($query)){
$admin_smiles .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"25%\" align=\"center\"><a href=\"CP.php?action=modify_smiles&amp;id=$row[id]\">$cp[modify]</a> | <a href=\"CP.php?action=delete_smiles&amp;id=$row[id]\">$cp[delete]</a></td><td class=\"td\" width=\"25%\" align=\"center\"><img src=\"images/smiles/$row[img_name]\" alt=\"\"></td><td class=\"td\" width=\"25%\" align=\"center\">$row[code]</td><td class=\"td\" width=\"25%\" align=\"center\">$row[img_name]</td></tr>";
}
return $admin_smiles;
}
#admin-rank listing
function admin_ranklisting(){

global $table2, $cp;

$sql = "SELECT * FROM ebb_ranks";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc($query)){
$admin_rank .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"33%\" align=\"center\"><a href=\"CP.php?action=rank_edit&amp;id=$row[id]\">$cp[modify]</a> | <a href=\"CP.php?action=rank_delete&amp;id=$row[id]\">$cp[delete]</a></td><td class=\"td\" width=\"34%\">$row[Name]</td><td class=\"td\" width=\"33%\" align=\"center\">$row[Post_req]</td></tr>";
}
return $admin_rank;

}
#admin-inactive user listing
function inactive_users(){

global $inactive_q, $table1, $table2, $border, $cp, $user_ct;

if($user_ct == 0){
$inactive_list = "<tr bgcolor=\"$table2\">
<td class=\"td\" colspan=\"3\" align=\"center\">$cp[noinactiveusers]</td></tr>";
}else{
while($r = mysql_fetch_assoc($inactive_q)){
$inactive_list .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"33%\" align=\"center\"><a href=\"CP.php?action=activate_user&amp;stat=accept&amp;id=$r[id]\">$cp[pendingaccept]</a> | <a href=\"CP.php?action=activate_user&amp;stat=deny&amp;id=$r[id]\">$cp[pendingdeny]</a></td><td class=\"td\" width=\"34%\">$r[Username]</td><td class=\"td\" width=\"33%\" align=\"center\">$r[Date_Joined]</td></tr>";
}
}
return $inactive_list;
}
#admin-style listing
function admin_stylelisting(){

global $table2, $cp;

$sql = "SELECT * FROM ebb_style";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc($query)){
$admin_style .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\" align=\"center\"><a href=\"CP.php?action=style_edit&amp;id=$row[id]\">$cp[modify]</a> | <a href=\"CP.php?action=style_delete&amp;id=$row[id]\">$cp[delete]</a></td><td class=\"td\" width=\"50%\">$row[Name]</td></tr>";
}
return $admin_style;
}
#admin-ban listing IP's
function admin_banlist_ip(){

global $table1, $table2, $border, $cp;

$sql = "SELECT * FROM ebb_banlist where ban_type='IP'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$num = mysql_num_rows($query);

$admin_banlist_ip = "<select name=\"ipsel\" class=\"text\">";

if ($num == 0){
$admin_banlist_ip .= "<option value=\"\">$cp[nobanlistip]";
}else{
while ($row = mysql_fetch_assoc($query)){
$admin_banlist_ip .= "<option value=\"$row[id]\">$row[ban_item]";
}
}
$admin_banlist_ip .= "</select>";

return $admin_banlist_ip;
}
#admin-ban listing Email
function admin_banlist_email(){

global $cp;

$sql = "SELECT * FROM ebb_banlist where ban_type='Email'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$num = mysql_num_rows($query);
$admin_banlist_email = "<select name=\"emailsel\" class=\"text\">";
if ($num == 0){
$admin_banlist_email .= "<option value=\"\">$cp[nobanlistemail]";
}else{
while ($row = mysql_fetch_assoc($query)){
$admin_banlist_email .= "<option value=\"$row[id]\">$row[ban_item]";
}
}
$admin_banlist_email .= "</select>";

return $admin_banlist_email;
}
#admin-censor listing
function admin_censorlist(){

global $table2, $cp;

$sql = "SELECT * FROM ebb_censor";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$num = mysql_num_rows($query);
//see if theres no entry at all.
if ($num == 0){
$admin_censorlist = "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\" colspan=\"2\">$cp[emptycensorlist]</td></tr>";
}else{
while ($row = mysql_fetch_assoc($query)){
$admin_censorlist .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\"><a href=\"CP.php?action=censor_modify&amp;id=$row[id]\">$cp[delete]</a></td><td class=\"td\" width=\"50%\">$row[Original_Word]</td></tr>";
}
}
$admin_censorlist .= "</table>";

return $admin_censorlist;
}
#admin-username blacklist
function admin_blacklist(){

global $cp;

$sql = "SELECT * FROM ebb_blacklist";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$num = mysql_num_rows($query);
$username_blacklist = "<select name=\"blkusersel\" class=\"text\">";
if ($num == 0){
$username_blacklist .= "<option value=\"\">$cp[noblacklistednames]";
}else{
while ($row = mysql_fetch_assoc($query)){
$username_blacklist .= "<option value=\"$row[id]\">$row[blacklisted_username]";
}
}
$username_blacklist .= "</select>";

return $username_blacklist;
}
?>
