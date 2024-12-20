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

require_once '/usr/share/bluecherry/www/vendor/autoload.php';




$timestamp = time();

function webhook_trigger($eventName, $deviceId = null, $payload = array()) {
	$webhooks = data::query('SELECT * FROM `webhooks` WHERE `status` = 1');

	if (empty($webhooks)) {
		return;
	}

	$handlers = array();
	$cct = curl_multi_init();

	foreach ($webhooks as $webhook) {

		$events = empty($webhook['events']) ? array() :  explode(',', $webhook['events']);

		// empty event value means all events
		if (!empty($events) && !in_array($eventName, $events)) {
			continue;
		}

		$devices = empty($webhook['cameras']) ? array() :  explode(',', $webhook['cameras']);

		// empty event value means all devices
		if (!empty($deviceId) && !empty($devices) && !in_array($deviceId, $devices)) {
			continue;
		}

		$uuidParam = data::query('SELECT `value` FROM `GlobalSettings` WHERE `parameter` = \'G_SERVER_UUID\' LIMIT 1');

		if (!empty($uuidParam)) {
			$payload['server_uuid'] = $uuidParam[0]['value'];
		}

		$body = json_encode(array_merge(array(
			'event_name' => $eventName,
			'event_datetime' => date(DATE_ATOM)
		), $payload));

		$ch = curl_init($webhook['url']);
		curl_setopt_array($ch, array(
			CURLOPT_CUSTOMREQUEST => 'POST',
			CURLOPT_POSTFIELDS => $body,
			CURLOPT_RETURNTRANSFER => true,
			CURLOPT_TIMEOUT => 5,
			CURLOPT_HTTPHEADER => array(
				'Content-Type: application/json',
				'Content-Length: ' . strlen($body))
			));

		curl_multi_add_handle($cct, $ch);
		$handlers[] = $ch;
	}

	if (empty($handlers)) {
		curl_multi_close($cct);
		return;
	}

	do {
		curl_multi_exec($cct, $active);
		curl_multi_select($cct);
	} while ($active);

	foreach ($handlers as $handler) {
		curl_multi_remove_handle($cct, $handler);
	}

	curl_multi_close($cct);
}



