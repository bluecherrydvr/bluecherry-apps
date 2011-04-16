<?php defined('INDVR') or exit(); 
	require('template.lib.php');


#temp PTZ fix

$ptzdevices = data::query("SELECT * FROM Devices WHERE ptz_serial_values <> ''");

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
	<title><?php echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_VIEWER; ?></title>
	<link rel="stylesheet" href="/template/styles.css">
	<script type="text/javascript" src="./lib/mootools.js"></script>
	<script type="text/javascript" src="./lib/mootools-more.js"></script>
	<script type="text/javascript" src="./lib/js/viewer.js"></script>
	<script type="text/javascript" src="./lib/js/var.php"></script>
	<script>
		window.addEvent('domready', function(){
			var usl = new viewerSaveLoadLayout();
		
		$('ptzControl').getChildren().each(function(el){			
			el.addEvents({
				'click': function(){
					sendPtzCommand($('ptzId').get('value'), 'move', el.get('class'), false);
				},
				'mousedown': function(){
					sendPtzCommand($('ptzId').get('value'), 'move', el.get('class'), true);
				},
				'mouseup': function(){
					sendPtzCommand($('ptzId').get('value'), 'stop', el.get('class'), true);
				},
				'mouseout': function(){
					sendPtzCommand($('ptzId').get('value'), 'stop', el.get('class'), true);
				}
			});			
			
		});
		});
	</script>
<style>
		#ptz { border:1px solid gray; -moz-border-radius: 5px; -webkit-border-radius: 5px; padding:10px 0 10px 17px; margin-bottom:15px; }
		#ptzControl { width: 146px; height:200px; position:relative; }
		#ptzControl	div	{ width:48px; height:48px; background-repeat: no-repeat background-position: center center; cursor:pointer; position:relative; float:left; }
		#ptzControl .lu	{ background-image: url("/img/ptz/lu.png"); }
		#ptzControl .ln	{ background-image: url("/img/ptz/l.png");  }
		#ptzControl .ld	{ background-image: url("/img/ptz/ld.png"); }
		#ptzControl .nd	{ background-image: url("/img/ptz/d.png");  }
		#ptzControl .ru	{ background-image: url("/img/ptz/ru.png"); }
		#ptzControl .rn	{ background-image: url("/img/ptz/r.png");  }
		#ptzControl .nu	{ background-image: url("/img/ptz/u.png");  }
		#ptzControl .rd	{ background-image: url("/img/ptz/rd.png"); }
		#ptzControl .t	{ background-image: url("/img/ptz/in.png");  }
		#ptzControl .w	{ background-image: url("/img/ptz/out.png"); margin-left:25px; }
</style>

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
	<div id='lvSavedLayouts'>
		<?php	$layouts =  (is_array($lv->layout_list[0])) ? array_merge(array(CHOOSE_LAYOUT), $lv->layout_list) : array(NO_SAVED_LAYOUTS);
			echo arrayToSelect($layouts, $_SESSION['load_layout_name'], 'layouts');
			echo "<INPUT type='Submit' id='deleteLayout' value='".DELETE_CAM."'><INPUT type='Submit' id='saveLayout' value='".SAVE."'><BR /><INPUT type='Submit' id='clearAll' value='".CLEAR_ALL."' ><INPUT type='Submit' id='saveLayoutAs' value='".SAVE_AS."' >";
		?>
	</div>
	<div id="ptz" style="text-align:center;">
		<select id='ptzId'>
<?php
	foreach($ptzdevices as $key => $device){
		echo "<option value='{$device['id']}'>{$device['device_name']}";
	}

?>
		</select>
		<div id="ptzControl">
			<div class="lu"></div>
			<div class="nu"></div>
			<div class="ru"></div>
			<div class="ln"></div>
			<div></div>
			<div class="rn"></div>
			<div class="ld"></div>
			<div class="nd"></div>
			<div class="rd"></div>
			<hr><div class="w"></div><div class="t"></div>
		</div>
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
