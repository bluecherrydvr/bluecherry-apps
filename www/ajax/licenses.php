<?php 

require_once('/usr/share/bluecherry/www/lib/bc_license_wrapper.php');

class licenses extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.licenses');

        $this->view->licenses = data::getObject('Licenses');
    }

    public function postData()
    {
        if (!empty($_GET['mode']) && $_GET['mode'] == 'add'){
        	if (!bc_license_check($_POST['licenseCode'])){
        		data::responseJSON(false, L_INVALID_LICENSE);
        		exit();
        	};
        	$exists = data::getObject('Licenses', 'license', $_POST['licenseCode']);
        	if (!empty($exists)){
        		data::responseJSON(false, L_INVALID_LICENSE_EXISTS);
        		exit();
        	};
        	$machine_id = bc_license_machine_id();

        	$url = VAR_LICENSE_AUTH."/?license_code={$_POST['licenseCode']}&id=".$machine_id;
        	$confirmation = trim($this->sendHttpReq($url));

        	#if auto ok,
        	if ((strlen($confirmation) < 9) && (is_numeric($confirmation))){
        		data::responseJSON('F', constant('L_AUTO_RESP_'.$confirmation));
        		exit();
        	} else {
        		if (preg_match("/^[a-zA-Z0-9]{4}\-[a-zA-Z0-9]{4}$/", $confirmation, $matches)) {
        			$result = data::query("INSERT INTO Licenses VALUES ('{$_POST['licenseCode']}', '{$matches[0]}', UNIX_TIMESTAMP())", true);
        			if ($result){
        				data::responseJSON(true, L_LICENSE_ADDED);
        				exit();
        			} else {
        				data::responseJSON(false, false);
        				exit();
        			}
        		} else {
        			#if auto auth fails, show message, opt to confirm manually
        			data::responseJSON('CONF', $machine_id);
        			exit();
        		}
        	}
        	exit();
        }

        if (!empty($_GET['mode']) && $_GET['mode'] == 'confirm'){
        	if (bc_license_check_auth($_POST['licenseCode'], $_POST['confirmLicense'])) {
        		$exists = data::getObject('Licenses', 'license', $_POST['licenseCode']);
        		if (!empty($exists)){
        			data::responseJSON(false, L_INVALID_LICENSE_EXISTS);
        			exit();
        		} else {
        			$result = data::query("INSERT INTO Licenses VALUES ('{$_POST['licenseCode']}', '{$_POST['confirmLicense']}', UNIX_TIMESTAMP())", true);
        			if ($result){
        				data::responseJSON(true, L_LICENSE_ADDED);
        				exit();
        			} else {
        				data::responseJSON(false, false);
        				exit();
        			}
        		}
        	} else {
        		data::responseJSON(false, L_INVALID_CONFIRMATION);
        		exit();
        	}
        }
        if (!empty($_GET['mode']) && $_GET['mode'] == 'delete'){
            $result = data::query("DELETE FROM Licenses WHERE license = '{$_GET['license']}'", true);
            data::responseJSON(true);
        }
        if (!empty($_GET['mode']) && $_GET['mode'] == 'v3license'){
            if (bc_v3license_check()) {
                data::responseJSON(true, L_LICENSE_ADDED);
                exit();
            } else {
                data::responseJSON(false, false);
                exit();
            }
        }

    }

    private function sendHttpReq($url)
    {
        $ch = curl_init($url);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 4);
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt($ch, CURLOPT_NOBODY, false);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt ($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt ($ch, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt ($ch, CURLOPT_HTTPHEADER, array('Expect:'));

        $res = curl_exec($ch);

        curl_close($ch);

        return $res;
    }
}



