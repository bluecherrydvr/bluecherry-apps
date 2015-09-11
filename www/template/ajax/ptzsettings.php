<?php defined('INDVR') or exit();
#template common functions
require('../template/template.lib.php');
?>


<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo PTZ_SET_SUBHEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"> <?php echo PTZ_SET_HEADER . $this_camera->info['device_name']; ?></li>
        </ol>
        </h1>
    </div>
</div>



<h1 class="header" id="header"><?php echo PTZ_SET_SUBHEADER; ?></h1>
<p><?php echo "<a href='#' id='backToList'>".ALL_DEVICES."</a> &gt; ".PTZ_SET_HEADER." {$this_camera->info['device_name']}"; ?></p>
<FORM method='Post' action='/ajax/ptzsettings.php' id='settingsForm'>
	<INPUT type="Hidden" name="id" value="<?php echo $this_camera->info['id']; ?>" />
	<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>
	<div><label><?php echo PTZ_PROTOCOL; ?></label><?php echo arrayToSelect($ptz['PROTOCOLS'], $this_camera->info['ptz_config']['protocol'], 'protocol', 'protocol'); ?></div>
	<div><label><?php echo PTZ_CONTOL_PATH; ?></label><INPUT type="Text" name="path" value="<?php echo (!empty($this_camera->info['ptz_config']['path'])) ? $this_camera->info['ptz_config']['path'] : $GLOBALS['ptz']['DEFAULT_PATH']; ?>" /></div>
	<div><label><?php echo PTZ_ADDR; ?></label><INPUT type="Text" name="addr" value="<?php echo $this_camera->info['ptz_config']['addr']; ?>" /></div>
	<div><label><?php echo PTZ_BAUD; ?></label><?php echo arrayToSelect($ptz['BAUD'], $this_camera->info['ptz_config']['baud'], 'baud', 'baud'); ?></div>
	<div><label><?php echo PTZ_BIT; ?></label><?php echo arrayToSelect($ptz['BIT'], $this_camera->info['ptz_config']['bit'], 'bit', 'bit'); ?></div>
	<div><label><?php echo PTZ_STOP_BIT; ?></label><?php echo arrayToSelect($ptz['STOP_BIT'], $this_camera->info['ptz_config']['stop_bit'], 'stop_bit', 'stop_bit'); ?></div>
	<div><label><?php echo PTZ_PARITY; ?></label><?php echo arrayToSelect($ptz['PARITY'], $this_camera->info['ptz_config']['parity'], 'parity', 'parity'); ?></div>
</FORM>
