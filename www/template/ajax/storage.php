<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');
?>
<h1 class="header"><?php echo STRAGE_HEADER; ?></h1><hr />
<div id='button' class='add'><?php echo ADD_LOCATION; ?></div>
<div id='saveButton'><?php echo SAVE; ?></div>
<div class='bClear'></div>
<div style="display:none;" id='tableEntry'>
<?php		print "<label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' class='path' value='' style='width:270px;' /> Max:<INPUT style='width:40px;' type='text' name='max[]' id='max[]' value='95' />% <div id='button' class='deleteShort'></div>"; ?>
</div>
<form id='storageForm' method="post" action="/ajax/storage.php">
<?php
foreach ($locations as $id => $location){
	$recordTime = intval($location['record_time']);
	$hours = floor($recordTime / 3600);
	$minutues = floor(($recordTime % 3600) / 60);
	print "<div id='separator'><label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' class='path' value='{$location['path']}' style='width:200px;' />&nbsp;&nbsp;Max:<INPUT style='width:40px;' type='text' name='max[]' id='max[]' value='".intval($location['max_thresh'])."' />%&nbsp;&nbsp;&nbsp;Time:".$hours." h ".$minutues." m"."<div id='button' class='deleteShort'>".L_DELETE."</div></div>";
}

?>

</form>
