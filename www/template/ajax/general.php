
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo GLOBAL_SETTINGS; ?></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">




<form action="/ajax/update.php" method="post" class="form-horizontal">

<div class="form-group">
    <div class="col-lg-12 col-md-12">
        <button class="btn btn-success pull-right send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
        <div class="clearfix"></div>
    </div>
</div>

<div class="panel panel-default">
    <div class="panel-heading"><?php echo G_PERF_TITLE; ?></div>


    <div class="panel-body">

        <div class="form-group">
            <label class="col-lg-4 col-md-4 control-label"><?php echo G_DVR_NAME; ?><p class='help-block'><small><?php echo G_DVR_NAME_EX; ?></small></p></label>

            <div class="col-lg-6 col-md-6">
                <input class="form-control input-group-unstyled" type='text' name='G_DVR_NAME' value='<?php echo $global_settings->data['G_DVR_NAME']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 col-md-4 control-label"><?php echo G_DEV_NOTES; ?><p class='help-block'><small><?php echo G_DEV_NOTES_EX; ?></small></p></label>

            <div class="col-lg-6 col-md-6">
                <textarea class="form-control" name='G_DEV_NOTES'><?php echo $global_settings->data['G_DEV_NOTES']; ?></textarea>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 col-md-4 control-label"><?php echo G_MAX_RECORD_TIME; ?><p class='help-block'><small><?php echo G_MAX_RECORD_TIME_EX; ?></small></p></label>

            <div class="col-lg-6 col-md-6">
    		    <?php echo arrayToSelect($GLOBALS['cts_record_durations'], $global_settings->data['G_MAX_RECORD_TIME'], 'G_MAX_RECORD_TIME'); ?>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 col-md-4 control-label"><?php echo G_MAX_RECORD_AGE; ?><p class='help-block'><small><?php echo G_MAX_RECORD_AGE_EX; ?></small></p></label>
        
            <div class="col-lg-6 col-md-6">
                <div class="input-group">

                    <input class="form-control" type='text' name='G_MAX_RECORD_AGE' value='<?php if (isset($global_settings->data['G_MAX_RECORD_AGE'])) { echo $global_settings->data['G_MAX_RECORD_AGE'] / (60 * 60 * 24); } ?>' />
                    <span class="input-group-addon">days</span>
                </div>

            </div>
        </div>


    </div>
</div>

<div class="panel panel-default">
    <div class="panel-heading"><?php echo G_PERF_TITLE; ?></div>

    <div class="panel-body">

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_VAAPI_DEVICE; ?><p class='help-block'><small><?php echo G_VAAPI_DEVICE_EX; ?></small></p></label>

            <div class="col-lg-6 col-md-6">
                    <?php echo arrayToSelect(getVaapiOptions(), $global_settings->data['G_VAAPI_DEVICE'], 'G_VAAPI_DEVICE'); ?>
            </div>
        </div>

    </div>
</div>

<div class="panel panel-default">
    <div class="panel-heading"><?php echo G_PERF_TITLE; ?></div>

        <div class="panel-body">

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_DISABLE_VERSION_CHECK; ?><p class='help-block'><small><?php echo G_DISABLE_VERSION_CHECK_EX; ?></small></p></label>

            <div class="col-lg-6 form-control-static">
                <input class="" type='checkbox' name='G_DISABLE_VERSION_CHECK' <?php echo (($global_settings->data['G_DISABLE_VERSION_CHECK']==0) ? 'checked' : ''); ?>>
            </div>
        </div>


    	<div class="form-group">
            <label class="col-lg-4 control-label">
                <?php echo G_DATA_SOURCE ?>
            </label>

            <div class="col-lg-6">
                <div class="btn-group" data-toggle="buttons">
                    <label class="btn btn-default <?php echo $global_settings->data['G_DATA_SOURCE'] != 'live' ? 'active' : '' ?> ">
                        <input type="radio" name="G_DATA_SOURCE" value="local" style="width: 30px;"
                           <?php echo $global_settings->data['G_DATA_SOURCE'] != 'live' ? ' checked="checked"' : '' ?> /> 
                        <?php echo G_DATA_SOURCE_LOCAL ?>
                    </label>

                    <label class="btn btn-default <?php echo $global_settings->data['G_DATA_SOURCE'] == 'live' ? 'active' : '' ?> ">
                        <input type="radio" name="G_DATA_SOURCE" value="live"  style="width: 30px;"
                           <?php echo $global_settings->data['G_DATA_SOURCE'] == 'live' ? ' checked="checked"' : '' ?> /> 
                        <?php echo G_DATA_SOURCE_LIVE ?>
                    </label>
                </div>
            </div>
        </div>
        
        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_DISABLE_IP_C_CHECK; ?><p class='help-block'><small><?php echo G_DISABLE_IP_C_CHECK_EX; ?></small></p></label>

            <div class="col-lg-6 form-control-static">
                <input class="" type='checkbox' name='G_DISABLE_IP_C_CHECK' <?php echo (($global_settings->data['G_DISABLE_IP_C_CHECK']==1) ? 'checked' : ''); ?>>
            </div>
        </div>


        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_DISABLE_WEB_STATS; ?><p class='help-block'><small><?php echo G_DISABLE_WEB_STATS_EX; ?></small></p></label>
            
            <div class="col-lg-6 form-control-static">
                <input type='checkbox' name='G_DISABLE_WEB_STATS' <?php echo (($global_settings->data['G_DISABLE_WEB_STATS']==1) ? 'checked' : ''); ?>>
            </div>
        </div>


    </div>
