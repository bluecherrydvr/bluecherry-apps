<?php

class profile extends Controller {

    public function __construct()
    {
        parent::__construct();
        $this->left_menu = false;
        $this->chAccess('viewer');
    }

    public function getData()
    {
        $this->setView('ajax.profile');


        $this->view->user_info = $this->getUserInfo();
    }

    public function postData()
    {
        //$result = user::update($_POST);
        //data::responseJSON($result[0], $result[1], $result[2]);

        $user_info = $this->getUserInfo();

        $password_current = Inp::post('password_current', '');
        $password_new = Inp::post('password_new', '');

        if (empty($password_current) || empty($password_new)) {
            data::responseJSON(7, NO_PASS);
        }

        $password_current = md5($password_current.$user_info['salt']);

        if ($password_current != $user_info['password']) {
            data::responseJSON(7, USERS_PASSWORD_WRONG);
        }

        $password_new = md5($password_new.$user_info['salt']);

        $query = data::formQueryFromArray('update', 'Users', Array('password' => $password_new), 'id', $user_info['id']);
        $check = (data::query($query, true)) ? true : false;

        data::responseJSON($check, false);
    }

    private function getUserInfo()
    {
        $user_id = $this->core->session->get('id');

        $user_info = data::getObject('Users', 'id', $user_id);
        $user_info[0]['emails_arr'] = explode('|', $user_info[0]['email']);

        return $user_info[0];
    }
}
