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
        bc_log(Info, "License is genuinely activated");
    }
    else {
        bc_log(Info, "License isn't genuinely activated");
    }

    return status;
}

V3LICENSE_API int bc_license_v3_IsTrialGenuine()
{
    int status = IsTrialGenuine();
    if (status == LA_OK) {
        bc_log(Info, "License is trially activated\n");
    }
    else {
        bc_log(Info, "License isn't trially activated");
    }

    return status;
}

V3LICENSE_API int bc_license_v3_GetLicenseMetadata(STRTYPE value, uint32_t length)
{
    int status = GetLicenseMetadata(METADATA_KEY, value, length);
    if (status == LA_OK) {
        bc_log(Info, "License authorized for %s cameras", value);
    }
    else {
        bc_log(Error, "Getting the number of allowed cameras failed (Error code: %d)\n", status);
    }

    return status;
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

V3LICENSE_API int bc_license_v3_ActivateLicense(const char* licenseKey)
{
    int status;

    bc_log(Error, "Activating license key(%s) ...", licenseKey);
    // status = bc_license_v3_Init();
    // if (LA_OK != status)
    // {
    //     bc_log(Error, "Initializing license failed (Error code: %d)", status);
    //     return status;
    // }

    status = SetLicenseKey(licenseKey);
    if (LA_OK != status)
    {
        bc_log(Error, "Setting license key failed (Error code: %d)", status);
        return status;
    }

    status = ActivateLicense();
    if (LA_OK == status)
    {
        bc_log(Info, "Activating license successed");
        return status;
    }
    else
    {
        bc_log(Error, "Activating license failed (Error code: %d)", status);
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
    int status = ActivateTrial();

    if (LA_OK == status) {
        bc_log(Info, "Activating trial successed");
    }
    else {
        bc_log(Error, "Activating trial failed (Error code: %d)", status);
    }

    return status;
}

V3LICENSE_API int bc_license_v3_DeactivateLicense()
{
    int status = DeactivateLicense();

    if (LA_OK == status) {
        bc_log(Info, "Deactivating license successed");
    }
    else {
        bc_log(Error, "Deactivating license failed (Error code: %d)", status);
    }

    return status;
}

#endif /* V3_LICENSING */
