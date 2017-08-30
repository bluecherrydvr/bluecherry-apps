<?php 
	
class gpio extends Controller {
	public $schedule_data;
    public $device_data;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    private function getCards()
    {
                $cards = data::query("SELECT card_id FROM AvailableSources WHERE driver LIKE 'solo%' GROUP BY card_id");
                $this->info['total_devices'] = 0;
                if (!$cards) { return false; }
                        else {
                                foreach ($cards as $key => $card){
                                        $this->cards[$card['card_id']] = new card($card['card_id']);
                                        $this->info['total_devices'] += count($this->cards[$card['card_id']]->cameras);
                                }

                        }
     }

    private function readPinVal($pin_id)
    {
	$v = '-';

	if (file_exists("/sys/class/gpio/gpio".$pin_id."/value"))
	{
		$h = fopen("/sys/class/gpio/gpio".$pin_id."/value", 'r');

		if ($h)
			$v = intval(fread($h, 1));

		fclose($h);
	}

	return $v;
    }

    private function getPinBasenums()
    {
	$this->pin_basenums = data::query("SELECT MIN(input_pin_id)-8 as basenum, card_id FROM GpioConfig GROUP BY card_id");
    }

    private function getPinStates()
    {
	$pinstates = array();


	foreach ($this->pin_basenums as $key => $basenum)
	{
		$basepin = $basenum['basenum'];

		$relays = array();
		$sensors = array();

		for ($pin_id = $basepin; $pin_id < $basepin + 8; $pin_id++)
			$relays[$pin_id - $basepin] = $this->readPinVal($pin_id);

		for ($pin_id = $basepin + 8; $pin_id < $basepin + 24; $pin_id++)
			$sensors[$pin_id - $basepin - 8] = $this->readPinVal($pin_id);

		$pinstates[$basenum['card_id']]['relays'] = $relays;
		$pinstates[$basenum['card_id']]['sensors'] = $sensors;

	}

	return $pinstates;
    }

    private function resetRelays()
    {
	foreach ($this->pin_basenums as $key => $basenum)
	{
		$basepin = $basenum['basenum'];
		$output = array();
	        $ret = 0;

		for ($relay_pin = $basepin; $relay_pin < $basepin + 8; $relay_pin++)
		{
			if (!file_exists("/sys/class/gpio/gpio".$relay_pin."/value"))
				exec("/usr/lib/bluecherry/gpiocmd export_pin ".$relay_pin, $output, $ret);

			exec("/usr/lib/bluecherry/gpiocmd write ".$relay_pin." 0", $output, $ret);
			//TODO check for errors, return ajax error
		}
	}
    }

    private function getGpioConfig()
    {
	//[sensor_card_id][relay_card_id][relay_pin][sensor_pin];
	//[sensor_card_id][camera_device_id][sensor_pin]
	$relay_mappings = array();
	$camera_mappings = array();

	$relay_to_sensors = array();//global linux pin id's
	$camera_to_sensors = array();

	$gpioconfig = data::query("SELECT card_id, input_pin_id, trigger_output_pins, trigger_devices FROM GpioConfig ORDER BY card_id, input_pin_id");

	foreach($gpioconfig as $row_id => $row) {
		if (!empty($row['trigger_output_pins'])) {
			$output_pins = explode(",",$row['trigger_output_pins']);

			foreach($output_pins as $key => $out_pin) {
				if (!array_key_exists($out_pin, $relay_to_sensors))
					$relay_to_sensors[$out_pin] = array();

				$relay_to_sensors[$out_pin][$row['input_pin_id']] = 1;
			}
		}

		if (!empty($row['trigger_devices'])) {
			$device_ids = explode(",", $row['trigger_devices']);

			foreach ($device_ids as $key => $device_id) {
				if(!array_key_exists($device_id, $camera_to_sensors))
					$camera_to_sensors[$device_id] = array();

				$camera_to_sensors[$device_id][$row['input_pin_id']] = 1;
			}
		}
	}

	foreach ($this->pin_basenums as $sensor_key => $s) {
		$relay_mappings[$s['card_id']] = array();
		foreach($this->pin_basenums as $relay_key => $r) {
			$relay_mappings[$s['card_id']][$r['card_id']] = array();

			for ($i = 0; $i < 8; $i++) {
				$relay_mappings[$s['card_id']][$r['card_id']][$i] = array();
				for ($j = 0; $j < 16; $j++) {

					$glob_relay_pin_id = $r['basenum'] + $i;
					$glob_sensor_pin_id = $s['basenum'] + $j + 8;
					$relay_mappings[$s['card_id']][$r['card_id']][$i][$j] = "0"."rm_sc".$s['card_id']."_sp".$glob_sensor_pin_id."_rp".$glob_relay_pin_id;

					if (isset($relay_to_sensors[$glob_relay_pin_id])
						 && isset($relay_to_sensors[$glob_relay_pin_id][$glob_sensor_pin_id])
						 && $relay_to_sensors[$glob_relay_pin_id][$glob_sensor_pin_id] == 1)
						$relay_mappings[$s['card_id']][$r['card_id']][$i][$j][0] = "1";
				}
			}
		}
	}

	$enabled_Devices = data::query("SELECT id, device_name FROM Devices WHERE disabled=0");

	foreach ($this->pin_basenums as $sensor_key => $s) {
		$camera_mappings[$s['card_id']] = array();

		foreach ($enabled_Devices as $key => $enabledDevice) {
			$camera_mappings[$s['card_id']][$enabledDevice['id']] = array();
			$camera_mappings[$s['card_id']][$enabledDevice['id']]['device_name'] = $enabledDevice['device_name'];

			for ($j = 0; $j < 16; $j++) {
				$glob_sensor_pin_id = $s['basenum'] + $j + 8;

				$camera_mappings[$s['card_id']][$enabledDevice['id']][$j] = "0"."cm_sc".$s['card_id']."_sp".$glob_sensor_pin_id."_cam".$enabledDevice['id'];

				if (isset($camera_to_sensors[$enabledDevice['id']])
					&& isset($camera_to_sensors[$enabledDevice['id']][$glob_sensor_pin_id])
					&& $camera_to_sensors[$enabledDevice['id']][$glob_sensor_pin_id] == 1 )
					$camera_mappings[$s['card_id']][$enabledDevice['id']][$j][0] = "1";
			}
		}
	}

	$this->relay_mappings = $relay_mappings;
	$this->camera_mappings = $camera_mappings;
    }

