<?php defined('INDVR') or exit(); ?>
<div id="generalSettings">
<h1 id="header"><?php echo GLOBAL_SETTINGS; ?></h1>
<form id="settingsForm" action="ajax/update.php" method="post">
<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>

<div class='container-separator'>
	<div class='title'><?php echo G_PERF_TITLE; ?></div>
	<div><label><?php echo G_DVR_NAME; ?><span class='sub'><?php echo G_DVR_NAME_EX; ?></span></label><input type='text' name='G_DVR_NAME' value='<?php echo $global_settings->data['G_DVR_NAME']; ?>' /></div>
	<div><label><?php echo G_DEV_NOTES; ?><span class='sub'><?php echo G_DEV_NOTES_EX; ?></span></label><textarea class='notes' name='G_DEV_NOTES'><?php echo $global_settings->data['G_DEV_NOTES']; ?></textarea></div>
</div>

<div class='container-separator'>
	<div class='title'><?php echo G_PERF_TITLE; ?></div>
	<div><label><?php echo G_DISABLE_VERSION_CHECK; ?><span class='sub'><?php echo G_DISABLE_VERSION_CHECK_EX; ?></span></label><input type='checkbox' name='G_DISABLE_VERSION_CHECK' <?php echo (($global_settings->data['G_DISABLE_VERSION_CHECK']==0) ? 'checked' : ''); ?>></div>
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
	<div><label><?php echo G_SMTP_HOST; ?><span class='sub'><?php echo G_SMTP_HOST_EX; ?></span></label><input type='text' name='G_SMTP_HOST' value='<?php echo  $global_settings->data['G_SMTP_HOST']; ?>' /></div>
	<div><label><?php echo G_SMTP_PORT; ?><span class='sub'><?php echo G_SMTP_PORT_EX; ?></span></label><input type='text' name='G_SMTP_PORT' value='<?php echo  $global_settings->data['G_SMTP_PORT']; ?>' /></div>
	<div><label><?php echo G_SMTP_SSL; ?><span class='sub'><?php echo G_SMTP_SSL_EX; ?></span></label><select name='G_SMTP_SSL'><option value='none' <?php echo ($global_settings->data['G_SMTP_SSL']=='none') ? 'selected': '';?>><?php echo L_NONE; ?></option><option value='tls' <?php echo ($global_settings->data['G_SMTP_SSL']=='tls') ? 'selected': '';?>>TLS/SSL</option></select></div>
</div>
<input type="Hidden" name="mode" value="global" />
</form>
</div>
