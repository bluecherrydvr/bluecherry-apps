<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

function checkNotification($data){
	if (empty($data['daysofweek'])){
		return array(false, NTF_E_DAYS);
	}
	if ($data['s_hr']>$data['e_hr'] || ($data['s_hr']==$data['e_hr'] && $data['s_min']>=$data['e_min'])){
		return array(false, NTF_E_TIME);
	}
	if (empty($data['cameras'])){
		return array(false, NTF_E_CAMS);
	}
	if (empty($data['users'])){
		return array(false, NTF_E_USERS);
	} 
	return array(true, '');
}

$m = (!empty($_GET['mode'])) ? $_GET['mode'] : false;

if (!empty($_POST)){
	$result = checkNotification($_POST);
	if ($result[0]){
		$daysoftheweek = implode($_POST['daysofweek']);
		$users = '|'.implode('|', $_POST['users']).'|';
		$cameras = '|'.implode('|', $_POST['cameras']).'|';
		$limit = intval($_POST['limit']);
			if (empty($_GET['id'])) { #if id not set -- create new
				$query = "INSERT INTO notificationSchedules values ('', '{$daysoftheweek}', '{$_POST['s_hr']}', '{$_POST['s_min']}', '{$_POST['e_hr']}', '{$_POST['e_min']}', '{$cameras}', '{$users}', {$limit}, 0)";
			} else { #if set update existing
				$query = "UPDATE notificationSchedules SET day='{$daysoftheweek}', s_hr='{$_POST['s_hr']}', s_min='{$_POST['s_min']}', e_hr='{$_POST['e_hr']}', e_min='{$_POST['e_min']}', cameras='{$cameras}', users='{$users}', nlimit='{$limit}' WHERE id=".intval($_GET['id']);
			}
		$result = data::query($query, true);
		data::responseJSON($result);
	} else {
		data::responseJSON($result[0], $result[1]);
	}
	exit();
}

if (!empty($_GET) && ($m == 'delete' || $m == 'changeStatus')){
	$result = false;
	$id = intval($_GET['id']);
	if (!empty($id)){
		switch($m){
			case 'delete':
				$query = "DELETE FROM notificationSchedules WHERE id={$id}";
			break;
			case 'changeStatus':
				$currentStatus = data::getObject('notificationSchedules', 'id', intval($_GET['id']));
				$query = "UPDATE notificationSchedules SET disabled=".(($currentStatus[0]['disabled'] == 1) ? 0 : 1)." WHERE id=".intval($_GET['id']);
			break;
		}
		$result = data::query($query, true);
	};
	data::responseJSON($result);
	exit();
}

$form_rule = new View('notifications.form_rule');
$notification = Array();
$notification['id'] = 0;
$notification['nlimit'] = 0;
$notification['day'] = Array();
$notification['users'] = Array();
$notification['cameras'] = Array();
$form_rule->notification = $notification;

$tmp = data::getObject('notificationSchedules');

$notifications = Array();
if ($tmp)
foreach($tmp as $id => $notification){
	$notifications[$notification['id']] = $notification;
}
$form_rule->notifications = $notifications;


$tmp = data::getObject('Users');
foreach($tmp as $key => $user){
	$users[$user['id']] = $user;
}
$form_rule->users = $users;

$tmp = data::getObject('Devices');
foreach($tmp as $key => $camera){
	$cameras[$camera['id']] = $camera;
}
$form_rule->cameras = $cameras;

if ($m == 'getedit'){
	$notification = $notifications[$_GET['id']];
	$notification['day'] = str_split($notification['day']);
	$notification['users'] = trim($notification['users'], '|');
	$notification['users'] = explode('|', $notification['users']);
	$notification['cameras'] = trim($notification['cameras'], '|');
    $notification['cameras'] = explode('|', $notification['cameras']);

    $form_rule->notification = $notification;

    echo $form_rule->render();
    die();
}

include_once('../template/ajax/notifications.php');
?>
