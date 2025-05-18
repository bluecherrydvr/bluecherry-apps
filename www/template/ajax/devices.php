<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo MMENU_DEVICES; ?>
    
        <ol class="breadcrumb">
           <li class="active"><?php echo TOTAL_DEVICES; ?>: <b><?php echo $devices->info['total_devices']; ?></b></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="pull-right">
            <a href="/discover-cameras" class="btn btn-success ajax-content" role="button"><i class="fa fa-search fa-fw"></i> <?php echo DISCOVER_IP_CAMERAS; ?></a>
            <a href="/addip" class="btn btn-success ajax-content" role="button"><i class="fa fa-plus fa-fw"></i> <?php echo AIP_HEADER; ?></a>
        </div>
        <div class="clearfix"></div>
    </div>
</div>
<br>

<div class="row">
    <div class="col-lg-12">
        <div class="panel-group" id="devices-bl" role="tablist" aria-multiselectable="true">

            <div class="panel panel-default">
                <div class="panel-heading" role="tab" id="devices-bl-ip-head">
                    <h4 class="panel-title">
                        <a href="#devices-bl-ip" role="button" data-toggle="collapse" data-parent="#devices-bl"><i class="fa fa-list-ul fa-fw"></i>IP Cameras</a>
                    </h4>
                </div>
                <div id="devices-bl-ip" class="panel-collapse collapse in" role="tabpanel" aria-labelledby="devices-bl-ip">
                    <div class="panel-body">

                    <div class="row" id="devices-ip-ok">
                        <span id="devices-text-status-OK" style="display: none;"><?php echo DEVICE_VIDEO_STATUS_CHANGE_OK; ?></span>
                        <span id="devices-text-status-disabled" style="display: none;"><?php echo DEVICE_VIDEO_STATUS_CHANGE_disabled; ?></span>
                        <?php foreach($devices->ipCameras->ok as $val) { echo $val; } ?>
                    </div>

                    <div class="row" id="devices-ip-disabled" <?php echo ($devices->ipCameras->disabled) ? '' : 'style="display: none;"'; ?>>
                        <hr>
                        <?php foreach($devices->ipCameras->disabled as $val) { echo $val; } ?>
                    </div>

                    </div>
                </div>
            </div>

            <?php if ($devices->cards){ ?>
            
            <?php 
                $counter = 0;
                foreach($devices->cards as $key => $card){ 
                $counter++;
            ?>

            <div class="panel panel-default">
                <div class="panel-heading" role="tab" id="devices-bl-cards-head<?php echo $counter; ?>">
                    <h4 class="panel-title">
                        <a href="#devices-bl-cards<?php echo $counter; ?>" role="button" data-toggle="collapse" data-parent="#devices-bl<?php echo $counter; ?>"><i class="fa fa-ellipsis-h fa-fw"></i>
                        #<?php echo $counter ." ".CARD_HEADER." ".$card->info['ports']." ".PORT." (".$card->info['driver'].")"; ?>
                        </a>
                    </h4>
                </div>
                <div id="devices-bl-cards<?php echo $counter; ?>" class="panel-collapse collapse" role="tabpanel" aria-labelledby="devices-bl-cards<?php echo $counter; ?>">
                    <div class="panel-body">


                        <div class="panel panel-default">
                            <div class="panel-body">
                                <div class="row">
                                    <div class="col-lg-6 col-md-6">
                                        Encoding: <?php echo (($card->info['encoding']=='notconfigured') ? '<b>'.SIGNAL_TYPE_NOT_CONFD."</b> | 
                                            <form action='/ajax/update.php' method='POST' style='display: inline-block'>
                                                <input type='hidden' name='mode' value='enableAll'>
                                                <input type='hidden' name='type' value='Devices'>
                                                <input type='hidden' name='id' value='null'>
                                                <input type='hidden' name='card_id' value='{$card->info['id']}'>
                                                <a  href='javascript:void(0);' class='send-req-form' id='{$card->info['id']}' data-func-after='devicesReloadPage'>".ENABLE_ALL_PORTS."</a>
                                            </form>" :

					"<form action='/ajax/update.php' method='POST' style='display: inline-block'>
						<input type='hidden' name='mode' value='updateEncoding'>
						<input type='hidden' name='type' value='Devices'>
						<input type='hidden' name='id' value='{$card->info['id']}'>
						<a  href='javascript:void(0);' class='send-req-form' data-func-after='devicesReloadPage' title='".CARD_CHANGE_ENCODING."' id='{$card->info['id']}'>{$card->info['encoding']}</a>
					</form>"); ?>
                                    </div>
                                    <?php if ($card->info['driver'] != 'tw5864') { ?>
                                    <div class="col-lg-6 col-md-6">
                                        Unused capacity: <a href="javascript:void(0);" class="devices-cards-capacity"  style="cursor: help;"><?php echo $card->info['available_capacity']; ?></a>
                                    </div>
                                    <?php } ?>
                                </div>
                            </div>
                        </div>
                        <div class="row"><div class="col-lg-12">&nbsp;</div></div>
                        <div class="row">
                        <?php
                            foreach ($card->cameras as $key =>$device){
                            	if (empty($device->info['id'])) { $device->info['id']=''; };
                                if (empty($device->info['device_name'])) { $device->info['device_name'] = ($device->info['status'] == 'notconfigured') ? DEVICE_VIDEO_NAME_notconfigured : DEVICE_UNNAMED; };
                                if (isset($device->info['driver']) && ($device->info['driver'] != 'tw5864')) $display_res_fps = true;
                                else $display_res_fps = false;
                        ?>

                            <div class="col-lg-4 col-md-4">
                                <div class="well well-sm <?php echo (($display_res_fps) ? 'display_res_fps' : 'without_res_fps'); ?>">
                                    <div class="row">
                                    <div class="col-lg-12 col-md-12">
                                    <h4 class="devices-device-name">
                                        <span class="devices-device-name-title <?php echo (($device->info['status'] != 'notconfigured') ? 'click-event' : ''); ?>" data-class="deviceChangeName.form(1)">
                                            <i class="fa fa-video-camera fa-fw text-status-<?php echo $device->info['status']; ?>"></i>
                                            <span><?php echo $device->info['device_name']; ?></span>
                                        </span>

                                        <form action="/devices/port-name" method="POST" class="devices-device-name-form">
                                            <input type="hidden" name="id" value="<?php echo $device->info['id']; ?>">

                                            <div class="row">
                                                <div class=" col-xs-6 col-sm-8col-md-6 col-lg-7 devices-device-name-form-inp">
                                                    <input type="text" class="form-control input-sm"  name="device_name" value="<?php echo $device->info['device_name']; ?>">
                                                </div>

                                                <div class="col-xs-6 col-sm-4 col-md-6 col-lg-5 btn-group" role="group">
                                                    <button type="submit" class="btn btn-primary btn-sm send-req-form" data-loading-text="..."><i class="fa fa-check fa-lg"></i></button>
                                                    <button type="button" class="btn btn-warning btn-sm click-event" data-class="deviceChangeName.form(0)"><i class="fa fa-close fa-lg"></i></button>
                                                </div>
                                            </div>
                                        </form>
                                    </h4>

                                    <div>
                                        <div class="pull-left divices-cards-port">
                                            <h4><?php echo $device->info['port']; ?></h4>
                                        </div>
                                        <?php if ($device->info['status'] != 'notconfigured') { ?> 
                                            <div  class="pull-left divices-cards-id-name">id: <?php echo $device->info['id']; ?></div>
                                        <?php } ?>
                                        <div class="clearfix"></div>
                                    </div>


                                    <p>
                                        <form action="/ajax/update.php" method="POST" id="devices-onoff-form-<?php echo $counter; ?>-<?php echo $device->info['port']; ?>">
                                            <input type="hidden" name="mode" value="changeState">
                                            <input type="hidden" name="type" value="Devices">
                                            <input type="hidden" name="do" value="true">
                                            <input type="hidden" name="id" value="<?php echo  $device->info['device']; ?>">
                                        </form>


                                        <div class="btn-group" role="group">
                                            <button type="button" class="btn btn-default devices-onoff-form send-req-form" data-form-id="devices-onoff-form-<?php echo $counter; ?>-<?php echo $device->info['port']; ?>" data-func-after="devicesReloadPage"><?php echo constant('DEVICE_VIDEO_STATUS_CHANGE_'.$device->info['status']); ?></button>
                                            <div class="btn-group" role="group">

                                                <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                                                    <?php echo SETTINGS; ?> <span class="caret"></span>
                                                </button>
                                                <ul class="dropdown-menu">
                                                    <li><a href="/videoadj?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_VIDADJ;?></a></li>
                                                    <li><a href="/motionmap?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_MMAP;?></a></li>
                                                    <li><a href="/deviceschedule?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_SCHED;?></a></li>
                                                    <li><a href="/ptzsettings?id=<?php echo $device->info['driver']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_PTZ;?></a></li>
						    <li><a href="/ajax/reencode.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_REENCODE; ?></a></li>
                                                </ul>
                                            </div>
                                        </div>
                                    </p>
                                    </div>
                                    </div>
                                    <?php if ((($device->info['status'] != 'notconfigured') && !$device->info['disabled']) && $display_res_fps) { ?>
                                    <div>
                                        <div class="row">
                                            <div class="col-lg-6 col-md-6">
                                            <form action="/ajax/update.php" method="POST">
                                                <input type="hidden" name="mode" value="RES">
                                                <input type="hidden" name="id" value="<?php echo  $device->info['device']; ?>">
                                                <div class="form-group">
                                                    <label><small>Resolution:</small></label>
                                                    <?php echo arrayToSelect($GLOBALS['resolutions'][$card->info['encoding']], $device->info['resolutionX'].'x'.$device->info['resolutionY'],  'value', 'RES input-sm send-req-form-select', false, 'data-class-after="devicesCards.setCapacity()"'); ?>
                                                </div>
                                            </form>
                                            </div>
                                        
                                            <div class="col-lg-6 col-md-6">
                                            <form action="/ajax/update.php" method="POST">
                                                <input type="hidden" name="mode" value="FPS">
                                                <input type="hidden" name="id" value="<?php echo  $device->info['device']; ?>">
                                                <div class="form-group">
                                                    <label><small>FPS:</small></label>
                                                    <?php echo arrayToSelect($GLOBALS['local_device_fps'], 30/$device->info['video_interval'], 'value','FPS input-sm send-req-form-select', false, 'data-class-after="devicesCards.setCapacity()"'); ?>
                                                </div>
                                            </form>
                                            </div>
                                        </div>
                                    </div>
                                <?php } ?>

                                </div>
                            </div>
                        <?php } ?>
                        </div>

                    </div>
                </div>
            </div>
            <?php } ?>
            <?php } ?>


        </div>    


    </div>
</div>

