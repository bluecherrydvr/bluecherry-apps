<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo STORAGE_HEADER; ?></h1>
    </div>
</div>

             <div class="alert alert-warning"><i class="fa fa-warning fa-fw"></i>Note: <ul><li>We will automatically create a directory for you and assign the correct permissions.</li><li>Verify that the storage path you are using is mounted on start or the start of Bluecherry server.  Usually this is done in /etc/fstab.</li><li>If using SMB (CIFS) mount it is recommended to mount network storage with sync option and disabled cache (option cache=none for CIFS)</li></ul></div>



<div class="row">
    <div class="col-lg-12">
        <div class="btn-group pull-right">
            <a href="javascript:void(0);" class="btn btn-success" id="store-add-but"><i class="fa fa-plus fa-fw"></i>  <?php echo ADD_LOCATION; ?></a>
            <a href="javascript:void(0);" class="btn btn-success send-req-form" data-form-id="storage-form"><i class="fa fa-check fa-fw"></i>  <?php echo SAVE_CHANGES; ?></a>
        </div>
        <div class="clearfix"></div>
    </div>
</div>

<br>

<form id="storage-form" action="/storage" method="POST">
    <div class="row">
        <div class="col-lg-12">
            <table class="table table-striped table-storage">
                <tbody>
                <?php foreach ($locations as $id => $location) {
                    $recordTime = intval($location['record_time']);
                    $used_storage = intval($location['used_percent']);
                    $days = floor($recordTime / (3600 * 24));
                    $hours = floor(($recordTime % (3600 * 24)) / 3600);
                    $minutes = floor(($recordTime % 3600) / 60);
                ?>
                    <tr class="table-storage-tr-data">
                        <td class="col-lg-1 col-md-1 col-sm-1 col-xs-1">
                            <label class="control-label"><?php echo LOCATION; ?></label>
                        </td>
                        <td class="col-lg-6 col-md-6 col-sm-6 col-xs-6">
                            <input class="form-control storage-path" name="path[]" type="text" value="<?php echo htmlspecialchars($location['path']); ?>" />
                        </td>
                        <td class="col-lg-1 col-md-1 col-sm-1 col-xs-1 align-right">
                            <label class="control-label">Max:</label>
                        </td>
                        <td class="col-lg-3 col-md-3 col-sm-3 col-xs-3">
                            <div class="input-group">
                                <input class="form-control" type="text" name="max[]" value='<?php echo intval($location['max_thresh']); ?>' /><span class="input-group-addon">%</span>
                            </div>
                        </td>
                        <td class="col-lg-1 col-md-1 col-sm-1 col-xs-1 align-right">
                            <a href="javascript:void(0);" class="btn btn-warning store-add-del"><i class="fa fa-times fa-fw"></i></a>
                        </td>
                    </tr>
                    <tr class="table-storage-tr-info">
                        <td colspan="5">Storage Estimation: <?php echo $days; ?> days <?php echo $hours; ?> hours and <?php echo $minutes; ?> minutes</td>
                    </tr>
                    <tr class="table-storage-tr-info">
                        <td colspan="5">Storage Used: <?php echo $used_storage . "%"; ?></td>
                    </tr>
                    <?php if (!$location['has_sufficient_data']): ?>
                    <tr class="table-storage-tr-info">
                        <td colspan="5" class="text-warning">
                            <i class="fa fa-exclamation-triangle"></i> Provisional estimate - Need at least 24 hours of recording data for accurate calculation
                            <?php if ($location['earliest_recording'] && $location['latest_recording']): ?>
                            <br>Current recording range: <?php echo date('Y-m-d H:i', $location['earliest_recording']); ?> to <?php echo date('Y-m-d H:i', $location['latest_recording']); ?>
                            (<?php echo round($location['total_duration']/3600, 1); ?> hours available)
                            <?php endif; ?>
                        </td>
                    </tr>
                    <?php endif; ?>
                    <tr class="table-storage-tr-info">
                        <td colspan="5">Total Recorded: <?php echo round($location['total_duration']/3600, 1); ?> hours | <?php echo round($location['total_size_mb']/1024, 1); ?> GB used | <?php echo $location['file_count']; ?> files | <?php echo $location['mb_per_hour']; ?> MB/hour average</td>
                    </tr>
                    <tr class="table-storage-tr-info">
                        <td colspan="5">Last 7 Days: <?php echo round($location['total_duration']/3600, 1); ?> hours recorded | <?php echo round($location['total_size_mb']/1024, 1); ?> GB used | <?php echo $location['file_count']; ?> files | <?php echo $location['mb_per_hour']; ?> MB/hour average</td>
                    </tr>
                <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
</form>

<!-- Hidden table row template for adding new storage locations -->
<table id="table-storage-orig" style="display: none;">
    <tr class="table-storage-tr-data">
        <td class="col-lg-1">
            <label class="control-label"><?php echo LOCATION; ?></label>
        </td>
        <td class="col-lg-6">
            <input class="form-control storage-path" name="path[]" type="text" value="" />
        </td>
        <td class="col-lg-1 align-right">
            <label class="control-label">Max:</label>
        </td>
        <td class="col-lg-2">
            <div class="input-group">
                <input class="form-control" type="text" name="max[]" value="95" /><span class="input-group-addon">%</span>
            </div>
        </td>
        <td class="col-lg-2 align-right">
            <a href="" class="btn btn-warning store-add-del"><?php echo L_DELETE; ?></a>
        </td>
    </tr>
</table>
