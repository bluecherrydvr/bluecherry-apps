/* LexActivator.h */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include "LexStatusCodes.h"
#ifdef _WIN32
    /*
    Make sure you're using the MSVC or Intel compilers on Windows.
    */
    #include <windows.h>

    #ifdef LEXACTIVATOR_EXPORTS
        #ifdef LEXACTIVATOR_STATIC
            #define LEXACTIVATOR_API extern "C"
        #else
            #define LEXACTIVATOR_API extern "C" __declspec(dllexport)
        #endif
    #else
        #ifdef __cplusplus
            #ifdef LEXACTIVATOR_STATIC
                #define LEXACTIVATOR_API extern "C"
            #else
                #define LEXACTIVATOR_API extern "C" __declspec(dllimport)
            #endif
        #else
            #ifdef LEXACTIVATOR_STATIC
                #define LEXACTIVATOR_API
            #else
                #define LEXACTIVATOR_API __declspec(dllimport)
            #endif
        #endif
    #endif

    #if defined(USE_STDCALL_DLL) && !defined(LEXACTIVATOR_STATIC)
        #define LA_CC __stdcall
    #else
        #define LA_CC __cdecl
    #endif
    typedef const wchar_t* CSTRTYPE;
    typedef wchar_t* STRTYPE;
#else
    #define LA_CC
    #if __GNUC__ >= 4
        #ifdef __cplusplus
            #define LEXACTIVATOR_API extern "C" __attribute__((visibility("default")))
        #else
            #define LEXACTIVATOR_API __attribute__((visibility("default")))
        #endif
    #else
        #ifdef __cplusplus
            #define LEXACTIVATOR_API extern "C"
        #else
            #define LEXACTIVATOR_API
        #endif
    #endif
    typedef const char* CSTRTYPE;
    typedef char* STRTYPE;
#endif

typedef void (LA_CC *CallbackType)(uint32_t);

#define LA_USER ((uint32_t)1)
#define LA_SYSTEM ((uint32_t)2)
#define LA_IN_MEMORY ((uint32_t)4)

/*
    FUNCTION: SetProductFile()

    PURPOSE: Sets the absolute path of the Product.dat file.

    This function must be called on every start of your program
    before any other functions are called.

    PARAMETERS:
    * filePath - absolute path of the product file (Product.dat)

    RETURN CODES: LA_OK, LA_E_FILE_PATH, LA_E_PRODUCT_FILE

    NOTE: If this function fails to set the path of product file, none of the
    other functions will work.
*/
LEXACTIVATOR_API int LA_CC SetProductFile(CSTRTYPE filePath);

/*
    FUNCTION: SetProductData()

    PURPOSE: Embeds the Product.dat file in the application.

    It can be used instead of SetProductFile() in case you want
    to embed the Product.dat file in your application.

    This function must be called on every start of your program
    before any other functions are called.

    PARAMETERS:
    * productData - content of the Product.dat file

    RETURN CODES: LA_OK, LA_E_PRODUCT_DATA

    NOTE: If this function fails to set the product data, none of the
    other functions will work.
*/
LEXACTIVATOR_API int LA_CC SetProductData(CSTRTYPE productData);

/*
    FUNCTION: SetProductId()

    PURPOSE: Sets the product id of your application.

    This function must be called on every start of your program before
    any other functions are called, with the exception of SetProductFile()
    or SetProductData() function.

    PARAMETERS:
    * productId - the unique product id of your application as mentioned
      on the product page in the dashboard.

    * flags - depending upon whether your application requires admin/root
      permissions to run or not, this parameter can have one of the following
      values: LA_SYSTEM, LA_USER, LA_IN_MEMORY

    RETURN CODES: LA_OK, LA_E_WMIC, LA_E_PRODUCT_FILE, LA_E_PRODUCT_DATA, LA_E_PRODUCT_ID,
    LA_E_SYSTEM_PERMISSION

    NOTE: If this function fails to set the product id, none of the other
    functions will work.
*/
LEXACTIVATOR_API int LA_CC SetProductId(CSTRTYPE productId, uint32_t flags);

