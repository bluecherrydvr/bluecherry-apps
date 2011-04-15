<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$user_to_login = new user('username', $_POST['login']);
$result = $user_to_login->doLogin($_POST['password'],
		defined($_POST['from_client']) ? $_POST['from_client'] : NULL);

echo $result;

?>
