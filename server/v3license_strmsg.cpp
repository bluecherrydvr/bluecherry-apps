#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "LexStatusCodes.h"
static const char* _get_status_messge (int err, char *buf, size_t len)
{
	const char *p;

	if(!len)
		return NULL;

	*buf = '\0';

	switch(err) {
		case LA_OK:
			p = "Success code";
			break;
		case LA_FAIL:
			p = "Failure code";
			break;
		case LA_EXPIRED:
			p = "The license has expired or system time has been tampered with. Ensure your date and time settings are correct.";
			break;
		case LA_SUSPENDED:
			p = "The license has been suspended";
			break;
		case LA_GRACE_PERIOD_OVER:
			p = "The grace period for server sync is over.";
			break;
		case LA_TRIAL_EXPIRED:
			p = "The local trial has expired or system time has been tampered with. Ensure your date and time settings are correct.";
			break;
		case LA_LOCAL_TRIAL_EXPIRED:
			p = "he local trial has expired or system time has been tampered with. Ensure your date and time settings are correct.";
			break;
		case LA_RELEASE_UPDATE_AVAILABLE:
			p = "A new update is available for the product. This means a new release has been published for the product.";
			break;
		case LA_RELEASE_NO_UPDATE_AVAILABLE:
			p = "No new update is available for the product. The current version is latest.";
			break;
		case LA_E_FILE_PATH:
			p = "Invalid file path.";
			break;
		case LA_E_PRODUCT_FILE:
			p = "Invalid or corrupted product file.";
			break;
		case LA_E_PRODUCT_DATA:
			p = "Invalid product data.";
			break;
		case LA_E_PRODUCT_ID:
			p = "The product id is incorrect.";
			break;
		case LA_E_SYSTEM_PERMISSION:
			p = "Insufficient system permissions. Occurs when LA_SYSTEM flag is used but application is not run with admin privileges.";
			break;
		case LA_E_FILE_PERMISSION:
			p = "No permission to write to file.";
			break;
		case LA_E_WMIC:
			p = "Fingerprint couldn't be generated.";
			break;
		case LA_E_TIME:
			p = "The difference between the network time and the system time is more than allowed clock offset.";
			break;
		case LA_E_INET:
			p = "Failed to connect to the server due to network error.";
			break;
		case LA_E_NET_PROXY:
			p = "Invalid network proxy.";
			break;
		case LA_E_HOST_URL:
			p = "Invalid Cryptlex host url.";
			break;
		case LA_E_BUFFER_SIZE:
			p = "The buffer size was smaller than required.";
			break;
		case LA_E_APP_VERSION_LENGTH:
			p = "App version length is more than 256 characters.";
			break;
		case LA_E_REVOKED:
			p = "The license has been revoked.";
			break;
		case LA_E_LICENSE_KEY:
			p = "Invalid license key.";
			break;
		case LA_E_LICENSE_TYPE:
			p = "Invalid license type. Make sure floating license is not being used.";
			break;
		case LA_E_OFFLINE_RESPONSE_FILE:
			p = "Invalid offline activation response file.";
			break;
		case LA_E_OFFLINE_RESPONSE_FILE_EXPIRED:
			p = "The offline activation response has expired.";
			break;
		case LA_E_ACTIVATION_LIMIT:
			p = "The license has reached it's allowed activations limit.";
			break;
		case LA_E_ACTIVATION_NOT_FOUND:
			p = "The license activation was deleted on the server.";
			break;
		case LA_E_DEACTIVATION_LIMIT:
			p = "The license has reached it's allowed deactivations limit.";
			break;
		case LA_E_TRIAL_NOT_ALLOWED:
			p = "Trial not allowed for the product.";
			break;
		case LA_E_TRIAL_ACTIVATION_LIMIT:
			p = "Your account has reached it's trial activations limit.";
			break;
		case LA_E_MACHINE_FINGERPRINT:
			p = "Machine fingerprint has changed since activation.";
			break;
		case LA_E_METADATA_KEY_LENGTH:
			p = "Metadata key length is more than 256 characters.";
			break;
		case LA_E_METADATA_VALUE_LENGTH:
			p = "Metadata value length is more than 256 characters.";
			break;
		case LA_E_ACTIVATION_METADATA_LIMIT:
			p = "The license has reached it's metadata fields limit";
			break;
		case LA_E_TRIAL_ACTIVATION_METADATA_LIMIT:
			p = "The trial has reached it's metadata fields limit.";
			break;
		case LA_E_METADATA_KEY_NOT_FOUND:
			p = "The metadata key does not exist.";
			break;
		case LA_E_TIME_MODIFIED:
			p = "The system time has been tampered (backdated).";
			break;
		case LA_E_RELEASE_VERSION_FORMAT:
			p = "Invalid version format.";
			break;
		case LA_E_AUTHENTICATION_FAILED:
			p = "Incorrect email or password.";
			break;
		case LA_E_METER_ATTRIBUTE_NOT_FOUND:
			p = "The meter attribute does not exist.";
			break;
		case LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED:
			p = "The meter attribute has reached it's usage limit.";
			break;
		case LA_E_CUSTOM_FINGERPRINT_LENGTH:
			p = "Custom device fingerprint length is less than 64 characters or more than 256 characters.";
			break;
		case LA_E_VM:
			p = "Application is being run inside a virtual machine / hypervisor, and activation has been disallowed in the VM.";
			break;
		case LA_E_COUNTRY:
			p = "Country is not allowed.";
			break;
		case LA_E_IP:
			p = "IP address is not allowed.";
			break;
		case LA_E_RATE_LIMIT:
			p = "Rate limit for API has reached, try again later.";
			break;
		case LA_E_SERVER:
			p = "Server error.";
			break;
		case LA_E_CLIENT:
			p = "Client error.";
			break;
		default:
			return NULL;
	}
	strncpy(buf, p, len);
	buf [len-1] = '\0';
	return buf;
}


const char* v3license_strmsg (int err, char *buf, size_t buflen)
{
	size_t max;

	if(!buflen)
		return NULL;

	max = buflen - 1;
	*buf = '\0';
	if (!_get_status_messge(err, buf, max)) 
	{
		snprintf(buf, max, "Unknown error %d (%#x)", err, err);
	}
}

