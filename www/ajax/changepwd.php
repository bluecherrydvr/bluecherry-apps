<?php

class changepwd extends Controller {

    public function __construct ()
    {
        parent::__construct();
	$this->chAccess('viewer');//????which one?
    }

    public function getData()
    {
        $this->setView('changepwd', false);
    }

    public function postData()
    {
	if (empty($_SESSION['id'])) {
		Reply::ajaxDie('2', '/login');
		exit();
	}

	$user_id = $_SESSION['id'];
	$user_info = data::getObject('Users', 'id', $user_id);
	$user_info = $user_info[0];

        $from_client = (empty($_POST['from_client'])) ? false : true;

	$password = $_POST['password'];
	$confirm_password = $_POST['confirm_password'];

	if ($password != $confirm_password) {
		Reply::ajaxDie('2', 'PASSWORDS DO NOT MATCH');
	}

	//salt, md5, update DB
        if (empty($password) || empty($confirm_password)) {
            data::responseJSON(7, NO_PASS);
        }

        $password = md5($password.$user_info['salt']);

        $query = data::formQueryFromArray('update', 'Users', Array('password' => $password, 'change_password' => '0'), 'id', $user_info['id']);
        $check = (data::query($query, true)) ? true : false;

	if ($check)
		$result = 'OK';
	else
		$result = 'FAIL';

        if (Inp::post('from_client')) {
            echo $result;
        } else {
            if ($result == 'OK') {
                if ($user_info['access_setup'] == 1) {
                    Reply::ajaxDie('1', '/');
                } else {
                    Reply::ajaxDie('1', '/liveview');
                }
            } else {
                Reply::ajaxDie('2', $result);
            }
        }

    }
}


?>
