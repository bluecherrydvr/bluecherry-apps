<?php defined('INDVR') or exit(); 
#template common functions
require('../template/template.lib.php');

if (false){
	?>
	<?php
	exit();
}
?>


<h1 id='header' class='header'><?php echo BACKUP_HEADER; ?></h1>
<p><?php echo BACKUP_SUBHEADER; ?></p>
<div class='container-separator' id='backup-container'>
	<div class='title main'><?php echo BACKUP_B_TITLE; ?></div>
	<form action='/ajax/backup.php?mode=prepare' method='post' id='backup-form'>
		<div class='container-separator'>
			<div class='title'><?php echo str_replace(array('%USERS%', '%DBNAME%'), array('root', $database_parameters['db']), BACKUP_PWD);?></div>
			<input type='password' name='pwd'>
		</div>
		<div class='container-separator'>
			<div class='title'><?php echo BACKUP_B_OPT; ?></div>
			<div><input type='checkbox' name='noevents' id='no-events' checked><?php echo BACKUP_B_OPT_NE; ?></div>
			<div><input type='checkbox' name='nodevices' id='no-devices'><?php echo BACKUP_B_OPT_ND; ?></div>
			<div><input type='checkbox' name='nousers' id='no-users'><?php echo BACKUP_B_OPT_NU; ?></div>
		</div>
		<div><input type='submit' value='<?php echo U_BACKUP; ?>' id='backup-form-submt'></div>
	</form>
</div>
<div class='container-separator' id='restore-container'>
	<form action='' method='post' id='restore-form' enctype="multipart/form-data">
	<div class='title main'><?php echo BACKUP_R_TITLE; ?></div>
	<div class='container-separator' id='restore-pwd-container' style='display:none;'>
		<div class='title'><?php echo str_replace(array('%USERS%', '%DBNAME%'), array('root', $database_parameters['db']), BACKUP_PWD);?></div>
		<input type='password' name='pwd'>
	</div>
	<div class='container-separator' id='restore-file-container'>
		<div class='title'><?php echo BACKUP_R_LABEL; ?></div>
		<input type='file' name='restore' id='restore-file'>	
	</div>
	<div id='restore-form-button-container' style='display:none;'></form><input type='submit' value='<?php echo U_RESTORE; ?>' id='restore-form-submt'><input type='submit' value='<?php echo U_CANCEL; ?>' id='restore-form-cancel'></div>
	
</div>
