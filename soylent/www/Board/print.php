<?
define('IN_EBB', true);
/*
Filename: print.php
Last Modified: 6/2/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
//get topic IDs.
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
//topic & board query.
$db->run = "select * FROM ebb_topics WHERE tid='$tid'";
$checkboard = $db->num_results();
$t_name = $db->result();
$db->close();
$db->run = "select * FROM ebb_boards WHERE id='$bid'";
$checktopic = $db->num_results();
$db->close();
//check to see if topic exists or not and if it doesn't kill the program.
if (($checkboard == 0) or ($checktopic == 0)){
$error = $viewtopic['doesntexist'];
echo error($error, "error");
}
//convert over any thing extra in the topic.
$string = $t_name['Body'];
$msg = $string;
$msg = smiles($msg);
$msg = BBCode_print($msg);
$msg = language_filter($msg);
$msg = nl2br($msg);
//get topic details.
$gmttime = gmdate ($time_format, $t_name['Original_Date']);
$topic_date = date($time_format,strtotime("$gmt hours",strtotime($gmttime)));
//get replies, if any.
$db->run = "select * from ebb_posts WHERE tid='$tid'";
$query = $db->query();
$db->close();
$print_reply = reply_listing_print();
//output
$allsmile = showall_smiles();
$page = new template($template_path ."/print.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$viewtopic[ptitle]",
  "SUBJECT" => "$t_name[Topic]",
  "TOPIC-DATE" => "$topic_date",
  "AUTHOR" => "$t_name[author]",
  "TOPIC" => "$msg",
  "REPLY" => "$print_reply"));

$page->output();
?>
