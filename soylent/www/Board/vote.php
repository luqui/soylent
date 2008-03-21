<?
define('IN_EBB', true);
/*
Filename: vote.php
Last Modified: 6/2/2006

Term of Use:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/
include "config.php";
include "header.php";
$bid = anti_injection($_GET['bid']);
$tid = anti_injection($_GET['tid']);
$vote = anti_injection($_POST['vote']);
//perform query.
$db->run = "insert into ebb_votes (Username, tid, Vote) values ('$logged_user', '$tid', '$vote')";
$db->query();
$db->close();
//direct user back to topic.
header ("Location: viewtopic.php?bid=$bid&tid=$tid");

ob_end_flush();
?>
