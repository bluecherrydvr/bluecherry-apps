<?php DEFINE('INDVR', true);

#include constants..
$nload = true; #do not load lang/var from lib

include("../var.php"); 
include("../lang.php");
include("../lib.php"); 

echo "
var_layoutToLoad = '".CHOOSE_LAYOUT."';
var_email = '".USERS_EMAIL."';
var_email_ex = '".USERS_EMAIL_EX."';
var_del_can_conf = '".DELTE_CAM_CONF."';
var_del_ipp_conf = '".IPP_DELETE."'
var_rm_client_download = '".RM_CLIENT_DOWNLOAD."'
var_check_connectivity = '".((!globalSettings::getParameter('G_DISABLE_IP_C_CHECK')) ? '1': '0')."';
";


?>
