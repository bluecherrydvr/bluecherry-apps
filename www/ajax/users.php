<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check


class Users {
	public $data;
	public $all;
	function __construct($id){
		if (!$id) { $this->data = $this->GetAllUsersInfo(); $this->all = true; }
			elseif  ($id=='new') { $this->data['name'] = USERS_NEW; $this->all = false; $this->data['new'] = true; } //enter only name
			else { $this->GetUserInfo($id); $this->data = $this->GetUserInfo($id); $this->all = false; }
	}
	
	private function GetAllUsersInfo(){
		return data::getObject('Users');
		
	}
	
	private function GetUserInfo($id){
		return data::getObject('Users', 'id', $id);
	}
}

$id = empty($_GET['id']) ? false : $_GET['id'];
$dvr_users = new Users($id);

#template
include_once('../template/ajax/users.php');
?>
