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
        // License management disabled for free and open source version
        data::responseJSON(false, "License management is not available in the free version");
        exit();

        // if (!empty($_GET['mode']) && $_GET['mode'] == 'confirm'){
        // 	if (bc_license_check_auth($_POST['licenseCode'], $_POST['confirmLicense'])) {
        // 		$exists = data::getObject('Licenses', 'license', $_POST['licenseCode']);
        // 		if (!empty($exists)){
        // 			data::responseJSON(false, L_INVALID_LICENSE_EXISTS);
        // 			exit();
        // 		} else {
        // 			$result = data::query("INSERT INTO Licenses VALUES ('{$_POST['licenseCode']}', '{$_POST['confirmLicense']}', UNIX_TIMESTAMP())", true);
        // 			if ($result){
        // 				data::responseJSON(true, L_LICENSE_ADDED);
        // 				exit();
        // 			} else {
        // 				data::responseJSON(false, false);
        // 				exit();
        // 			}
        // 		}
        // 	} else {
        // 		data::responseJSON(false, L_INVALID_CONFIRMATION);
        // 		exit();
        // 	}
        // }

        // License deactivation disabled for free and open source version
        if (!empty($_GET['mode']) && $_GET['mode'] == 'deactivate') {
            data::responseJSON(false, "License management is not available in the free version");
            exit();
        }

    }

	private function updateVerifiedLicense($licenseCode) {
		$licenses = data::getObject('Licenses');
		$result = false;

		if (empty($licenses)) {
			$result = data::query("INSERT INTO Licenses VALUES ('{$licenseCode}', '', UNIX_TIMESTAMP())", true);
		}
		else {
			$current = $licenses[0]['license'];
			$result = data::query("UPDATE  Licenses SET license='{$licenseCode}', added=UNIX_TIMESTAMP() WHERE license='$current'", true);
		}

		return $result;
	}

	public static function getLicenseStatusMessage($status)
	{
		$message = L_LA_OK;

		switch($status) {
			case Constant('LA_OK'):
				$message = L_LA_OK;
				break;
			case Constant('LA_FAIL'):
				$message = L_LA_FAIL;
				break;
			case Constant('LA_EXPIRED'):
				$message = L_LA_EXPIRED;
				break;
			case Constant('LA_SUSPENDED'):
				$message = L_LA_SUSPENDED;
				break;
			case Constant('LA_GRACE_PERIOD_OVER'):
				$message = L_LA_GRACE_PERIOD_OVER;
				break;
			case Constant('LA_TRIAL_EXPIRED'):
				$message = L_LA_TRIAL_EXPIRED;
				break;
			case Constant('LA_LOCAL_TRIAL_EXPIRED'):
				$message = L_LA_LOCAL_TRIAL_EXPIRED;
				break;
			case Constant('LA_RELEASE_UPDATE_AVAILABLE'):
				$message = L_LA_RELEASE_UPDATE_AVAILABLE;
				break;
			case Constant('LA_RELEASE_NO_UPDATE_AVAILABLE'):
				$message = L_LA_RELEASE_NO_UPDATE_AVAILABLE;
				break;
			case Constant('LA_E_FILE_PATH'):
				$message = L_LA_E_FILE_PATH;
				break;
			case Constant('LA_E_PRODUCT_FILE'):
				$message = L_LA_E_PRODUCT_FILE;
				break;
			case Constant('LA_E_PRODUCT_DATA'):
				$message = L_LA_E_PRODUCT_DATA;
				break;
			case Constant('LA_E_PRODUCT_ID'):
				$message = L_LA_E_PRODUCT_ID;
				break;
			case Constant('LA_E_SYSTEM_PERMISSION'):
				$message = L_LA_E_SYSTEM_PERMISSION;
				break;
			case Constant('LA_E_FILE_PERMISSION'):
				$message = L_LA_E_FILE_PERMISSION;
				break;
			case Constant('LA_E_WMIC'):
				$message = L_LA_E_WMIC;
				break;
			case Constant('LA_E_TIME'):
				$message = L_LA_E_TIME;
				break;
			case Constant('LA_E_INET'):
				$message = L_LA_E_INET;
				break;
			case Constant('LA_E_NET_PROXY'):
				$message = L_LA_E_NET_PROXY;
				break;
			case Constant('LA_E_HOST_URL'):
				$message = L_LA_E_HOST_URL;
				break;
			case Constant('LA_E_BUFFER_SIZE'):
				$message = L_LA_E_BUFFER_SIZE;
				break;
			case Constant('LA_E_APP_VERSION_LENGTH'):
				$message = L_LA_E_APP_VERSION_LENGTH;
				break;
			case Constant('LA_E_REVOKED'):
				$message = L_LA_E_REVOKED;
				break;
			case Constant('LA_E_LICENSE_KEY'):
				$message = L_LA_E_LICENSE_KEY;
				break;
			case Constant('LA_E_LICENSE_TYPE'):
				$message = L_LA_E_LICENSE_TYPE;
				break;
			case Constant('LA_E_OFFLINE_RESPONSE_FILE'):
				$message = L_LA_E_OFFLINE_RESPONSE_FILE;
				break;
			case Constant('LA_E_OFFLINE_RESPONSE_FILE_EXPIRED'):
				$message = L_LA_E_OFFLINE_RESPONSE_FILE_EXPIRED;
				break;
			case Constant('LA_E_ACTIVATION_LIMIT'):
				$message = L_LA_E_ACTIVATION_LIMIT;
				break;
			case Constant('LA_E_ACTIVATION_NOT_FOUND'):
				$message = L_LA_E_ACTIVATION_NOT_FOUND;
				break;
			case Constant('LA_E_DEACTIVATION_LIMIT'):
				$message = L_LA_E_DEACTIVATION_LIMIT;
				break;
			case Constant('LA_E_TRIAL_NOT_ALLOWED'):
				$message = L_LA_E_TRIAL_NOT_ALLOWED;
				break;
			case Constant('LA_E_TRIAL_ACTIVATION_LIMIT'):
				$message = L_LA_E_TRIAL_ACTIVATION_LIMIT;
				break;
			case Constant('LA_E_MACHINE_FINGERPRINT'):
				$message = L_LA_E_MACHINE_FINGERPRINT;
				break;
			case Constant('LA_E_METADATA_KEY_LENGTH'):
				$message = L_LA_E_METADATA_KEY_LENGTH;
				break;
			case Constant('LA_E_METADATA_VALUE_LENGTH'):
				$message = L_LA_E_METADATA_VALUE_LENGTH;
				break;
			case Constant('LA_E_ACTIVATION_METADATA_LIMIT'):
				$message = L_LA_E_ACTIVATION_METADATA_LIMIT;
				break;
			case Constant('LA_E_TRIAL_ACTIVATION_METADATA_LIMIT'):
				$message = L_LA_E_TRIAL_ACTIVATION_METADATA_LIMIT;
				break;
			case Constant('LA_E_METADATA_KEY_NOT_FOUND'):
				$message = L_LA_E_METADATA_KEY_NOT_FOUND;
				break;
			case Constant('LA_E_TIME_MODIFIED'):
				$message = L_LA_E_TIME_MODIFIED;
				break;
			case Constant('LA_E_RELEASE_VERSION_FORMAT'):
				$message = L_LA_E_RELEASE_VERSION_FORMAT;
				break;
			case Constant('LA_E_AUTHENTICATION_FAILED'):
				$message = L_LA_E_AUTHENTICATION_FAILED;
				break;
			case Constant('LA_E_METER_ATTRIBUTE_NOT_FOUND'):
				$message = L_LA_E_METER_ATTRIBUTE_NOT_FOUND;
				break;
			case Constant('LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED'):
				$message = L_LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED;
				break;
			case Constant('LA_E_CUSTOM_FINGERPRINT_LENGTH'):
				$message = L_LA_E_CUSTOM_FINGERPRINT_LENGTH;
				break;
			case Constant('LA_E_VM'):
				$message = L_LA_E_VM;
				break;
			case Constant('LA_E_COUNTRY'):
				$message = L_LA_E_COUNTRY;
				break;
			case Constant('LA_E_IP'):
				$message = L_LA_E_IP;
				break;
			case Constant('LA_E_RATE_LIMIT'):
				$message = L_LA_E_RATE_LIMIT;
				break;
			case Constant('LA_E_SERVER'):
				$message = L_LA_E_SERVER;
				break;
			case Constant('LA_E_CLIENT'):
				$message = L_LA_E_CLIENT;
				break;
			default:
				$message = L_LA_E_UNKNOWN;
		}

		return $message;

	}
}



