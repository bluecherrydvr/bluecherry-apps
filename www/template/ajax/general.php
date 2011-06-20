<?php defined('INDVR') or exit(); ?>
<div id="generalSettings">
<h1 class="header"><?php echo GLOBAL_SETTINGS; ?></h1>
<form id="settingsForm" action="ajax/update.php" method="post">
<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>
<?php
echo "<div><label>".G_DVR_NAME."<span class='sub'>".G_DVR_NAME_EX."</span></label><input type='text' name='G_DVR_NAME' value='{$global_settings->data['G_DVR_NAME']}' /></div>";
echo "<div><label>".G_DEV_NOTES."<span class='sub'>".G_DEV_NOTES_EX."</span></label><textarea class='notes' name='G_DEV_NOTES'>{$global_settings->data['G_DEV_NOTES']}</textarea></div>";
echo "<div><label>".G_DISABLE_VERSION_CHECK."<span class='sub'>".G_DISABLE_VERSION_CHECK_EX."</span></label><input type='checkbox' name='G_DISABLE_VERSION_CHECK' ".(($global_settings->data['G_DISABLE_VERSION_CHECK']==0) ? 'checked' : '')."></div>";
?>
<input type="Hidden" name="mode" value="global" />
</form>
</div>
