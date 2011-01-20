<?php  DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
#$current_user = new DVRUser();
#$current_user->CheckStatus();
#$current_user->StatusAction('admin');
#/auth check

class DVRIPCameras{
	public $data;
	public function __construct(){
		
		switch($_GET['m']){
			case 'model': $this->getModels($_GET['manufacturer']); break;
			case 'ops': $this->getOptions($_GET['model']); break;
			default: $this->getManufacturers(); break; 
		}
	}
	private function getManufacturers(){
		$db = DVRDatabase::getInstance();
		$this->data['manufacturers'] = $db->DBFetchAll("SELECT manufacturer FROM ipCameras GROUP by manufacturer");
	}
	private function getModels($m){
		$db = DVRDatabase::getInstance();
		$this->data['models'] = $db->DBFetchAll("SELECT model FROM ipCameras WHERE manufacturer='$m' ORDER BY model ASC");
	}
	private function getOptions($model){
		$db = DVRDatabase::getInstance();
		$this->data = $db->DBFetchAll("SELECT * FROM ipCameras WHERE model='$model'");
	}
}

$ipCamData = new DVRIPCameras;

#template
include_once('../template/ajax/addip.php');
?>
