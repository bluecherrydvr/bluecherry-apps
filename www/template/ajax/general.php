<?php defined('INDVR') or exit(); include("../template/template.lib.php"); ?>
<div id="generalSettings">
<h1 id="header"><?php echo GLOBAL_SETTINGS; ?></h1>
<form id="settingsForm" action="ajax/update.php" method="post">
<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>

<div class='container-separator'>
	<div class='title'><?php echo G_PERF_TITLE; ?></div>
	<div><label><?php echo G_DVR_NAME; ?><span class='sub'><?php echo G_DVR_NAME_EX; ?></span></label><input type='text' name='G_DVR_NAME' value='<?php echo $global_settings->data['G_DVR_NAME']; ?>' /></div>
	<div><label><?php echo G_DEV_NOTES; ?><span class='sub'><?php echo G_DEV_NOTES_EX; ?></span></label><textarea class='notes' name='G_DEV_NOTES'><?php echo $global_settings->data['G_DEV_NOTES']; ?></textarea></div>
	<div><label><?php echo G_MAX_RECORD_TIME; ?><span class='sub'><?php echo G_MAX_RECORD_TIME_EX; ?></span></label>
		<?php echo arrayToSelect($GLOBALS['cts_record_durations'], $global_settings->data['G_MAX_RECORD_TIME'], 'G_MAX_RECORD_TIME'); ?>
	</div>
	<div><?php echo G_MAX_RECORD_AGE; ?><span class='sub'><?php echo G_MAX_RECORD_AGE_EX; ?></span></label>
		<input type='text' name='G_MAX_RECORD_AGE' value='<?php if (isset($global_settings->data['G_MAX_RECORD_AGE'])) { echo $global_settings->data['G_MAX_RECORD_AGE'] / (60 * 60 * 24); } ?>' /> days
	</div>
</div>

<div class='container-separator'>
	<div class='title'><?php echo G_PERF_TITLE; ?></div>
	<div><label><?php echo G_DISABLE_VERSION_CHECK; ?><span class='sub'><?php echo G_DISABLE_VERSION_CHECK_EX; ?></span></label><input type='checkbox' name='G_DISABLE_VERSION_CHECK' <?php echo (($global_settings->data['G_DISABLE_VERSION_CHECK']==0) ? 'checked' : ''); ?>></div>
	<div>
            <label style="display: block; heigth: 50px">
                <?php echo G_DATA_SOURCE ?>
            </label>
            <div style="float: left; height: 50px; width: 450px">
                <input type="radio" name="G_DATA_SOURCE" value="local" style="width: 30px;"
                       <?php echo $global_settings->data['G_DATA_SOURCE'] != 'live' ? ' checked="checked"' : '' ?> /> 
                    <?php echo G_DATA_SOURCE_LOCAL ?>
                <br clear="all" />
                <input type="radio" name="G_DATA_SOURCE" value="live"  style="width: 30px;"
                       <?php echo $global_settings->data['G_DATA_SOURCE'] == 'live' ? ' checked="checked"' : '' ?> /> 
                    <?php echo G_DATA_SOURCE_LIVE ?>
            </div>
        </div>
        
	<div><label><?php echo G_DISABLE_IP_C_CHECK; ?><span class='sub'><?php echo G_DISABLE_IP_C_CHECK_EX; ?></span></label><input type='checkbox' name='G_DISABLE_IP_C_CHECK' <?php echo (($global_settings->data['G_DISABLE_IP_C_CHECK']==1) ? 'checked' : ''); ?>></div>
	<div><label><?php echo G_DISABLE_WEB_STATS; ?><span class='sub'><?php echo G_DISABLE_WEB_STATS_EX; ?></span></label><input type='checkbox' name='G_DISABLE_WEB_STATS' <?php echo (($global_settings->data['G_DISABLE_WEB_STATS']==1) ? 'checked' : ''); ?>></div>
</div>

<div class='container-separator'>
	<?php
		if (!empty($global_settings->data['G_SMTP_FAIL'])){
			echo '<div class="F" id="message">'.G_SMTP_FAILED.': '.$global_settings->data['G_SMTP_FAIL'].'</div>';
		}
	?>
	<div class='title'><?php echo G_SMTP_TITLE; ?></div>
	<div><label><?php echo G_SMTP_SERVICE; ?><span class='sub'><?php echo G_SMTP_SERVICE_EX; ?></span></label><select name='G_SMTP_SERVICE'><option value='default' <?php echo ($global_settings->data['G_SMTP_SERVICE']=='default') ? 'selected': '';?>><?php echo G_SMTP_DEF_MTA; ?></option><option value='smtp' <?php echo ($global_settings->data['G_SMTP_SERVICE']=='smtp') ? 'selected': '';?>><?php echo G_SMTP_SMTP; ?></option></select></div>
	<div><label><?php echo G_SMTP_USERNAME; ?><span class='sub'><?php echo G_SMTP_USERNAME_EX; ?></span></label><input type='text' name='G_SMTP_USERNAME' value='<?php echo $global_settings->data['G_SMTP_USERNAME']; ?>' /></div>
	<div><label><?php echo G_SMTP_PASSWORD; ?><span class='sub'><?php echo G_SMTP_PASSWORD_EX; ?></span></label><input type='password' name='G_SMTP_PASSWORD' value='<?php echo $global_settings->data['G_SMTP_PASSWORD']; ?>' /></div>
	<div><label><?php echo G_SMTP_EMAIL_FROM; ?><span class='sub'><?php echo G_SMTP_EMAIL_FROM_EX; ?></span></label><input type='text' name='G_SMTP_EMAIL_FROM' value='<?php echo $global_settings->data['G_SMTP_EMAIL_FROM']; ?>' /></div>
	<div><label><?php echo G_SMTP_HOST; ?><span class='sub'><?php echo G_SMTP_HOST_EX; ?></span></label><input type='text' name='G_SMTP_HOST' value='<?php echo  $global_settings->data['G_SMTP_HOST']; ?>' /></div>
	<div><label><?php echo G_SMTP_PORT; ?><span class='sub'><?php echo G_SMTP_PORT_EX; ?></span></label><input type='text' name='G_SMTP_PORT' value='<?php echo  $global_settings->data['G_SMTP_PORT']; ?>' /></div>
	<div><label><?php echo G_SMTP_SSL; ?><span class='sub'><?php echo G_SMTP_SSL_EX; ?></span></label><select name='G_SMTP_SSL'><option value='none' <?php echo ($global_settings->data['G_SMTP_SSL']=='none') ? 'selected': '';?>><?php echo L_NONE; ?></option><option value='tls' <?php echo ($global_settings->data['G_SMTP_SSL']=='tls') ? 'selected': '';?>>TLS/SSL</option></select></div>
</div>
<input type="Hidden" name="mode" value="global" />
</form>
</div>