/*
    FUNCTION: SetCustomDeviceFingerprint()

    PURPOSE: In case you don't want to use the LexActivator's advanced
    device fingerprinting algorithm, this function can be used to set a custom
    device fingerprint.

    If you decide to use your own custom device fingerprint then this function must be
    called on every start of your program immediately after calling SetProductFile()
    or SetProductData() function.

    The license fingerprint matching strategy is ignored if this function is used.

    PARAMETERS:
    * fingerprint - string of minimum length 64 characters and maximum length 256 characters.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_CUSTOM_FINGERPRINT_LENGTH
*/
LEXACTIVATOR_API int LA_CC SetCustomDeviceFingerprint(CSTRTYPE fingerprint);

/*
    FUNCTION: SetLicenseKey()

    PURPOSE: Sets the license key required to activate the license.

    PARAMETERS:
    * licenseKey - a valid license key.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY
*/
LEXACTIVATOR_API int LA_CC SetLicenseKey(CSTRTYPE licenseKey);

/*
    FUNCTION: SetLicenseUserCredential()

    PURPOSE: Sets the license user email and password for authentication.

    This function must be called before ActivateLicense() or IsLicenseGenuine()
    function if 'requireAuthentication' property of the license is set to true.

    PARAMETERS:
    * email - user email address.
    * password - user password.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY
*/
LEXACTIVATOR_API int LA_CC SetLicenseUserCredential(CSTRTYPE email, CSTRTYPE password);

/*
    FUNCTION: SetLicenseCallback()

    PURPOSE: Sets server sync callback function.

    Whenever the server sync occurs in a separate thread, and server returns the response,
    license callback function gets invoked with the following status codes:
    LA_OK, LA_EXPIRED, LA_SUSPENDED,
    LA_E_REVOKED, LA_E_ACTIVATION_NOT_FOUND, LA_E_MACHINE_FINGERPRINT
    LA_E_AUTHENTICATION_FAILED, LA_E_COUNTRY, LA_E_INET, LA_E_SERVER,
    LA_E_RATE_LIMIT, LA_E_IP

    PARAMETERS:
    * callback - name of the callback function

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY
*/
LEXACTIVATOR_API int LA_CC SetLicenseCallback(CallbackType callback);
/*
    FUNCTION: SetActivationMetadata()

    PURPOSE: Sets the activation metadata.

    The  metadata appears along with the activation details of the license
    in dashboard.

    PARAMETERS:
    * key - string of maximum length 256 characters.
    * value - string of maximum length 256 characters.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_METADATA_KEY_LENGTH,
    LA_E_METADATA_VALUE_LENGTH, LA_E_ACTIVATION_METADATA_LIMIT
*/
LEXACTIVATOR_API int LA_CC SetActivationMetadata(CSTRTYPE key, CSTRTYPE value);

/*
    FUNCTION: SetTrialActivationMetadata()

    PURPOSE: Sets the trial activation metadata.

    The  metadata appears along with the trial activation details of the product
    in dashboard.

    PARAMETERS:
    * key - string of maximum length 256 characters.
    * value - string of maximum length 256 characters.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_LENGTH,
    LA_E_METADATA_VALUE_LENGTH, LA_E_TRIAL_ACTIVATION_METADATA_LIMIT
*/
LEXACTIVATOR_API int LA_CC SetTrialActivationMetadata(CSTRTYPE key, CSTRTYPE value);

/*
    FUNCTION: SetAppVersion()

    PURPOSE: Sets the current app version of your application.

    The app version appears along with the activation details in dashboard. It
    is also used to generate app analytics.

    PARAMETERS:
    * appVersion - string of maximum length 256 characters.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_APP_VERSION_LENGTH
*/
LEXACTIVATOR_API int LA_CC SetAppVersion(CSTRTYPE appVersion);

