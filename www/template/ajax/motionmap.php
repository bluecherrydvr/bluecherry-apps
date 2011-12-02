<?php defined('INDVR') or exit(); 
	require('../template/template.lib.php');
	  echo "<div id='header'>".MMAP_HEADER."</div>"; 
	  echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > ".EDITING_MMAP." <b>".(empty($motion_map->data[0]['device_name']) ? $motion_map->data[0]['id'] : $motion_map->data[0]['device_name'])."</b></p>";
	  
?>
<INPUT type="Hidden" id="cameraID" value="<?php echo $camera->info['id']; ?>" />
<INPUT type="Hidden" id="cameraType" value="<?php echo $camera->info['protocol']; ?>" />
<INPUT type="Hidden" id="valueString" value="<?php echo $camera->info['motion_map']; ?>" />
<hr />
<?php echo PRE_REC_BUFFER.':'.arrayToSelect($GLOBALS['buffer']['pre'], $camera->info['buffer_prerecording'], 'buffer_prerecording').' | '.POST_REC_BUFFER.':'.arrayToSelect($GLOBALS['buffer']['post'], $camera->info['buffer_postrecording'], 'buffer_postrecording'); ?>
<hr />
<div class='bClear'></div>
<div id="mmapContainer">
<div id="cameraOutputContainer">
	<img id="cameraOutput" width="352" src="/media/mjpeg.php?id=<?php echo $camera->info['id']; ?>" />
</div>
</div>
<div class='bClear'></div>
<div id="lvlSelect">
	<div id='saveButton' style="border:1px solid black; display:inline; cursor:pointer;"><?php echo SAVE_CHANGES; ?></div><br /><br />
	<div id='button' class='showHideImg'><?php echo HIDE_IMG; ?></div>
	<ul>
		<li value="0" id="li0">Off</li>
		<li value="1" id="li1">Minimal</li>
		<li value="2" id="li2">Low</li>
		<li value="3" id="li3" class="on">Average</li>
		<li value="4" id="li4">High</li>
		<li value="5" id="li5">Very High</li>
		<hr />
		<li id="clearAll">Clear All</li>
		<li id="fillAll">Fill All</li>
	</ul>	
</div>
