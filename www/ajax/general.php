<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

if (isset($_POST['mode']) && ($_POST['mode'] == 'testemail')) {
    include_once('Mail.php');
    include_once('Mail/mime.php');
    $crlf = "\r\n";
    $mime = new Mail_mime($crlf);
    $email = $_POST['email'];
    
    $headers = array("From"=>$global_settings->data['G_SMTP_EMAIL_FROM'], "Subject" => G_TEST_EMAIL_SUBJECT);
    $body = G_TEST_EMAIL_BODY . ' from ' . $global_settings->data['G_DVR_NAME'];
    $mime->setHTMLBody($body);  
    $headers = $mime->headers($headers);
    $body = $mime->get();
    switch($global_settings->data['G_SMTP_SERVICE']){
    	case 'default': #use MTA
    		$mail = Mail::factory('mail');
    	break;
    	case 'smtp': #user user supplied SMTP config
    		$smtp_params['host'] = (($global_settings->data['G_SMTP_SSL'] == 'none') ? '' : 'ssl://').$global_settings->data['G_SMTP_HOST'];
    		$smtp_params['port'] = $global_settings->data['G_SMTP_PORT'];
    		$smtp_params['username'] = $global_settings->data['G_SMTP_USERNAME'];
    		$smtp_params['password'] = $global_settings->data['G_SMTP_PASSWORD'];
    		$smtp_params['auth'] = true;
    		$mail = Mail::factory('smtp', $smtp_params);
    		if ($global_settings->data['G_SMTP_SSL'] == 'tls') {
    			$mail->SMTPSecure = "ssl";
    		};
    	break;
    }
	$re = $mail->send($email, $headers, $body); 
    if (PEAR::isError($re)) {
		data::responseJSON(false, "E: ".$re->getMessage());
    } else {
        data::responseJSON(true, G_TEST_EMAIL_SENT);
    }
}



#require template to show data
include_once('../template/ajax/general.php');
?>
