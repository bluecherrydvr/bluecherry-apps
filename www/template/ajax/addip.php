<?php defined('INDVR') or exit(); 
require('../template/template.lib.php');
if (isset($_GET['m'])){
	if ($_GET['m']=='model'){
		
		echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), $ipCamData->data['models']), '', 'models');
		exit;
	}
	else {
		echo arrayToSelect(array_merge(array(AIP_CHOOSE_RES), explode(",", $ipCamData->data['models'])), '', 'models');
		#options tba
		exit;
	}
};

?>

<h1 id="header" class="header"><?php echo AIP_HEADER; ?></h1>
<hr>
<p><?php echo AIP_SUBHEADER; ?></p>


<FORM id="settingsForm" action="/ajax/update.php" method="post">
<div id="aip">
	<INPUT type="hidden" name="mode" value="addip">
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MANUF; ?>:</label><?php echo arrayToSelect(array_merge(array(AIP_CHOOSE_MANUF), $ipCamData->data['manufacturers']), '', 'manufacturers'); ?></div>
	<div><label id="addipLabel"><?php echo AIP_CHOOSE_MODEL; ?>:</label><span id="modelSelector"><?php echo arrayToSelect(array(AIP_CH_MAN_FIRST), AIP_CH_MAN_FIRST, 'models', '', true); ?></span></div>
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><INPUT type="Text" disabled id="ipAddr" name="ipAddr"></div>
	<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><INPUT disabled id="port" type="Text" name="port"></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><INPUT disabled id="user" type="Text" name="user"></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><INPUT disabled id="pass" type="Password" name="pass"></div>
	<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><INPUT disabled id="mjpeg" type="Text" name="mjpeg"></div>
	<div><label id="addipLabel">&nbsp;</label><INPUT disabled id="saveButton" type="Submit" value="<?php echo AIP_ADD; ?>"></div>
</div>
</FORM>