/*
    FUNCTION: SetOfflineActivationRequestMeterAttributeUses()

    PURPOSE: Sets the meter attribute uses for the offline activation request.

    This function should only be called before GenerateOfflineActivationRequest()
    function to set the meter attributes in case of offline activation.

    PARAMETERS:
    * name - name of the meter attribute
    * uses - the uses value

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY

*/
LEXACTIVATOR_API int LA_CC SetOfflineActivationRequestMeterAttributeUses(CSTRTYPE name, uint32_t uses);

/*
    FUNCTION: SetNetworkProxy()

    PURPOSE: Sets the network proxy to be used when contacting Cryptlex servers.

    The proxy format should be: [protocol://][username:password@]machine[:port]

    Following are some examples of the valid proxy strings:
        - http://127.0.0.1:8000/
        - http://user:pass@127.0.0.1:8000/
        - socks5://127.0.0.1:8000/

    PARAMETERS:
    * proxy - proxy string having correct proxy format

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_NET_PROXY

    NOTE: Proxy settings of the computer are automatically detected. So, in most of the
    cases you don't need to care whether your user is behind a proxy server or not.
*/
LEXACTIVATOR_API int LA_CC SetNetworkProxy(CSTRTYPE proxy);

/*
    FUNCTION: SetCryptlexHost()

    PURPOSE: In case you are running Cryptlex on-premise, you can set the
    host for your on-premise server.

    PARAMETERS:
    * host - the address of the Cryptlex on-premise server

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_HOST_URL
*/
LEXACTIVATOR_API int LA_CC SetCryptlexHost(CSTRTYPE host);

/*
    FUNCTION: GetProductMetadata()

    PURPOSE: Gets the product metadata as set in the dashboard.

    This is available for trial as well as license activations.

    PARAMETERS:
    * key - key to retrieve the value
    * value - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the value parameter

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_NOT_FOUND, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetProductMetadata(CSTRTYPE key, STRTYPE value, uint32_t length);

/*
    FUNCTION: GetLicenseMetadata()

    PURPOSE: Gets the license metadata as set in the dashboard.

    PARAMETERS:
    * key - key to retrieve the value
    * value - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the value parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_NOT_FOUND, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseMetadata(CSTRTYPE key, STRTYPE value, uint32_t length);

/*
    FUNCTION: GetLicenseMeterAttribute()

    PURPOSE: Gets the license meter attribute allowed, total and gross uses.

    PARAMETERS:
    * name - name of the meter attribute
    * allowedUses - pointer to the integer that receives the value
    * totalUses - pointer to the integer that receives the value
    * grossUses - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METER_ATTRIBUTE_NOT_FOUND
*/
LEXACTIVATOR_API int LA_CC GetLicenseMeterAttribute(CSTRTYPE name, uint32_t *allowedUses, uint32_t *totalUses, uint32_t *grossUses = NULL);

