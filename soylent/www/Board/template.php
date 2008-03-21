<?
if (!defined('IN_EBB') ) {
die("<B>!!ACCESS DENIED HACKER!!</B>");
}
/*
Filename: template.php
Last Modified: 5/31/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
class template
{
  var $page;

  function template($template) {
    if (file_exists($template))
      $this->page = join("", file($template));
    else
      die("Template file $template was not found.");
  }

   function parse($file) {
    ob_start();
    include($file);
    $buffer = ob_get_contents();
    ob_end_clean();
    return $buffer;
  }

  function replace_tags($tags = array()) {
    if (sizeof($tags) > 0){
      foreach ($tags as $tag => $data) {
        $this->page = eregi_replace("{" . $tag . "}", $data,
                      $this->page);
        }
    }else{
      die("No tags designated for replacement.");
    }
  }

  function output() {
    echo $this->page;
  }
}
?>
