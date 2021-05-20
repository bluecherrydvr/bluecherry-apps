#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#include "libbluecherry.h"

#ifdef V3_LICENSING
#include "v3license_processor.h"

V3LICENSE_API int bc_license_v3_Init()
{
    int status;

    status = SetProductData(PRODUCT_DATA);
    if (LA_OK != status)
    {
        return LA_FAIL;
    }
    status = SetProductId(PRODUCT_ID, LA_USER);
    if (LA_OK != status)
    {
        return LA_FAIL;
    }

    status = SetAppVersion(PRODUCT_VERSION);
    if (LA_OK != status)
    {
        return LA_FAIL;
    }

    return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsActivated()
{
    int status = IsLicenseGenuine();
    if (LA_OK == status)
    {
        bc_log(Info, "IsLicenseGenuine OK: %d", status);
        return status;
    }
    else if (LA_EXPIRED == status 
            || LA_SUSPENDED == status 
            || LA_GRACE_PERIOD_OVER == status)
    {
        bc_log(Info, "LA_EXPIRED or LA_SUSPENDED or LA_GRACE_PERIOD_OVER: %d", status);
        return status;
    }
    else
    {
        int trialStatus;
        trialStatus = IsTrialGenuine();
        if (LA_OK == trialStatus)
        {
            bc_log(Info, "IsTrialGenuine: %d", trialStatus);
        }
        else
        {
            bc_log(Error, "FAIL License: %d", trialStatus);
        }
        return trialStatus;
    }
    return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsLicenseGenuine()
{
    int status = IsLicenseGenuine();
    if (status == LA_OK) {
        bc_log(Info, "License is genuinely activated!\n");
    }
    else {
        bc_log(Info, "License isn't genuinely activated! (status=%d)\n", status);
    }

    return status;
}

V3LICENSE_API int bc_license_v3_IsTrialGenuine()
{
    return IsTrialGenuine();
}

V3LICENSE_API int bc_license_v3_GetLicenseMetadata(STRTYPE value, uint32_t length)
{
    return GetLicenseMetadata(METADATA_KEY, value, length);
}

V3LICENSE_API int bc_license_v3_GetLicenseExpiryDate(int32_t *isUnlimited, int32_t *expiryDate)
{
    if (!expiryDate || !isUnlimited)
        return LA_FAIL;

    uint32_t date = 0;
    int status = GetLicenseExpiryDate(&date);
    if (status != LA_OK)
        return status;

    int daysLeft = 0;
    if (date == 0) {
        *isUnlimited = 1;
        daysLeft = -1;
        bc_log(Info, "Genuine Days left: unlimited\n");
    }
    else {
        *isUnlimited = 0;
        daysLeft = (date - time(NULL)) / 86400;
        bc_log(Info, "Genuine Days left: %d\n", daysLeft);
    }

    *expiryDate = daysLeft;

    return LA_OK;
}

V3LICENSE_API int bc_license_v3_GetTrialActivationMetadata(STRTYPE value, uint32_t length)
{
    return GetTrialActivationMetadata(METADATA_KEY, value, length);
}

V3LICENSE_API int bc_license_v3_GetTrialExpiryDate(uint32_t *expiryDate)
{
    if (!expiryDate)
        return LA_FAIL;

    unsigned int trialExpiryDate = 0;
    GetTrialExpiryDate(&trialExpiryDate);
    int daysLeft = (trialExpiryDate - time(NULL)) / 86400;
    bc_log(Info, "Trial days left: %d", daysLeft);

    *expiryDate = daysLeft;

    return LA_OK;
}

// Ideally on a button click inside a dialog
int activate()
{
    int status;

    status = SetLicenseKey(LICENSE_KEY);
    if (LA_OK != status)
    {
        bc_log(Error, "V3_LICENSE_SETLICENSEKEY FAIL: %d", status);
        return status;
    }

    status = ActivateLicense();
    if (LA_OK == status 
            || LA_EXPIRED == status 
            || LA_SUSPENDED == status)
    {
        bc_log(Info, "V3_LICENSE_OK: %d", status);
    }
    else
    {
        bc_log(Error, "V3_LICENSE_FAIL: %d", status);
    }
    return status;
}

V3LICENSE_API int bc_license_v3_ActivateLicense(const char* licenseKey)
{
    int status;

    bc_log(Error, "licenseKey: %s", licenseKey);
    status = bc_license_v3_Init();
    if (LA_OK != status)
    {
        bc_log(Error, "V3_LICENSE_INITFAIL: %d", status);
        return status;
    }

    status = SetLicenseKey(licenseKey);
    if (LA_OK != status)
    {
        bc_log(Error, "V3_LICENSE_KEY_SETUP_FAIL: %d", status);
        return status;
    }

    status = ActivateLicense();
    if (LA_OK == status 
            || LA_EXPIRED == status 
            || LA_SUSPENDED == status)
    {
        bc_log(Info, "V3_LICENSE_OK: %d", status);
        return status;
    }
    else
    {
        return status;
    }

    return LA_OK;
}

V3LICENSE_API int bc_license_v3_IsLicenseValid()
{
    return IsLicenseValid();
}

V3LICENSE_API int bc_license_v3_ActivateTrial()
{
    return ActivateTrial();
}

#if 0
int bc_license_v3_init()
{
    int status;

    status = SetProductData(PRODUCT_DATA);
    if (LA_OK != status)
    {
        bc_log(Info, "SetProductData: %s", strerror(status));
        return V3_LICENSE_FAIL;
    }
    status = SetProductId(PRODUCT_ID, LA_USER);
    if (LA_OK != status)
    {
        bc_log(Error, "SetProductId: %s", strerror(status));
        return V3_LICENSE_FAIL;
    }

    status = SetAppVersion(PRODUCT_VERSION);
    if (LA_OK != status)
    {
        bc_log(Error, "SetAppVersion: %s", strerror(status));
        return V3_LICENSE_FAIL;
    }

    return V3_LICENSE_OK;
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
        bc_log(Info, "%s\n", "bc_license_v3_init");
        return V3_LICENSE_FAIL;
    }

    int status = IsLicenseGenuine();
    if (LA_OK == status)
    {
        bc_log(Info, "%s\n", "IsLicenseGenuine");
        return V3_LICENSE_OK;
    }
    else if (LA_EXPIRED == status 
            || LA_SUSPENDED == status 
            || LA_GRACE_PERIOD_OVER == status)
    {
        bc_log(Info, "%s\n", "LA_EXPIRED or LA_SUSPENDED or LA_GRACE_PERIOD_OVER");
        return V3_LICENSE_OK;
    }
    else
    {
        int trialStatus;
        trialStatus = IsTrialGenuine();
        if (LA_OK == trialStatus)
        {
            bc_log(Info, "Trial days left: %s", "IsTrialGenuine");
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
#endif /*  if 0 */

#endif /* V3_LICENSING */
