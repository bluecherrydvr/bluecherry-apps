<?php DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check



class devSchedule{
	public $schedule_data;
	public $device_data;
	public function __construct(){
		$db = DVRDatabase::getInstance();
		$this->schedule_data = $db->DBFetchAll($db->DBQuery("SELECT * FROM deviceSchedule WHERE id='global'"));
	}
}

$device_schedule = new devSchedule();
$device_schedule->device_data[0]['id'] = 'global';


include_once('../template/ajax/deviceschedule.php');
?>