if ($argv[1] == "motion_event") {
	$event = data::getObject('Media', 'id', $argv[2]);

	$timestamp = $event[0]['start'];
	$device_id = $event[0]['device_id'];
	#get device details
	$device = data::getObject('Devices', 'id', $device_id);

	$event_cam = data::getObject('EventsCam', 'media_id', $argv[2]);

	if ($event_cam[0]['type_id'] == 'motion') {
		webhook_trigger('motion_event', $device_id, array(
			'device_id' => $device_id,
			'device_name' => $device[0]['device_name'],
			'dvr_name' => $global_settings->data['G_DVR_NAME']
		));
	}

} else if ($argv[1] == "device_state") {
	$device_id = $argv[2];
	$state = $argv[3];  // e.g. OFFLINE, BACK ONLINE
	#get device details
	$device = data::getObject('Devices', 'id', $device_id);

	webhook_trigger('device_state', $device_id, array(
		'device_id' => $device_id,
		'device_name' => $device[0]['device_name'],
		'state' => $state,
		'dvr_name' => $global_settings->data['G_DVR_NAME']
	));

	$subject = "Status notification for device {$device[0]['device_name']} on server {$global_settings->data['G_DVR_NAME']}";
	$message = new \Swift_Message($subject);

    if (!empty($global_settings->data['G_SMTP_EMAIL_FROM'])) {
		$message->setFrom($global_settings->data['G_SMTP_EMAIL_FROM']);
    }

	$message->setBody("
	<html>
		<body>
Device {$device[0]['device_name']} got $state on server {$global_settings->data['G_DVR_NAME']}
		</body>
	</html>", 'text/html');
} else if ($argv[1] == "solo") {
	$state = $argv[2];

	webhook_trigger('solo', null, array(
		'state' => $state,
		'dvr_name' => $global_settings->data['G_DVR_NAME']
	));

	$subject = "Status notification for Bluecherry SOLO card(s) on server {$global_settings->data['G_DVR_NAME']}";
    
	$message = new \Swift_Message($subject);

    if (!empty($global_settings->data['G_SMTP_EMAIL_FROM'])) {
		$message->setFrom($global_settings->data['G_SMTP_EMAIL_FROM']);
    }
        
	$message->setBody("
	<html>
		<body>
SOLO card(s) got $state on server {$global_settings->data['G_DVR_NAME']}
		</body>
	</html>", 'text/html');
} else {
	exit('E: Unknown event type');
}

$dow = array('M', 'T', 'W', 'R', 'F', 'S', 'U');

#parse start timestamp into dow/hour/minute
$p['day'] = $dow[(date('w', $timestamp) + 6) % 7];
$p['hour'] = date('G', $timestamp);
$p['min'] = date('i', $timestamp);

#form a query and select applicable rules
$query = "SELECT * FROM notificationSchedules 
    WHERE ";
if (isset($device_id)) {
    $query .= " cameras LIKE '%|$device_id|%' AND ";
}
$query .= " day LIKE '%{$p['day']}%' AND 
    (
        (s_hr = {$p['hour']} AND s_min < {$p['min']}) 
        OR 
        (e_hr = {$p['hour']} AND e_min > {$p['min']}) 
        OR 
        (s_hr < {$p['hour']} AND e_hr > {$p['hour']})
    )
    AND disabled = 0
";
$rules = data::query($query);

if (!$rules) {
# If there are no rules (yet) don't display anything bad.
} else {

    if ($argv[1] == "motion_event") {
        #get path to image
        $path_to_image = str_replace('mp4', 'jpg', $event[0]['filepath']);

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
        
		$message = new \Swift_Message($subject);

        if (!empty($global_settings->data['G_SMTP_EMAIL_FROM'])) {
            $message->setFrom($global_settings->data['G_SMTP_EMAIL_FROM']);
        }
            

		$attachment = \Swift_Image::fromPath($path_to_image)
			->setContentType('image/jpeg')
			->setFilename($image_name . '.jpg')
			->setDisposition('inline');
        $message->setBody("
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
                <img height='240' src='{$message->embed($attachment)}'>
            </div>
            </body>
        </html>", 'text/html');
    }
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
#	exit('I: All applicable rules exhausted limit');
# Don't complain if no rules are setup
	exit ();
}


#get rid of user duplicates, if rules are overlapping
$users = array_unique($users);

#get email addresses for each user involved
$emails = array();

foreach($users as $id => $user){
	$user = data::getObject('Users', 'id', $user);
	$emails = array_merge($emails, explode('|', $user[0]['email']));
}
if (empty($emails)) {
	exit(); // Nobody to notify
}

$message->setTo(array_unique($emails));

switch($global_settings->data['G_SMTP_SERVICE']){
	case 'default': #use MTA
		$transport = new \Swift_SendmailTransport('/usr/sbin/sendmail -bs');
	break;
	case 'smtp': #user user supplied SMTP config

		$transport = (new \Swift_SmtpTransport($global_settings->data['G_SMTP_HOST']))
			->setUsername($global_settings->data['G_SMTP_USERNAME'])
			->setPassword($global_settings->data['G_SMTP_PASSWORD']);

		if (isset($global_settings->data['G_SMTP_PORT'])) {
			$transport->setPort($global_settings->data['G_SMTP_PORT']);
		}

		if (isset($global_settings->data['G_SMTP_SSL']) && in_array($global_settings->data['G_SMTP_SSL'], ['ssl', 'tls'], true)) {
            $transport->setEncryption($global_settings->data['G_SMTP_SSL']);
        }
	break;
}

$logger = new \Swift_Plugins_Loggers_ArrayLogger();
$transport->registerPlugin(new \Swift_Plugins_LoggerPlugin($logger));



if ($transport->send($message)) {
	data::query("UPDATE GlobalSettings set value='' WHERE parameter='G_SMTP_FAIL'", true);
	exit('OK');

} else {
	$message = $logger->dump();
	data::query("UPDATE GlobalSettings set value='{$message}' WHERE parameter='G_SMTP_FAIL'", true);
	exit("E: " . $message);
}
?>
