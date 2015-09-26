<?php defined('INDVR') or exit(); 
	require('../template/template.lib.php');
	  echo "<h1 id='header'>".MMAP_HEADER."</h1>"; 
	  echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".EDITING_MMAP." <b>".(empty($camera->info['device_name']) ? $camera->info['id'] : $camera->info['device_name'])."</b></p>";
	  
?>
<INPUT type="Hidden" id="cameraID" value="<?php echo $camera->info['id']; ?>" />
<INPUT type="Hidden" id="cameraType" value="<?php echo $camera->info['protocol']; ?>" />
<INPUT type="Hidden" id="valueString" value="<?php echo $camera->info['motion_map']; ?>" />
<div id='saveButton' style="border:1px solid black; display:inline; cursor:pointer;"><?php echo SAVE_CHANGES; ?></div>
<div class='bClear'></div>
<div class='container-separator'>
	<div class='title'><?php echo BUFFER_TITLE; ?></div>
	<?php echo '<span>'.PRE_REC_BUFFER.':'.arrayToSelect($GLOBALS['buffer']['pre'], $camera->info['buffer_prerecording'], 'buffer_prerecording').'</span> | <span>'.POST_REC_BUFFER.':'.arrayToSelect($GLOBALS['buffer']['post'], $camera->info['buffer_postrecording'], 'buffer_postrecording').'</span>'; ?>
</div>

<div class='container-separator container-separator-form'>
	<div class='title'><?php echo MOTION_ALGORITHM_TITLE; ?></div>

	<div>
		<label style="display: block; width: 150px; heigth: 50px">
			<?php echo MOTION_ALGORITHM; ?>
		</label>
		<div style="float: left; height: 50px; width: 410px">
			<input type="radio" name="motion_algorithm" value="0" style="width: 30px;"
				<?php echo $camera->info['motion_algorithm'] == 0 ? ' checked="checked"' : '' ?> /> 
				<?php echo MOTION_DEFAULT; ?>
			<br clear="all" />
			<input type="radio" name="motion_algorithm" value="1" style="width: 30px;"
				<?php echo $camera->info['motion_algorithm'] == 1 ? ' checked="checked"' : '' ?> />
				<?php echo MOTION_EXPERIMENTAL; ?>
		</div>
	</div>

	<div>
		<label style="display: block; width: 150px; heigth: 50px">
			<?php echo 'frame factor'; ?>
		</label>
		<div style="float: left; width: 410px">
			<?php echo arrayToSelect(Array('0.1' => '0.1', '0.2' => '0.2', '0.3' => '0.3', '0.4' => '0.4', '0.5' => '0.5', '0.6' => '0.6', '0.7' => '0.7', '0.8' => '0.8', '0.9' => '0.9', '1.0' => '1.0'), $camera->info['frame_downscale_factor'], 'frame_downscale_factor'); ?>
		</div>
	</div>

	<div class='bClear'></div>
	<div id="videoAdjSliders">
		<div class="control">
			<label style="width: 160px;"><?php echo 'min_motion_area'; ?>:<input disabled="disabled" value="<?php echo $camera->info['min_motion_area']; ?>" id="min_motion_area_value" type="text"></label>
			<div id="min_motion_area" class="slider" style="width: 180px;">
				<div class="knob"></div>
			</div>
		</div>

		<div class='bClear'></div>
	</div>
</div>

<div id="lvlSelect" class='container-separator'>
	<div class='title'><?php echo MMAP_SELECTOR_TITLE; ?></div>
	<ul>
		<li value="0" id="li0">Off</li>
		<li value="1" id="li1">Minimal</li>
		<li value="2" id="li2">Low</li>
		<li value="3" id="li3" class="on">Average</li>
		<li value="4" id="li4">High</li>
		<li value="5" id="li5">Very High</li>
	</ul>
	<ul class="all">
		<li id="clearAll">Clear All</li>
		<li id="fillAll">Fill All</li>
	</ul>
	<div class='bClear'></div>
</div>
<div class='container-separator'>
<div id="mmapContainer" >
	<div id="cameraOutputContainer">
		<img id="cameraOutput" width="352" src="/media/mjpeg.php?id=<?php echo $camera->info['id']; ?>" />
		<div id='button' class='showHideImg'><?php echo HIDE_IMG; ?></div>
	</div>
</div>
</div>
