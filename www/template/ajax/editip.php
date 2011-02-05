<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

echo "<div id='header'>".PROPS_HEADER."</div>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > {$ipCamera->camera[0]['device_name']} </p>"; 	
?>


<FORM id="settingsForm" action="/ajax/update.php" method="post">
<div id="aip">
	<INPUT type="hidden" name="mode" value="editIp">
	<INPUT type="hidden" name="id" value="<?php echo $ipCamera->camera[0]['id']; ?>">
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><INPUT type="Text" id="ipAddr" name="ipAddr" value="<?php echo $ipCamera->camera[0]['ipAddr'];?>"></div>
	<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><INPUT id="port" type="Text" name="port" value="<?php echo $ipCamera->camera[0]['port'];?>"></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><INPUT id="user" type="Text" name="user" value="<?php echo $ipCamera->camera[0]['rtsp_username'];?>"></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><INPUT id="pass" type="Password" name="pass" value="<?php echo $ipCamera->camera[0]['rtsp_password'];?>"></div>
	<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><INPUT id="rtsppath" type="Text" name="rtsp" value="<?php echo $ipCamera->camera[0]['rtsp'];?>"></div>
	<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><INPUT id="mjpeg" type="Text" name="mjpeg" value="<?php echo $ipCamera->camera[0]['mjpeg_path'];?>"></div>
	<div><label id="addipLabel">&nbsp;</label><INPUT id="saveButton" type="Submit" value="<?php echo SAVE_CHANGES; ?>"></div>
</div>
</FORM>
	