<?php DEFINE(INDVR, true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check


class Users extends DVRData{
	public $data;
	public $all;
	function __construct($id = false){
		if (!isset($id)) { $this->data = $this->GetAllUsersInfo(); $this->all = true; }
			elseif  ($id=='new') { $this->data['name'] = USERS_NEW; $this->all = false; $this->data['new'] = true; } //enter only name
			else { $this->GetUserInfo($id); $this->data = $this->GetUserInfo($id); $this->all = false; }
	}
	
	private function GetAllUsersInfo(){
		$db = DVRDatabase::getInstance();
		return $this->GetObjectData('Users');
		
	}
	
	private function GetUserInfo($id){
		return $this->GetObjectData('Users', 'id', $id);
	}
}


$dvr_users = new Users($_GET['id']);

#template
include_once('../template/ajax/users.php');
?>