/*
    FUNCTION: GetLicenseKey()

    PURPOSE: Gets the license key used for activation.

    PARAMETERS:
    * licenseKey - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the licenseKey parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseKey(STRTYPE licenseKey, uint32_t length);

/*
    FUNCTION: GetLicenseAllowedActivations()

    PURPOSE: Gets the allowed activations of the license.

    PARAMETERS:
    * allowedActivations - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetLicenseAllowedActivations(uint32_t *allowedActivations);

/*
    FUNCTION: GetLicenseTotalActivations()

    PURPOSE: Gets the total activations of the license.

    PARAMETERS:
    * totalActivations - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetLicenseTotalActivations(uint32_t *totalActivations);

/*
    FUNCTION: GetLicenseExpiryDate()

    PURPOSE: Gets the license expiry date timestamp.

    PARAMETERS:
    * expiryDate - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetLicenseExpiryDate(uint32_t *expiryDate);

/*
    FUNCTION: GetLicenseUserEmail()

    PURPOSE: Gets the email associated with license user.

    PARAMETERS:
    * email - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the email parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED,
    LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseUserEmail(STRTYPE email, uint32_t length);

/*
    FUNCTION: GetLicenseUserName()

    PURPOSE: Gets the name associated with the license user.

    PARAMETERS:
    * name - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the name parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED,
    LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseUserName(STRTYPE name, uint32_t length);

/*
    FUNCTION: GetLicenseUserCompany()

    PURPOSE: Gets the company associated with the license user.

    PARAMETERS:
    * company - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the company parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED,
    LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseUserCompany(STRTYPE company, uint32_t length);

/*
    FUNCTION: GetLicenseUserMetadata()

    PURPOSE: Gets the metadata associated with the license user.

    PARAMETERS:
    * key - key to retrieve the value
    * value - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the value parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_NOT_FOUND, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseUserMetadata(CSTRTYPE key, STRTYPE value, uint32_t length);

/*
    FUNCTION: GetLicenseType()

    PURPOSE: Gets the license type (node-locked or hosted-floating).

    PARAMETERS:
    * licenseType - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the licenseType parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED,
    LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLicenseType(STRTYPE licenseType, uint32_t length);

/*
    FUNCTION: GetActivationMetadata()

    PURPOSE: Gets the activation metadata.

    PARAMETERS:
    * key - key to retrieve the value
    * value - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the value parameter

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_NOT_FOUND, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetActivationMetadata(CSTRTYPE key, STRTYPE value, uint32_t length);

/*
    FUNCTION: GetActivationMeterAttributeUses()

    PURPOSE: Gets the meter attribute uses consumed by the activation.

    PARAMETERS:
    * name - name of the meter attribute
    * allowedUses - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METER_ATTRIBUTE_NOT_FOUND
*/
LEXACTIVATOR_API int LA_CC GetActivationMeterAttributeUses(CSTRTYPE name, uint32_t *uses);

/*
    FUNCTION: GetServerSyncGracePeriodExpiryDate()

    PURPOSE: Gets the server sync grace period expiry date timestamp.

    PARAMETERS:
    * expiryDate - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetServerSyncGracePeriodExpiryDate(uint32_t *expiryDate);

/*
    FUNCTION: GetTrialActivationMetadata()

    PURPOSE: Gets the trial activation metadata.

    PARAMETERS:
    * key - key to retrieve the value
    * value - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the value parameter

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_METADATA_KEY_NOT_FOUND, LA_E_BUFFER_SIZE
*/

LEXACTIVATOR_API int LA_CC GetTrialActivationMetadata(CSTRTYPE key, STRTYPE value, uint32_t length);

/*
    FUNCTION: GetTrialExpiryDate()

    PURPOSE: Gets the trial expiry date timestamp.

    PARAMETERS:
    * trialExpiryDate - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetTrialExpiryDate(uint32_t *trialExpiryDate);

/*
    FUNCTION: GetTrialId()

    PURPOSE: Gets the trial activation id. Used in case of trial extension.

    PARAMETERS:
    * trialId - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the trialId parameter

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME, LA_E_TIME_MODIFIED,
    LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetTrialId(STRTYPE trialId, uint32_t length);

/*
    FUNCTION: GetLocalTrialExpiryDate()

    PURPOSE: Gets the trial expiry date timestamp.

    PARAMETERS:
    * trialExpiryDate - pointer to the integer that receives the value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GetLocalTrialExpiryDate(uint32_t *trialExpiryDate);

/*
    FUNCTION: GetLibraryVersion()

    PURPOSE: Gets the version of this library.

    PARAMETERS:
    * libraryVersion - pointer to a buffer that receives the value of the string
    * length - size of the buffer pointed to by the libraryVersion parameter

    RETURN CODES: LA_OK, LA_E_BUFFER_SIZE
*/
LEXACTIVATOR_API int LA_CC GetLibraryVersion(STRTYPE libraryVersion, uint32_t length);

