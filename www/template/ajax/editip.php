<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

echo "<div id='header'>".PROPS_HEADER."</div>";
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> > {$ipCamera->info['device_name']} </p>"; 	
?>

<FORM id="settingsForm" action="/ajax/editip.php" method="post">
<div id="aip" class="container-separator">
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MANUF; ?>:</label><?php echo arrayToSelect(ipCameras('manufacturers'), $ipCamera->info['manufacturer'], 'manufacturers'); ?></div>
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MODEL; ?>:</label><span id="modelSelector"><?php echo arrayToSelect(ipCameras('models', $ipCamera->info['manufacturer']), $ipCamera->info['model'], 'models', true); ?></span></div>
	<div><label id="addipLabel"><?php echo VA_AUDIO_ENABLE; ?></label><input id='audio_enabled' type='Checkbox' name="audio_enabled"  <?php echo !($ipCamera->info['audio_disabled']) ? ' checked="checked"' : ''; ?>/>
		<div id='message' class='INFO'>Audio support is currently experimental. It may cause problems and numerous errors in the log files. Use at your own risk.</div>
	</div>
	<input type="hidden" name="mode" value="editIp" />
	<input type="hidden" name="id" value="<?php echo $ipCamera->info['id']; ?>" />
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><input type="Text" id="ipAddr" name="ipAddr" value="<?php echo $ipCamera->info['ipAddr'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><input id="user" type="Text" name="user" value="<?php echo $ipCamera->info['rtsp_username'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><input id="pass" type="Password" name="pass" value="<?php echo $ipCamera->info['rtsp_password'];?>" /></div>
			<div><label id="addipLabel"><?php echo AIP_PROTOCOL; ?></label>
			<select name="protocol" id="protocol">
				<option value="IP-RTSP">RTSP</option>
				<option value="IP-MJPEG" <?php echo ($ipCamera->info['protocol']) == 'IP-MJPEG' ? "selected" : "";  ?>>MJPEG</option>
			</select>
		</div>
	<div  id='rtsp-settings' <?php echo ($ipCamera->info['protocol']) == 'IP-MJPEG' ? 'style="display:none;"' : ""; ?>>
		<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><input id="rtsppath" type="Text" name="rtsp" value="<?php echo $ipCamera->info['rtsp'];?>" /></div>
		<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><input id="port" type="Text" name="port" value="<?php echo $ipCamera->info['port'];?>" /></div>
	</div>
	<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><input id="mjpeg" type="Text" name="mjpeg" value="<?php echo $ipCamera->info['mjpeg_path'];?>" /></div>
	<div style="display:none;"><label id="addipLabel"><?php echo AIP_IP_ADDR_MJPEG; ?></label><input type="Text" id="ipAddrMjpeg" name="ipAddrMjpeg" value="<?php echo $ipCamera->info['ipAddrMjpeg'];?>" /></div>
	<div><label id="addipLabel"><?php echo AIP_PORT_MJPEG; ?></label><input id="portMjpeg" type="Text" name="portMjpeg" value="<?php echo $ipCamera->info['portMjpeg'];?>" /></div>
	<div><label id="addipLabel"><?php echo IPCAM_TCPUDP_LEVEL; ?></label><select name="prefertcp"><option value="1">TCP</option><option value="0" <?php echo ($ipCamera->info['rtsp_rtp_prefer_tcp']==0) ? "selected" : ""; ?>>UDP</option></select></div>
	<div class="bClear"></div>
	<div id="advancedSettingsSwitch">[<?php echo AIP_ADVANCED_SETTINGS; ?>]</div>
	<div class="bClear"></div>
	<div id="advancedSettings">
		<div><label id="addipLabel"><?php echo IPCAM_DEBUG_LEVEL; ?></label><input type='checkbox' name='debug_level' <?php echo ($ipCamera->info['debug_level']==1) ? 'checked' : ''; ?>></div>
		
	</div>
	<div class="bClear"></div>
	<div><label id="addipLabel"></label><input id="saveButton" type="Submit" value="<?php echo SAVE_CHANGES; ?>" /></div>
</div>
</FORM>
