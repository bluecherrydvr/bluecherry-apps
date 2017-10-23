
                        <div class="col-lg-4 col-md-4">
                            <div class="well well-sm well-cameras">
                                <h4 class="devices-device-name">
                                    <span class="devices-device-name-title click-event" data-class="deviceChangeName.form(1)">
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
                                <p>
                                    <?php echo ((isset($device->info['manufacturer'])) ? $device->info['manufacturer'] : '' ); ?> 
                                    <?php echo ((isset($device->info['manufacturer']) && ($device->info['manufacturer'] == $device->info['model'])) ? '' : $device->info['model']); ?>
                                    | id: <?php echo $device->info['id']; ?>
                                </p>
                                <p>
                                    <form action="/ajax/update.php" method="POST" id="devices-onoff-form-<?php echo $device->info['id']; ?>">
                                        <input type="hidden" name="mode" value="changeState">
                                        <input type="hidden" name="type" value="Devices">
                                        <input type="hidden" name="do" value="true">
                                        <input type="hidden" name="id" value="<?php echo  $device->info['id']; ?>">
                                    </form>

                                        <div class="btn-group" role="group">
                                            <button type="button" class="btn btn-default devices-onoff-form send-req-form" data-form-id="devices-onoff-form-<?php echo $device->info['id']; ?>" data-func-after="deviceSwith"><?php echo constant('DEVICE_VIDEO_STATUS_CHANGE_'.$device->info['status']); ?></button>
                                            <div class="btn-group" role="group">

                                                <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                                                    <?php echo SETTINGS; ?> <span class="caret"></span>
                                                </button>
                                                <ul class="dropdown-menu">
                                                    <li><a href="/ajax/editip.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_DETAILS;?></a></li>
                                                    <?php if (!$_SESSION['from_client']) { ?> 
                                                    <li><a href="/ajax/redirect.php?id=<?php echo $device->info['id']; ?>" target="_blank"><?php echo DEVICE_EDIT_ONCAM; ?></a></li> 
                                                    <?php } ?>
                                            		<li><a href="/ajax/motionmap.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_MMAP;?></a></li>
                                            		<li><a href="/ajax/deviceschedule.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_SCHED; ?></a></li>
                                            		<li><a href="/ajax/ptzpresetlist.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_PTZ; ?></a></li>
							<li><a href="/ajax/reencode.php?id=<?php echo $device->info['id']; ?>" class="ajax-content"><?php echo DEVICE_EDIT_REENCODE; ?></a></li>
                                                    <li>
                                                        <form action="/ajax/update.php" method="POST">
                                                            <input type="hidden" name="mode" value="deleteIp">
                                                            <input type="hidden" name="type" value="Devices">
                                                            <input type="hidden" name="do" value="true">
                                                            <input type="hidden" name="id" value="<?php echo  $device->info['id']; ?>">
                                                        </form>

                                                        <a href="javascript:void(0);" class="send-req-form" data-form-id="prev" data-func="delCamera" data-confirm="Are you sure you want to delete this camera (ID:<?php echo  $device->info['id']; ?>)?"><?php echo DELETE_CAM;?></a>
                                                    </li>
                                                </ul>
                                            </div>
                                    </div>




                                </p>
                            </div>
                        </div>                        			
			
