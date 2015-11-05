<?php 

class general extends Controller {

    public function __construct ()
    {
        parent::__construct();
        $this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.general');
        
    }

    public function postData()
    {
        if (Inp::post('mode') == 'testemail') {
            $global_settings = $this->varpub->global_settings;

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
    }
}


#require template to show data
//include_once('../template/ajax/general.php');
?>
