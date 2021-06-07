#ifndef V3LICENSE_PROCESSOR_H
#define V3LICENSE_PROCESSOR_H

#include <time.h>
#include "LexActivator.h"

#define PRODUCT_DATA "MDM1MTY5MzZBN0VDRkQ3OTkxOEE4MEI5Nzk0NjQ0NDA=.Ir5Mm6ni/AAkaG7njv2SCTFATNsdqPcAeWWhj2AIAbKu5Tutp76zHNKMdtsjHCv77Frr9G8M8IuptUzsShaey8BbJnSCRItGvllojohTgWH0/qA3jtm4nnhoVhpeaaTc2cYUeAfuk3mbH9sDPJDkrSE6Oy/EhxQVQVyqK5N/ke3VBXomKfmWAwlVXys+OWi3hKIUynvTPHgMiZgpJJFG1I3iaCCmv7hkBPff+Lfwi4ZARKWDlvSJbXNeWiQLczhH/BVR5g9YPn0QmhcxIKd6AKRjk5GnLr1vlAVVbfPFmuC5SUaSrIEruTzjxZM0INmwLRpA5ND0C4ELgsw7Iez7W82RA+uiM8/CUdaSUxYDhLngv2iDG+inhV5STN9WCs41fpndtDBurIu0CGTk9F5G2zBhMcxzL2vZmwXOoyu4XvDd22p7jhuZnX+Pq0tHmW5/f9Ue+bMvzeiqSJQ7+apjqUDWBe9ZoBh7kM433vOjLb//WUK/HjNRC0KUop+Nq6z7aJRG867Yy56lN2hNa/9nCOFcHkZKBJThSovGbHysUW0g7IzpDrhmXXEAstNWiT4+oasT7xpYBrn2yFvwwmDjiA+Q52b4acs73U33IYocP2YmU0pmJZQfGJbn+4TLoPLCKSL+aqJgvpPPxdMLfqF6f51jAMkxHrmZedribNs+kwsLZY/jImeAfSCW1nv2YrZmVmu4Qt4gFT8HX+VBm+7V4P/NGa1jMUgLX1PpzlpwfC8="

#define LICENSE_KEY "36E898-AC0FD0-46D28E-F4D165-8E9743-07934E"
#define PRODUCT_ID "9c5dcb22-b7a8-4f0c-9deb-c5426bfe6ae6"
#define PRODUCT_VERSION "3.1.0-rc1"

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