/*
    FUNCTION: CheckForReleaseUpdate()

    PURPOSE: Checks whether a new release is available for the product.

    This function should only be used if you manage your releases through
    Cryptlex release management API.

    PARAMETERS:
    * platform - release platform e.g. windows, macos, linux
    * version - current release version
    * channel - release channel e.g. stable
    * releaseUpdateCallback - name of the callback function.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_RELEASE_VERSION_FORMAT
*/
LEXACTIVATOR_API int LA_CC CheckForReleaseUpdate(CSTRTYPE platform, CSTRTYPE version, CSTRTYPE channel, CallbackType releaseUpdateCallback);

/*
    FUNCTION: ActivateLicense()

    PURPOSE: Activates the license by contacting the Cryptlex servers. It
    validates the key and returns with encrypted and digitally signed token
    which it stores and uses to activate your application.

    This function should be executed at the time of registration, ideally on
    a button click.

    RETURN CODES: LA_OK, LA_EXPIRED, LA_SUSPENDED, LA_E_REVOKED, LA_FAIL, LA_E_PRODUCT_ID,
    LA_E_INET, LA_E_VM, LA_E_TIME, LA_E_ACTIVATION_LIMIT, LA_E_SERVER, LA_E_CLIENT,
    LA_E_AUTHENTICATION_FAILED, LA_E_LICENSE_TYPE, LA_E_COUNTRY, LA_E_IP, LA_E_RATE_LIMIT, LA_E_LICENSE_KEY
*/
LEXACTIVATOR_API int LA_CC ActivateLicense();

/*
    FUNCTION: ActivateLicenseOffline()

    PURPOSE: Activates your licenses using the offline activation response file.

    PARAMETERS:
    * filePath - path of the offline activation response file.

    RETURN CODES: LA_OK, LA_EXPIRED, LA_FAIL, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_OFFLINE_RESPONSE_FILE
    LA_E_VM, LA_E_TIME, LA_E_FILE_PATH, LA_E_OFFLINE_RESPONSE_FILE_EXPIRED
*/
LEXACTIVATOR_API int LA_CC ActivateLicenseOffline(CSTRTYPE filePath);

/*
    FUNCTION: GenerateOfflineActivationRequest()

    PURPOSE: Generates the offline activation request needed for generating
    offline activation response in the dashboard.

    PARAMETERS:
    * filePath - path of the file for the offline request.

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_FILE_PERMISSION
*/
LEXACTIVATOR_API int LA_CC GenerateOfflineActivationRequest(CSTRTYPE filePath);

/*
    FUNCTION: DeactivateLicense()

    PURPOSE: Deactivates the license activation and frees up the corresponding activation
    slot by contacting the Cryptlex servers.

    This function should be executed at the time of de-registration, ideally on
    a button click.

    RETURN CODES: LA_OK, LA_E_DEACTIVATION_LIMIT, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME
    LA_E_LICENSE_KEY, LA_E_INET, LA_E_SERVER, LA_E_RATE_LIMIT, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC DeactivateLicense();

/*
    FUNCTION: GenerateOfflineDeactivationRequest()

    PURPOSE: Generates the offline deactivation request needed for deactivation of
    the license in the dashboard and deactivates the license locally.

    A valid offline deactivation file confirms that the license has been successfully
    deactivated on the user's machine.

    PARAMETERS:
    * filePath - path of the file for the offline request.

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_FILE_PERMISSION,
    LA_E_TIME, LA_E_TIME_MODIFIED
*/
LEXACTIVATOR_API int LA_CC GenerateOfflineDeactivationRequest(CSTRTYPE filePath);

