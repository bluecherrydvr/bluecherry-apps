<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

class globalSettings{
	var $data;
	function __construct(){
		$this->data =$this->getGlobalInfo();
	}
	function GetGlobalInfo(){
		return data::getObject('GlobalSettings');
	}
}

#run class/get data
$global_settings = new globalSettings();

#require template to show data
include_once('../template/ajax/general.php');
?>
