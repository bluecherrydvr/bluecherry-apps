<?php 

class logout extends Controller {

    public function getData() {
        if (isset($_SESSION['id'])) {
            $current_user = new user('id', $_SESSION['id']);
            $current_user->checkAccessPermissions('viewer');

            $current_user->doLogout();
        }
        $_COOKIE['currentPage'] = '';

        header("Location: /login");
		exit;
    }
}

