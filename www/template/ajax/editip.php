
<div class="row">
    <div class="col-lg-12 col-md-12">
        <h1 class="page-header"><?php echo PROPS_HEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"><?php echo $ipCamera->info['device_name']; ?></li>
        </ol>
        </h1>
    </div>
</div>


<div class="row">
    <div class="col-lg-12 col-md-12">
        <form action="/ajax/editip.php" method="POST" class="form-horizontal">
	        <input type="hidden" name="mode" value="editIp" />
        	<input type="hidden" name="id" value="<?php echo $ipCamera->info['id']; ?>" />

            <div class="panel panel-default">
                <div class="panel-body">
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo VA_AUDIO_ENABLE; ?></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input class="" type="checkbox" name="audio_enabled" <?php echo (!($ipCamera->info['audio_disabled']) ? ' checked="checked"' : ''); ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_CAMERA_NAME; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="camName" value="<?php echo $ipCamera->info['device_name']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_IP_ADDR; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="ipAddr" value="<?php echo $ipCamera->info['ipAddr']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_USER; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="user" value="<?php echo $ipCamera->info['rtsp_username']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_PASS; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="password" name="pass" value="<?php echo $ipCamera->info['rtsp_password']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_PROTOCOL; ?></label>

                        <div class="col-lg-6 col-md-6">
			                <select name="protocol" class="form-control change-event" data-function="protocolBlocks">
                				<option value="IP-RTSP">RTSP</option>
                				<option value="IP-MJPEG" <?php echo ($ipCamera->info['protocol']) == 'IP-MJPEG' ? "selected" : "";  ?>>MJPEG</option>
                			</select>
                        </div>
                    </div>
                    
                    <div class="devices-rtsp-setting" <?php echo ($ipCamera->info['protocol']) == 'IP-MJPEG' ? 'style="display:none;"' : ""; ?>>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_RTSP; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="rtsp" value="<?php echo $ipCamera->info['rtsp']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_PORT; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="port" value="<?php echo $ipCamera->info['port']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo IPCAM_TCPUDP_LEVEL; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <select name="prefertcp" class="form-control">
                                <option value="1">TCP</option>
                                <option value="0" <?php echo ($ipCamera->info['rtsp_rtp_prefer_tcp']==0) ? "selected" : ""; ?>>UDP</option>
                            </select>
                        </div>
                    </div>

                    </div>

                    <div class="devices-mjpeg-setting" <?php echo ($ipCamera->info['protocol']) == 'IP-MJPEG' ? '' : 'style="display:none;"'; ?>>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_MJPATH; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="mjpeg" value="<?php echo $ipCamera->info['mjpeg_path']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group" style="display:none;">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_IP_ADDR_MJPEG; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="ipAddrMjpeg" value="<?php echo $ipCamera->info['ipAddrMjpeg']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_PORT_MJPEG; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="portMjpeg" value="<?php echo $ipCamera->info['portMjpeg']; ?>"  />
                        </div>
                    </div>
                    </div>


                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_SUBSTREAM_ENABLE; ?></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input class="" type="checkbox" name="substream_enabled" <?php echo (($ipCamera->info['substream_enabled'] == 1) ? ' checked="checked"' : ''); ?>  />
                        </div>
		    </div>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_SUBSTREAM_PATH; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type="text" name="substream" value="<?php echo $ipCamera->info['substream']; ?>"  />
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo AIP_ONVIF_PORT; ?></label>

                        <div class="col-lg-6 col-md-6">
                            <div class="input-group">
                                <input class="form-control" type="text" name="onvif_port" value="<?php echo $ipCamera->info['onvif_port']; ?>" />
                                <span class="input-group-btn">
                                    <button type="button" class="btn btn-default click-event" data-function="addIpCheckOnvifPort"><?php echo AIP_CHECK_ONVIF_PORT; ?></button>
                                </span>
                            </div>
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label">
                            <a href="javascript:void(0);" class="btn btn-default" id="edittip-advanced-settings"><?php echo AIP_ADVANCED_SETTINGS; ?></a>
                        </label>
                        <div class="col-lg-6 col-md-6">
                        </div>
                    </div>
                    
                    <div class="form-group" id="edittip-advanced-settings-block" style="display: none;">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo IPCAM_DEBUG_LEVEL; ?></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input type="checkbox" name="debug_level" <?php echo ($ipCamera->info['debug_level']==1) ? 'checked' : ''; ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <div class="col-lg-6 col-lg-offset-4 col-md-6 col-md-offset-4">
                            <button class="btn btn-success send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                        </div>
                    </div>

                </div>
            </div>

        
        </form>
    </div>
</div>

