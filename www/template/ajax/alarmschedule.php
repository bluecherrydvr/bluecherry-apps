<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo ALRMSCHED_HEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"> <?php echo  '<b>'.EDITING_GLOBAL_SCHED.'</b>'; ?></li>
        </ol>
        </h1>
    </div>
</div>



<div class="row">
    <div class="col-lg-12">
    <form action="/ajax/update.php" method="post" class="form-horizontal" id="motion-submit">

        <input type="hidden" name="id" value="global" />
        <input type="hidden" name="G_ALARM_SCED" id="motion-map" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
        <input type="hidden" name="type" value="Devices" />
        <input type="hidden" name="mode" value="global" />

        <div class="form-group">
            <div class="col-lg-12">
                <button class="btn btn-success pull-right send-req-form" type="submit" data-func="getMotionMap"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                <div class="clearfix"></div>
            </div>
        </div>

        <div class="form-group">
            <div class="col-lg-12">
                <div class="panel panel-default">
                    <div class="panel-heading"><?php echo SCHED_SELECTOR_TITLE; ?></div>
                        <div class="panel-body">
                
                            <div class="form-group motion-sens-bl">
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-default btn-block motion-btn-sens click-event disabled" data-class="motionGrid.off()"><?php echo SCHED_OFF; ?></button>
                                </div>
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-danger btn-block motion-btn-sens click-event disabled" data-class="motionGrid.veryHigh()"><?php echo SCHED_SENSOR; ?></button>
                                </div>
                            </div>

                            <div class="form-group">
                                <div class="col-lg-12 col-md-12">
                                </div>
                            </div>

                            <div class="form-group">
                                <div class="col-lg-12 col-md-12">
                                    <button type="button" class="btn btn-grey click-event" data-class="motionGrid.setSchedule().fillAll()"><?php echo SCHED_FILLALL; ?></button>
                                </div>
                            </div>
    
                        </div>
                </div>
            </div>
        </div>

        <div class="form-group">
            <div class="col-lg-12">
                <div class="panel panel-default">
                    <div class="panel-body">
                        <div class=" grid-bl table-responsive">
                        </div>
                    </div>
                </div>
            </div>
        </div>

    </form>
    </div>
</div>

<?php 

addJs("
$(function() {
    var mg = new motionGrid($('.motion-sens-bl').find('.btn-success'));
    mg.setSchedule();
    mg.minimal();
    mg.sheduleDrawGrid();
});
");
?>

