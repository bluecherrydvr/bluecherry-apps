<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

class ActiveUsers{
	public $users;
	public function __construct(){
		$tmp = data::query("SELECT * FROM ActiveUsers");
		foreach($tmp as $k => $user){
			$this->users[$k] = new user('id', $user['id']);
			$this->users[$k]->info['ip'] = $user['ip'];
			$this->users[$k]->info['kick'] = $user['kick'];
			$this->users[$k]->info['from_client'] = $user['from_client'];
		}
		//var_dump_pre($this->users);
		if ($_GET['XML']) $this->MakeXML(); else
			include_once('../template/ajax/activeusers.php');
	}
	private function MakeXML(){
		$xml = '<?xml version="1.0" encoding="UTF-8" ?><activeusers>';
		foreach($this->users as $k => $user){
			$xml .= "<user id='{$user->data[0]['id']}'>";
				$xml .= "<name>{$user->data[0]['name']}</name>
						<ip>{$user->data[0]['ip']}</ip>
						<kick>{$user->data[0]['kick']}</kick>";
			$xml .= "</user>";
		}
		$xml .='</activeusers>';
		header('Content-type: text/xml');
		print_r($xml);
	}
}

$au = new ActiveUsers;

?>
