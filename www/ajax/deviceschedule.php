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
	public function __construct($id){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll($db->DBQuery("SELECT * FROM deviceSchedule WHERE id='$id'"));
		#if there is no schedule insert default
		if (count($tmp)==0){
			$db->DBQuery("INSERT INTO deviceSchedule (id) VALUES ('{$id}')");
			$tmp = $db->DBFetchAll($db->DBQuery("SELECT * FROM deviceSchedule WHERE id='$id'"));
		} else {
			$this->schedule_data = $tmp;
		}
		if ($id=='global') { $this->device_data[0]['id']='global'; }
		 else {($this->device_data = $db->DBFetchAll($db->DBQuery("SELECT id, device_name FROM Devices WHERE id='$id'"))); };
	}
}

$id = ($_GET['id']) ? intval($_GET['id']) : 'global';

$device_schedule = new devSchedule($id);

#require template to show data
include_once('../template/ajax/deviceschedule.php');
?>
