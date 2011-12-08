<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

switch ($statistics->type) {
	case 'html':
		echo "<div id='statisticsResult'>
				<div id='boxContainer'>
					<div id='header'>".str_replace('%TYPE%', $statistics->event_type, STS_TOTAL_EVENTS)." {$statistics->total_records}</div>";
		
			echo "<div id='boxSection'><table id='statisticsTable'><tr><th>{$data['primary_grouping']}</td>".(($data['primary_grouping'] != $data['secondary_grouping']) ?  "<th>{$data['secondary_grouping']}</th>" : "")."<th>".STS_NUM_EVENTS."</th><th>".STS_PERCENTAGE_OF_TOTAL."</th></tr>";
			foreach($statistics->results as $key => $result){
				$primary_grouping_value = '';
				if ($key!=0 && $result[$data['primary_grouping']]!= $statistics->results[$key-1][$data['primary_grouping']]){ #change in primary grouping
					$primary_grouping_td = 'primarySeparator';
					$secondary_grouping_td = 'primarySeparator';
					$primary_grouping_value = $result[$data['primary_grouping']];
				} elseif ($key == 0){
					$primary_grouping_value = $result[$data['primary_grouping']];
					$primary_grouping_td = 'nonSeparated';
				} else {
					$primary_grouping_td = 'nonSeparated';
					$secondary_grouping_td = '';
				}
				echo "<tr><td class='{$primary_grouping_td}'>{$primary_grouping_value}</td>".(($data['primary_grouping'] != $data['secondary_grouping']) ? "<td class='{$secondary_grouping_td}'>{$result[$data['secondary_grouping']]}</td>" : "")."<td class='{$secondary_grouping_td}'>{$result['counter']}</td><td class='{$secondary_grouping_td}'>".round($result['counter']/$statistics->total_records*100, 2)."%</td></tr>";
			}
			echo "</table></div>";
		echo "</div></div>";
	break;
	case 'JSON':
	break;
	default:
		echo "
		<div id='boxContainer'>
			<div id='header'>".STS_PICKER_HEADER.":</div>
			<FORM id='sForm' action='/ajax/statistics.php' method='post'>
			<div id='statisticsForm'>
				<div class='dates'>
					<div>".STS_START_DATE.":</div> <input type='text' id='start' name='start' value='{$statistics->result['first_event']}' /><br/>
					<div>".STS_END_DATE.":</div> <input type='text' id='end' name='end' value='{$statistics->result['last_event']}' /><br />
				</div>
				<div class='grouping'>
					<div>".STS_PR_GRP.":</div> ".arrayToSelect(array_keys($query_params), '', 'primary_grouping')."<br />
					<div>".STS_SC_GRP.":</div> ".arrayToSelect(array_keys($query_params), '', 'secondary_grouping')."
				</div>
				<div class='bClear'></div>
				<div class='eventTypes'>
					<input type='Checkbox' name='all_events' checked='checked'>".STS_ET_ALL."</input>
					<input type='Checkbox' name='motion_events'>".STS_ET_M."</input>
					<input type='Checkbox' name='continuous_events'>".STS_ET_C."</input>
				</div>
				<div class='submitButton'>
					<input type='Submit' value='".STS_SUBMIT."' id='formSubmit' />
				</div>
			</div>
			</FORM>
		</div>
		<div id='sResults' width='100%'></div>";	
	break;
}

?>
