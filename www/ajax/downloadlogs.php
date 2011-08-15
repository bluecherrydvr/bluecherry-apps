<?php DEFINE('INDVR', true);
#lib

include("../lib/lib.php");  #common functions

#auth check
$id = (!empty($_SESSION['id'])) ? $_SESSION['id'] : false;
$current_user = new user('id', $id);
$current_user->checkAccessPermissions('devices');
#/auth check

shell_exec("lshw > /tmp/sysinfo.txt; dmesg > /tmp/dmesg.txt;");
shell_exec("tar -cf ".VAR_TARRED_LOGS_TMP_LOCATION." ".VAR_LOG_PATH." ".VAR_WWW_LOG_PATH." /tmp/dmesg.txt /tmp/sysinfo.txt > /dev/null"); 


header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Length: ' . filesize(VAR_TARRED_LOGS_TMP_LOCATION));
header('Content-Disposition: attachment; filename=' . basename(VAR_TARRED_LOGS_TMP_LOCATION));
readfile(VAR_TARRED_LOGS_TMP_LOCATION);

exit();
?>