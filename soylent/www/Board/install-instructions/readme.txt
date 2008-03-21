install guide for Elite Bulletin Board version 1.0:

This readme file will help you install Eliter Bulletin Board.


=====
PRESTEPS. .zip file

The .zip file contains everything you will need in order to install and run xdb. The only thing you DON'T upload is the install-instructions floder or anything in there. Everything else will go into your website.


Now extract everything out of the zip and place it into a folder. I suggest Board or something like that, and then edit the config file.

NOTE: the spell checker now has a add to dictionary feature, but if your a *nix user, you have to chmos the personal dictionary.txt file to 646.
=====
STEP 1. edit config.php

this file basically just does all of your connecting. It only supports MySQL at this moment.
you will need to edit:

$DB_HOST = "localhost"; //usually this is localhost if it isnt ask your provider
$DB_NAME = "ebb"; //Name of your Database
$DB_USER = "user"; //Username of Database
$DB_PASS = "pass"; //Password of database
=====
STEP 2. upload everything to your web server

upload everything in your board directory to your website.

*nix users: CHMOD the folder 777 so that the install files can get deleted after the install is finished, otherwise you'll have to remove them yourself. if you don't know how to CHMOD, contact me in the community board and I'll help you with it.
=====
STEP 3. run install.php

this file will:

*check to see if a version of this is already installed.
*import all tables and default data into your database.
*setup the settings
*register yourself on the board
*add one category on your site, just so you can start it and see how it's done.
*add one board on your site, just so you can start it and see how it's done.
*delete the install files(if you chmoded the folder to 777)

If you plan to do a clean install, click on the install link, but if you already have the previous version installed, then click on the upgrade link.
=====
STEP 4. enjoy the board

enjoy your board and get some members and post away!

=====
if you run into any trouble, go to http://elite-board.us and click on Community Board, that is the support board and also where you may post other things as well.