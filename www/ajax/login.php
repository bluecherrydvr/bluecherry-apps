<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$user_to_login = new user('username', $_POST['login']);
$from_client = (empty($_POST['from_client'])) ? false : true;
$result = $user_to_login->doLogin($_POST['password'], $from_client);

echo $result;

?>
