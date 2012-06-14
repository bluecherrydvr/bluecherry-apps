<?php DEFINE('INDVR', true); #prepares, compressed and serves logs and computer info
#lib

include("../lib/lib.php");  #common functions

#auth check
$id = (!empty($_SESSION['id'])) ? $_SESSION['id'] : false;
$current_user = new user('id', $id);
$current_user->checkAccessPermissions('devices');
#/auth check

#get the computer info/dmesg log
shell_exec("lshw > /tmp/sysinfo.txt; dmesg > /tmp/dmesg.txt;");
#tar the logs
shell_exec("tar -cf ".VAR_TARRED_LOGS_TMP_LOCATION." ".VAR_LOG_PATH." ".VAR_WWW_LOG_PATH." /tmp/dmesg.txt /tmp/sysinfo.txt > /dev/null"); 

#output the tarred files
header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Length: ' . filesize(VAR_TARRED_LOGS_TMP_LOCATION));
header('Content-Disposition: attachment; filename=' . basename(VAR_TARRED_LOGS_TMP_LOCATION));
readfile(VAR_TARRED_LOGS_TMP_LOCATION);

exit();
?>