<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check



class logs{
	public $log;
	public $log_size;
	function __construct($log_type, $lines){
		$log_path = '';
		switch ($log_type){
			case 'www':	$log_path = VAR_WWW_LOG_PATH;
				break;
			default:		$log_path = VAR_LOG_PATH;
				break;
			
		}
		$this->log = $this->GetLog($log_path, $lines);
		$this->log_size = (file_exists($log_path)) ? intval(filesize($log_path)/1024) : LOG_FILE_DOES_NOT_EXIST;
	}
	function GetLog($log_path, $lines){
		if (!empty($lines) && $lines=='all'){
			$content = file($log_path);
		} else {
			$lines = empty($lines) ? 20 : intval($lines);
			# get VAR_LOG_PATH file {$lines} last lines, explode by
			# new line chars and filter out empty lines
			$content = array_filter(explode("\n" ,	nl2br(shell_exec("tail -n $lines ".$log_path))), 'strlen');
		}
		return (empty($content)) ? array(LOG_EMPTY) : $content;
	}
}

#run class/get data
$type = (!empty($_GET['type'])) ? $_GET['type'] : false;
$lines = (!empty($_GET['lines'])) ? $_GET['lines'] : false;
$log = new logs($type, $lines);

#require template to show data
require('../template/ajax/log.php');
?>
