<?
define('IN_EBB', true);
/*
Filename: smiles.php
Last Modified: 3/17/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
//output
$allsmile = showall_smiles();
$page = new template($template_path ."/smiles.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "LANG-TITLE" => "$post[moresmiles]",
  "BORDER" => "$border",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "TABLE1"=> "$table1",
  "TABLE2"=> "$table2",
  "LANG-TEXT" => "$post[smiletxt]",
  "SMILE" => "$allsmile",
  "LANG-CLOSEWINDOW" => "$txt[closewindow]"));

$page->output();
?>
