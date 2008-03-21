<?
if (!defined('IN_EBB') ) {
die("<B>!!ACCESS DENIED HACKER!!</B>");
}
/*
Filename: function.php
Last Modified: 12/27/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
#anti-sql injection function
function anti_injection($string){
//strip any slahes found.
if (get_magic_quotes_gpc()){
$string = stripslashes($string);
}
//if not a number or a numeric string
if (!is_numeric($string)){
$string = mysql_real_escape_string($string);
}
return $string;
}
#html-cleaning functions
function removeEvilAttributes($string){
$stripAttrib = "' (style|class)=\"(.*?)\"'i";
$string = stripslashes($string);
$string = preg_replace($stripAttrib, '', $string);
return $string;
}
#ban function
function check_ban(){

global $stat, $txt;

if($stat == "Banned"){
$error = $txt['banned'];
echo error($error, "general");
}
$sql = "SELECT * FROM ebb_banlist WHERE ban_type='IP'";
$errorq = $sql;
$ban_q = mysql_query($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($ban_q)){

$uip = $_SERVER['REMOTE_ADDR'];
if ($uip == $row['ban_item']){
$error = $txt['banned'];
echo error($error, "general");
}
}
}
#ban email check.
function check_email($string){
   global $db;

#domain check.
$checkDomain = explode("@", $string);

$db->run = "SELECT * FROM ebb_banlist WHERE ban_type='Email' AND ban_item like '$checkDomain[1]' or ban_item='$string'";
$emailmatch_chk = $db->num_results();
$emailban_q = $db->query();
$db->close();

if ($emailmatch_chk == 0){
$emailban = 0;
}else{
while ($row = mysql_fetch_assoc($emailban_q)) {
if ($row['match_type'] == "Wildcard") {
$emailban = 1;
}else{
if ($row['ban_item'] == $string) {
$emailban = 1;
}
}
}
}
return ($emailban);
}
#blacklisted username check.
function blacklisted_usernames($value){

$result = mysql_query("SELECT blacklisted_username FROM ebb_blacklist") or die(mysql_error());

while($row = mysql_fetch_assoc($result)) {
if (strstr($value,$row['blacklisted_username'])) {
$blklist = 1;
}else{
$blklist = 0;
}
}
return ($blklist);
}
#php parsing function for information ticker.
function nl2p($string) {
  return "<p>" . str_replace("\n", "</p><p>", $string) . "</p>";
}
#Smiles function - converts smiles bbcode
function smiles($string) {
$sql = "SELECT * FROM ebb_smiles";
$errorq = $sql;
$words = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_array ($words, MYSQL_ASSOC)) {
$smilecode = array ($row['code']);

   foreach ($smilecode as $smiles) {

           $string = str_replace($smiles,"<img src=\"images/smiles/$row[img_name]\" alt=\"\">",$string);
}
}
         return $string;
}
#email format checker.
function valid_email($string) { 

if(ereg("([[:alnum:]\.\-]+)(\@[[:alnum:]\.\-]+\.+)", $string)) {
return 0;
}else{
return 1;
} 
}
#smiles output for the form
function form_smiles(){

global $allowsmile;

if ($allowsmile == "Off"){
$smile = '';
}
else{
$x=0; // we will use this to count to four later
$sql = "SELECT * FROM ebb_smiles limit 12";
$errorq = $sql;
$smiles = mysql_query($sql) or die(error($error, "mysql", $errorq));
while($row = mysql_fetch_assoc($smiles))
{
if (($x % 4) == 0) {
$smile .= "<br>";  // $x == 4 so we start the line again
  $x=0; // $x is now 4 so we reset it here to start the next line
}

$smile .= "<a href=\"javascript:bbcode('$row[code]')\"><img src=\"images/smiles/$row[img_name]\" border=\"0\" alt=\"\"></a>&nbsp;";
$x++; // increment $x by 1 so we get our 4

}
}
return $smile;
}
#show all smiles output for the form.
function showall_smiles(){

global $allowsmile;

if ($allowsmile == "Off"){
$allsmile = '';
}
else{
$x=0; // we will use this to count to eight later
$sql = "SELECT * FROM ebb_smiles";
$errorq = $sql;
$smiles = mysql_query($sql) or die(error($error, "mysql", $errorq));
while($row = mysql_fetch_assoc($smiles))
{
if (($x % 8) == 0) {
$allsmile .= "<br>";  // $x == 8 so we start the line again
  $x=0; // $x is now 8 so we reset it here to start the next line
}

$allsmile .= "<img src=\"images/smiles/$row[img_name]\" title=\"$row[code]\" alt=\"$row[code]\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
$x++; // increment $x by 1 so we get our 8

}
}
return $allsmile;
}
#BBCode function - converts bbcode
function BBCode($string, $allowimgs = false) {

global $table1, $table2, $border;

        $string = preg_replace('~\[i\](.*?)\[\/i\]~is', '<i>\\1</i>', $string);
        $string = preg_replace('~\[b\](.*?)\[\/b\]~is', '<b>\\1</b>', $string);
        $string = preg_replace('~\[u\](.*?)\[\/u\]~is', '<u>\\1</u>', $string);
        $string = preg_replace('~\[url\](.*?)\[\/url\]~is', '<a href="\\1">\\1</a>', $string);
        $string = preg_replace('#([^\'"=\]]|^)(http[s]?|ftp[s]?|gopher|irc){1}://([:a-z_\-\\./0-9%~]+){1}(\?[a-z=0-9\-_&;]*)?(\#[a-z0-9]+)?#mi', '\1<a href="\2://\3\4\5" target="_blank">\2://\3\4\5</a>', $string);
        $string = preg_replace('~\[list\](.*?)\[\/list\]~is', '<li>\\1</li>', $string);
        $string = preg_replace('~\[center\](.*?)\[\/center\]~is', '<div align="center">\\1</div>', $string);
        $string = preg_replace('~\[right\](.*?)\[\/right\]~is', '<div align="right">\\1</div>', $string);
        $string = preg_replace('~\[left\](.*?)\[\/left\]~is', '<div align="left">\\1</div>', $string);
        $string = preg_replace('~\[sub\](.*?)\[\/sub\]~is', '<sub>\\1</sub>', $string);
        $string = preg_replace('~\[sup\](.*?)\[\/sup\]~is', '<sup>\\1</sup>', $string);
        $string = preg_replace('~\[marque\](.*?)\[\/marque\]~is', '<marquee>\\1</marquee>', $string);
        $string = preg_replace('~\[quote\](.*?)\[\/quote\]~is', "<br><table width=\"80%\" border=\"0\" bgcolor=\"$border\" cellspacing=\"1\" cellpadding=\"1\" align=\"center\"><tr bgcolor=\"$table1\"><td class=\"td\"><b>Quote:</b></td></tr><tr bgcolor=\"$table2\"><td class=\"td\">\\1</td></tr></table>", $string);
        $string = preg_replace('~\[code\](.*?)\[\/code\]~is', "<br><table width=\"80%\" border=\"0\" bgcolor=\"$border\" cellspacing=\"1\" cellpadding=\"1\" align=\"center\"><tr bgcolor=\"$table1\"><td class=\"td\"><b>Code:</b></td></tr><tr bgcolor=\"$table2\"><td class=\"td\">\\1</td></tr></table>", $string);


    //we don't want to allow imgs all the time!
        if ($allowimgs == true) {

        $string = preg_replace('~\[img\](.*?)\[\/img\]~is', '<img src="\\1" border="0" alt="">', $string);
        }

         return $string;
}
#printable-version bbcode
function BBCode_print($string) {

        $string = preg_replace('~\[i\](.*?)\[\/i\]~is', '<i>\\1</i>', $string);
        $string = preg_replace('~\[b\](.*?)\[\/b\]~is', '<b>\\1</b>', $string);
        $string = preg_replace('~\[u\](.*?)\[\/u\]~is', '<u>\\1</u>', $string);
        $string = preg_replace('~\[url\](.*?)\[\/url\]~is', '<a href="\\1">\\1</a>', $string);
        $string = preg_replace('#([^\'"=\]]|^)(http[s]?|ftp[s]?|gopher|irc){1}://([:a-z_\-\\./0-9%~]+){1}(\?[a-z=0-9\-_&;]*)?(\#[a-z0-9]+)?#mi', '\1<a href="\2://\3\4\5" target="_blank">\2://\3\4\5</a>', $string);
        $string = preg_replace('~\[list\](.*?)\[\/list\]~is', '<li>\\1</li>', $string);
        $string = preg_replace('~\[center\](.*?)\[\/center\]~is', '<div align="center">\\1</div>', $string);
        $string = preg_replace('~\[right\](.*?)\[\/right\]~is', '<div align="right">\\1</div>', $string);
        $string = preg_replace('~\[left\](.*?)\[\/left\]~is', '<div align="left">\\1</div>', $string);
        $string = preg_replace('~\[sub\](.*?)\[\/sub\]~is', '<sub>\\1</sub>', $string);
        $string = preg_replace('~\[sup\](.*?)\[\/sup\]~is', '<sup>\\1</sup>', $string);
        $string = preg_replace('~\[marque\](.*?)\[\/marque\]~is', '<marquee>\\1</marquee>', $string);
        $string = preg_replace('~\[quote\](.*?)\[\/quote\]~is', "<br><table width=\"80%\" border=\"0\" cellspacing=\"1\" cellpadding=\"1\" align=\"center\"><tr><td class=\"td\"><b>Quote:</b></td></tr><tr><td class=\"td\">\\1</td></tr></table>", $string);
        $string = preg_replace('~\[code\](.*?)\[\/code\]~is', "<br><table width=\"80%\" border=\"0\" cellspacing=\"1\" cellpadding=\"1\" align=\"center\"><tr><td class=\"td\"><b>Code:</b></td></tr><tr><td class=\"td\">\\1</td></tr></table>", $string);

         return $string;
}
#bbcode button output
function bbcode_form(){

global $allowbbcode, $allowimg;

if ($allowbbcode == "Off"){
$bbcode = '';
}
else{
$bbcode = "<input type=\"button\" value=\"B\" onclick=\"javascript:bbcode(' [b][/b] ')\" class=\"submit\">
<input type=\"button\" value=\"I\" onclick=\"javascript:bbcode(' [i][/i] ')\" class=\"submit\">
<input type=\"button\" value=\"U\" onclick=\"javascript:bbcode(' [u][/u] ')\" class=\"submit\">
<input type=\"button\" value=\"http://\" onclick=\"javascript:bbcode(' [url][/url] ')\" class=\"submit\">
<input type=\"button\" value=\"Quote\" onclick=\"javascript:bbcode(' [quote][/quote] ')\" class=\"submit\">
<input type=\"button\" value=\"Code\" onclick=\"javascript:bbcode(' [code][/code] ')\" class=\"submit\">
<input type=\"button\" value=\"Marque\" onclick=\"javascript:bbcode(' [marque][/marque] ')\" class=\"submit\">
<input type=\"button\" value=\"Superscript\" onclick=\"javascript:bbcode(' [sup][/sup] ')\" class=\"submit\">
<input type=\"button\" value=\"Subscript\" onclick=\"javascript:bbcode(' [sub][/sub] ')\" class=\"submit\">&nbsp;
<input type=\"button\" value=\"List\" onclick=\"javascript:bbcode(' [list][/list] ')\" class=\"submit\">&nbsp;";
if ($allowimg == 1){
$bbcode .= "<input type=\"button\" value=\"Image\" onclick=\"javascript:bbcode(' [img][/img] ')\" class=\"submit\">";
}
$bbcode .= "<input type=\"button\" value=\"Left\" onclick=\"javascript:bbcode(' [left][/left] ')\" class=\"submit\">
<input type=\"button\" value=\"Center\" onclick=\"javascript:bbcode(' [center][/center] ')\" class=\"submit\">
<input type=\"button\" value=\"Right\" onclick=\"javascript:bbcode(' [right][/right] ')\" class=\"submit\">";
}

return $bbcode;
}
#language filter function - filters words
function language_filter($string) {

$sql = "SELECT * FROM `ebb_censor`";
$errorq = $sql;
$words = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_array ($words, MYSQL_ASSOC)) {
$obscenities = array ($row['Original_Word']);

   foreach ($obscenities as $curse_word) {

       if (stristr(trim($string),$curse_word)) {
           $length = strlen($curse_word);
           for ($i = 1; $i <= $length; $i++) {
               $stars .= "*";
           }
           $string = eregi_replace($curse_word,$stars,trim($string));
           $stars = "";
       }
}
}
   return $string;
}
#random password generator
function makeRandomPassword() {
  $salt = "abchefghjkmnpqrstuvwxyz0123456789";
  srand((double)microtime()*1000000);
  	$i = 0;
  	while ($i <= 7) {
    		$num = rand() % 33;
    		$tmp = substr($salt, $num, 1);
    		$pass = $pass . $tmp;
    		$i++;
  	}
  	return $pass;
}
#moderator listing
function moderator_boardlist(){
global $id, $db;

$sql = "select * from ebb_grouplist where board_id='$id' order by group_id";
$errorq = $sql;
$moderator_r = mysql_query($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($moderator_r)) {
//get group details.
$db->run = "select * from ebb_groups where id='$row[group_id]'";
$group_r = $db->result();
$db->close();
//output the info.
$board_moderator .= "<a href=\"groups.php?mode=view&amp;id=$group_r[id]\">$group_r[Name]</a>&nbsp;";
}
return $board_moderator;
}
#whosonline display function
function whosonline(){

global $db;

$sql = "select DISTINCT Username from ebb_online where ip=''";
$errorq = $sql;
$online_logged = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($online_logged)) {
//see what the users status is.
$db->run = "select * from ebb_users where Username='$row[Username]'";
$stat = $db->result();
$db->close();
if ($stat['Status'] == "groupmember"){
$db->run = "SELECT * FROM ebb_group_users where Username='$row[Username]'";
$groupchk = $db->result();
$db->close();
$db->run = "SELECT * FROM ebb_groups where id='$groupchk[gid]'";
$level_type = $db->result();
$db->close();
if ($level_type['Level'] == 1){
$online .= "<b><a href=\"Profile.php?mode=view&user=$row[Username]\">$row[Username]</a></b>&nbsp;";
}else{
$online .= "<i><a href=\"Profile.php?mode=view&user=$row[Username]\">$row[Username]</a></i>&nbsp;";
}
}elseif (($stat['Status'] == "Member") or ($level_type['Level'] == 3)){
$online .= "<a href=\"Profile.php?mode=view&user=$row[Username]\">$row[Username]</a>&nbsp;";
}else{
$online .= "&nbsp;";
}
}
return $online;
}
#group detail display
function display_group(){
global $table2;
$sql = "select * from ebb_groups where Enrollment!='2'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($query)){

$grouplist .="<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"groups.php?mode=view&amp;id=$row[id]\">$row[Name]</a></td></tr>";
}
return $grouplist;
}
#group details
function group_details(){
global $groups, $table2, $id;

$sql = "select * from ebb_groups where id='$id'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$checknum = mysql_num_rows($query);
//make sure this group exist first, if not kill the program and display error msg.
if ($checknum == 0){
$error = $groups['notexist'];
echo error($error, "general");
}

while ($row = mysql_fetch_assoc ($query)){

if ($row['Enrollment'] == 1){
$groupdetails .="<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[name]</td>
<td class=\"td\" align=\"50%\">$row[Name]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[description]</td>
<td class=\"td\" align=\"50%\">$row[Description]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[leader]</td>
<td class=\"td\" align=\"50%\">$row[Leader]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[groupstat]</td>
<td class=\"td\" align=\"50%\">$groups[open]</a></td>
</tr>";
}else{
$groupdetails .="<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[name]</td>
<td class=\"td\" align=\"50%\">$row[Name]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[description]</td>
<td class=\"td\" align=\"50%\">$row[Description]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[leader]</td>
<td class=\"td\" align=\"50%\">$row[Leader]</a></td>
</tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"50%\">$groups[groupstat]</td>
<td class=\"td\" align=\"50%\">$groups[closed]</a></td>
</tr>";
}
}
return $groupdetails;
}
#view group members
function view_group(){
global $groups, $table2, $id, $db, $gmt, $time_format, $template_path, $pm, $members;

$sql = "select * from ebb_group_users where gid='$id' and Status='Active'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));
$gnum = mysql_num_rows($query);

if ($gnum == 0){
$groupmembers = "<tr bgcolor=\"$table2\">
<td class=\"td\" colspan=\"4\" align=\"center\">$groups[nomembers]</td>
</tr>";
}else{
while ($row = mysql_fetch_assoc ($query)){

$db->run = "select * from ebb_users where Username='$row[Username]'";
$r = $db->result();
$db->close();

$gmttime = gmdate ($time_format, $r['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));

$groupmembers .= "<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\"><a href=\"PM.php?action=write&user=$r[Username]\"><img src=\"$template_path/images/pm.gif\" alt=\"$pm[postpmalt]\" border=\"0\"></a></td><td class=\"td\" align=\"center\"><a href=\"Profile.php?mode=view&amp;user=$r[Username]\">$r[Username]</a></td><td class=\"td\" align=\"center\">$r[Post_Count]</td><td class=\"td\" align=\"center\">$join_date</td>
</tr>";
}
}
return $groupmembers;
}
#ip-viewer code
function ip_checker(){

global $ip, $index, $u, $db;
$sql = "select * from ebb_users where Username='$u' or IP='$ip'";
$errorq = $sql;
$query = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($query)){
//get number of times the ip was used by this user.
$db->run = "select * from ebb_topics where author='$row[Username]' and IP='$ip'";
$count1 = $db->num_results();
$db->close();
$db->run = "select * from ebb_posts where re_author='$row[Username]' and IP='$ip'";
$count2 = $db->num_results();
$db->close();
$total_count = $count1 + $count2;

$iplist .= "$row[Username] - $total_count $index[posts]<br>";
}
return $iplist;
}
#get other ips the poster used before.
function other_ip_check(){

global $ip, $index, $u;
$sql = "select DISTINCT IP from ebb_topics where author='$u'";
$errorq = $sql;
$q = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row = mysql_fetch_assoc ($q)){
//get number of times the ip was used by this user.
$ipcheck .= "$row[IP]<br>";
}
$sql = "select DISTINCT IP from ebb_posts where re_author='$u'";
$errorq = $sql;
$q2 = mysql_query($sql) or die(error($error, "mysql", $errorq));

while ($row2 = mysql_fetch_assoc ($q2)){
//get number of times the ip was used by this user.
$ipcheck .= "$row[IP]<br>";
}
return $ipcheck;
}
#memberlist display function
function memberlist(){

global $table2, $members, $gmt, $query, $template_path, $pm, $time_format;

while ($row = mysql_fetch_assoc ($query)){
$total = $row['Post_Count'];
$gmttime = gmdate ($time_format, $row['Date_Joined']);
$join_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));

$memberlist .= "<tr bgcolor=\"$table2\">
<td class=\"td\">$row[id]</td><td class=\"td\"><a href=\"PM.php?action=write&user=$row[Username]\"><img src=\"$template_path/images/pm.gif\" alt=\"$Pm[postpmalt]\" border=\"0\"></a></td><td class=\"td\"><a href=\"Profile.php?mode=view&amp;user=$row[Username]\">$row[Username]</a></td><td class=\"td\" align=\"center\">$total</td><td class=\"td\">$join_date</td>
</tr>";
}
return $memberlist;
}
#search results-topics
function search_results_topic(){

global $table2, $search_result;

while ($row = mysql_fetch_assoc($search_result)) {

$searchresults .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\"><a href=\"viewtopic.php?bid=$row[bid]&amp;tid=$row[tid]\">$row[Topic]</a></td><td class=\"td\" width=\"50%\">$row[author]</td></tr>";
}
return $searchresults;
}
#search results-posts
function search_results_post(){

global $db, $table2, $search_result;

while ($row = mysql_fetch_assoc($search_result)){

$db->run = "SELECT * FROM ebb_topics where tid='$row[tid]'";
$topic_r = $db->result();
$db->close();

$searchresults .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\"><a href=\"viewtopic.php?bid=$row[bid]&amp;tid=$row[tid]&amp;pid=$row[pid]#$row[pid]\">$topic_r[Topic]</a></td><td class=\"td\" width=\"50%\">$row[re_author]</td></tr>";
}
return $searchresults;
}
#search results-new posts
function search_results_newposts(){

global $db, $table2, $search_result, $search_result2, $logged_user;
//output any topics
$count = 0;
while ($row = mysql_fetch_assoc($search_result)) {

$db->run = "select * from ebb_read WHERE Topic='$row[tid]' and user='$logged_user'";
$read_ct = $db->num_results();
$db->close();
if ($read_ct == 0){
//increment count
$count++;
$searchresults .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\"><a href=\"viewtopic.php?bid=$row[bid]&tid=$row[tid]\">$row[Topic]</a></td><td class=\"td\" width=\"50%\">$row[author]</td></tr>";
}
}
//output any posts
while ($row2 = mysql_fetch_assoc($search_result2)){

$db->run = "SELECT * FROM ebb_topics where tid='$row2[tid]'";
$topic_r = $db->result();
$db->close();
//see if post is new.
$db->run = "select * from ebb_read WHERE Topic='$row2[tid]' and user='$logged_user'";
$read_ct2 = $db->num_results();
$db->close();
if ($read_ct2 == 0){
//increment count
$count++;
$searchresults .= "<tr bgcolor=\"$table2\">
<td class=\"td\" width=\"50%\"><a href=\"viewtopic.php?bid=$row2[bid]&tid=$row2[tid]&pid=$row2[pid]#$row2[pid]\">$topic_r[Topic]</a></td><td class=\"td\" width=\"50%\">$row2[re_author]</td></tr>";
}
}
$result_info = array($searchresults, $count);
return $result_info;
}
#board select function
function board_select(){

global $search;

$sql = "SELECT * FROM ebb_boards";
$errorq = $sql;
$board_search = mysql_query($sql) or die(error($error, "mysql", $errorq));
$boardlist = "<select name=\"board\" class=\"text\">
<option value=\"\">$search[selboard]";
while ($row = mysql_fetch_assoc ($board_search)){

$boardlist .= "<option value=\"$row[id]\">$row[Board]";
}
$boardlist .= "</select>";
return $boardlist;
}
#timezone select function
function timezone_select(){

global $profile_result, $settings;

$timezone = '<select name="time_zone" class="text">';
if (($profile_result['Time_Zone'] == "-12") or ($settings['Default_Zone'] == "-12")){
$timezone .= '<option value="-12" selected>(GMT -12:00) Eniwetok, Kwajalein';
}
else{
$timezone .= '<option value="-12">(GMT -12:00) Eniwetok, Kwajalein';
}
if (($profile_result['Time_Zone'] == "-11") or ($settings['Default_Zone'] == "-11")){
$timezone .= '<option value="-11" selected>(GMT -11:00) Midway Island, Samoa';
}
else{
$timezone .= '<option value="-11">(GMT -11:00) Midway Island, Samoa';
}
if (($profile_result['Time_Zone'] == "-10") or ($settings['Default_Zone'] == "-10")){
$timezone .= '<option value="-10" selected>(GMT -10:00) Hawaii';
}
else{
$timezone .= '<option value="-10">(GMT -10:00) Hawaii';
}
if (($profile_result['Time_Zone'] == "-9") or ($settings['Default_Zone'] == "-9")){
$timezone .= '<option value="-9" selected>(GMT -9:00) Alaska';
}
else{
$timezone .= '<option value="-9">(GMT -9:00) Alaska';
}
if (($profile_result['Time_Zone'] == "-8") or ($settings['Default_Zone'] == "-8")){
$timezone .= '<option value="-8" selected>(GMT -8:00) Pacific Time (US &amp; Canada), Tijuana';
}
else{
$timezone .= '<option value="-8">(GMT -8:00) Pacific Time (US &amp; Canada), Tijuana';
}
if (($profile_result['Time_Zone'] == "-7") or ($settings['Default_Zone'] == "-7")){
$timezone .= '<option value="-7" selected>(GMT -7:00) Mountain Time (US &amp; Canada), Arizona';
}
else{
$timezone .= '<option value="-7">(GMT -7:00) Mountain Time (US &amp; Canada), Arizona';
}
if (($profile_result['Time_Zone'] == "-6") or ($settings['Default_Zone'] == "-6")){
$timezone .= '<option value="-6" selected>(GMT -6:00) Central Time (US &amp; Canada), Mexico City, Central America';
}
else{
$timezone .= '<option value="-6">(GMT -6:00) Central Time (US &amp; Canada), Mexico City, Central America';
}
if (($profile_result['Time_Zone'] == "-5") or ($settings['Default_Zone'] == "-5")){
$timezone .= '<option value="-5" selected>(GMT -5:00) Eastern Time (US &amp; Canada), Bogota, Lima, Quito';
}
else{
$timezone .= '<option value="-5">(GMT -5:00) Eastern Time (US &amp; Canada), Bogota, Lima, Quito';
}
if (($profile_result['Time_Zone'] == "-4") or ($settings['Default_Zone'] == "-4")){
$timezone .= '<option value="-4" selected>(GMT -4:00) Atlantic Time (Canada), Caracas, La Paz, Santiago';
}
else{
$timezone .= '<option value="-4">(GMT -4:00) Atlantic Time (Canada), Caracas, La Paz, Santiago';
}
if (($profile_result['Time_Zone'] == "-3.5") or ($settings['Default_Zone'] == "-3.5")){
$timezone .= '<option value="-3.5" selected>(GMT -3:30) Newfoundland';
}
else{
$timezone .= '<option value="-3.5">(GMT -3:30) Newfoundland';
}
if (($profile_result['Time_Zone'] == "-3") or ($settings['Default_Zone'] == "-3")){
$timezone .= '<option value="-3" selected>(GMT -3:00) Brasilia, Buenos Aires, Georgetown, Greenland';
}
else{
$timezone .= '<option value="-3">(GMT -3:00) Brasilia, Buenos Aires, Georgetown, Greenland';
}
if (($profile_result['Time_Zone'] == "-2") or ($settings['Default_Zone'] == "-2")){
$timezone .= '<option value="-2" selected>(GMT -2:00) Mid-Atlantic, Ascension Islands, St. Helena';
}
else{
$timezone .= '<option value="-2">(GMT -2:00) Mid-Atlantic, Ascension Islands, St. Helena';
}
if (($profile_result['Time_Zone'] == "-1") or ($settings['Default_Zone'] == "-1")){
$timezone .= '<option value="-1" selected>(GMT -1:00) Azores, Cape Verde Islands';
}
else{
$timezone .= '<option value="-1">(GMT -1:00) Azores, Cape Verde Islands';
}
if (($profile_result['Time_Zone'] == "0") or ($settings['Default_Zone'] == "0")){
$timezone .= '<option value="0" selected>(GMT) Casablanca, Dublin, Edinburgh, Lisbon, London, Monrovia';
}
else{
$timezone .= '<option value="0">(GMT) Casablanca, Dublin, Edinburgh, Lisbon, London, Monrovia';
}
if (($profile_result['Time_Zone'] == "1") or ($settings['Default_Zone'] == "1")){
$timezone .= '<option value="1" selected>(GMT +1:00) Amsterdam, Berlin, Brussels, Madrid, Paris, Rome';
}
else{
$timezone .= '<option value="1">(GMT +1:00) Amsterdam, Berlin, Brussels, Madrid, Paris, Rome';
}
if (($profile_result['Time_Zone'] == "2") or ($settings['Default_Zone'] == "2")){
$timezone .= '<option value="2" selected>(GMT +2:00) Cairo, Helsinki, Kaliningrad, South Africa';
}
else{
$timezone .= '<option value="2">(GMT +2:00) Cairo, Helsinki, Kaliningrad, South Africa';
}
if (($profile_result['Time_Zone'] == "3") or ($settings['Default_Zone'] == "3")){
$timezone .= '<option value="3" selected>(GMT +3:00) Baghdad, Riyadh, Moscow, Nairobi';
}
else{
$timezone .= '<option value="3">(GMT +3:00) Baghdad, Riyadh, Moscow, Nairobi';
}
if (($profile_result['Time_Zone'] == "3.5") or ($settings['Default_Zone'] == "3.5")){
$timezone .= '<option value="3.5" selected>(GMT +3:30) Tehran';
}
else{
$timezone .= '<option value="3.5">(GMT +3:30) Tehran';
}
if (($profile_result['Time_Zone'] == "4") or ($settings['Default_Zone'] == "4")){
echo '<option value="4" selected>(GMT +4:00) Abu Dhabi, Baku, Muscat, Tbilii';
}
else{
$timezone .= '<option value="4">(GMT +4:00) Abu Dhabi, Baku, Muscat, Tbilii';
}
if (($profile_result['Time_Zone'] == "4.5") or ($settings['Default_Zone'] == "4.5")){
$timezone .= '<option value="4.5" selected>(GMT +4:30) Kabul';
}
else{
$timezone .= '<option value="4.5">(GMT +4:30) Kabul';
}
if (($profile_result['Time_Zone'] == "5") or ($settings['Default_Zone'] == "5")){
$timezone .= '<option value="5" selected>(GMT +5:00) Ekaterinburg, Islamabad, Karachi, Tashkent';
}
else{
$timezone .= '<option value="5">(GMT +5:00) Ekaterinburg, Islamabad, Karachi, Tashkent';
}
if (($profile_result['Time_Zone'] == "5.5") or ($settings['Default_Zone'] == "5.5")){
$timezone .= '<option value="5.5" selected>(GMT +5:30) Bombay, Calcutta, Madras, New Delhi';
}
else{
$timezone .= '<option value="5.5">(GMT +5:30) Bombay, Calcutta, Madras, New Delhi';
}
if (($profile_result['Time_Zone'] == "5.75") or ($settings['Default_Zone'] == "5.75")){
$timezone .= '<option value="5.75" selected>(GMT +5:45) Kathmandu';
}
else{
$timezone .= '<option value="5.75">(GMT +5:45) Kathmandu';
}
if (($profile_result['Time_Zone'] == "6") or ($settings['Default_Zone'] == "6")){
$timezone .= '<option value="6" selected>(GMT +6:00) Almaty, Colombo, Dhaka, Novosibirsk, Sri Jayawardenepura';
}
else{
$timezone .= '<option value="6">(GMT +6:00) Almaty, Colombo, Dhaka, Novosibirsk, Sri Jayawardenepura';
}
if (($profile_result['Time_Zone'] == "6.5") or ($settings['Default_Zone'] == "6.5")){
$timezone .= '<option value="6.5" selected>(GMT +6:30) Rangoon';
}
else{
$timezone .= '<option value="6.5">(GMT +6:30) Rangoon';
}
if (($profile_result['Time_Zone'] == "7") or ($settings['Default_Zone'] == "7")){
$timezone .= '<option value="7" selected>(GMT +7:00) Bangkok, Hanoi, Jakarta, Krasnoyarsk';
}
else{
$timezone .= '<option value="7">(GMT +7:00) Bangkok, Hanoi, Jakarta, Krasnoyarsk';
}
if (($profile_result['Time_Zone'] == "8") or ($settings['Default_Zone'] == "8")){
$timezone .= '<option value="8" selected>(GMT +8:00) Beijing, Hong Kong, Perth, Singapore, Taipei';
}
else{
$timezone .= '<option value="8">(GMT +8:00) Beijing, Hong Kong, Perth, Singapore, Taipei';
}
if (($profile_result['Time_Zone'] == "9") or ($settings['Default_Zone'] == "9")){
$timezone .= '<option value="9" selected>(GMT +9:00) Osaka, Sapporo, Seoul, Tokyo, Yakutsk';
}
else{
$timezone .= '<option value="9">(GMT +9:00) Osaka, Sapporo, Seoul, Tokyo, Yakutsk';
}
if (($profile_result['Time_Zone'] == "9.5") or ($settings['Default_Zone'] == "9.5")){
$timezone .= '<option value="9.5" selected>(GMT +9:30) Adelaide, Darwin';
}
else{
$timezone .= '<option value="9.5">(GMT +9:30) Adelaide, Darwin';
}
if (($profile_result['Time_Zone'] == "10") or ($settings['Default_Zone'] == "10")){
$timezone .= '<option value="10" selected>(GMT +10:00) Canberra, Guam, Melbourne, Sydney, Vladivostok';
}
else{
$timezone .= '<option value="10">(GMT +10:00) Canberra, Guam, Melbourne, Sydney, Vladivostok';
}
if (($profile_result['Time_Zone'] == "11") or ($settings['Default_Zone'] == "11")){
$timezone .= '<option value="11" selected>(GMT +11:00) Magadan, New Caledonia, Solomon Islands';
}
else{
$timezone .= '<option value="11">(GMT +11:00) Magadan, New Caledonia, Solomon Islands';
}
if (($profile_result['Time_Zone'] == "12") or ($settings['Default_Zone'] == "12")){
$timezone .= '<option value="12" selected>(GMT +12:00) Auckland, Fiji, Kamchatka, Marshall Island, Wellington';
}
else{
$timezone .= '<option value="12">(GMT +12:00) Auckland, Fiji, Kamchatka, Marshall Island, Wellington';
}
if (($profile_result['Time_Zone'] == "13") or ($settings['Default_Zone'] == "13")){
$timezone .= '<option value="13" selected>(GMT +13:00) Nuku\' alofa';
}
else{
$timezone .= '<option value="13">(GMT +13:00) Nuku\' alofa';
}
$timezone .= '</select>';

return $timezone;
}
#style select function
function style_select(){

global $profile_result, $settings;

$style_select = "<select name=\"style\" class=\"text\">";
$sql = "SELECT * FROM ebb_style";
$errorq = $sql;
$style_query = mysql_query ($sql) or die(error($error, "mysql", $errorq));
$numOfRows = mysql_num_rows ($style_query);
for ($i = 0; $i < $numOfRows; $i++)
{
$styleid = mysql_result ($style_query, $i, "id");
$stylename = mysql_result ($style_query, $i, "Name");
if (($profile_result['Style'] == $styleid) or ($settings['Default_Style'] == $styleid)){
$style_select .= "<option value=\"$styleid\" selected>$stylename";
}
else{
$style_select .= "<option value=\"$styleid\">$stylename";
}
}
$style_select .= "</select>";

return $style_select;
}
#language select function
function lang_select(){

global $profile_result, $settings;

$lang = "<select name=\"default_lang\" class=\"text\">";
$handle = opendir("lang");
while (($file = readdir($handle))) {
if (is_file("lang/$file") && false !== strpos($file, '.lang.php')) {

$file = str_replace(".lang.php", "", $file);
if (($profile_result[Language] == $file) or ($settings['Default_Language'] == $file)){
$lang .= "<option value=\"$file\" selected>$file";
}
else{
$lang .= "<option value=\"$file\">$file";
}
}
}
$lang .= "</select>";
return $lang;
}
#category select function
function category_select(){

global $modify;

$cat_select = "<select name=\"catsel\" class=\"text\">";
$sql = "SELECT * FROM ebb_category";
$errorq = $sql;
$cat_q = mysql_query($sql) or die(error($error, "", $errorq));

while ($row = mysql_fetch_assoc ($cat_q)){

if ($modify['Category'] == $row['id']){
$cat_selected = "selected";
}
else{
$cat_selected = '';
}

$cat_select .= "<option value=\"$row[id]\" $cat_selected>$row[Name]";
}
$cat_select .= "</select>";

return $cat_select;
}
#group joined list
function groups_joined(){

global $db, $logged_user, $userinfo, $table2;

$sql = "SELECT * FROM ebb_groups";
$errorq = $sql;
$joined_q = mysql_query($sql) or die(error($error, "mysql", $errorq));
while ($row = mysql_fetch_assoc ($joined_q)) {
//see if user already joined this.
$db->run = "SELECT * FROM ebb_group_users where gid='$row[id]' and Username='$logged_user'";
$result = $db->result();
$join_chk = $db->num_results();
$db->close();
if($join_chk == 1){
if ($result['Status'] == "Pending"){
$joined_group .= "<tr bgcolor=\"$table2\"><td class=\"td\" width=\"50%\">$userinfo[pending]</td><td class=\"td\" width=\"50%\">$row[Name]</td></tr>";
}else{
$joined_group .= "<tr bgcolor=\"$table2\"><td class=\"td\" width=\"50%\"><a href=\"Profile?mode=unjoin_group&amp;id=$result[gid]\">[$userinfo[unjoingroup]]</a></td><td class=\"td\" width=\"50%\">$row[Name]</td></tr>";
}
}else{
if ($row['Enrollment'] == 1){
$joined_group .= "<tr bgcolor=\"$table2\"><td class=\"td\" width=\"50%\"><a href=\"Profile?mode=join_group&amp;id=$row[id]\">[$userinfo[joingroup]]</a></td><td class=\"td\" width=\"50%\">$row[Name]</td></tr>";
}elseif($row['Enrollment'] == 0){
$joined_group .= "<tr bgcolor=\"$table2\"><td class=\"td\" width=\"50%\">$userinfo[locked]</td><td class=\"td\" width=\"50%\">$row[Name]</td></tr>";
}else{
$joined_group .= "";
}
}
}
return $joined_group;
}
#list subscriptions function
function digest_list(){

global $db, $logged_user, $cp, $userinfo, $table2;

$sql = "SELECT * FROM ebb_topic_watch WHERE username='$logged_user'";
$errorq = $sql;
$sub_q = mysql_query($sql) or die(error($error, "mysql", $errorq));
$sub_ct = mysql_num_rows($sub_q);
if ($sub_ct == 0){
$sub = "<td class=\"td\" colspan=\"2\"  align=\"center\">$userinfo[nosubscription]</td>";
}
else{
while ($row = mysql_fetch_assoc ($sub_q)) {

$db->run = "SELECT * FROM ebb_topics where tid='$row[tid]'";
$result = $db->result();
$db->close();

$sub .= "<tr bgcolor=\"$table2\"><td class=\"td\" width=\"50%\"><a href=\"Profile?mode=digest_process&amp;del=$row[tid]\">[$cp[delete]]</a></td><td class=\"td\" width=\"50%\">$result[Topic]</td></tr>";
}
}
return $sub;
}
#avatar gallery function
function avatar_gallery(){

global $table1, $border;

$x=0; // we will use this to count to three later
$handle = opendir("images/avatar");
while(($file = readdir($handle)))
{
if (is_file("images/avatar/$file") and false !== strpos($file, '.gif') or false !== strpos($file, '.jpg') or false !== strpos($file, '.jpeg') or false !== strpos($file, '.png')){

if (($x % 4) == 0) {
$gallery .= "</tr><tr>";  // $x == 4 so we start the line again
  $x=0; // $x is now 4 so we reset it here to start the next line
}

$gallery .= "<td bgcolor=\"$table1\" align=\"center\"><img src=\"images/avatar/$file\" alt=\"$file\" title=\"$file\"><br><input type=\"radio\" name=\"avatarsel\" value=\"images/avatar/$file\" class=\"text\"></td>";
$x++; // increment $x by 1 so we get our 4
}
}
return $gallery;
}
#list board
function board_listing(){

global $db, $table2, $board_rule, $access_level, $stat, $viewboard, $num, $query, $logged_user, $bid, $template_path, $time_format, $gmt;

//check board auth.
if (($board_rule['B_Read'] == 1) AND ($access_level !== 1)){
$board = "<tr bgcolor=\"$table2\">
<td colspan=\"6\" align=\"center\" class=\"td\">$viewboard[noread]</td>
</tr>";
}
elseif (($board_rule['B_Read'] == 2) AND ($stat == "Member") OR ($access_level == 3)){
$board = "<tr bgcolor=\"$table2\">
<td colspan=\"6\" align=\"center\" class=\"td\">$viewboard[noread]</td>
</tr>";
}
elseif (($board_rule['B_Read'] == 3) AND ($stat == "guest")){
$board = "<tr bgcolor=\"$table2\">
<td colspan=\"6\" align=\"center\" class=\"td\">$viewboard[noread]</td>
</tr>";
}
elseif ($num == 0){
$board .= "<tr bgcolor=\"$table2\">
<td colspan=\"6\" align=\"center\" class=\"td\">$viewboard[nopost]</td>
</tr>";
}else{
while ($row = mysql_fetch_assoc ($query))
{
//grab posted date info
$gmttime = gmdate ($time_format, $row['last_update']);
$topic_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
$posted_by = $row["Posted_User"];
$post_link = $row["Post_Link"];
$views = $row["Views"];
//get reply number
$db->run = "select * from ebb_posts WHERE tid='$row[tid]'";
$reply_num = $db->num_results();
$db->close();
//see if the post is new to the user or not.
$db->run = "select * from ebb_read WHERE Topic='$row[tid]' and user='$logged_user'";
$read_count = $db->num_results();
$db->close();
if (($read_count == 1) OR ($stat == "guest")){
$read_status = "old";
}else{
$read_status = "new";
}
//decide on icon.
if ($row['important'] == 1){
$icon = "$template_path/images/important.gif";
}else{
if ($row['Locked'] == 1){
$icon = "$template_path/images/locked_topic.gif";
}
if ($row['Type'] == "Poll"){
$icon = "$template_path/images/poll.gif";
}elseif ($reply_num >= 15){
$icon = "$template_path/images/hottopic.gif";
}elseif ($read_status == "new"){
$icon = "$template_path/images/new.gif";
}elseif ($read_status == "old"){
$icon = "$template_path/images/old.gif";
}
}
$board .= "<tr bgcolor=\"$table2\">";
$board .= "<td class=\"td\" align=\"center\" width=\"5%\"><img src=\"$icon\"></td><td width=\"40%\" class=\"td\"><a href=\"viewtopic.php?bid=$bid&amp;tid=$row[tid]\">$row[Topic]</a></td><td width=\"15%\" class=\"td\">$row[author]</td><td width=\"10%\" align=\"center\" class=\"td\">$reply_num</td><td width=\"10%\" align=\"center\" class=\"td\">$views</td><td width=\"20%\" class=\"td\">$topic_date<br><a href=\"viewtopic.php?$post_link\">Posted by</a>: $posted_by</td>";
$board .= "</tr>";
}
}
return $board;
}
#reply listing
function reply_listing(){

global $db, $query, $gmt, $template_path, $table1, $table2, $border, $allowsmile, $allowbbcode, $allowhtml, $allowimg, $stat, $access_level, $logged_user, $viewtopic, $time_format, $bid, $tid, $board_rule, $checkmod;

while ($row = mysql_fetch_assoc ($query)) {

$db->run = "select * from ebb_users WHERE Username='$row[re_author]'";
$re_user = $db->result();
$db->close();
$re_total = $re_user['Post_Count'];
//get date
$gmttime = gmdate ($time_format, $row['Original_Date']);
$post_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
//get rank stars.
if ($re_user['Status'] == "groupmember"){
//find what usergroup this user belongs to.
$db->run = "SELECT * FROM ebb_group_users where Username='$row[re_author]'";
$groupchk = $db->result();
$db->close();
//get the access level of this group.
$db->run = "SELECT * FROM ebb_groups where id='$groupchk[gid]'";
$level_r = $db->result();
$db->close();
if($level_r['Level'] == 1){
$rankicon = "<img src=\"$template_path/images/adminstar.gif\" alt=\"$level_r[Name]\">";
$rank = $level_r['Name'];
}elseif ($level_r['Level'] == 2){
$rankicon = "<img src=\"$template_path/images/modstar.gif\" alt=\"$level_r[Name]\">";
$rank = $level_r['Name'];
}elseif($level_r['Level'] == 3){
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $re_total ORDER BY Post_req DESC";
$rank2 = $db->result();
$db->close();
//get outoput.
$rankicon = "<img src=\"$template_path/images/$rank2[Star_Image]\" alt=\"$rank\">";
$rank = $level_r['Name'];
}
}elseif($re_user['Status'] == "Banned"){
$rankicon = "";
$rank = "Banned";
}else{
$db->run = "SELECT * FROM ebb_ranks WHERE Post_req <= $re_total ORDER BY Post_req DESC";
$rank2 = $db->result();
$db->close();
$rank = $rank2[Name];
$rankicon = "<img src=\"$template_path/images/$rank2[Star_Image]\" alt=\"$rank\">";
}
//set bbcode for replies.
$string = $row['Body'];
$re_msg = $string;
//see if user wish to disable smiles in post.
if($row['disable_smiles'] == 0){
if ($allowsmile == 1){
$re_msg = smiles($re_msg);
}
}
//see if user wish to disable bbcode in post.
if($row['disable_bbcode'] == 0){
if ($allowimg == 1){
$re_msg = BBCode($re_msg, true);
}
if ($allowbbcode == 1){
$re_msg = BBCode($re_msg);
}
}
if($re_user['Avatar'] == ""){
$avatar = "images/noavatar.gif";
}else{
$avatar = $re_user['Avatar'];
}
if($re_user['Sig'] == ""){
$sig = "";
}else{
$psig = nl2br($re_user['Sig']);
$sig = "-----------<br>$psig";
}
$re_msg = language_filter($re_msg);
$re_msg = nl2br($re_msg);
if ($allowhtml == 1){
$re_msg = html_entity_decode($re_msg);
}

$reply .= "<table border=\"0\" bgcolor=\"$border\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">";
if ($checkmod == 1){
$reply .= "<tr>
<td colspan=\"2\" align=\"right\" bgcolor=\"$table1\" class=\"td\"><a href=\"manage.php?mode=viewip&amp;ip=$row[IP]&amp;u=$row[re_author]&amp;bid=$bid&amp;tid=$tid\"><img src=\"$template_path/images/ip_logged.gif\" alt=\"$viewtopic[iplogged]\" border=\"0\"></a>&nbsp;<a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_post&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td>
</tr>";
}else{
if (($logged_user == $row['re_author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1) AND ($board_rule['B_Delete'] == 1)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($access_level == 1) AND ($board_rule['B_Edit'] == 1)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($access_level == 1) AND ($board_rule['B_Delete'] == 1)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2) AND ($board_rule['B_Delete'] == 2)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Edit'] == 2)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($access_level == 1) or ($access_level == 2) AND ($board_rule['B_Delete'] == 2)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($board_rule['B_Edit'] == 3) AND ($board_rule['B_Delete'] == 3)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($board_rule['B_Edit'] == 3)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"edit.php?mode=editpost&amp;bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/edit.gif\" border=\"0\"></a></td></tr>";
}elseif (($logged_user == $row['re_author']) AND ($board_rule['B_Delete'] == 3)){
$reply .= "<tr><td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"delete.php?action=del_post&bid=$bid&amp;tid=$tid&amp;pid=$row[pid]\"><img src=\"$template_path/images/delete.gif\" border=\"0\"></a></td></tr>";
}
}
$reply .= "<tr>
<td colspan=\"2\" bgcolor=\"$table1\" class=\"td\">$viewtopic[postedon] $post_date <a name=\"$row[pid]\"></a></td>
</tr><tr>
<td width=\"15%\" bgcolor=\"$table1\" class=\"td\" valign=\"top\"><b>$row[re_author]</b><br><i>$rank</i><br>$rankicon<br><br><img src=\"$avatar\"><br><br>$viewtopic[posts]: $re_total</td><td width=\"85%\" valign=\"top\" bgcolor=\"$table2\" class=\"td\">$re_msg<br><br>$sig</td>
</tr>
<tr>
<td colspan=\"2\" align=\"right\" bgcolor=\"$table1\"><a href=\"Profile.php?mode=view&amp;user=$row[re_author]\"><img src=\"$template_path/images/profile.gif\" border=\"0\" alt=\"\"></a>&nbsp;<a href=\"PM.php?action=write&amp;user=$row[re_author]\"><img src=\"$template_path/images/pm.gif\" border=\"0\" alt=\"\"></a>&nbsp;<a href=\"report.php?mode=post&amp;pid=$row[pid]\"><img src=\"$template_path/images/report.gif\" border=\"0\"></a></td>
</tr></table><br>";
}
return $reply;
}
#printable reply listing
function reply_listing_print(){

global $query, $gmt, $time_format;

while ($row = mysql_fetch_assoc ($query)) {
//get date
$gmttime = gmdate ($time_format, $row['Original_Date']);
$post_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
//set bbcode for replies.
$string = $row['Body'];
$re_msg = $string;
$re_msg = smiles($re_msg);
$re_msg = BBCode_print($re_msg);
$re_msg = language_filter($re_msg);
$re_msg = nl2br($re_msg);
//output the replies.
$print_reply .= "<hr><p class=\"td\">". $row['re_author'] . "&nbsp;-&nbsp;" . $post_date . "</p><p class=\"td\">" . $re_msg ."</p>";
}
return $print_reply;
}
#im inbox function
function pm_inbox(){

global $table2, $pm, $num, $query, $time_format, $gmt, $template_path;

if ($num == "0"){
$inbox = "<td colspan=\"4\" bgcolor=\"$table2\" class=\"td\">$pm[nopm]</td>";
}
while ($row = mysql_fetch_assoc($query)) {

$gmttime = gmdate ($time_format, $row['Date']);
$pm_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
if ($row['Read_Status'] == "old"){
$icon = "$template_path/images/old.gif";
}else{
$icon = "$template_path/images/new.gif";
}

$inbox .= "<tr bgcolor=\"$table2\">
<td align=\"center\" class=\"td\"><img src=\"$icon\" alt=\"\"></td><td class=\"td\"><a href=\"PM.php?action=read&id=$row[id]\">$row[Subject]</a></td><td class=\"td\"><a href=\"Profile.php?mode=view&user=$row[Sender]\">$row[Sender]</a></td><td class=\"td\">$pm_date</td>
</tr>";
}
return $inbox;
}
#banlist
function view_banlist(){

global $border, $table1, $table2, $pm, $cp, $logged_user;

$sql = "SELECT * FROM ebb_pm_banlist WHERE Ban_Creator='$logged_user'";
$errorq = $sql;
$banlistquery = mysql_query ($sql) or die(error($error, "mysql", $errorq));
$ban_num = mysql_num_rows ($banlistquery);

if ($ban_num == "0"){
$banlist = "<td class=\"td\" colspan=\"2\" align=\"center\" bgcolor=\"$table2\">$pm[noban]</td></table>";
}
else{
while ($row = mysql_fetch_assoc($banlistquery)) {

$banlist .= "<tr bgcolor=\"$table2\">
<td class=\"td\">$row[Banned_User]</td><td class=\"td\"><a href=\"PM.php?action=del_ban&id=$row[id]\">$cp[delete]</a></td></tr>";
}
}
return $banlist;
}
#view poll function
function view_poll(){

global $bid, $tid, $border, $table1, $table2, $pollbox;

//pick-up question of this poll.
$sql = "SELECT * FROM ebb_topics WHERE tid='$tid'";
$errorq = $sql;
$question_query =  mysql_query ($sql) or die(error($error, "mysql", $errorq));
$question_result = mysql_fetch_array ($question_query);
//get poll options
$sql = "SELECT * FROM ebb_poll WHERE tid='$tid'";
$errorq = $sql;
$res = mysql_query ($sql) or die(error($error, "mysql", $errorq));
//output the poll form.
$poll = "<form  method=\"post\" action=\"vote.php?bid=$bid&amp;tid=$tid\">
<table border=\"0\" bgcolor=\"$border\" width=\"100%\" cellpadding=\"1\" cellspacing=\"1\">
<tr bgcolor=\"$table1\">
<td align=\"center\" class=\"td\">$question_result[Question]</td>
</tr><tr bgcolor=\"$table1\">
<td align=\"center\" class=\"td\">";
while ($i = mysql_fetch_assoc($res)){
$poll .= "<input type=\"radio\" name=\"vote\" value=\"$i[option_id]\" class=\"text\">$i[Option]<BR>";
}
$poll .= "</td></tr>
<tr bgcolor=\"$table1\">
<td align=\"center\" class=\"td\">
<input type=\"hidden\" name=\"user_name\" value=\"$logged_user\">
<input type=\"submit\" name=\"submit\" value=\"$pollbox[vote]\" class=\"submit\"></td>
</table></form>";

return $poll;
}
#view poll results function
function view_results(){

global $tid, $border, $table1, $table2, $pollbox, $template_path;

//count how many votes were casted
$sql = "SELECT * FROM ebb_votes WHERE tid='$tid'";
$errorq = $sql;
$sql = mysql_query($sql) or die(error($error, "mysql", $errorq));
$poll_count = mysql_num_rows($sql);
//pick up the info on the poll.
$sql = "SELECT * FROM ebb_poll WHERE tid='$tid'";
$errorq = $sql;
$poll_questions = mysql_query($sql) or die(error($error, "mysql", $errorq));
//pick-up question of this poll.
$sql = "SELECT * FROM ebb_topics WHERE tid='$tid'";
$errorq = $sql;
$question_query =  mysql_query ($sql) or die(error($error, "mysql", $errorq));
$question_result = mysql_fetch_array ($question_query);
//begin to output poll.
$poll = "<table border=\"0\" bgcolor=\"$border\" width=\"100%\" cellpadding=\"1\" cellspacing=\"1\">
<tr bgcolor=\"$table1\">
<td align=\"center\" class=\"td\">$question_result[Question]</td>
</tr><tr bgcolor=\"$table1\">
<td align=\"center\" class=\"td\">";
while ($i = mysql_fetch_assoc($poll_questions)){
//grab results.
$sql = "SELECT * FROM ebb_votes WHERE Vote='$i[option_id]' AND tid='$tid'";
$errorq = $sql;
$poll_results_q = mysql_query ($sql) or die(error($error, "mysql", $errorq));
$poll_results = mysql_num_rows($poll_results_q);
//get percentage.
$VotePercent = Round(($poll_results / $poll_count) * 100);
//output results.
$poll .= "<b>$i[Option]</b> $VotePercent%<br> <img src=\"$template_path/images/bar.gif\" width=\"$VotePercent\" height=\"5\" alt=\"$VotePercent\"><br>";
}
$poll .= "<br>$pollbox[total]: <b>$poll_count</b>
</td></tr>
</table>";

return $poll;
}
#error logging
function error($error, $type, $errorq = 'N/A'){

global $title, $border, $table1, $table2, $txt, $db, $board_email;
if (($error == "") and ($type == "")){
echo "Function not defined correctly!";
exit();
}
switch($type){
case 'error':
echo "<a href=\"index.php\">$title</a> &#187; $txt[error]<br><br>
<table border=\"0\" width=\"100%\" bgcolor=\"$border\" cellspacing=\"1\" cellpadding=\"1\">
<tr bgcolor=\"$table1\">
<td class=\"td\"align=\"center\">$txt[error]</td></tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\">$error</td></tr>
</table><br>
-<a href=\"mailto:$board_email\">$txt[errorinfo]</a>";
exit();
break;
case 'general':
echo "<a href=\"index.php\">$title</a> &#187; $txt[info]<br><br>
<table border=\"0\" width=\"100%\" bgcolor=\"$border\" cellspacing=\"1\" cellpadding=\"1\">
<tr bgcolor=\"$table1\">
<td class=\"td\"align=\"center\">$txt[info]</td></tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\">$error</td></tr>
</table><br>";
exit();
break;
case 'mysql':
$error = "Failed to query the database.<br>". mysql_error();
echo "<a href=\"index.php\">$title</a> &#187; $txt[error]<br><br>
<table border=\"0\" width=\"100%\" bgcolor=\"$border\" cellspacing=\"1\" cellpadding=\"1\">
<tr bgcolor=\"$table1\">
<td class=\"td\"align=\"center\">$txt[error]</td></tr>
<tr bgcolor=\"$table2\">
<td class=\"td\" align=\"center\">$error<br><br><strong>Query Used:</strong><br><textarea name=\"sqlquery\" rows=\"5\" cols=\"150\" class=\"text\">$errorq</textarea></td></tr>
</table><br>
-<a href=\"mailto:$board_email\">$txt[errorinfo]</a>";
exit();
break;
}
}
#mysql class-making queries easier
class db {
  var $run;
  var $standby = false;

  function connect() {
  global $DB_HOST, $DB_USER, $DB_PASS;
  global $DB_NAME;
     @mysql_connect($DB_HOST, $DB_USER, $DB_PASS) or die("Failed to connect to MySQL host.<br>". mysql_error() ."<br><br><strong>Line:</strong> ". __LINE__ ."<br><strong>File:</strong> ". __FILE__);
     @mysql_select_db($DB_NAME) or die("Failed to select mysql DB $DB_NAME<br>". mysql_error() ."<br><br><strong>Line:</strong> ". __LINE__ ."<br><strong>File:</strong> ". __FILE__);
	return($connect);
  }

  function close() {
    @mysql_close($this->connect());
  }

  function query() {
  $this->connect();
  $errorq = $this->run;
    $query = @mysql_query($this->run) or die(error($error, "mysql", $errorq));
	if ($this->standby == false) {
	  $this->close();
	}
	return($query);
  }

  function num_results() {
  $this->connect();
    $total = @mysql_num_rows($this->query());
	return($total);
  }

  function result() {
  $this->connect();
    $result = @mysql_fetch_assoc($this->query());
	return($result);
  }
}
require "phpmailer/class.phpmailer.php";
#mail class-entends the class created by phpmailer.
class ebbmail extends PHPMailer
{
    var $priority = 3;
    var $to_name;
    var $to_email;
    var $From = null;
    var $FromName = null;
    var $Sender = null;

function ebbmail(){

global $db;
//get setting values for smtp.
$db->run = "select mail_type,smtp_server,smtp_port,smtp_user,smtp_pass,Board_Email,Site_Title from ebb_settings";
$smtp = $db->result();
$db->close();
//setup smtp server connection.
if($row['mail_type'] == 0){
$this->Host = $smtp['smtp_server'];
$this->Port = $smtp['smtp_port'];
//setup the login details.
if($row['smtp_user'] !== ""){
$this->SMTPAuth  = true;
$this->Username  = $smtp['smtp_user'];
$this->Password  = $smtp['smtp_pass'];
}
$this->Mailer = "smtp";
}
//setup from address.
if(!$this->From){
$this->From = $smtp['Board_Email'];
}
//setup from name.
if(!$this->FromName){
$this-> FromName = $smtp['Site_Title'];
}
//setup sender address.
if(!$this->Sender){
$this->Sender = $smtp['Board_Email'];
}
//set priority of email.
$this->Priority = $this->priority;
}
}
?>
