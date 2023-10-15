#ifndef V3LICENSE_PROCESSOR_H
#define V3LICENSE_PROCESSOR_H

#include <time.h>
#include "LexActivator.h"

#define PRODUCT_DATA "MzI1MzZERDFEMTlEQUFFNzk1RTVCQTRENDRBRUU0OEM=.A9VQeDTah2XiwEgZeXjpZ54XoDGV8jf+qibDOuoaNwz/lBm2YExUhBTAG6vNie9AzhMKyPhMO3UbPJuBdOG/tdqnzsXdHwpRDJerHMEJpuAm+f1mjG2CrxKglVm8vx3AYYAbKiCRioQsZZgGNyFEHKni5O9ihvA8gVqKLpQCw1wniBkYzZMQUKsqL4kAidDX1UyXNJ7Dnk5ZpGWhZdqyzvoyrRHpJlh0OYgLRAy0dBFzZ1DwBmxRo1MP156lTHH+gr5IALHnMRTMZg9xeH1gMYS/6GUHMvurue6bLEeTxiGY0KSkAvujIdzHijN9uMdyBTZwgQkGntCBrDTJ02qjZQo+nGIDa5Qucsg7W7Ek9sOpfJqEkPjd0voZdf5FUuQtAYoZG51byAh0PPJSa1zvMi1yKYZwz9IGnrjJ1ruRhyunaNZnMAIzi9xggGBpeRe9x1Q+eGGkclFOUCS/2m1sYVxDWZw8aAH+47jRSi/ywuwr9MLlBM63tK0UJ48QgAQMqdHCn0ueMqL6HkF4r3VTbAi8Ix/GFEnnIdw4bG5MjuJo5mFXzlx1DjMCNeLf3UW3VEEyIeW6EN+5HxetAk7NminwLyFL+UBVtVwhXGQjhOvNk8xEJLqOVaEKOkRQ3ZwroDWzMCSepwhNRMaSDLnJf5cCFqLqZZQB3jznF4OcKQ1rXkm+R6xCcl3uViGNz9zm53HMmruKSTtDFg8+bSSJWOPsAEcCxPBuL0um/FuDmu6uwuT8Iu13zCPdqhkaF6iUCAqO9yxXh5YjZzqvg/EklnsffBKL9mtQXwH0Zu4SggE="

#define PRODUCT_ID "9c5dcb22-b7a8-4f0c-9deb-c5426bfe6ae6"
#define PRODUCT_VERSION "3.1.0-rc9"

#define METADATA_KEY "streams"

#define V3_LICENSE_OK    0
#define V3_LICENSE_FAIL -1

int bc_license_v3_check();
/*
   Checks whether the computer has been activated.
Returns: LA_OK on success. Handle all other return codes as failures.
Possible return codes: LA_OK, TA_FAIL, TA_E_GUID, TA_E_PDETS, TA_E_COM,
TA_E_IN_VM, TA_E_ANDROID_NOT_INIT
*/
#define V3LICENSE_API extern "C"

V3LICENSE_API int bc_license_v3_Init();
V3LICENSE_API int bc_license_v3_IsActivated();
V3LICENSE_API int bc_license_v3_IsLicenseGenuine();
V3LICENSE_API int bc_license_v3_IsTrialGenuine();
V3LICENSE_API int bc_license_v3_GetLicenseMetadata(STRTYPE value, uint32_t length);
V3LICENSE_API int bc_license_v3_GetLicenseExpiryDate(int32_t *isUnlimited, int32_t *expiryDate);
V3LICENSE_API int bc_license_v3_GetTrialActivationMetadata(STRTYPE value, uint32_t length);
V3LICENSE_API int bc_license_v3_GetTrialExpiryDate(uint32_t *trialExpiryDate);
V3LICENSE_API int bc_license_v3_ActivateLicense(CSTRTYPE productKey);
V3LICENSE_API int bc_license_v3_IsLicenseValid();
V3LICENSE_API int bc_license_v3_ActivateTrial();
V3LICENSE_API int bc_license_v3_DeactivateLicense();

#endif /* V3LICENSE_PROCESSOR_H */