    public function postData()
    {
	$this->getPinBasenums();

	$enabled_dev_ids = data::query("SELECT id FROM Devices WHERE disabled=0");

	foreach ($this->pin_basenums as $sensor_key => $s) {
		for ($j = 0; $j < 16; $j++) {
			$glob_sensor_pin_id = $s['basenum'] + $j + 8;
			$trigger_output_pins = array();
			$trigger_devices = array();

			foreach ($this->pin_basenums as $relay_key => $r) {
				for ($i = 0; $i < 8; $i++) {
					$glob_relay_pin_id = $r['basenum'] + $i;

					if (isset($_POST["rm_sc".$s['card_id']."_sp".$glob_sensor_pin_id."_rp".$glob_relay_pin_id]))
						array_push($trigger_output_pins, $glob_relay_pin_id);
				}
			}

			foreach ($enabled_dev_ids as $key => $enabled_device) {
				if (isset($_POST["cm_sc".$s['card_id']."_sp".$glob_sensor_pin_id."_cam".$enabled_device['id']]))
					array_push($trigger_devices, $enabled_device['id']);
			}

			if (!empty($trigger_output_pins)) {
				$trigger_output_pins = "'".implode(",", $trigger_output_pins)."'";
			} else
				$trigger_output_pins = "NULL";

			if (!empty($trigger_devices)) {
				$trigger_devices = "'".implode(",", $trigger_devices)."'";
			} else
				$trigger_devices = "NULL";

			data::query("UPDATE GpioConfig SET trigger_output_pins=".$trigger_output_pins.", trigger_devices=".$trigger_devices." WHERE card_id=".$s['card_id']." AND input_pin_id=".$glob_sensor_pin_id);

		}
	}

	data::responseJSON(true);
    }

    public function getData()
    {
	$this->getPinBasenums();

        if (!empty($_GET['mode']) && $_GET['mode'] == 'reset') {
                $this->resetRelays();
		header("Location: /gpio");
		exit;
	}

        $this->setView('ajax.gpio');

	$this->view->pinstates = $this->getPinStates();
	$this->getCards();
	$this->view->cards = $this->cards;

	$this->getGpioConfig();
	$this->view->relay_mappings = $this->relay_mappings;
	$this->view->camera_mappings = $this->camera_mappings;

        $id = (isset($_GET['id']) and $_GET['id'] != '') ? intval($_GET['id']) : 'global';

		$q = ($id != 'global') ? "SELECT id, device_name, protocol, schedule, schedule_override_global FROM Devices WHERE id='$id'" : "SELECT value as schedule FROM GlobalSettings WHERE parameter='G_DEV_SCED'";
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


