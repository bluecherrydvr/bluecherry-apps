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
        if (Inp::post('mode') !== 'testemail') {
            return;
        }

        $global_settings = $this->varpub->global_settings;


        $message = (new \Swift_Message(G_TEST_EMAIL_SUBJECT))
            ->setTo($_POST['email'])
            ->setBody(G_TEST_EMAIL_BODY . ' from ' . $global_settings->data['G_DVR_NAME'], 'text/html');

        if (!empty($global_settings->data['G_SMTP_EMAIL_FROM'])) {
            $message->setFrom($global_settings->data['G_SMTP_EMAIL_FROM']);
        }


        switch($global_settings->data['G_SMTP_SERVICE']){
            case 'default': #use MTA
                $transport = new \Swift_SendmailTransport('/usr/sbin/sendmail -bs');
                break;
            case 'smtp': #user user supplied SMTP config

                $transport = (new \Swift_SmtpTransport($global_settings->data['G_SMTP_HOST']))
                    ->setUsername($global_settings->data['G_SMTP_USERNAME'])
                    ->setPassword($global_settings->data['G_SMTP_PASSWORD']);

                if (isset($global_settings->data['G_SMTP_PORT'])) {
                    $transport->setPort($global_settings->data['G_SMTP_PORT']);
                }

                if (isset($global_settings->data['G_SMTP_SSL']) && in_array($global_settings->data['G_SMTP_SSL'], ['ssl', 'tls'], true)) {
                    $transport->setEncryption($global_settings->data['G_SMTP_SSL']);
                }

                break;
        }

        $logger = new \Swift_Plugins_Loggers_ArrayLogger();
        $transport->registerPlugin(new \Swift_Plugins_LoggerPlugin($logger));


        if ($transport->send($message)) {
            data::responseJSON(true, G_TEST_EMAIL_SENT);

        } else {
            $message = $logger->dump();
            data::responseJSON(false, "E: " . $message);
        }
    }
}


#require template to show data
//include_once('../template/ajax/general.php');
?>
