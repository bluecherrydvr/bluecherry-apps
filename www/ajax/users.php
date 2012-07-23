<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check


class users {
	public $data;
	public $all;
	function __construct($id){
		if (!$id) { $this->data = $this->GetAllUsersInfo(); $this->all = true; }
			elseif  ($id=='new') { $this->data['name'] = USERS_NEW; $this->all = false; $this->data['new'] = true; $this->data[0]['emails'] = array(' : '); } //enter only name
			else { $this->data = $this->GetUserInfo($id); $this->all = false; }
	}
	
	private function GetAllUsersInfo(){
		return data::getObject('Users');	
	}
	
	private function GetUserInfo($id){
		$user_data = data::getObject('Users', 'id', $id);
		$tmp = explode('|', $user_data[0]['email']);
		
		foreach($tmp as $key => $value){
			$value = explode(':', $value);
			$user_data[0]['emails'][$key]['addrs'] = $value[0];
		}
		return $user_data;
	}
}

$id = empty($_GET['id']) ? false : $_GET['id'];
$dvr_users = new users($id);

#template
include_once('../template/ajax/users.php');
?>
