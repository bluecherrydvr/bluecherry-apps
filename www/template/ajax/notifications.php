<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

if ($_GET['mode']=='list'){
	if (!$notifications){
		echo NTF_NO_RULES;
	} else {
		foreach($notifications as $id => $notification){
			#prepare hours
			$notification['start'] = str_pad($notification['s_hr'], 2, '0').':'.str_pad($notification['s_min'], 2, '0');
			$notification['end'] = str_pad($notification['e_hr'], 2, '0').':'.str_pad($notification['e_min'], 2, '0');
			#prepare days
			$notification['day'] = str_split($notification['day']);
			$tmp = '';
			foreach($notification['day'] as $day){
				$tmp .= constant('DW_'.$day).', ';
			}
			$notification['day'] = trim($tmp, ', ');
			#prepare users
			$notification['users'] = trim($notification['users'], '|');
			$notification['users'] = explode('|', $notification['users']);
			$tmp = '';
			foreach($notification['users'] as $id){
				$tmp .= $users[$id]['name'].'<br />';
			}
			$notification['users'] = $tmp;
			#prepare cameras
			$notification['cameras'] = trim($notification['cameras'], '|');
			$notification['cameras'] = explode('|', $notification['cameras']);
			$tmp = '';
			foreach($notification['cameras'] as $id){
				$tmp .= $cameras[$id]['device_name'].'<br />';
			}
			$notification['cameras'] = $tmp;
			
			#output the rule
			echo "
				<div class='container-separator' id='existing-rule'>
					<div id='{$notification['id']}' class='delete-button'>".L_DELETE."</div>
					<table>
						<tr><td class='desc'>".NTF_T_TO.":</td><td class='cont'>{$notification['users']}</td></tr>
						<tr><td class='desc'>".NTF_T_DOW.":</td><td class='cont'>{$notification['day']}</td></tr>
						<tr><td class='desc'>".NTF_T_TIME.":</td><td class='cont'>{$notification['start']} - {$notification['end']}</td></tr>
						<tr><td class='desc'>".NTD_T_CAMS.":</td><td class='cont'>{$notification['cameras']}</td></tr>
					</table>
				</div>
			";
		}
	}
	exit();
}

?>
<h1 id='header' class='header'><?php echo NTF_HEADER; ?></h1>
<p><?php echo NTF_SUBHEADER; ?></p>
<div id='notifications'>
	<div class='container-separator' id='blank-rule-container'>
		<form method='post' action='/ajax/notifications.php' id='new-rule-form'>
		<div class='title'><?php echo NTF_ADD_RULE_TITLE; ?></div>
		<div id='button' class='add-rule-button'><?php echo NTF_ADD_RULE; ?></div>
		<div class='bClear'></div>
		<div class='container-separator' id='rule-date-time'>
			<div class='title'><?php echo NTF_ADD_RULE_DATETIME; ?></div>
			<div id='day-of-week'>
				<select name="daysofweek[]" multiple="" size="7">
					<option value='M'><?php echo DW_M; ?></option>
					<option value='T'><?php echo DW_T; ?></option>
					<option value='W'><?php echo DW_W; ?></option>
					<option value='R'><?php echo DW_R; ?></option>
					<option value='F'><?php echo DW_F; ?></option>
					<option value='S'><?php echo DW_S; ?></option>
					<option value='U'><?php echo DW_U; ?></option>
				</select>
			</div>
			<div id='time'>
				<div class='container-separator' id='start'>
					<div class='title'><?php echo NTF_START_TIME; ?></div>
					<select name='s_hr'><?php for($i=0; $i<=23; $i++){ echo "<option value='{$i}'>$i</option>"; } ?></select>
					:<select name='s_min'><?php for($i=0; $i<=59; $i++){ echo "<option value='{$i}'>$i</option>"; } ?></select>
				</div>
				<div class='container-separator'>
					<div class='title'><?php echo NTF_END_TIME; ?></div>
					<select name='e_hr'><?php for($i=0; $i<=23; $i++){ echo "<option value='{$i}'>$i</option>"; } ?></select>
					:<select name='e_min'><?php for($i=0; $i<=59; $i++){ echo "<option value='{$i}'>$i</option>"; } ?></select>
				</div>
			</div>
			<div class='bClear'></div>
		</div><!--end date/time !-->
		<div class='container-separator' id='cameras-list'>
			<div class='title'><?php echo NTF_CAMERAS; ?></div>
			<select name='cameras[]' multiple='' size='7'>
				<?php foreach($cameras as $id => $camera){ echo "<option value='{$camera['id']}'>{$camera['device_name']}</option>"; }
				?>
			</select>
			<div class='bClear'></div>
		</div><!--end cameras list!-->
		<div class='container-separator' id='users-list'>
			<div class='title'><?php echo NTF_USERS; ?></div>
			<select name='users[]' multiple='' size='7'>
				<?php foreach($users as $id => $user){ echo "<option value='{$user['id']}'>{$user['name']}</option>"; }
				?>
			</select>
			<div class='bClear'></div>
		</div><!--end users list!-->
		<div class='bClear'></div>
		</form>
	</div><!--end blank rule!-->
	<div class='container-separator'>
		<div class='title'><?php echo NTF_EXISTING_RULES; ?></div>
		<div id='existing-rules-container'>
		
		</div>
	</div>
	
</div>
