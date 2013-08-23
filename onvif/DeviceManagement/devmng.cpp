#include "stdafx.h"
#ifdef LINUX_NAS
#include <string>
using namespace std;

#include "devmng.h"
#include "gsoap/soapH.h"
#include "gsoap/DeviceBinding.nsmap"
#include "gsoap/smdevp.h"
#include "gsoap/wsseapi.h"

/************************************************************************************/
/*  Function Name	:	getMediaUrl													*/
/*  Function Desc	:	get media service url										*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
bool getMediaUrl(char* xAddress, char* userId, char* password, char* mediaUrl)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(mediaUrl, "%s", xAddress);

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetCapabilities getCapabilitiesRequest;
	struct _ns1__GetCapabilitiesResponse getCapabilitiesResponse;

	getCapabilitiesRequest.__sizeCategory = 1;

	enum ns2__CapabilityCategory category;
	category = ns2__CapabilityCategory__All;
	getCapabilitiesRequest.Category = &category;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	soap_call___ns1__GetCapabilities(soap, xAddress, "", &getCapabilitiesRequest, &getCapabilitiesResponse);

	if (soap->error)
	{ 
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetCapabilities(soap, xAddress, "", &getCapabilitiesRequest, &getCapabilitiesResponse);
		if (soap->error)
		{
			soap_destroy(soap);
			soap_end(soap);
			soap_done(soap);
			delete soap;
			return false;
		}

		authFlag = 0;			
	}

	if (getCapabilitiesResponse.Capabilities == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	if (getCapabilitiesResponse.Capabilities->Media != NULL && getCapabilitiesResponse.Capabilities->Media->XAddr != NULL)
	{
		sprintf(mediaUrl, "%s", getCapabilitiesResponse.Capabilities->Media->XAddr);
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool getDeviceInfo(char* xAddress, char* userId, char* password, char* make, char* model, char* firmware)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(make, "%s", "");
	sprintf(model, "%s", "");
	sprintf(firmware, "%s", "");

	struct soap* soap = new struct soap;

	char serverUri[100];

	sprintf(serverUri, "%s", xAddress);

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetDeviceInformation request;
	struct _ns1__GetDeviceInformationResponse result;

	soap_call___ns1__GetDeviceInformation(soap, serverUri, "", &request, &result);

	if (soap->error)
	{ 
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetDeviceInformation(soap, serverUri, "", &request, &result);
		if (soap->error)
		{
			soap_destroy(soap);
			soap_end(soap);
			soap_done(soap);
			delete soap;
			return false;
		}

		authFlag = 0;
	}

	if (result.Manufacturer != NULL)
		sprintf(make, "%s", result.Manufacturer);
	if (result.Model != NULL)
		sprintf(model, "%s", result.Model);
	if (result.FirmwareVersion != NULL)
		sprintf(firmware, "%s", result.FirmwareVersion);

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;

	return true;
}

#endif
