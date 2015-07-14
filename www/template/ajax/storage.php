<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');
?>
<h1 class="header"><?php echo STRAGE_HEADER; ?></h1><hr />
<div id='button' class='add'><?php echo ADD_LOCATION; ?></div>
<div id='saveButton'><?php echo SAVE; ?></div>
<div class='bClear'></div>
<div style="display:none;" id='tableEntry'>
<?php		print "<label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' class='path' value='' style='width:270px;margin-left:5px;' /> Max:<INPUT style='width:40px;margin-left:5px;' type='text' name='max[]' id='max[]' value='94' />% <div id='button' class='deleteShort'>".L_DELETE."</div>"; ?>
</div>
<form id='storageForm' method="post" action="/ajax/storage.php">
<?php
foreach ($locations as $id => $location){
	$recordTime = intval($location['record_time']);
	$days = floor($recordTime / (3600 * 24));
	$hours = floor(($recordTime % (3600* 24)) / 3600);
	$minutues = floor(($recordTime % 3600) / 60);
	print "<div id='separator'><div>".LOCATION."<INPUT name='path[]' type='text' id='path' class='path' value='{$location['path']}' style='width:270px; margin-left:5px;' /> Max:<INPUT style='width:40px; margin-left:5px;' type='text' name='max[]' id='max[]' value='".intval($location['max_thresh'])."' />%</div><div>Storage Estimation: ".$days." days ".$hours." hours and ".$minutues." minutues</div><div id='button' class='deleteShort' style='margin-top:-32px;'>".L_DELETE."</div></div>";
}

?>

</form>
