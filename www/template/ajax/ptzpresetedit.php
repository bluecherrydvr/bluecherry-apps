

<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header">
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li><a href="/ajax/ptzpresetlist.php?id=<?php echo $camera->info['id']; ?>" class="ajax-content"><?php echo IPP_HEADER; ?> <b><?php echo $camera->info['device_name']; ?></b></a></li>
            <li class="active"> <?php echo IPP_EDIT; ?> <b><?php echo $preset->preset['name']; ?></b></li>
        </ol>
        </h1>
    </div>
</div>


<div class="row">
    <div class="col-lg-12">
        <form action="/ajax/ptzpresetedit.php?id=<?php echo ((!empty($preset->preset['id'])) ? $preset->preset['id'] : 'new'); ?>&ref=<?php echo $camera->info['id']; ?>" method="POST" class="form-horizontal">
        	<input type="hidden" name="ref" value="<?php echo $camera->info['id']; ?>" />
        	<input type="hidden" name="presetId" value="<?php echo ((!empty($preset->preset['id'])) ? $preset->preset['id'] : 'new'); ?>" />

            <div class="panel panel-default">
                <div class="panel-body">
            
                    <div class="form-group">
                        <div class="col-lg-12">
                            <div class="alert alert-warning">
                                <span class="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>
                                <?php echo IPP_EDIT_HELP; ?>
                            </div>
                        </div>
                    </div>

                    <div class="form-group">
                        <div class="col-lg-12">
                            <button class="btn btn-success pull-right send-req-form" type="submit" data-redirect-success="/ajax/ptzpresetlist.php?id=<?php echo $camera->info['id']; ?>"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                        </div>
                        <div class="clearfix"></div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_NAME; ?><p class='help-block'><small><?php echo IPP_PRESET_NAME_EX; ?></small></p></label>
                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="name" value="<?php echo $preset->preset['name']; ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_RIGHT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="mright" value="<?php echo ((empty($preset->preset['mright'])) ? "" : $preset->preset['mright']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_LEFT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="mleft" value="<?php echo ((empty($preset->preset['mleft'])) ? "" : $preset->preset['mleft']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_UP; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="up" value="<?php echo ((empty($preset->preset['up'])) ? "" : $preset->preset['up']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_DOWN; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="down" value="<?php echo ((empty($preset->preset['down'])) ? "" : $preset->preset['down']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_UPRIGHT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="up_right" value="<?php echo ((empty($preset->preset['up_right'])) ? "" : $preset->preset['up_right']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_UPLEFT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="up_left" value="<?php echo ((empty($preset->preset['up_left'])) ? "" : $preset->preset['up_left']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_DOWNRIGHT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="down_right" value="<?php echo ((empty($preset->preset['down_right'])) ? "" : $preset->preset['down_right']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_DOWNLEFT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="down_left" value="<?php echo ((empty($preset->preset['down_left'])) ? "" : $preset->preset['down_left']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_WIDE; ?><p class='help-block'><small><?php echo IPP_PRESET_WIDE_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="wide" value="<?php echo ((empty($preset->preset['wide'])) ? "" : $preset->preset['wide']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_TIGHT; ?><p class='help-block'><small><?php echo IPP_PRESET_TIGHT_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="tight" value="<?php echo ((empty($preset->preset['tight'])) ? "" : $preset->preset['tight']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_FOCUSIN; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="focus_in" value="<?php echo ((empty($preset->preset['focus_in'])) ? "" : $preset->preset['focus_in']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_FOCUSOUT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="focus_out" value="<?php echo ((empty($preset->preset['focus_out'])) ? "" : $preset->preset['focus_out']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_PRESET_SAVE; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="preset_save" value="<?php echo ((empty($preset->preset['preset_save'])) ? "" : $preset->preset['preset_save']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_PRESET_GO; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="preset_go" value="<?php echo ((empty($preset->preset['preset_go'])) ? "" : $preset->preset['preset_go']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_STOP; ?><p class='help-block'><small><?php echo IPP_PRESET_STOP_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="stop" value="<?php echo ((empty($preset->preset['stop'])) ? "" : $preset->preset['stop']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_NEEDS_STOP; ?><p class='help-block'><small><?php echo IPP_PRESET_NEEDS_STOP_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="needs_stop" <?php echo ((empty($preset->preset['needs_stop'])) ? '' : 'checked'); ?> />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_HTTP_AUTH; ?><p class='help-block'><small><?php echo IPP_PRESET_HTTP_AUTH_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="http_auth" <?php echo ((empty($preset->preset['http_auth'])) ? '' : 'checked'); ?> />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_PROTOCOL; ?></label>

                        <div class="col-lg-6">
                            <select class="form-control" name="protocol">
                                <option value="http" <?php echo ($preset->preset['protocol'] == "http" ? "selected" : ""); ?>>http</option>
                                <option value="https" <?php echo ($preset->preset['protocol'] == "https" ? "selected" : ""); ?>>https</option>
                            </select>
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPP_PRESET_PORT; ?><p class='help-block'><small><?php echo IPP_PRESET_PORT_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="port" value="<?php echo ((empty($preset->preset['port'])) ? "" : $preset->preset['port']); ?>"  />
                        </div>
                    </div>


                </div>
            </div>

        </form>
    </div>
</div>

