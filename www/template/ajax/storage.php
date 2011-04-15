<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');
?>
<h1 class="header"><?php echo STRAGE_HEADER; ?></h1><hr>
<div id='button' class='add'><?php echo ADD_LOCATION; ?></div>
<div id='saveButton'><?php echo SAVE; ?></div>
<div class='bClear'></div>
<div style="display:none;" id='tableEntry'>
<?php		print "<label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' value=''> Max:".arrayToSelect($GLOBALS['storage_limits'], 95, 'max[]')." Min:".arrayToSelect($GLOBALS['storage_limits'], 90, 'min[]')."<div id='button' class='deleteShort'></div>"; ?>
</div>
<form id='storageForm' method="post" action="/ajax/storage.php">
<?php
foreach ($locations as $id => $location){
	print "<div id='separator'><label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' value='{$location['path']}'> Max:".arrayToSelect($GLOBALS['storage_limits'], $location['max_thresh'], 'max[]')." Min:".arrayToSelect($GLOBALS['storage_limits'], $location['min_thresh'], 'min[]')."<div id='button' class='deleteShort'></div></div>";
}

?>

</form>