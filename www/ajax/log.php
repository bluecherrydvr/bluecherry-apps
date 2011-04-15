<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check



class DVRLog{
	public $log;
	public $log_size;
	function __construct(){
		$this->log = $this->GetLog();
		$this->log_size = (file_exists(VAR_LOG_PATH)) ? intval(filesize(VAR_LOG_PATH)/1024) : LOG_FILE_DOES_NOT_EXIST;
	}
	function GetLog(){
		if (!empty($_GET['lines']) && $_GET['lines'] == 'all'){
			$content = file(VAR_LOG_PATH);
		} else {
			$lines = empty($_GET['lines']) ? 20 : intval($_GET['lines']);
			# get VAR_LOG_PATH file {$lines} last lines, explode by
			# new line chars and filter out empty lines
			$content = array_filter(explode("\n" ,
				nl2br(shell_exec("tail -n $lines ".
				VAR_LOG_PATH))), 'strlen');
		}
		
		return (empty($content)) ? array(LOG_EMPTY) : $content;
	}
}

#run class/get data
$log = new DVRLog;

#require template to show data
require('../template/ajax/log.php');
?>
