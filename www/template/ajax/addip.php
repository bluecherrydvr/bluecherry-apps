<?php defined('INDVR') or exit(); 
require('../template/template.lib.php');

$mode = (!empty($_GET['m'])) ? $_GET['m'] : false;

if ($mode=='model'){
	if ($_GET['manufacturer'] == 'Generic'){
		echo "<INPUT type='hidden' name='models' id='models' value='Generic' readonly>";
		exit();
	}
	echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), Cameras::getList($_GET['manufacturer'])), '', 'models');
	exit;
};
if ($mode=='ops') {
    Cameras::getCamDetails($_GET['model']);
    exit;
};

?>
	
<h1 id="header" class="header"><?php echo AIP_HEADER; ?></h1>
<hr />
<p><?php echo AIP_SUBHEADER; ?></p>

<div id="message" class="INFO">
    <?php echo IPCAM_WIKI_LINK ?>
</div>
<FORM id="settingsForm" action="/ajax/addip.php" method="post">
<div id="aip">
	<input type="hidden" name="mode" value="addip" />
        <div><label id="addipLabel"><?php echo AIP_CHOOSE_MANUF; ?>:</label><?php echo arrayToSelect(array_merge(array(AIP_CHOOSE_MANUF), Manufacturers::getList()), '', 'manufacturers'); ?></div>
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MODEL; ?>:</label><span id="modelSelector"><?php echo arrayToSelect(array(AIP_CH_MAN_FIRST), AIP_CH_MAN_FIRST, 'models', '', true); ?></span></div>
	<!--<div><label id="addipLabel"><?php echo AIP_CHOOSE_FPSRES; ?>:</label><span id="fpsresSelector"><?php echo arrayToSelect(array(AIP_CH_MOD_FIRST), AIP_CH_MOD_FIRST, 'fpsres', '', true); ?></span></div>!-->
	<div><label id="addipLabel"><?php echo AIP_CAMERA_NAME; ?></label><input type="Text" disabled="disabled" id="camName" name="camName" /></div>
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><input type="Text" disabled="disabled" id="ipAddr" name="ipAddr" /></div>
	<div style="display:none;"><label id="addipLabel"><?php echo AIP_IP_ADDR_MJPEG; ?></label><input type="Text" disabled="disabled" id="ipAddrMjpeg" name="ipAddrMjpeg" /></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><input disabled="disabled" id="user" type="Text" name="user" /></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><input disabled="disabled" id="pass" type="Password" name="pass" /></div>
	<div id="advancedSettingsSwitch">[<?php echo AIP_ADVANCED_SETTINGS; ?>]</div>
	<div id="advancedSettings">
		<div><label id="addipLabel"><?php echo AIP_PROTOCOL; ?></label>
			<select name="protocol" id="protocol">
				<option value="IP-RTSP">RTSP</option>
				<option value="IP-MJPEG">MJPEG</option>
			</select>
		</div>
		<div id='rtsp-settings'>
			<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><input disabled="disabled" id="rtsppath" type="Text" name="rtsp" /></div>
			<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><input disabled="disabled" id="port" type="Text" name="port" value="554" /></div>
			<div><label id="addipLabel"><?php echo IPCAM_TCPUDP_LEVEL; ?></label><select name="prefertcp"><option value="1">TCP</option><option value="0">UDP</option></select></div>
		</div>
		<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><input disabled="disabled" id="mjpeg" type="Text" name="mjpeg" /></div>
		<div><label id="addipLabel"><?php echo AIP_PORT_MJPEG; ?></label><input disabled="disabled" id="portMjpeg" type="Text" name="portMjpeg" value="80" /></div>
	</div>
	<div class='bClear'></div>
	<div><label id="addipLabel"></label><input disabled="disabled" id="saveButton" type="Submit" value="<?php echo AIP_ADD; ?>" /></div>
	
</div>
</FORM>
