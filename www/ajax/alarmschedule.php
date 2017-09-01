<?php 
	
class alarmschedule extends Controller {
	public $schedule_data;
    public $device_data;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.alarmschedule');

	$q =  "SELECT value as schedule FROM GlobalSettings WHERE parameter='G_ALARM_SCED'";
	$tmp =  data::query($q);
	$this->schedule_data = $tmp;
	$this->device_data[0]['id'] = 'global';

        $device_schedule = new StdClass();
        $device_schedule->schedule_data = $this->schedule_data;
        $device_schedule->device_data = $this->device_data;

        $this->view->device_schedule = $device_schedule;
    }

}


