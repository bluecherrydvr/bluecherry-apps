#ifndef V3LICENSE_PROCESSOR_H
#define V3LICENSE_PROCESSOR_H

#include <time.h>
#include "LexActivator.h"

#define PRODUCT_DATA "QzU4N0M0RTIyODBBM0MwQTRFQUFCOTE4NEZFOTdFQTE=.SzSitGIsOm3pefjCKHT3D/TjeyRhHp+QkMmazr/Vj741Kkyp8VQ/qERM4IniLJBENBLBqs2W83FYhfu5VNyj1PKsovsuiPc85qaPS8bPafH6SL7hVV2Q9OMNJp0nxBqT/eEkGl0zwsqE5Lv9A7j4HyA1BucL/aCiQNJehnKxrZ4EoYcE6n6j9HlTy94DLfTtyCy63UcSqoWEGUOp9acMYzs9FGSDY0pJVtIRSgu5bT7Cule4HLepvu4qV5TJbJrywJ2S6pXKY1vdtOsCgVLHou4y50Q5lp5AQUQu3uaA1I46rippMoXOR/Nv9TAOWmn6LVc5Yy22GspQ2VOhIBfASK1KLAn+roqmWYQOs7d0vzkroRsHv4aaYa2mStvfRqaVXu7H2O5F7XRR4eaRwmQbXY1vmpK8jCtxWfpZaTWP6Mjopdrmle3if8NygASZWZiFw2XIaq55qyDknX9eGZ/sqGfxHHPRh3KDca3Be99amZT0/SiM3fCL6WAAoObE4uoPfGaNJwUczb8x3iQSJ3sS07cfATw5cPTulNt1rp+4QfaPljzwN0T6WBZe2f3c/krpcHUb5NsnyPizCTveLsOUPlz+k+QyPvQaiBMrzMP5X39Zyw8cA9q8WtIBxcWYpmQSBND5rxcdAZwSCUvUCU8UKcTmu8/qmc23j1qX2MUP1qE2LWwcdaqnN6PmZt3xqTZtHB8qzHDvnOqxevPjev/RcsDZFBIS1/ilPJ4dlTPXwYiZC41QLl/aplQ2IDJCtDileRWcH9XkmOPmtU+ItMMLdM8urKg3lffBccJdYQrqY+Y="

#define PRODUCT_ID "9c5dcb22-b7a8-4f0c-9deb-c5426bfe6ae6"
#define PRODUCT_VERSION "3.1.0-rc8"

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

