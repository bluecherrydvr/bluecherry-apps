<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo SCHED_HEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"> <?php echo (($global) ? '<b>'.EDITING_GLOBAL_SCHED.'</b>' : EDITING_SCHED." <b>".(($device_schedule->schedule_data[0]['device_name']) ? $device_schedule->schedule_data[0]['device_name'] : $device_schedule->schedule_data[0]['id'])."</b>"); ?></li>
        </ol>
        </h1>
    </div>
</div>



<div class="row">
    <div class="col-lg-12">
    <form action="/ajax/update.php" method="post" class="form-horizontal" id="motion-submit">

        <input type="hidden" name="id" value="<?php echo ($global) ? 'global' : $device_schedule->schedule_data[0]['id']; ?>" />
        <input type="hidden" name="<?php echo (!$global) ? 'schedule' : 'G_DEV_SCED' ; ?>" id="motion-map" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
        <input type="hidden" name="type" value="Devices" />
        <input type="hidden" name="mode" value="<?php echo (!$global) ? 'update' : 'global' ; ?>" />


        <div class="form-group">
            <div class="col-lg-12">
                <?php if (!$global) { ?>
                    <div class="alert alert-<?php echo (($og) ? 'success' : 'warning'); ?>">
                        <label><i class="fa fa-fw <?php echo (($og) ? 'fa-check-circle' : 'fa-warning'); ?>"></i></label>
                        <label class="checkbox-inline form-control-static">
                            <input type="checkbox" class="click-event" data-function="scheduleOverrideGlobal" <?php echo (($og) ? ' checked="checked"': '' ); ?> /> <?php echo GLOBAL_SCHED_OG; ?>
                            <input type="hidden" name="schedule_override_global" value="<?php echo (($og) ? '1': '0' ); ?>" />
                        </label>
                    </div>
                <?php } ?>
            </div>
        </div>
                    
        <div class="form-group">
            <div class="col-lg-12">
                <?php if (!$global) { ?>
                        <label class="checkbox-inline form-control-static">
                            <input type="checkbox" class="click-event" data-function="triggerONVIFevents"  <?php echo (($device_schedule->schedule_data[0]['onvif_events_enabled']) ? ' checked="checked"': '' ); ?> /> <?php echo TRIGGER_ONVIF_EVENTS; ?>
                            <input type="hidden" name="onvif_events_enabled" value="<?php echo (($device_schedule->schedule_data[0]['onvif_events_enabled']) ? '1': '0' ); ?>" />
                        </label>
                        <div id="onvif-live-event-types-list" style="margin-top:10px;"></div>
                <?php } ?>
            </div>
        </div>

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
                                    <button type="button" class="btn btn-default btn-block motion-btn-sens click-event disabled" data-class="motionGrid.off()">Off</button>
                                </div>
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-danger btn-block motion-btn-sens click-event disabled" data-class="motionGrid.veryHigh()">Continuous</button>
                                </div>
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-success btn-block motion-btn-sens click-event" data-class="motionGrid.minimal()">Motion</button>
                                </div>
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-primary btn-block motion-btn-sens click-event disabled" data-class="motionGrid.average()">Continuous+Motion</button>
                                </div>
                                <div class="col-lg-2 col-md-3">
                                    <button type="button" class="btn btn-warning btn-block motion-btn-sens click-event disabled" data-class="motionGrid.high()">Trigger</button>
                                </div>
                            </div>

                            <div class="form-group">
                                <div class="col-lg-12 col-md-12">
                                </div>
                            </div>

                            <div class="form-group">
                                <div class="col-lg-12 col-md-12">
                                    <button type="button" class="btn btn-grey click-event" data-class="motionGrid.setSchedule().fillAll()">Fill All</button>
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

