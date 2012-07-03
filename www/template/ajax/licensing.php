<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');

$mode = (empty($_GET['mode'])) ? false : $_GET['mode'];

if ($mode=='list'){
	?>
	
	<div class='title main'><?php echo L_CURRENT;?></div>
	<?php
		if (empty($licenses)){
			echo "<div class='infoMessage INFO'>".L_NO_LICENSES."</div>";
		} else {
			echo '<table class="license"><tr><th>License code</th><th>Type</th><th></th></tr>';
			foreach($licenses as $license){
				echo "<tr><td>{$license['license']}</td><td>".bc_license_check($license['license'])." ".L_PORTS."</td><td><div class='delete-button removeLicense' id='{$license['license']}'>".L_DELETE."</div></td></tr>";
			}
			echo '</table>';
		}
		exit();
}
?>

<div class='container-separator add-license-container' id='add-license-container'>
	<span id='addition'>
		<div class='title main'><?php echo L_ADDCODE; ?></div>
		<form method='post' action='/ajax/licensing.php?mode=add' id='add-license-form'>
			<INPUT type='text' class='license' id='license-code' name='licenseCode'>
			<INPUT type='submit' value='<?php echo L_ADD; ?>' id='add-license-submit'>
		</form>
	</span>
	<span style="display:none;" id='confirmation'>
		<div class='title main'><?php echo L_CONFIRMCODE; ?></div>
		<div class='infoMessage INFO'><?php echo L_CONFIRMCODE_MSG; ?><span id='machine-id'></span></div>
		<form method='post' action='/ajax/licensing.php?mode=confirm' id='confirmation-form'>
			<INPUT type='hidden' id='confirn-license-code' name='licenseCode'>
			<INPUT type='text' class='license' name='confirmLicense'>
			<INPUT type='submit' value='<?php echo L_CONFIRM; ?>' id='confirmation-submit'>
		</form>
	</span>
</div>
<div class='container-separator' id='licenses-container'>
</div>
