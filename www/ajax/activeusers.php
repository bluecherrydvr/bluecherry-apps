<?php DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

class ActiveUsers{
	public $users;
	public function __construct(){
		$db = DVRDatabase::getInstance();
		$tmp = $db->DBFetchAll($db->DBQuery("SELECT * FROM ActiveUsers"));
		foreach($tmp as $k => $user){
			$this->users[$k] = new DVRUser('id', $user['id']);
			$this->users[$k]->data[0]['ip'] = $user['ip'];
			$this->users[$k]->data[0]['kick'] = $user['kick'];
			$this->users[$k]->data[0]['from_client'] = $user['from_client'];
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