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

<div class="row">
    <div class="col-lg-12">

        <form action="/ajax/ptzsettings.php" method="post" class="form-horizontal">
	    <input type="hidden" name="id" value="<?php echo $this_camera->info['id']; ?>" />

        <div class="form-group">
            <div class="col-lg-12">
                <button class="btn btn-success pull-right send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                <div class="clearfix"></div>
            </div>
        </div>

        <div class="panel panel-default">
            <div class="panel-body">

                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo PTZ_PROTOCOL; ?></label>

                    <div class="col-lg-6 col-md-6">
                        <?php echo arrayToSelect($ptz['PROTOCOLS'], $this_camera->info['ptz_config']['protocol'], 'protocol', 'protocol'); ?>
                    </div>
                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo PTZ_CONTOL_PATH; ?></label>

                    <div class="col-lg-6 col-md-6">
                        <input class="form-control" type="text" name="path" value="<?php echo (!empty($this_camera->info['ptz_config']['path'])) ? $this_camera->info['ptz_config']['path'] : $GLOBALS['ptz']['DEFAULT_PATH']; ?>"  />
                    </div>
                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo PTZ_ADDR; ?></label>

                    <div class="col-lg-6 col-md-6">
                        <input class="form-control" type="text" name="addr" value="<?php echo $this_camera->info['ptz_config']['addr']; ?>"  />
                    </div>
                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo PTZ_BAUD; ?></label>

                    <div class="col-lg-2 col-md-2">
                        <?php echo arrayToSelect($ptz['BAUD'], $this_camera->info['ptz_config']['baud'], 'baud', 'baud'); ?>
                    </div>

                    <label class="col-lg-2 col-md-2 control-label"><?php echo PTZ_BIT; ?></label>

                    <div class="col-lg-2 col-md-2">
                        <?php echo arrayToSelect($ptz['BIT'], $this_camera->info['ptz_config']['bit'], 'bit', 'bit'); ?>
                    </div>
                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo PTZ_STOP_BIT; ?></label>

                    <div class="col-lg-2 col-md-2">
                        <?php echo arrayToSelect($ptz['STOP_BIT'], $this_camera->info['ptz_config']['stop_bit'], 'stop_bit', 'stop_bit'); ?>
                    </div>

                    <label class="col-lg-2 col-md-2 control-label"><?php echo PTZ_PARITY; ?></label>

                    <div class="col-lg-2 col-md-2">
                        <?php echo arrayToSelect($ptz['PARITY'], $this_camera->info['ptz_config']['parity'], 'parity', 'parity'); ?>
                    </div>
                </div>



            </div>
        </div>
    </div>
</div>

