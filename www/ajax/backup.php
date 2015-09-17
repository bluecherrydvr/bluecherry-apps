<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');


$database_parameters = data::query('SELECT database() as db, user() as usr');
$database_parameters = $database_parameters[0];
$database_parameters['usr'] = explode('@', $database_parameters['usr']);


if (!empty($_GET['mode'])){
	switch($_GET['mode']){
		case 'prepare':
			$_POST['noevents'] = (!empty($_POST['nodevices'])) ? 'on' : $_POST['noevents']; #make sure that events are not backed up if the devices are not
			#select tables to ignore
			$ignore_tables = '';
			if ((!empty($_POST['noevents'])) || (!empty($_POST['nodevices'])) || (!empty($_POST['nodevices']))) {
				#we are not backing up events or any related data
				if (!empty($_POST['noevents'])){
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.EventsCam";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.EventComments";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.EventTags";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.Media";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.EventsSystem";
				}
				#we are not backing up users or related data
				if (!empty($_POST['nousers'])){
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.Users";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.ActiveUsers";
				}
				#we are not backing up devices or related data
				if (!empty($_POST['nodevices'])){
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.Devices";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.PTZPresets";
					$ignore_tables .= " --ignore-table={$database_parameters['db']}.AvailableSources";
				}
				
			}
			shell_exec("mysqldump --single-transaction -u root --password=\"{$_POST['pwd']}\" {$database_parameters['db']} {$ignore_tables}> /tmp/bcbackup.sql");
			if (filesize("/tmp/bcbackup.sql")==0){
				data::responseJSON(false, BACKUP_FAILED);
			} else {
				$current_time = time();
				$backup_info = time().'|'.((!empty($_POST['noevents'])) ? '0' : '1').'|'.((!empty($_POST['nodevices'])) ? '0' : '1').'|'.((!empty($_POST['nousers'])) ? '0' : '1');
				$info_file = fopen('/tmp/bcbackupinfo', 'w');
				fwrite($info_file, $backup_info);
				shell_exec('tar --directory="/tmp" -zcvf '.VAR_MYSQLDUMP_TMP_LOCATION.' bcbackup.sql bcbackupinfo');
				#clean up
				unlink('/tmp/bcbackupinfo');
				unlink('/tmp/bcbackup.sql');
				data::responseJSON(true, true);
			}
		break;
		case 'download':
			header('Content-Description: File Transfer');
			header('Content-Type: application/octet-stream');
			header('Content-Length: ' . filesize(VAR_MYSQLDUMP_TMP_LOCATION));
			header('Content-Disposition: attachment; filename=' . date('ymd', time()).'_'.basename(VAR_MYSQLDUMP_TMP_LOCATION));
			readfile(VAR_MYSQLDUMP_TMP_LOCATION);
		break;
		case 'restore':
			if ($_FILES['restoreDataFile']['type']!= 'application/x-compressed-tar'){ #uploaded file is not a compressed tar archive
				data::responseJSON(false, BACKUP_R_WRONG_FILETYPE);
			} else {
				move_uploaded_file($_FILES['restoreDataFile']['tmp_name'], "/tmp/bcrestore.sql.gz");
				shell_exec("tar zxvf /tmp/bcrestore.sql.gz -C /tmp/");
				$info_file = fopen('/tmp/bcbackupinfo', 'r');
				if (!$info_file){ #no info file was in archive
					data::responseJSON('INFO', BACKUP_R_NOINFO);
				} else {
					$backup_info = fread($info_file, '1025');
					$backup_info = explode('|', $backup_info);
					$backup_info[0] = date('r', $backup_info[0]);
					$backup_info[1] = ($backup_info[1]) ? U_INCLUDED : U_NOTINCLUDED;
					$backup_info[2] = ($backup_info[2]) ? U_INCLUDED : U_NOTINCLUDED;
					$backup_info[3] = ($backup_info[3]) ? U_INCLUDED : U_NOTINCLUDED;
					$backup_info = str_replace(array('%DATE%', '%E%', '%D%', '%U%'), array($backup_info[0], $backup_info[1], $backup_info[2], $backup_info[3]), BACKUP_R_INFO);
					data::responseJSON(true, $backup_info);
					#clean up infofile
					unlink('/tmp/bcbackupinfo');
				};
			};
		break;
		case 'confirmRestore': #run restore
			$response = shell_exec("mysql -u root --password=\"{$_POST['pwd']}\" {$database_parameters['db']} < /tmp/bcbackup.sql 2>&1 1> /dev/null");
			if (!empty($response)){
				data::responseJSON(false, BACKUP_R_FAILED.$response);
			} else {
				data::responseJSON(true, BACKUP_R_SUCCESS);
			}
		break;
	}
	

	exit();
}

#template
include_once('../template/ajax/backup.php');

?>
