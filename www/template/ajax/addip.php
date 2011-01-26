<?php defined('INDVR') or exit(); ?>
<h1 id="header" class="header"><?php echo AIP_HEADER; ?></h1>
<hr>
<p><?php echo AIP_SUBHEADER; ?></p>

<FORM id="settingsForm" action="/ajax/update.php" method="post">
<div id="aip">
	<INPUT type="hidden" name="mode" value="addip">
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><INPUT type="Text" id="ipAddr" name="ipAddr"></div>
	<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><INPUT id="port" type="Text" name="port"></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><INPUT id="user" type="Text" name="user"></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><INPUT id="pass" type="Password" name="pass"></div>
	<div><label id="addipLabel"><?php echo AIP_RTSP_PATH; ?></label><INPUT id="rtsp" type="Text" name="rtsp"></div>
	<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><INPUT id="mjpeg" type="Text" name="mjpeg"></div>
	<div><label id="addipLabel">&nbsp;</label><div id='saveButton'><?php echo AIP_ADD; ?></div></div>
</div>
</FORM>
