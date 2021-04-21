#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#include "libbluecherry.h"

#ifdef V3_LICENSING
#include "v3license_processor.h"

V3LICENSE_API int bc_license_v3_Init(CSTRTYPE productData, CSTRTYPE productId, CSTRTYPE productVersion)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsActivated()
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsLicenseGenuine()
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsTrialGenuine()
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsProductKeyValid()
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetProductMetadata(CSTRTYPE key, STRTYPE value, uint32_t length)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetLicenseMetadata(CSTRTYPE key, STRTYPE value, uint32_t length)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetLicenseTotalActivations(uint32_t *totalActivations)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetLicenseExpiryDate(uint32_t *expiryDate)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetTrialActivationMetadata(CSTRTYPE key, STRTYPE value, uint32_t length)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetTrialExpiryDate(uint32_t *trialExpiryDate)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_ActivateLicense(CSTRTYPE productKey)
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsLicenseValid()
{
	return LA_OK;
}

V3LICENSE_API int bc_license_v3_ActivateTrial()
{
	return LA_OK;
}

int bc_license_v3_init()
{
	int status;

	status = SetProductData(PRODUCT_DATA);
	if (LA_OK != status)
	{
		printf("%s\n", "SetProductData");
		return V3_LICENSE_FAIL;
	}
	status = SetProductId(PRODUCT_ID, LA_USER);
	if (LA_OK != status)
	{
		printf("%s error code = %d\n", "SetProductId", status);
		return V3_LICENSE_FAIL;
	}

	status = SetAppVersion(PRODUCT_VERSION);
	if (LA_OK != status)
	{
		printf("%s\n", "PRODUCT_VERSION");
		return V3_LICENSE_FAIL;
	}

	return V3_LICENSE_OK;
}

// Ideally on a button click inside a dialog
int activate()
{
	int status;

	status = SetLicenseKey(PRODCUT_KEY);
	if (LA_OK != status)
	{
		return V3_LICENSE_FAIL;
	}

	status = ActivateLicense();
	if (LA_OK == status 
			|| LA_EXPIRED == status 
			|| LA_SUSPENDED == status)
	{
		printf("%s\n", "V3_LICENSE_OK");
		return V3_LICENSE_OK;
	}
	else
	{
		printf("%s\n", "V3_LICENSE_FAIL");
		return V3_LICENSE_FAIL;
	}
}

// Ideally on a button click inside a dialog
int activateTrial()
{
	int status;

	status = ActivateTrial();
	if (LA_OK == status)
	{
		return V3_LICENSE_OK;
	}
	else
	{
		return V3_LICENSE_FAIL;
	}
}

int bc_license_v3_check()
{
	if (bc_license_v3_init() != LA_OK)
	{
		printf("%s\n", "bc_license_v3_init");
		return V3_LICENSE_FAIL;
	}

	int status = IsLicenseGenuine();
	if (LA_OK == status)
	{
		printf("%s\n", "IsLicenseGenuine");
		return V3_LICENSE_OK;
	}
	else if (LA_EXPIRED == status 
			|| LA_SUSPENDED == status 
			|| LA_GRACE_PERIOD_OVER == status)
	{
		printf("%s\n", "LA_EXPIRED or LA_SUSPENDED or LA_GRACE_PERIOD_OVER");
		return V3_LICENSE_OK;
	}
	else
	{
		int trialStatus;
		trialStatus = IsTrialGenuine();
		if (LA_OK == trialStatus)
		{
			printf("Trial days left: %s", "IsTrialGenuine");
			return V3_LICENSE_OK;
		}
		else if (LA_TRIAL_EXPIRED == trialStatus)
		{
			// Time to buy the license and activate the app
			return activate();
		}
		else
		{
			// Activating the trial
			return activateTrial();
		}
	}
	return V3_LICENSE_OK;
}

#endif /* V3_LICENSING */
