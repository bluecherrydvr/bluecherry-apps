<?php DEFINE('INDVR', true);

/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#lib


include("/usr/share/bluecherry/www/lib/lib.php");  #common functions

if (empty($argv[1]) || empty($argv[2])){
	exit("E: Usage: ${argv[0]} <event class> <arg>...");
}

#PEAR::Mail mailer
include_once('Mail.php');
include_once('Mail/mime.php');

$crlf = "\r\n";
$mime = new Mail_mime($crlf);

$timestamp = time();

if ($argv[1] == "motion_event") {
	$event = data::getObject('Media', 'id', $argv[2]);

	$timestamp = $event[0]['start'];
	$device_id = $event[0]['device_id'];
	#get device details
	$device = data::getObject('Devices', 'id', $device_id);

} else if ($argv[1] == "device_state") {
	$device_id = $argv[2];
	$state = $argv[3];  // e.g. OFFLINE, BACK ONLINE
	#get device details
	$device = data::getObject('Devices', 'id', $device_id);

	$subject = "Status notification for device {$device[0]['device_name']} on server {$global_settings->data['G_DVR_NAME']}";
	$html = "
	<html>
		<body>
Device {$device[0]['device_name']} got $state on server {$global_settings->data['G_DVR_NAME']}
		</body>
	</html>";
} else if ($argv[1] == "solo") {
	$state = $argv[2];
	$subject = "Status notification for Bluecherry SOLO card(s) on server {$global_settings->data['G_DVR_NAME']}";
	$html = "
	<html>
		<body>
SOLO card(s) got $state on server {$global_settings->data['G_DVR_NAME']}
		</body>
	</html>";
} else {
	exit('E: Unknown event type');
}

$dow = array('M', 'T', 'W', 'R', 'F', 'S', 'U');

#parse start timestamp into dow/hour/minute
$p['day'] = $dow[date('w', $timestamp)-1];
$p['hour'] = date('G', $timestamp);
$p['min'] = date('i', $timestamp);


#form a query and select applicable rules
$query = "SELECT * FROM notificationSchedules 
	WHERE ";
if (isset($device_id))
$query .= "
		cameras LIKE '%|$device_id|%' 
	AND ";
$query .= "
		day LIKE '%{$p['day']}%' 
	AND 
	(
		(s_hr = {$p['hour']} AND s_min < {$p['min']}) 
		OR 
		(e_hr = {$p['hour']} AND e_min > {$p['min']}) 
		OR 
		(s_hr < {$p['hour']} AND e_hr > {$p['hour']})
	)
	AND
		disabled = 0
";
$rules = data::query($query);
if (!$rules){
	exit('I: No matching rules');
} else {

    if ($argv[1] == "motion_event") {
        #get path to image
        $path_to_image = str_replace('mkv', 'jpg', $event[0]['filepath']);

        # Check the case of such short recording that snapshotting delay hasn't
        # come, so making a snapshot afterwards
        if (!file_exists($path_to_image)) {
            $cmd = 'LD_LIBRARY_PATH=/usr/lib/bluecherry/ /usr/lib/bluecherry/ffmpeg'
                .' -ss '.$device[0]['buffer_prerecording']
                .' -i '.$event[0]['filepath'].' -frames 1 '.$path_to_image
                .' 2>&1 && echo SUCCEED';
            $output = shell_exec($cmd);
            if (strstr($output, 'SUCCEED') === false) {
                echo($cmd);
                echo($output);
                exit('E: ffmpeg snapshot creation process failed');
            }
        }

        $image_name = data::getRandomString(8);
        $subject = "Event on device {$device[0]['device_name']} on server {$global_settings->data['G_DVR_NAME']}";
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
                <img height='240' src='{$image_name}.jpg'>
            </div>
            </body>
        </html>";
        $mime->addHTMLImage($path_to_image, "image/jpeg", "{$image_name}.jpg", true, $image_name);
    }
}
$headers = array("From"=>$global_settings->data['G_SMTP_EMAIL_FROM'], "Subject" => $subject);
$mime->setHTMLBody($html);
$headers = $mime->headers($headers);
$body = $mime->get();


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
	$tmp = explode('|', $user[0]['email']);
	$emails = array_merge($emails, $tmp);
}
$emails = array_unique($emails);

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

$error = null;
foreach($emails as $email){
		$re = $mail->send($email, $headers, $body); 
		if ($re !== TRUE)
			$error = $re;
}

if (PEAR::isError($error)) {
	$tmp = data::query("UPDATE GlobalSettings set value='{$error->getMessage()}' WHERE parameter='G_SMTP_FAIL'", true);
	exit("E: ".$error->getMessage());
} else {
	$tmp = data::query("UPDATE GlobalSettings set value='' WHERE parameter='G_SMTP_FAIL'", true);
	exit('OK');
}
?>
