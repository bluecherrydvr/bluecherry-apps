<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

class DVRLogin{
	public $msg;
	public function __construct(){
		$this->msg = LOGIN_WRONG;
		$db = DVRDatabase::getInstance();
		$login = $db->DBEscapeString($_POST['login']);
		$user = new DVRUser('username', $login);
		!$this->CheckLoginData($user) or $this->DoLogin($user);
		
	}
	private function CheckLoginData($user){
		$password = md5($_POST['password'].$user->data[0]['salt']);
		return ($password===$user->data[0]['password']) ? true : false;
	}
	private function DoLogin($user){
		if ($user->data[0]['access_setup']==true) { $_SESSION['l'] = 'admin'; }
			elseif ($user->data[0]['access_setup']==true) { $_SESSION['l'] = 'viewer'; };
		$this->msg = 'OK';
		$_SESSION['id'] = $user->data[0]['id'];
		$_SESSION['from_client'] = empty($_POST['from_client']) ? 1 : 0;
	}
}

$login = new DVRLogin;
echo $login->msg;
?>