/*
    FUNCTION: IsLicenseGenuine()

    PURPOSE: It verifies whether your app is genuinely activated or not. The verification is
    done locally by verifying the cryptographic digital signature fetched at the time of
    activation.

    After verifying locally, it schedules a server check in a separate thread. After the
    first server sync it periodically does further syncs at a frequency set for the license.

    In case server sync fails due to network error, and it continues to fail for fixed
    number of days (grace period), the function returns LA_GRACE_PERIOD_OVER instead of LA_OK.

    This function must be called on every start of your program to verify the activation
    of your app.

    RETURN CODES: LA_OK, LA_EXPIRED, LA_SUSPENDED, LA_GRACE_PERIOD_OVER, LA_FAIL,
    LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_TIME, LA_E_TIME_MODIFIED

    NOTE: If application was activated offline using ActivateLicenseOffline() function, you
    may want to set grace period to 0 to ignore grace period.
*/
LEXACTIVATOR_API int LA_CC IsLicenseGenuine();

/*
    FUNCTION: IsLicenseValid()

    PURPOSE: It verifies whether your app is genuinely activated or not. The verification is
    done locally by verifying the cryptographic digital signature fetched at the time of
    activation.

    This is just an auxiliary function which you may use in some specific cases, when you
    want to skip the server sync.

    RETURN CODES: LA_OK, LA_EXPIRED, LA_SUSPENDED, LA_GRACE_PERIOD_OVER, LA_FAIL,
    LA_E_PRODUCT_ID, LA_E_LICENSE_KEY, LA_E_TIME, LA_E_TIME_MODIFIED

    NOTE: You may want to set grace period to 0 to ignore grace period.
*/
LEXACTIVATOR_API int LA_CC IsLicenseValid();

/*
    FUNCTION: ActivateTrial()

    PURPOSE: Starts the verified trial in your application by contacting the
    Cryptlex servers.

    This function should be executed when your application starts first time on
    the user's computer, ideally on a button click.

    RETURN CODES: LA_OK, LA_TRIAL_EXPIRED, LA_FAIL, LA_E_PRODUCT_ID, LA_E_INET,
    LA_E_VM, LA_E_TIME, LA_E_SERVER, LA_E_CLIENT, LA_E_COUNTRY, LA_E_IP, LA_E_RATE_LIMIT
*/
LEXACTIVATOR_API int LA_CC ActivateTrial();

/*
    FUNCTION: ActivateTrialOffline()

    PURPOSE: Activates your trial using the offline activation response file.

    PARAMETERS:
    * filePath - path of the offline activation response file.

    RETURN CODES: LA_OK, LA_TRIAL_EXPIRED, LA_FAIL, LA_E_PRODUCT_ID, LA_E_OFFLINE_RESPONSE_FILE
    LA_E_VM, LA_E_TIME, LA_E_FILE_PATH, LA_E_OFFLINE_RESPONSE_FILE_EXPIRED
*/
LEXACTIVATOR_API int LA_CC ActivateTrialOffline(CSTRTYPE filePath);

/*
    FUNCTION: GenerateOfflineTrialActivationRequest()

    PURPOSE: Generates the offline trial activation request needed for generating
    offline trial activation response in the dashboard.

    PARAMETERS:
    * filePath - path of the file for the offline request.

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_FILE_PERMISSION
*/
LEXACTIVATOR_API int LA_CC GenerateOfflineTrialActivationRequest(CSTRTYPE filePath);

/*
    FUNCTION: IsTrialGenuine()

    PURPOSE: It verifies whether trial has started and is genuine or not. The
    verification is done locally by verifying the cryptographic digital signature
    fetched at the time of trial activation.

    This function must be called on every start of your program during the trial period.

    RETURN CODES: LA_OK, LA_TRIAL_EXPIRED, LA_FAIL, LA_E_TIME, LA_E_PRODUCT_ID, LA_E_TIME_MODIFIED

*/
LEXACTIVATOR_API int LA_CC IsTrialGenuine();

/*
    FUNCTION: ActivateLocalTrial()

    PURPOSE: Starts the local(unverified) trial.

    This function should be executed when your application starts first time on
    the user's computer.

    PARAMETERS:
    * trialLength - trial length in days

    RETURN CODES: LA_OK, LA_LOCAL_TRIAL_EXPIRED, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME_MODIFIED

    NOTE: The function is only meant for local(unverified) trials.
*/
LEXACTIVATOR_API int LA_CC ActivateLocalTrial(uint32_t trialLength);

