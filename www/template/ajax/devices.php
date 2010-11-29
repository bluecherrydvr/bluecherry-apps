<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

echo "<div id='header'>".TOTAL_DEVICES."&nbsp;<b>{$devices->total_devices}</b></div><div id='devicesGroup'>";

if (!$devices->cards){
	echo NO_CARDS;
} else {
foreach($devices->cards as $key => $card){
	echo "<div id='dvrCard' class='$card->id'><div class='cardHeader'>#{$card->id} ".CARD_HEADER." ({$card->type} ".PORT.")</div><div class='cardContent'>"; #begin card/header
	echo "<div id='cardInfo'>Encoding: ".(($card->signal_type=='notconfigured') ? '<b>'.SIGNAL_TYPE_NOT_CONFD.'</b>' : "<a href='#'><div class='cardEncoding' title='".CARD_CHANGE_ENCODING."' id='$card->id'>{$card->signal_type}</div></a>")." | Unused capacity: <div id='unusedCapacity' class='uc{$card->id}'><a>{$card->fps_available}</a></div></div>";
	
foreach ($card->devices as $key =>$device){
	if (!$device['device_name']) { $device['device_name'] = ($device['status'] == 'notconfigured') ? DEVICE_VIDEO_NAME_notconfigured : DEVICE_UNNAMED; };
	echo "<div id='localDevice' class='{$device['id']}'><div><div id='{$device['id']}' class='name'>{$device['device_name']}&nbsp;</div></div><div id='status' id='{$device['id']}' class='{$device['status']}'>".constant('DEVICE_VIDEO_STATUS_'.$device['status'])." <a href='#' class='change_state' id='{$device['as_id']}'>[".constant('DEVICE_VIDEO_STATUS_CHANGE_'.$device['status'])."]</a></div><div id='port'>{$device['port']}</div>";
	if ($device['status'] != 'notconfigured' && !$device['disabled']) {
		echo "
				<hr>
				<div id='resolutionFps'>
					<div id='RES'>Resolution:".arrayToSelect($resolutions[$card->signal_type], $device['resolutionX'].'x'.$device['resolutionY'], $device['id'] ,'RES')."</div>
					<div id='FPS'>FPS:".arrayToSelect($local_device_fps, 30/$device['video_interval'], $device['id'],'FPS')."</div>
				</div>
				<div id='videoadj'><a href='#' id='{$device['id']}' class='videoadj'>".DEVICE_EDIT_VIDADJ."</a></div>
				<div id='motionDetection'><a href='#' id='{$device['id']}' class='editMap' id=''>".DEVICE_EDIT_MMAP."</a></div>
				<div id='scheduling'><a href='#' id='{$device['id']}' class='deviceSchedule'>".DEVICE_EDIT_SCHED."</a></div>";
	}
	echo	"</div>";
}

	echo "<div class='bClear'></div></div></div>"; #end card
}

echo '</div>';
};
?>
