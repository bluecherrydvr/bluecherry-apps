<?php defined('INDVR') or exit(); ?>
<div id="generalSettings">
<h1 class="header"><?php echo GLOBAL_SETTINGS; ?></h1>
<form id="settingsForm" action="ajax/update.php" method="post">
<div id='saveButton'><?php echo SAVE_CHANGES; ?></div>
<?php
foreach($global_settings->data as $property => $value){
	if ($value['parameter']!='G_DEV_SCED') echo "<div><label>".constant($value['parameter'])."<span class='sub'>".constant($value['parameter'].'_EX')."</span></label>
		".(($value['parameter']=="G_DEV_NOTES") ? "<textarea class='notes' name='G_DEV_NOTES'>{$value['value']}</textarea>" : "<input type='text' name='{$value['parameter']}' value='{$value['value']}' />")."</div>";
}
?>
<input type="Hidden" name="mode" value="global" />
</form>
</div>
