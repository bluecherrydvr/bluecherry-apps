<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

$query_params = array(
	'day' => '%d',
	'month' => '%m',
	'year' => '%y',
	'hour' => '%h',
	'minute' => '%i',
	'second' => '%s'
);

$date_format = "Y-m-d H:i:s";

class stats{
	public $type;
	public $event_type;
	public $results;
	public $total_records;
	public function getData($data, $query_params){
		$where = "WHERE type_id != 'not found' ";
		if (!empty($data['start']) && !empty($data['end'])) $where .= "time>={$data['start']} AND time<={$data['end']} ";
		if (!$data['event_filter']['all_events'] && ($data['event_filter']['motion_events'] || $data['event_filter']['continuous_events']) && !($data['event_filter']['motion_events'] && $data['event_filter']['continuous_events'])) {
			$this->event_type = ($data['event_filter']['motion_events']) ? "motion" : "continuous";
			$where .= (($where != 'WHERE ') ? ' AND ' : '')."type_id = '".$this->event_type."'";		
		}
		$where = ($where != 'WHERE ') ? $where : '';
		$query = "SELECT count(*) as counter, DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$data['primary_grouping']]}') as '{$data['primary_grouping']}', DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$data['secondary_grouping']]}') as '{$data['secondary_grouping']}' FROM EventsCam {$where} GROUP BY {$data['primary_grouping']}, {$data['secondary_grouping']}";
		$this->results = data::query($query);
		$this->total_records = count(data::query("SELECT id FROM EventsCam {$where}"));
	}
	public function getFirstLast($date_format){ #default values for date fields are first and last events in db
		$result = data::query("SELECT id, time FROM EventsCam ORDER BY id ASC LIMIT 1");
		
		$this->result['first_event'] = $result[0]['time'];
		$result = data::query("SELECT id, time FROM EventsCam ORDER BY id DESC LIMIT 1");
		$this->result['last_event'] = $result[0]['time']; 
	}
	
}

$statistics = new stats();

if ($_POST){
	#prepare data
	$data['primary_grouping'] = $_POST['primary_grouping'];
	$data['secondary_grouping'] = $_POST['secondary_grouping'];
	$data['start'] = @strtotime($_POST['start']);
	$data['end'] = @strtotime($_POST['end']);
	$data['event_filter']['all_events'] = ($_POST['all_events'] == 'on') ? true : false;
	$data['event_filter']['motion_events'] = ($_POST['motion_events'] == 'on') ? true : false;
	$data['event_filter']['continuous_events'] = ($_POST['continuous_events'] == 'on') ? true : false;
	#if type not set return html
	$statistics->type = (!empty($_POST['type'])) ? $_POST['type'] : 'html';
	#get data
	$statistics->getData($data, $query_params);
} else {
	$statistics->getFirstLast($date_format);
}

#template
include_once('../template/ajax/statistics.php');
?>