</div>

<div class="panel panel-default">
    <div class="panel-heading"><?php echo G_SMTP_TITLE; ?></div>

        <div class="panel-body">

        	<?php
        		if (!empty($global_settings->data['G_SMTP_FAIL'])){
        			echo '<div class="alert alert-warning">'.G_SMTP_FAILED.': '.$global_settings->data['G_SMTP_FAIL'].'</div>';
        		}
            ?>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_SERVICE; ?><p class='help-block'><small><?php echo G_SMTP_SERVICE_EX; ?></small></p></label>

            <div class="col-lg-6">
                <select class="form-control" name='G_SMTP_SERVICE'>
                    <option value='default' <?php echo ($global_settings->data['G_SMTP_SERVICE']=='default') ? 'selected': '';?>><?php echo G_SMTP_DEF_MTA; ?></option>
                    <option value='smtp' <?php echo ($global_settings->data['G_SMTP_SERVICE']=='smtp') ? 'selected': '';?>><?php echo G_SMTP_SMTP; ?></option>
                </select>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_USERNAME; ?><p class='help-block'><small><?php echo G_SMTP_USERNAME_EX; ?></small></p></label>

            <div class="col-lg-6">
                <input class="form-control" type='text' name='G_SMTP_USERNAME' value='<?php echo $global_settings->data['G_SMTP_USERNAME']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_PASSWORD; ?><p class='help-block'><small><?php echo G_SMTP_PASSWORD_EX; ?></small></p></label>

            <div class="col-lg-6">
                <input class="form-control" type='password' name='G_SMTP_PASSWORD' value='<?php echo $global_settings->data['G_SMTP_PASSWORD']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_EMAIL_FROM; ?><p class='help-block'><small><?php echo G_SMTP_EMAIL_FROM_EX; ?></small></p></label>

            <div class="col-lg-6">
                <input class="form-control" type='text' name='G_SMTP_EMAIL_FROM' value='<?php echo $global_settings->data['G_SMTP_EMAIL_FROM']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_HOST; ?><p class='help-block'><small><?php echo G_SMTP_HOST_EX; ?></small></p></label>
            
            <div class="col-lg-6">
                <input class="form-control" type='text' name='G_SMTP_HOST' value='<?php echo  $global_settings->data['G_SMTP_HOST']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_PORT; ?><p class='help-block'><small><?php echo G_SMTP_PORT_EX; ?></small></p></label>

            <div class="col-lg-6">
                <input class="form-control" type='text' name='G_SMTP_PORT' value='<?php echo  $global_settings->data['G_SMTP_PORT']; ?>' />
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SMTP_SSL; ?><p class='help-block'><small><?php echo G_SMTP_SSL_EX; ?></small></p></label>

            <div class="col-lg-6">
                <select class="form-control" name='G_SMTP_SSL'>
                    <option value='none' <?php echo ($global_settings->data['G_SMTP_SSL']=='none') ? 'selected': '';?>><?php echo L_NONE; ?></option>
                    <option value='tls' <?php echo ($global_settings->data['G_SMTP_SSL']=='tls') ? 'selected': '';?>>TLS/SSL</option>
                </select>
            </div>
        </div>


        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo G_SEND_TEST_EMAIL_TO; ?></label>

            <div class="col-lg-6">
                <div class="input-group">
                    <input type="text" class="form-control" name="test_email" value="" id="general-test-email">
                    <span class="input-group-btn">
                        <button type="button" class="btn btn-default send-req-form" data-func="generalTestEmail" data-form-id="general-test-email-form"><?php echo G_SEND_TEST_EMAIL; ?></button>
                    </span>
                </div>
            </div>
        </div>


    </div>
</div>
<input type="hidden" name="mode" value="global" />

<div class="form-group">
    <div class="col-lg-12">
        <button class="btn btn-success pull-right send-req-form" type="submit"><?php echo SAVE_CHANGES; ?></button>
        <div class="clearfix"></div>
    </div>
</div>

</form>


    </div>
</div>

<form action="/general" method="post" class="form-horizontal hidden" id="general-test-email-form">
    <input type="hidden" name="mode" value="testemail">
    <input type="hidden" name="email" value="">
</form>

