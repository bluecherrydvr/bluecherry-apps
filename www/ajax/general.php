<?php DEFINE(INDVR, true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check


class DVRGlobalSettings extends DVRData{
	var $data;
	function __construct(){
		$this->data = $this->GetGlobalInfo();
	}
	function GetGlobalInfo(){
		$db = DVRDatabase::getInstance();
		return $this->GetObjectData('GlobalSettings');;
	}
}

#run class/get data
$global_settings = new DVRGlobalSettings();

#require template to show data
include_once('../template/ajax/general.php');
?>