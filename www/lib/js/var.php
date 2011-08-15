<?php DEFINE('INDVR', true);

#include constants..
include("../var.php"); 
include("../lang.php");

echo "

var_layoutToLoad = '".CHOOSE_LAYOUT."';
var_email = '".USERS_EMAIL."';
var_email_ex = '".USERS_EMAIL_EX."';
var_del_can_conf = '".DELTE_CAM_CONF."';
var_del_ipp_conf = '".IPP_DELETE."'
";


?>