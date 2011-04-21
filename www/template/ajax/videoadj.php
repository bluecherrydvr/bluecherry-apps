<?php defined('INDVR') or exit();
echo "<div id='header'>".VA_HEADER."</div>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".VIDEO_ADJ." <b>".(($device_data[0]['device_name']) ? $device_data[0]['device_name'] : $device_data[0]['id'])."</b></p>"; 
		
?>
<div id="liveViewContainer">
	<img id="liveViewImg" width='352' src='/media/mjpeg.php?id=<?php echo $_GET['id']; ?>' />
</div>
<div id="videoAdjControls">
	<INPUT value="<?php echo $device_data[0]['id']; ?>" type="Hidden" id="device_id" />
	<div id="videoAdjSliders">
		<div class="control" id="audio_volume"><label><?php echo VA_AUDIO; ?>:<INPUT disabled="disabled" value="<?php echo $device_data[0]['audio_volume']; ?>" type="text" id="audio_volume_value" /></label><div id="audio_volume" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_BRIGHTNESS; ?>:<INPUT disabled="disabled" value="<?php echo $device_data[0]['brightness']; ?>" type="text" id="brightness_value" /></label><div id="brightness" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_HUE; ?>:<INPUT disabled="disabled" value="<?php echo $device_data[0]['hue']; ?>" type="text" id="hue_value" /></label><div id="hue" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_SATURATION; ?>:<INPUT disabled="disabled" value="<?php echo $device_data[0]['saturation']; ?>" type="text" id="saturation_value" /></label><div id="saturation" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_CONTRAST; ?>:<INPUT disabled="disabled" value="<?php echo $device_data[0]['contrast']; ?>" type="text" id="contrast_value" /></label><div id="contrast" class="slider"><div class="knob"></div></div></div>
	</div>
			<a id="setToDefault"><?php echo VA_SETDEFAULT; ?></a>
</div>
