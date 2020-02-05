<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo REENCODE_SUBHEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"> <?php echo REENCODE_HEADER .' '. $this_camera->info['device_name']; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">

        <form action="/ajax/reencode.php" method="post" class="form-horizontal" >
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
                    <label class="col-lg-4 col-md-4 control-label"><?php echo REENCODE_ENABLED; ?></label>

	           <div class="col-lg-6 col-md-6">
	                <input type="checkbox" name="reencode_enabled" <?php echo $this_camera->info['reencode_livestream'] ? ' checked="checked"':''; ?> <?php echo empty($vaapi_na) ? '' : 'disabled="disabled"' ; ?>  />
	            </div>

                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo REENCODE_BITRATE; ?></label>

                    <div class="col-lg-6 col-md-6">
                        <?php echo arrayToSelect($reencode_bitrates, $this_camera->info['reencode_bitrate'], 'bitrate', 'bitrate'); ?>
                    </div>
                </div>
                    
                <div class="form-group">
                    <label class="col-lg-4 col-md-4 control-label"><?php echo REENCODE_RESOLUTION; ?></label>

                    <div class="col-lg-6 col-md-6">
                        <?php $reencode_res = $this_camera->info['reencode_frame_width'].'x'.$this_camera->info['reencode_frame_height'];
			echo arrayToSelect($reencode_resolutions, $reencode_res, 'resolution', 'resolution'); ?>
                    </div>
                </div>
<div class="col-lg-10 col-md-10">
		<div class="alert alert-warning"><i class="fa fa-warning fa-fw"></i> <?php echo "Note 1:  Livestream reencoding is available only if your GPU supports VAAPI hardware acceleration" ?></div>
		<div class="alert alert-warning"><i class="fa fa-warning fa-fw"></i> <?php echo "Note 2:  Enabling option \"Use substream for liveview\" in IP camera properties overrides this feature, stream provided by IP camera substream is used for live view instead of the reencoded main video stream" ?></div>
</div>
            </div>
        </div>
    </div>
</div>

