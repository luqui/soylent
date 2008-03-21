<?
define('IN_EBB', true);
/*
Filename: logout.php
Last Modified: 5/1/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
require "header.php";
//remove user from who's online list.
$db->run = "delete from ebb_online where Username='$logged_user'";
$db->query();
$db->close();
//decide which login method to delete.
if(isset($_COOKIE['ebbuser'])){
//remove cookie.
$currtime = time() - (2592000);
setcookie("ebbuser", $username, $currtime, $cookie_path, $cookie_domain, $cookie_secure);
setcookie("ebbpass", $password, $currtime, $cookie_path, $cookie_domain, $cookie_secure);
}else{
//clear session data.
session_destroy();
}
//direct to index page.
header("Location: index.php");
ob_end_flush();
?>
