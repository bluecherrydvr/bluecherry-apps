<?php 
	
class deviceschedule extends Controller {
	public $schedule_data;
    public $device_data;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.deviceschedule');

        $id = (isset($_GET['id']) and $_GET['id'] != '') ? intval($_GET['id']) : 'global';

		$q = ($id != 'global') ? "SELECT id, device_name, protocol, schedule, schedule_override_global, onvif_events_enabled FROM Devices WHERE id='$id'" : "SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'";
		$tmp =  data::query($q);
		#if there is no schedule copy global
		if ($id != 'global' && empty($tmp[0]['schedule'])){
			$global_schedule = data::query("SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'");
			data::query("UPDATE Devices SET schedule='{$global_schedule[0]['schedule']}' WHERE id='$id'");
			$tmp = data::query($q);
		}
		$this->schedule_data = $tmp;
		if ($id=='global') $this->device_data[0]['id'] = 'global';

        $device_schedule = new StdClass();
        $device_schedule->schedule_data = $this->schedule_data;
        $device_schedule->device_data = $this->device_data;

        $this->view->global = ($device_schedule->device_data[0]['id']=='global') ? true : false;
        $this->view->og = (!isset($device_schedule->schedule_data[0]['schedule_override_global'])) ? false : $device_schedule->schedule_data[0]['schedule_override_global'];

        $this->view->device_schedule = $device_schedule;
    }

}


