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
		data::responseXml($result);
	} else {
		data::responseXml($result[0], $result[1]);
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
	data::responseXml($result);
	exit();
}

$tmp = data::getObject('notificationSchedules');
foreach($tmp as $id => $notification){
	$notifications[$notification['id']] = $notification;
}

$tmp = data::getObject('Users');
foreach($tmp as $key => $user){
	$users[$user['id']] = $user;
}

$tmp = data::getObject('Devices');
foreach($tmp as $key => $camera){
	$cameras[$camera['id']] = $camera;
}

include_once('../template/ajax/notifications.php');
?>
