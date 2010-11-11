<?php defined('INDVR') or exit();
echo "<div id='header'>".VA_HEADER."</div>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".VIDEO_ADJ." <b>".(($device_data[0]['device_name']) ? $device_data[0]['device_name'] : $device_data[0]['id'])."</b></p>"; 
		
?>
<style>
#liveViewContainer 				{ position:relative; display:inline; }
#liveViewContainer img 			{ width:352px; }
#videoAdjSliders				{ display: block; width:350px; }
#videoAdjSliders .control		{ float:left; height:1em; padding:5px 0 5px 0; display:block; margin:5px 0 5px 0; display: block; }
#videoAdjSliders label			{ float: left; text-align:right; display:block; width:100px; margin:5px 10px 5px 0; font-size:1em; }
#videoAdjSliders .slider		{ float: left; width:200px; background: url("/img/icons/ln.png") 0 center repeat-x; height:1em; margin:10px; }
#videoAdjSliders .slider .knob	{ height:1em; width:1em;  background: url("/img/icons/knob.png") no-repeat center center; }
#videoAdjSliders #audio_volume.control { margin-bottom:30px; }
#setToDefault { display: block; float:left; font-size:1.2em; background-color:#ddffdd; padding:5px; border:1px dotted #afa; margin-left:120px; margin-top:20px;}
#setToDefault:hover { cursor:pointer; background-color:#bbffbb; }

</style>
<div id="liveViewContainer">
	<img id="liveViewImg" src='/media/mjpeg.php?id=<?php echo $_GET['id']; ?>'>
</div>
<div id="videoAdjControls">
	<INPUT value="<?php echo $device_data[0]['id']; ?>" type="Hidden" id="device_id">
	<INPUT value="<?php echo $device_data[0]['brightness']; ?>" type="Hidden" id="brightness_value">
	<INPUT value="<?php echo $device_data[0]['hue']; ?>" type="Hidden" id="hue_value">
	<INPUT value="<?php echo $device_data[0]['saturation']; ?>" type="Hidden" id="saturation_value">
	<INPUT value="<?php echo $device_data[0]['contrast']; ?>" type="Hidden" id="contrast_value">
	<INPUT value="<?php echo $device_data[0]['audio_volume']; ?>" type="Hidden" id="audio_volume_value">
	<div id="videoAdjSliders">
		<div class="control" id="audio_volume"><label><?php echo VA_AUDIO; ?>:</label><div id="audio_volume" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_BRIGHTNESS; ?>:</label><div id="brightness" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_HUE; ?>:</label><div id="hue" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_SATURATION; ?>:</label><div id="saturation" class="slider"><div class="knob"></div></div></div>
		<div class="control"><label><?php echo VA_CONTRAST; ?>:</label><div id="contrast" class="slider"><div class="knob"></div></div></div>
	</div>
			<a id="setToDefault"><?php echo VA_SETDEFAULT; ?></a>
</div>
<div id='test1'></div>