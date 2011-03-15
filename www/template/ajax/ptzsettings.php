<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');

?>
<h1 class="header" id="header"><?php echo PTZ_SET_HEADER."{$this_camera->data['device_name']}"; ?></h1><hr>
<p><?php echo PTZ_SET_SUBHEADER; ?></p>
<FORM method='Post' action='/ajax/ptzsettings.php' id='settingsForm'>
	<INPUT type="Hidden" name="id" value="<?php echo $this_camera->data['id']; ?>">
	<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>
	<div><label><?php echo PTZ_CONTOL_PATH; ?></label><INPUT type="Text" name="path" value="<?php echo $this_camera->ptz_config['path']; ?>"></div>
	<div><label><?php echo PTZ_VALUES; ?></label><INPUT type="Text" name="values" value="<?php echo $this_camera->ptz_config['values']; ?>"></div>
	<div><label><?php echo PTZ_BAUD; ?></label><?php echo arrayToSelect($ptz['BAUD'], $this_camera->ptz_config['baud'], 'baud', 'baud'); ?></div>
	<div><label><?php echo PTZ_BIT; ?></label><?php echo arrayToSelect($ptz['BIT'], $this_camera->ptz_config['bit'], 'bit', 'bit'); ?></div>
	<div><label><?php echo PTZ_STOP_BIT; ?></label><?php echo arrayToSelect($ptz['STOP_BIT'], $this_camera->ptz_config['stop_bit'], 'stop_bit', 'stop_bit'); ?></div>
	<div><label><?php echo PTZ_PARITY; ?></label><?php echo arrayToSelect($ptz['PARITY'], $this_camera->ptz_config['parity'], 'parity', 'parity'); ?></div>
</FORM>