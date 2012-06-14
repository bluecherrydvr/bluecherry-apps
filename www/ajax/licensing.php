<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

/**********************************************/

function bc_license_machine_id(){ return 'ABCD-1234'; };
function bc_license_check($key){ return 16; };
function bc_license_check_auth($key, $auth){ return true; };
function bc_license_devices_allowed(){ return 16; };

/**********************************************/


$licenses = data::getObject('Licenses');

if ($_GET['mode'] == 'add'){
	if (!bc_license_check($_POST['licenseCode'])){
		data::responseXml(false, L_INVALID_LICENSE);
		exit();
	};
	//here be automatic authorization//
	if (empty($confirmation)){
		data::responseXml('CONF', bc_license_machine_id());
		exit();
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
