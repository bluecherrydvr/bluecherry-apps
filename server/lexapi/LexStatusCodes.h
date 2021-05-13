#ifndef LEX_STATUS_CODES_H
#define LEX_STATUS_CODES_H

enum LexStatusCodes
{
    /*
        CODE: LA_OK

        MESSAGE: Success code.
    */
    LA_OK = 0,

    /*
        CODE: LA_FAIL

        MESSAGE: Failure code.
    */
    LA_FAIL = 1,

    /*
        CODE: LA_EXPIRED

        MESSAGE: The license has expired or system time has been tampered
        with. Ensure your date and time settings are correct.
    */
    LA_EXPIRED = 20,

    /*
        CODE: LA_SUSPENDED

        MESSAGE: The license has been suspended.
    */
    LA_SUSPENDED = 21,

    /*
        CODE: LA_GRACE_PERIOD_OVER

        MESSAGE: The grace period for server sync is over.
    */
    LA_GRACE_PERIOD_OVER = 22,

    /*
        CODE: LA_TRIAL_EXPIRED

        MESSAGE: The trial has expired or system time has been tampered
        with. Ensure your date and time settings are correct.
    */
    LA_TRIAL_EXPIRED = 25,

    /*
        CODE: LA_LOCAL_TRIAL_EXPIRED

        MESSAGE: The local trial has expired or system time has been tampered
        with. Ensure your date and time settings are correct.
    */
    LA_LOCAL_TRIAL_EXPIRED = 26,

    /*
        CODE: LA_RELEASE_UPDATE_AVAILABLE

        MESSAGE: A new update is available for the product. This means a new release has
        been published for the product.
    */
    LA_RELEASE_UPDATE_AVAILABLE = 30,

    /*
        CODE: LA_RELEASE_NO_UPDATE_AVAILABLE

        MESSAGE: No new update is available for the product. The current version is latest.
    */
    LA_RELEASE_NO_UPDATE_AVAILABLE = 31,

    /*
        CODE: LA_E_FILE_PATH

        MESSAGE: Invalid file path.
    */
    LA_E_FILE_PATH = 40,

    /*
        CODE: LA_E_PRODUCT_FILE

        MESSAGE: Invalid or corrupted product file.
    */
    LA_E_PRODUCT_FILE = 41,

    /*
        CODE: LA_E_PRODUCT_DATA

        MESSAGE: Invalid product data.
    */
    LA_E_PRODUCT_DATA = 42,

    /*
        CODE: LA_E_PRODUCT_ID

        MESSAGE: The product id is incorrect.
    */
    LA_E_PRODUCT_ID = 43,

    /*
        CODE: LA_E_SYSTEM_PERMISSION

        MESSAGE: Insufficient system permissions. Occurs when LA_SYSTEM flag is used
        but application is not run with admin privileges.
    */
    LA_E_SYSTEM_PERMISSION = 44,

    /*
        CODE: LA_E_FILE_PERMISSION

        MESSAGE: No permission to write to file.
    */
    LA_E_FILE_PERMISSION = 45,

    /*
        CODE: LA_E_WMIC

        MESSAGE: Fingerprint couldn't be generated because Windows Management
        Instrumentation (WMI) service has been disabled. This error is specific
        to Windows only.
    */
    LA_E_WMIC = 46,

    /*
        CODE: LA_E_TIME

        MESSAGE: The difference between the network time and the system time is
        more than allowed clock offset.
    */
    LA_E_TIME = 47,

    /*
        CODE: LA_E_INET

        MESSAGE: Failed to connect to the server due to network error.
    */
    LA_E_INET = 48,

    /*
        CODE: LA_E_NET_PROXY

        MESSAGE: Invalid network proxy.
    */
    LA_E_NET_PROXY = 49,

    /*
        CODE: LA_E_HOST_URL

        MESSAGE: Invalid Cryptlex host url.
    */
    LA_E_HOST_URL = 50,

    /*
        CODE: LA_E_BUFFER_SIZE

        MESSAGE: The buffer size was smaller than required.
    */
    LA_E_BUFFER_SIZE = 51,

    /*
        CODE: LA_E_APP_VERSION_LENGTH

        MESSAGE: App version length is more than 256 characters.
    */
    LA_E_APP_VERSION_LENGTH = 52,

    /*
        CODE: LA_E_REVOKED

        MESSAGE: The license has been revoked.
    */
    LA_E_REVOKED = 53,

    /*
        CODE: LA_E_LICENSE_KEY

        MESSAGE: Invalid license key.
    */
    LA_E_LICENSE_KEY = 54,

    /*
        CODE: LA_E_LICENSE_TYPE

        MESSAGE: Invalid license type. Make sure floating license
        is not being used.
    */
    LA_E_LICENSE_TYPE = 55,

