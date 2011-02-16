<?php defined('INDVR') or exit(); 
	require('template.lib.php');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
	<title><?php echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_VIEWER; ?></title>
	<link rel="stylesheet" href="/template/styles.css">
	<script type="text/javascript" src="./lib/mootools.js"></script>
	<script type="text/javascript" src="./lib/mootools-more.js"></script>
	<script type="text/javascript" src="./lib/js/viewer.js"></script>
	<script type="text/javascript" src="./lib/js/main.js"></script>
	<script type="text/javascript" src="./lib/js/var.php"></script>
	<script>
		window.addEvent('domready', function(){
			var usl = new viewerSaveLoadLayout();
		});
	</script>
</head>
<body>
<div id="leftColumn">
	<?php echo "<div id='backToAdmin'><a href='/'>".BACK_TO_ADMIN."</a></div>"; ?>
	<div id="lvControls">
		<div><?php echo LAYOUT_CONTROL; ?></div>
		<ul id="lvLayouts">
			<div class='bClear'></div>
			<li class='l1'></li>
			<li class='l4'></li>
			<li class='l9'></li>
			<li class='l16'></li>
			<div class='bClear'></div><hr />
			<li class='ar'></li>
			<li class='dr'></li>
			<li class='ac'></li>
			<li class='dc'></li>
			<div class='bClear'></div>
		</ul>
	</div>
	<div id='lvSavedLayouts'
		<?php	$layouts =  (is_array($lv->layout_list[0])) ? array_merge(array(CHOOSE_LAYOUT), $lv->layout_list) : array(NO_SAVED_LAYOUTS);
			echo arrayToSelect($layouts, $_SESSION['load_layout_name'], 'layouts');
			echo "<INPUT type='Submit' id='deleteLayout' value='".DELETE_CAM."'><INPUT type='Submit' id='saveLayout' value='".SAVE."'><BR /><INPUT type='Submit' id='clearAll' value='".CLEAR_ALL."' ><INPUT type='Submit' id='saveLayoutAs' value='".SAVE_AS."' >";
		?>
	</div>
	<div id="lvCameras">
	<div><?php echo AVAILABLE_DEVICES; ?></div>
	<?php
		foreach($lv->devices as $key => $device){
			if (!in_array($device['id'], $lv->access_list))
				echo "<div class='device' id='{$device['id']}'>{$device['device_name']}</div><div class='bClear'></div>";
		}
	?>
	
	</div>
</div>
<div id="liveViewContainer">
</div>

</body>
</html>