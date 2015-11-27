<?php

class login extends Controller {

    public function __construct ()
    {
        parent::__construct();
    }

    public function getData()
    {
        $this->setView('login', false);
    }

    public function postData()
    {
        $user_to_login = new user('username', $_POST['login']);
        $from_client = (empty($_POST['from_client'])) ? false : true;
        $result = $user_to_login->doLogin($_POST['password'], $from_client);

        if (Inp::post('from_client')) {
            echo $result;
        } else {
            if ($result == 'OK') {
                if ($user_to_login->info['access_setup'] == 1) {
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
