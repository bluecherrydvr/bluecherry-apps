<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');
?>
<h1 class="header"><?php echo STRAGE_HEADER; ?></h1><hr>
<div id='button' class='add'><?php echo ADD_LOCATION; ?></div>
<div id='saveButton'><?php echo SAVE; ?></div>
<div class='bClear'></div>
<div style="display:none;" id='tableEntry'>
<?php		print "<label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' value='' style='width:270px;'> Max: <INPUT style='width:40px;' type='text' name='max[]' id='max[]' value='95'>% Min:<INPUT style='width:40px;' type='text' name='min[]' id='min[]' value='90'>%<div id='button' class='deleteShort'></div>"; ?>
</div>
<form id='storageForm' method="post" action="/ajax/storage.php">
<?php
foreach ($locations as $id => $location){
	print "<div id='separator'><label>".LOCATION."</label><INPUT name='path[]' type='text' id='path' value='{$location['path']}' style='width:270px;'> Max: <INPUT style='width:40px;' type='text' name='max[]' id='max[]' value='{$location['max_thresh']}'>% Min:<INPUT style='width:40px;' type='text' name='min[]' id='min[]' value='{$location['min_thresh']}'>%<div id='button' class='deleteShort'></div></div>";
}

?>

</form>