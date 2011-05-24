<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

echo "<div id='header'>".PROPS_HEADER."</div>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > {$ipCamera->info['device_name']} </p>"; 	
?>


<FORM id="settingsForm" action="/ajax/update.php" method="post">
<div id="aip">
	<input type="hidden" name="mode" value="editIp" />
	<input type="hidden" name="id" value="<?php echo $ipCamera->info['id']; ?>" />
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><input type="Text" id="ipAddr" name="ipAddr" value="<?php echo $ipCamera->info['ipAddr'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><input id="port" type="Text" name="port" value="<?php echo $ipCamera->info['port'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><input id="user" type="Text" name="user" value="<?php echo $ipCamera->info['rtsp_username'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><input id="pass" type="Password" name="pass" value="<?php echo $ipCamera->info['rtsp_password'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><input id="rtsppath" type="Text" name="rtsp" value="<?php echo $ipCamera->info['rtsp'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><input id="mjpeg" type="Text" name="mjpeg" value="<?php echo $ipCamera->info['mjpeg_path'];?>" /></div>
	<div style="display:none;"><label id="addipLabel"><?php echo AIP_IP_ADDR_MJPEG; ?></label><input type="Text" id="ipAddrMjpeg" name="ipAddrMjpeg" value="<?php echo $ipCamera->info['ipAddrMjpeg'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_PORT_MJPEG; ?></label><input id="portMjpeg" type="Text" name="Mjpegport" value="<?php echo $ipCamera->info['portMjpeg'];?>" /></div>
	<div><label id="addipLabel"></label><input id="saveButton" type="Submit" value="<?php echo SAVE_CHANGES; ?>" /></div>
</div>
</FORM>
	
