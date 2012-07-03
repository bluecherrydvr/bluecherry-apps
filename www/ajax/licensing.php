<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

$licenses = data::getObject('Licenses');

if ($_GET['mode'] == 'add'){
	if (!bc_license_check($_POST['licenseCode'])){
		data::responseXml(false, L_INVALID_LICENSE);
		exit();
	};
	$exists = data::getObject('Licenses', 'license', $_POST['licenseCode']);
	if (!empty($exists)){
		data::responseXml(false, L_INVALID_LICENSE_EXISTS);
		exit();
	};
	$machine_id = bc_license_machine_id();
	$confirmation = fopen(VAR_LICENSE_AUTH."/?license_code={$_POST['licenseCode']}&id=".$machine_id, 'r');
	#if auto auth fails, show message, opt to confirm manually
	if (empty($confirmation)){
		data::responseXml('CONF', $machine_id);
		exit();
	}
	#if auto ok, 
	$confirmation = fread($confirmation, 1024);
	if (strlen($confirmation) < 9){
		data::responseXml('F', constant('L_AUTO_RESP_'.$confirmation));
		exit();
	} else {
		$confirmation = preg_replace('/([^0-9a-zA-Z\-])/', '', $confirmation);
		$result = data::query("INSERT INTO Licenses VALUES ('{$_POST['licenseCode']}', '{$confirmation}', UNIX_TIMESTAMP())", true);
		if ($result){
			data::responseXml(true, L_LICENSE_ADDED);
			exit();
		} else {
			data::responseXml(false, false);
			exit();
		}
	}
	exit();
}
if ($_GET['mode'] == 'confirm'){
	if (bc_license_check_auth($_POST['licenseCode'], $_POST['confirmLicense'])) {
		$exists = data::getObject('Licenses', 'license', $_POST['licenseCode']);
		if (!empty($exists)){
			data::responseXml(false, L_INVALID_LICENSE_EXISTS);
			exit();
		} else {
			$result = data::query("INSERT INTO Licenses VALUES ('{$_POST['licenseCode']}', '{$_POST['confirmLicense']}', UNIX_TIMESTAMP())", true);
			if ($result){
				data::responseXml(true, L_LICENSE_ADDED);
				exit();
			} else {
				data::responseXml(false, false);
				exit();
			}
		}
	} else {
		data::responseXml(false, L_INVALID_CONFIRMATION);
		exit();
	}
}
if ($_GET['mode'] == 'delete'){
	$result = data::query("DELETE FROM Licenses WHERE license = '{$_GET['license']}'", true);
}

#require template to show data
include_once('../template/ajax/licensing.php');

?>
