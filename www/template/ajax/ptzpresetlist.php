<?php defined('INDVR') or exit(); 
require('../template/template.lib.php');

echo "<p> <a href='#' id='backToList'>".ALL_DEVICES."</a> &gt; ".IPP_HEADER." {$camera->info['device_name']}</p>";
echo "<div id='button' class='add'>Create preset</div><div class='bClear'></div>";
echo "<input type='hidden' id='cameraId' value='{$camera->info['id']}'>";
echo "<div id='ipPtzPresets'>";
echo "<div class='preset' id='".((empty($camera->info['ptz_control_path'])) ? 'selected' : '')."'><div class='name' id=''>".IPP_DISABLE_PTZ."</div></div>";
foreach($presets as $n => $preset){
	echo "<div class='preset' id='".(($preset['id']==$camera->info['ptz_control_path']) ? 'selected' : '')."'><div class='name' id='{$preset['id']}'>{$preset['name']}</div><div id='pButton' class='delete'></div><div id='pButton' class='edit'></div></div>";
}
echo "</div>";

?>