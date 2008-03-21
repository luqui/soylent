<?
if (!defined('IN_EBB') ) {
die("<B>!!ACCESS DENIED HACKER!!</B>");
}
//email message for PM Notify.
$pm_subject = "New PM in your Inbox";
$pm_message = "Hello $pm_data[Reciever],

$pm_data[Sender] has written you an PM titled, $pm_data[Subject].

$board_address/PM.php?action=read&id=$pm_data[id]

If you wish to stop recieving these notices, just edit your profile.";
//email message for digest subscription.
$digest_message = "hi $digest[username],

You have recieved this because $topic[re_author] has replied to $name[Topic].

$board_address/viewtopic.php?bid=$topic[bid]&tid=$topic[tid]&pid=$topic[pid]#$topic[pid]

There may be other replies to this topic, but you will not recieve any new emails until you view this topic.

If you wish to stop recieving notices about this topic, go to your control panel and click on Subscription Settings.

regards,

$title Staff";
//email message for registering account.
$register_subject = "New account created at $title";
$register_message = "Welcome new user,

You just joined $title. here is your login info:

User: $username
Pass: $password

to login go here:
$board_address/login.php

remember your password as it is encrypted into the database and if you forget, you will have to get a new password.

If you wish to add an avatar to your profile, you will have to login first, then go to your profile and click on Avatar Settings.

if you have any questions please email the admin of the board. This is a automated email, do not reply.

$title Staff";

//lost password email.
$lost_subject = "Password Recovery";
$lost_message = "hello,

you are recieving this email because you requested for your password from: $title, if you did not ask for this, send this email to your admin so that they can do something about it.

for security reasons, a new password was made for you, below is that new password.

$random_password

to change it, go to your profile.

$title staff";
//report post email.
$report_subject = "Reported Post Alert";
$report_topic_msg = "Hello,

It has come to out attention that a user abusing the board. Below is what the reported has written:

Reason for report: $reason
Message: $msg

the topic can be found at:

$board_address/viewtopic.php?bid=$t[bid]&tid=$tid";
$report_post_msg = "Hello,

It has come to out attention that a user abusing the board. Below is what the reported has written:

Reason for report: $reason
Message: $msg

the topic can be found at:

$board_address/viewtopic.php?bid=$t[bid]&tid=$tid#$pid";
//email verify for user.
$user_verify_subject = "Verify Board Account";
$user_verify_msg = "Hello,

You are recieving this because you have registered an account on $title. Below is your login info.

User: $username
Pass: $password

But before you can login you have to activate your account by clicking on the link below.

$board_address/login.php?mode=verify_acct&u=$username&key=$act_key

If you don't want to be a member of the board, please contact the admin of the board so they can delete the account.

Regards,

$title staff";
//email verify for admin.
$admin_verify_subject = "Board Account Under Review";
$admin_verify_msg = "Hello,

You are recieving this because you have registered an account on $title.

Before you can do anything on this board, the administrator has requested that they check your account first. When
they make their decision, you will get an email wheather they accept you or not.

Regards,

$title staff";
//email to inform of approved review.
$acct_pass_subject = "Account Approved";
$acct_pass_msg = "Hello,

You have been approved to become a member of $title. You may login at:

$board_address/login.php

if you dont remember your login details, ask the admin to find it for you.

Regards,

$title staff";
//email to inform of denied review.
$acct_fail_subject = "Account Denied";
$acct_fail_msg = "Hello,

I am sorry to say the admin at $title has rejected your request to join the board.

To find out why, you may contact them and ask them why they made that choice.

Regards,

$title staff";
?>
