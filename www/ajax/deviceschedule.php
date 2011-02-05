<?php DEFINE('INDVR', true);
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
	public function __construct($id){
		$db = DVRDatabase::getInstance();
		$q = ($id != 'global') ? "SELECT id, device_name, protocol, schedule, schedule_override_global FROM Devices WHERE id='$id'" : "SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'";
		$tmp =  $db->DBFetchAll($q);
		#if there is no schedule copy global
		if ($id != 'global' && empty($tmp[0]['schedule'])){
			$global_schedule = $db->DBFetchAll("SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'");
			$db->DBQuery("UPDATE Devices SET schedule='{$global_schedule[0]['schedule']}' WHERE id='$id'");
			$tmp =  $db->DBFetchAll($q);
		}
		$this->schedule_data = $tmp;
		if ($id=='global') $this->device_data[0]['id'] = 'global';
	}
}

$id = (isset($_GET['id']) and $_GET['id'] != '') ? intval($_GET['id']) : 'global';

$device_schedule = new devSchedule($id);

#require template to show data
include_once('../template/ajax/deviceschedule.php');
?>
