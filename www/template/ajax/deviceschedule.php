<?php defined('INDVR') or exit();
		$global = ($device_schedule->device_data[0]['id']=='global') ? true : false;
		$og = ($device_schedule->schedule_data[0]['schedule_override_global']) ? true : false;
		echo "<div id='header'>".SCHED_HEADER."</div>";
		echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".(($global) ? '<b>'.EDITING_GLOBAL_SCHED.'</b>' : EDITING_SCHED." <b>".(($device_schedule->schedule_data[0]['device_name']) ? $device_schedule->schedule_data[0]['device_name'] : $device_schedule->schedule_data[0]['id'])."</b>")."</p>"; 	
?>

<INPUT type="Hidden" id="cameraID" value="<?php echo ($global) ? 'global' : $device_schedule->schedule_data[0]['id']; ?>" />
<INPUT type="Hidden" id="valueString" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
<?php if (!$global) { echo "<div id='message' class='".(($og) ? 'OK' : 'INFO')."'><INPUT type='Checkbox' id='overrideGlobal' ".(($og) ? 'checked': '' ).">".GLOBAL_SCHED_OG."</div>";}; ?>
<div class="bClear"></div>
<div id="lvlSelect">
	<ul>
		<li id="liN">Off</li>
		<li id="liM" class='on'>Motion</li>
		<li id="liC">Continuous</li>
		<li id="liS">Sensor</li>
	</ul>
	
	<div id='saveButton' style="cursor:pointer;"><?php echo SAVE_CHANGES; ?></div>
</div>
<div class="bClear"></div>
<div id="scheduleContainer">


</div>
<div class="bClear"></div>