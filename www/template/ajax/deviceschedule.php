<?php defined('INDVR') or exit();
$global = ($device_schedule->device_data[0]['id']=='global') ? true : false;
$og = (!isset($device_schedule->schedule_data[0]['schedule_override_global'])) ? false : $device_schedule->schedule_data[0]['schedule_override_global'];
echo "<h1 class='header' id='header'>".SCHED_HEADER."</h1>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".(($global) ? '<b>'.EDITING_GLOBAL_SCHED.'</b>' : EDITING_SCHED." <b>".(($device_schedule->schedule_data[0]['device_name']) ? $device_schedule->schedule_data[0]['device_name'] : $device_schedule->schedule_data[0]['id'])."</b>")."</p>"; ?>
<INPUT type="Hidden" id="cameraID" value="<?php echo ($global) ? 'global' : $device_schedule->schedule_data[0]['id']; ?>" />
<INPUT type="Hidden" id="valueString" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
<?php if (!$global) { echo "<div id='message' class='".(($og) ? 'OK' : 'INFO')."'><INPUT type='Checkbox' id='overrideGlobal'".(($og) ? ' checked="checked"': '' )." />".GLOBAL_SCHED_OG."</div>";}; ?>
<div id='saveButton' style="cursor:pointer;"><?php echo SAVE_CHANGES; ?></div>
<div class="bClear"></div>
<div id="lvlSelect" class='container-separator'>
<div class="title"><?php echo SCHED_SELECTOR_TITLE; ?></div>
	<ul>
		<li id="liN"><?php echo  SCHED_OFF; ?></li>
		<li id="liC" class="on"><?php echo  SCHED_CONT; ?></li>
		<li id="liM"><?php echo  SCHED_MOTION; ?></li>
		<!-- do not show until GPIO is supported
			<li id="liS"><?php echo SCHED_SENSOR; ?></li>
		!-->
		<li id="liT"><?php echo SCHED_TRIGGER; ?></li>
	</ul>
	<ul class='all'>
		<li id="fillAll"><?php echo  SCHED_FILLALL; ?></li>
	</ul>
	<div class="bClear"></div>
</div>
<div class='container-separator'>
	<div id="scheduleContainer"></div>
</div>
