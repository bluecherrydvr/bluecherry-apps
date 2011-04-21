<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions
include("../template/template.lib.php");
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

if (!empty($_POST)){
	$primary_grouping = $_POST['primary_grouping'];
	$secondary_grouping = $_POST['secondary_grouping'];
	$start = @strtotime($_POST['start']);
	$end = @strtotime($_POST['end']);
	$where = "WHERE ";
	if (!empty($start) && !empty($end)) $where .= "time>={$start} AND time<={$end} ";
	if ($_POST['all_events'] != 'on' && ($_POST['motion_events']!='on' || $_POST['continuous_events']!='on')) {
		$type = ($_POST['motion_events']=='on') ? "motion" : "continuous";
		$where .= (($where != 'WHERE ') ? ' AND ' : '')."type_id = '".$type."'";
		
	}
	$where = ($where != 'WHERE ') ? $where : '';
	$query = "SELECT count(*) as counter, DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$primary_grouping]}') as '{$_POST['primary_grouping']}', DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$secondary_grouping]}') as '{$secondary_grouping}' FROM EventsCam {$where} GROUP BY {$_POST['primary_grouping']}, {$secondary_grouping}";
	$results = data::query($query);
	$total_records = count(data::query("SELECT id FROM EventsCam {$where}"));
	echo "Total <b>{$type}</b> events for the period: ".$total_records;
	echo "<table id='statisticsTable'><tr><td>{$primary_grouping}</td><td>{$secondary_grouping}</td><td>Number of events</td></tr>";
	foreach($results as $key => $result){
		$td_class = ($key!=0 && $result[$primary_grouping]!= $results[$key-1][$primary_grouping]) ? 'primarySeparator' :''; #change in primary grouping
		echo "<tr><td class='{$td_class}'>{$result[$primary_grouping]}</td><td class='{$td_class}'>{$result[$secondary_grouping]}</td><td class='{$td_class}'>{$result['counter']}</td></tr>";
	}
	echo '</table>';
	
} else {
	$result = data::query("SELECT id, time FROM EventsCam ORDER BY id ASC LIMIT 1");
	$first_event_date = date($date_format, $result[0]['time']);
	$result = data::query("SELECT id, time FROM EventsCam ORDER BY id DESC LIMIT 1");
	$last_event_date = date($date_format, $result[0]['time']);
	?>
<FORM id='sForm' action="/ajax/statistics.php" method="post">
	Start date: <input type="text" name='start' value='<?php echo $first_event_date; ?>' /><br /><br />
	End Date : <input type="text" name='end' value='<?php echo $last_event_date; ?>' /><br /><br />
	<div>Date/time format <b>YYYY-MM-DD HH:MM:SS</b></div>
	Primary grouping: <?php echo arrayToSelect(array_keys($query_params), '', 'primary_grouping'); ?><br />
	Secondary grouping: <?php echo arrayToSelect(array_keys($query_params), '', 'secondary_grouping'); ?><br /><br />
	Select events:<br />
	<input type="Checkbox" name="all_events" checked="checked">All events</input><br />
	<input type="Checkbox" name="motion_events">Motion</input><br />
	<input type="Checkbox" name="continuous_events">Continuous</input><br />
	<input type="Submit" value="Get statistics" id='formSubmit' />
</FORM>	
<hr />
<div id='sResults' width='100%'></div>
	<?php
}
?>