/*
    FUNCTION: IsLocalTrialGenuine()

    PURPOSE: It verifies whether trial has started and is genuine or not. The
    verification is done locally.

    This function must be called on every start of your program during the trial period.

    RETURN CODES: LA_OK, LA_LOCAL_TRIAL_EXPIRED, LA_FAIL, LA_E_PRODUCT_ID,
    LA_E_TIME_MODIFIED

    NOTE: The function is only meant for local(unverified) trials.
*/
LEXACTIVATOR_API int LA_CC IsLocalTrialGenuine();

/*
    FUNCTION: ExtendLocalTrial()

    PURPOSE: Extends the local trial.

    PARAMETERS:
    * trialExtensionLength - number of days to extend the trial

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_TIME_MODIFIED

    NOTE: The function is only meant for local(unverified) trials.
*/
LEXACTIVATOR_API int LA_CC ExtendLocalTrial(uint32_t trialExtensionLength);

/*
    FUNCTION: IncrementActivationMeterAttributeUses()

    PURPOSE: Increments the meter attribute uses of the activation.

    PARAMETERS:
    * name - name of the meter attribute
    * increment - the increment value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METER_ATTRIBUTE_NOT_FOUND,
    LA_E_INET, LA_E_TIME, LA_E_SERVER, LA_E_CLIENT, LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED,
    LA_E_AUTHENTICATION_FAILED, LA_E_COUNTRY, LA_E_IP, LA_E_RATE_LIMIT, LA_E_LICENSE_KEY

*/
LEXACTIVATOR_API int LA_CC IncrementActivationMeterAttributeUses(CSTRTYPE name, uint32_t increment);

/*
    FUNCTION: DecrementActivationMeterAttributeUses()

    PURPOSE: Decrements the meter attribute uses of the activation.

    PARAMETERS:
    * name - name of the meter attribute
    * decrement - the decrement value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METER_ATTRIBUTE_NOT_FOUND,
    LA_E_INET, LA_E_TIME, LA_E_SERVER, LA_E_CLIENT, LA_E_RATE_LIMIT, LA_E_LICENSE_KEY,
    LA_E_AUTHENTICATION_FAILED, LA_E_COUNTRY, LA_E_IP, LA_E_ACTIVATION_NOT_FOUND

    NOTE: If the decrement is more than the current uses, it resets the uses to 0.
*/
LEXACTIVATOR_API int LA_CC DecrementActivationMeterAttributeUses(CSTRTYPE name, uint32_t decrement);

/*
    FUNCTION: ResetActivationMeterAttributeUses()

    PURPOSE: Resets the meter attribute uses consumed by the activation.

    PARAMETERS:
    * name - name of the meter attribute
    * decrement - the decrement value

    RETURN CODES: LA_OK, LA_FAIL, LA_E_PRODUCT_ID, LA_E_METER_ATTRIBUTE_NOT_FOUND,
    LA_E_INET, LA_E_TIME, LA_E_SERVER, LA_E_CLIENT, LA_E_RATE_LIMIT, LA_E_LICENSE_KEY,
    LA_E_AUTHENTICATION_FAILED, LA_E_COUNTRY, LA_E_IP, LA_E_ACTIVATION_NOT_FOUND
*/
LEXACTIVATOR_API int LA_CC ResetActivationMeterAttributeUses(CSTRTYPE name);

/*
    FUNCTION: Reset()

    PURPOSE: Resets the activation and trial data stored in the machine.

    This function is meant for developer testing only.

    RETURN CODES: LA_OK, LA_E_PRODUCT_ID

    NOTE: The function does not reset local(unverified) trial data.
*/
LEXACTIVATOR_API int LA_CC Reset();
