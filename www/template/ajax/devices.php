<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');
?>
<ul id="settingsMenu">
		<li><a href="#videoadj" class="videoadj"><?php echo DEVICE_EDIT_VIDADJ;?></a></li>
		<li class="separator"><a href="#editmap" class="editmap"><?php echo DEVICE_EDIT_MMAP;?></a></li>
		<li class="separator"><a href="#deviceschedule" class="deviceschedule"><?php echo DEVICE_EDIT_SCHED; ?></a></li>
		<li class="separator"><a href="#ptzsettings" class="ptzsettings"><?php echo DEVICE_EDIT_PTZ; ?></a></li>
</ul>
<?php
echo "<div id='header'>".TOTAL_DEVICES.": <b>{$devices->info['total_devices']}</b></div><div id='addIPCamera'>".AIP_HEADER."</div><div class='bClear'></div><div id='devicesGroup'>";

if ($devices->ipCameras){
	echo "<div id='dvrCard'><div class='cardHeader'>IP Cameras</div><div class='cardContent'>";
		foreach($devices->ipCameras as $key => $device){
			echo "<div id='ipDevice' class='{$device->info['id']}'><div><div id='{$device->info['id']}' class='name'>{$device->info['device_name']}</div></div><div id='{$device->info['id']}' class='{$device->info['status']}'>".constant('DEVICE_VIDEO_STATUS_'.$device->info['status'])." <a href='#' class='change_state' id='{$device->info['id']}'>[".constant('DEVICE_VIDEO_STATUS_CHANGE_'.$device->info['status'])."]</a> | <a href='#' class='deleteIp' id='{$device->info['id']}'>[".DELETE_CAM."]</a></div>";
			echo "<div id='scheduling'><a href='#' id='{$device->info['id']}' class='ipDeviceSchedule'>".DEVICE_EDIT_SCHED."</a></div>";
			echo "<div id='editing'><a href='#' id='{$device->info['id']}' class='deviceProps'>".DEVICE_EDIT_DETAILS."</a></div>";
			echo "</div>";
		}
	echo "<div class='bClear'></div></div></div>"; #end ip cameras
}

if (!$devices->cards){
	echo '<div id="ajaxMessage" class="INFO">'.NO_CARDS.'</div>';
} else {
	$counter = 0;
	foreach($devices->cards as $key => $card){
		$counter++;
		echo "<div id='dvrCard' class='{$card->info['id']}'><div class='cardHeader'>#{$counter} ".CARD_HEADER." ({$card->info['ports']} ".PORT.")</div><div class='cardContent'>"; #begin card/header
		echo "<div id='cardInfo'>Encoding: ".(($card->info['encoding']=='notconfigured') ? '<b>'.SIGNAL_TYPE_NOT_CONFD.'</b>' : "<a href='#'><div class='cardEncoding' title='".CARD_CHANGE_ENCODING."' id='{$card->info['id']}'>{$card->info['encoding']}</div></a>")." | Unused capacity: <div id='unusedCapacity' class='uc{$card->info['id']}'><a>{$card->info['available_capacity']}</a></div></div>";
foreach ($card->cameras as $key =>$device){
	if (empty($device->info['id'])) { $device->info['id']=''; };
	if (empty($device->info['device_name'])) { $device->info['device_name'] = ($device->info['status'] == 'notconfigured') ? DEVICE_VIDEO_NAME_notconfigured : DEVICE_UNNAMED; };
	echo "<div id='localDevice' class='{$device->info['id']}'>\n";
	echo "<div><div id='{$device->info['id']}' ".
		"class='name'>{$device->info['device_name']}</div></div>\n";
	echo "<div id='{$device->info['id']}' class='{$device->info['status']}'>".
		constant('DEVICE_VIDEO_STATUS_'.$device->info['status'])."\n";
	echo "<a href='#' class='change_state' id='"."{$device->info['device']}'>[".
		constant('DEVICE_VIDEO_STATUS_CHANGE_'.$device->info['status']).
		"]</a>\n";
	if ($device->info['status'] != 'notconfigured') {
		echo "<a class='settingsOpen' href='#' id='{$device->info['id']}'".
			" name='{$device->info['device']}'>[Settings]</a>\n";
	}
	echo "</div><div id='port'>{$device->info['port']}</div>";
	if ($device->info['status'] != 'notconfigured' && !$device->info['disabled']) {
		echo "<div class='background-color:#efefef;' id='cameraSettings'>
				<hr />
				<div id='resolutionFps'>
					<div id='RES'>Resolution:".arrayToSelect($GLOBALS['resolutions'][$card->info['encoding']], $device->info['resolutionX'].'x'.$device->info['resolutionY'], $device->info['device'] ,'RES')."</div>
					<div id='FPS'>FPS:".arrayToSelect($GLOBALS['local_device_fps'], 30/$device->info['video_interval'], $device->info['device'],'FPS')."</div>
				</div>
			 </div>";
	}
	echo	"</div>";
}

	echo "<div class='bClear'></div></div></div>"; #end card
}

echo '</div>';
};
?>
