<?
define('IN_EBB', true);
/*
Filename: color_picker.php
Last Modified: 12/20/2005

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
include "header.php";

$page = new template($template_path ."/header.htm");
$page->replace_tags(array(
  "TITLE" => "$title",
  "PAGETITLE" => "$color[title]",
  "BGCOLOR" => "$bg_color",
  "TEXTCOLOR" => "$text_color",
  "LINKCOLOR" => "$link",
  "VLINKCOLOR" => "$vlink",
  "FONTFACE" => "$font"));

$page->output();
//display color chart
$page = new template($template_path ."/colorpicker.htm");
$page->replace_tags(array(
  "TEXT" => "$color[pickertext]",
  "LANG-PICKINGCOLOR" => "$color[pickingcolor]",
  "LANG-CLOSEWINDOW" => "$txt[closewindow]"));

$page->output();
//display footer
$page = new template($template_path ."/footer.htm");
$page->replace_tags(array(
  "LANG-POWERED" => "$index[poweredby]"));
$page->output();
?>
