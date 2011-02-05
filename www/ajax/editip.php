<?php DEFINE(INDVR, true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

$id = (isset($_GET['id'])) ? intval($_GET['id']) : false;
$ipCamera = new DVRIPCameras($id);

include_once('../template/ajax/editip.php');
?>