    /*
        CODE: LA_E_OFFLINE_RESPONSE_FILE

        MESSAGE: Invalid offline activation response file.
    */
    LA_E_OFFLINE_RESPONSE_FILE = 56,

    /*
        CODE: LA_E_OFFLINE_RESPONSE_FILE_EXPIRED

        MESSAGE: The offline activation response has expired.
    */
    LA_E_OFFLINE_RESPONSE_FILE_EXPIRED = 57,

    /*
        CODE: LA_E_ACTIVATION_LIMIT

        MESSAGE: The license has reached it's allowed activations limit.
    */
    LA_E_ACTIVATION_LIMIT = 58,

    /*
        CODE: LA_E_ACTIVATION_NOT_FOUND

        MESSAGE: The license activation was deleted on the server.
    */
    LA_E_ACTIVATION_NOT_FOUND = 59,

    /*
        CODE: LA_E_DEACTIVATION_LIMIT

        MESSAGE: The license has reached it's allowed deactivations limit.
    */
    LA_E_DEACTIVATION_LIMIT = 60,

    /*
        CODE: LA_E_TRIAL_NOT_ALLOWED

        MESSAGE: Trial not allowed for the product.
    */
    LA_E_TRIAL_NOT_ALLOWED = 61,

    /*
        CODE: LA_E_TRIAL_ACTIVATION_LIMIT

        MESSAGE: Your account has reached it's trial activations limit.
    */
    LA_E_TRIAL_ACTIVATION_LIMIT = 62,

    /*
        CODE: LA_E_MACHINE_FINGERPRINT

        MESSAGE: Machine fingerprint has changed since activation.
    */
    LA_E_MACHINE_FINGERPRINT = 63,

    /*
        CODE: LA_E_METADATA_KEY_LENGTH

        MESSAGE: Metadata key length is more than 256 characters.
    */
    LA_E_METADATA_KEY_LENGTH = 64,

    /*
        CODE: LA_E_METADATA_VALUE_LENGTH

        MESSAGE: Metadata value length is more than 256 characters.
    */
    LA_E_METADATA_VALUE_LENGTH = 65,

    /*
        CODE: LA_E_ACTIVATION_METADATA_LIMIT

        MESSAGE: The license has reached it's metadata fields limit.
    */
    LA_E_ACTIVATION_METADATA_LIMIT = 66,

    /*
        CODE: LA_E_TRIAL_ACTIVATION_METADATA_LIMIT

        MESSAGE: The trial has reached it's metadata fields limit.
    */
    LA_E_TRIAL_ACTIVATION_METADATA_LIMIT = 67,

    /*
        CODE: LA_E_METADATA_KEY_NOT_FOUND

        MESSAGE: The metadata key does not exist.
    */
    LA_E_METADATA_KEY_NOT_FOUND = 68,

    /*
        CODE: LA_E_TIME_MODIFIED

        MESSAGE: The system time has been tampered (backdated).
    */
    LA_E_TIME_MODIFIED = 69,

    /*
        CODE: LA_E_RELEASE_VERSION_FORMAT

        MESSAGE: Invalid version format.
    */
    LA_E_RELEASE_VERSION_FORMAT = 70,

    /*
        CODE: LA_E_AUTHENTICATION_FAILED

        MESSAGE: Incorrect email or password.
    */
    LA_E_AUTHENTICATION_FAILED = 71,

    /*
        CODE: LA_E_METER_ATTRIBUTE_NOT_FOUND

        MESSAGE: The meter attribute does not exist.
    */
    LA_E_METER_ATTRIBUTE_NOT_FOUND = 72,

    /*
        CODE: LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED

        MESSAGE: The meter attribute has reached it's usage limit.
    */
    LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED = 73,

    /*
        CODE: LA_E_CUSTOM_FINGERPRINT_LENGTH

        MESSAGE: Custom device fingerprint length is less than 64 characters
        or more than 256 characters.
    */
    LA_E_CUSTOM_FINGERPRINT_LENGTH = 74,

    /*
        CODE: LA_E_VM

        MESSAGE: Application is being run inside a virtual machine / hypervisor,
        and activation has been disallowed in the VM.
    */
    LA_E_VM = 80,

    /*
        CODE: LA_E_COUNTRY

        MESSAGE: Country is not allowed.
    */
    LA_E_COUNTRY = 81,

    /*
        CODE: LA_E_IP

        MESSAGE: IP address is not allowed.
    */
    LA_E_IP = 82,

    /*
        CODE: LA_E_RATE_LIMIT

        MESSAGE: Rate limit for API has reached, try again later.
    */
    LA_E_RATE_LIMIT = 90,

    /*
        CODE: LA_E_SERVER

        MESSAGE: Server error.
    */
    LA_E_SERVER = 91,

    /*
        CODE: LA_E_CLIENT

        MESSAGE: Client error.
    */
    LA_E_CLIENT = 92
};

#endif // LEX_STATUS_CODES_H
