<?php defined('INDVR') or exit(); 
require('../template/template.lib.php');
$mode = (!empty($_GET['m'])) ? $_GET['m'] : false;
if ($mode) {
	if ($mode=='model'){
		
		echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), $ipCamData->data['models']), '', 'models');
		exit;
	}
	else {
		$data = "
			<mjpegPath><![CDATA[{$ipCamData->data['mjpeg_path']}]]></mjpegPath>
			<rtspPath><![CDATA[{$ipCamData->data['rtsp_path']}]]></rtspPath>
			<mjpegPort><![CDATA[{$ipCamData->data['mjpeg_port']}]]></mjpegPort>
			<rtspPort><![CDATA[{$ipCamData->data['rtsp_port']}]]></rtspPort>
			<resolutions><![CDATA[{$ipCamData->data['resolutions']}]]></resolutions>
		";
		data::responseXml(true, true, $data);
		exit;
	}
};

?>
	
<h1 id="header" class="header"><?php echo AIP_HEADER; ?></h1>
<hr />
<p><?php echo AIP_SUBHEADER; ?></p>

<div id="ajaxMessage" class="INFO"><?php echo IPCAM_WIKI_LINK; ?></div>
<FORM id="settingsForm" action="/ajax/addip.php" method="post">
<div id="aip">
	<input type="hidden" name="mode" value="addip" />
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MANUF; ?>:</label><?php echo arrayToSelect(array_merge(array(AIP_CHOOSE_MANUF), $ipCamData->data['manufacturers']), '', 'manufacturers'); ?></div>
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MODEL; ?>:</label><span id="modelSelector"><?php echo arrayToSelect(array(AIP_CH_MAN_FIRST), AIP_CH_MAN_FIRST, 'models', '', true); ?></span></div>
	<!--<div><label id="addipLabel"><?php echo AIP_CHOOSE_FPSRES; ?>:</label><span id="fpsresSelector"><?php echo arrayToSelect(array(AIP_CH_MOD_FIRST), AIP_CH_MOD_FIRST, 'fpsres', '', true); ?></span></div>!-->
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><input type="Text" disabled="disabled" id="ipAddr" name="ipAddr" /></div>
	<div style="display:none;"><label id="addipLabel"><?php echo AIP_IP_ADDR_MJPEG; ?></label><input type="Text" disabled="disabled" id="ipAddrMjpeg" name="ipAddrMjpeg" /></div>
	<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><input disabled="disabled" id="port" type="Text" name="port" value="554" /></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><input disabled="disabled" id="user" type="Text" name="user" /></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><input disabled="disabled" id="pass" type="Password" name="pass" /></div>
	<div id="advancedSettingsSwitch">[<?php echo AIP_ADVANCED_SETTINGS; ?>]</div>
	<div id="advancedSettings">
		<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><input disabled="disabled" id="rtsppath" type="Text" name="rtsp" /></div>
		<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><input disabled="disabled" id="mjpeg" type="Text" name="mjpeg" /></div>
		<div><label id="addipLabel"><?php echo AIP_PORT_MJPEG; ?></label><input disabled="disabled" id="portMjpeg" type="Text" name="portMjpeg" value="80" /></div>
	</div>
	<div><label id="addipLabel"></label><input disabled="disabled" id="saveButton" type="Submit" value="<?php echo AIP_ADD; ?>" /></div>
	
</div>
</FORM>
