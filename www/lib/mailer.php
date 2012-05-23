<?php DEFINE('INDVR', true);

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

#lib


include("/usr/share/bluecherry/www/lib/lib.php");  #common functions

if (empty($argv[1])){
	exit('E: No argument, argv[1] is expcted to contain event id');
}
$dow = array('M', 'T', 'W', 'R', 'F', 'S', 'U');

#get the event data
$event = data::getObject('Media', 'id', $argv[1]);

#parse start timestamp into dow/hour/minute
$p['day'] = $dow[date('w', $event[0]['start'])-1];
$p['hour'] = date('G', $event[0]['start']);
$p['min'] = date('i', $event[0]['start']);

#get path to image
$path_to_image = str_replace('mkv', 'jpg', $event[0]['filepath']);

#form a query and select applicable rules
$query = ("SELECT * FROM notificationSchedules 
	WHERE 
		cameras LIKE '%|{$event[0]['device_id']}|%' 
	AND 
		day LIKE '%{$p['day']}%' 
	AND 
	(
		(s_hr = {$p['hour']} AND s_min < {$p['min']}) 
		OR 
		(e_hr = {$p['hour']} AND e_min > {$p['min']}) 
		OR 
		(s_hr < {$p['hour']} AND e_hr > {$p['hour']})
	)
");
$rules = data::query($query);
if (!$rules){
	exit('I: No matching rules');
}
$rules_ids = '';

#clean up old (>1hr) notificationsSent records
$tmp = data::query('DELETE FROM notificationsSent WHERE time < '.(time()-3600), true);

#now get users who should get notified, and get the list of rules involved
$users = array();

foreach($rules as $id => $rule){
	$emails_sent = data::query("SELECT count(*) as n FROM notificationsSent WHERE rule_id={$rule['id']}");
	if ($emails_sent[0]['n']<$rule['nlimit'] || $rule['nlimit']==0){ #if we are below limit(or unlim), use the rule, check next
		$rules_ids[$id] = $rule['id'];
		$tmp = trim($rule['users'], '|');
		$tmp = explode('|', $tmp);
		$users = array_merge($users, $tmp);
		$tmp = data::query("INSERT INTO notificationsSent VALUES({$rule['id']}, ".time().")", true);
	}
}

if (empty($rules_ids)){
	exit('I: All applicable rules exhausted limit');
}


#get rid of user duplicates, if rules are overlapping
$users = array_unique($users);

#get email addresses for each user involved
$emails = array();

foreach($users as $id => $user){
	$user = data::getObject('Users', 'id', $user);
	$es = explode('|', $user[0]['email']);
	$tmp = array();
	foreach($es as $id => $e){
		$tmp[$id] = explode(':', $e);
		$tmp[$id] = $tmp[$id][0];
	}
	$emails = array_merge($emails, $tmp);
}
$emails = array_unique($emails);

#get device details -- only name is currently used
$device = data::getObject('Devices', 'id', $event[0]['device_id']);

#PEAR::Mail mailer

include_once('Mail.php');
include_once('Mail/mime.php');

$html = "
<html>
	<head>
		<style>
			table {
				border-collapse:collapse;
			}
			td {
				border:1px solid gray;	
			}
			td.desc {
				width:120px;
				text-align:right;
			}
			div.screenshot{
				margin-top:30px;
			}
		</style>
	</head>
	<body>
		<div>Source:
			<table>
				<tr><td class='desc'>Server:</td><td>{$global_settings->data['G_DVR_NAME']}</td></tr>
				<tr><td class='desc'>Device:</td><td>{$device[0]['device_name']}</td></tr>
				<tr><td class='desc'>Time:</td><td>".date('r', $event[0]['start'])."</td></tr>
			</table>
		</div>
		<div class='screenshot'>
			Event screenshot:<br />
			<img height='240' src='image.jpg'>
		</div>
		</body>
</html>"; 
$crlf = "\r\n"; 
$headers = array("From"=>"donotreply@bluecherryserver", "Subject"=>"Event on device {$device[0]['device_name']} on server {$global_settings->data['G_DVR_NAME']}");
$mime = new Mail_mime($crlf);  
$mime->setHTMLBody($html);  
$mime->addHTMLImage($path_to_image, "image/jpeg", "image.jpg", true, "camimage"); 
$headers = $mime->headers($headers);
$body = $mime->get();
switch($global_settings->data['G_SMTP_SERVICE']){
	case 'default': #use MTA
		$mail = Mail::factory('mail');
	break;
	case 'smtp': #user user supplied SMTP config
		$smtp_params['host'] = (($global_settings->data['G_SMTP_SSL'] == 'none') ? '' : 'ssl://').$global_settings->data['G_SMTP_HOST'];
		$smtp_params['port'] = $global_settings->data['G_SMTP_PORT'];
		$smtp_params['username'] = $global_settings->data['G_SMTP_USERNAME'];
		$smtp_params['password'] = $global_settings->data['G_SMTP_PASSWORD'];
		$smtp_params['auth'] = true;
		$mail = Mail::factory('smtp', $smtp_params);
		if ($global_settings->data['G_SMTP_SSL'] == 'tls') {
			$mail->SMTPSecure = "ssl";
		};
	break;
}

foreach($emails as $email){
		$mail->send($email, $headers, $body); 
}

if (PEAR::isError($mail)) {
	$tmp = data::query("UPDATE GlobalSettings set value='{$mail->getMessage()}' WHERE parameter='G_SMTP_FAIL'", true);
	exit("E: ".$mail->getMessage());
} else {
	$tmp = data::query("UPDATE GlobalSettings set value='' WHERE parameter='G_SMTP_FAIL'", true);
	exit('OK');
}
?>
