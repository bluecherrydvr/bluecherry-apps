#include "stdafx.h"
#ifdef LINUX_NAS
#include <string>
using namespace std;

#include "devmng.h"
#include "gsoap/soapH.h"
#include "gsoap/DeviceBinding.nsmap"
#include "gsoap/smdevp.h"
#include "gsoap/wsseapi.h"

static char addr[256] = "";
static int cnt = 0;

bool getDeviceInfo(char* xAddress, char* userId, char* password, char* ipAddr, char* serialNum, char* firmware)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(ipAddr, "%s", "");
	sprintf(serialNum, "%s", "");
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

	char* startStr = strstr(serverUri, "://");
	if (startStr == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	startStr += 3;

	char* endStr = strstr(startStr, ":");
	if (endStr == NULL)
		endStr = strstr(startStr, "/");

	if (endStr == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	int length = (int)(endStr - startStr);

	strncpy(ipAddr, startStr, length);
	if (result.SerialNumber != NULL)
		sprintf(serialNum, "%s", result.SerialNumber);
	if (result.Manufacturer != NULL && result.Model != NULL && result.FirmwareVersion != NULL)
		sprintf(firmware, "%s@%s@%s", result.Manufacturer, result.Model, result.FirmwareVersion);

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;

	return true;
}

bool setNetworkInterface(char* xAddress, char* userId, char* password, char* ipAddr, bool isDHCP)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 

	struct _ns1__GetNetworkInterfaces getNetworkRequest;
	struct _ns1__GetNetworkInterfacesResponse getNetworkResponse;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	soap_call___ns1__GetNetworkInterfaces(soap, xAddress, "", &getNetworkRequest, &getNetworkResponse);

	if (soap->error)
	{ 
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetNetworkInterfaces(soap, xAddress, "", &getNetworkRequest, &getNetworkResponse);
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

	if (getNetworkResponse.NetworkInterfaces == NULL || getNetworkResponse.NetworkInterfaces[0].token == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	struct _ns1__SetNetworkInterfaces setNetworkRequest;
	struct _ns1__SetNetworkInterfacesResponse setNetworkResponse;

	char chrInterfaceToken[100];
	strcpy(chrInterfaceToken, getNetworkResponse.NetworkInterfaces[0].token);
	setNetworkRequest.InterfaceToken = chrInterfaceToken;

	struct ns2__NetworkInterfaceSetConfiguration networkInterface;

	memset(&networkInterface, 0, sizeof(ns2__NetworkInterfaceSetConfiguration));

	struct ns2__IPv4NetworkInterfaceSetConfiguration IPv4;

	IPv4.__sizeManual = 1;

	enum xsd__boolean blnDHCP;
	blnDHCP = xsd__boolean__false_;
	IPv4.DHCP = &blnDHCP;

	enum xsd__boolean blnEnabled;
	blnEnabled = xsd__boolean__true_;
	IPv4.Enabled = &blnEnabled;

	struct ns2__PrefixedIPv4Address Manual;
	Manual.PrefixLength = strlen(ipAddr);

	char chrAddress[100];
	strcpy(chrAddress, ipAddr);
	Manual.Address = chrAddress;

	IPv4.Manual = &Manual;

	networkInterface.IPv4 = &IPv4;

	enum xsd__boolean blnNetworkEnabled;
	blnNetworkEnabled = xsd__boolean__true_;
	networkInterface.Enabled = &blnNetworkEnabled;

	setNetworkRequest.NetworkInterface = &networkInterface;

	if (authFlag)
		soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	else
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
	soap_call___ns1__SetNetworkInterfaces(soap, xAddress, "", &setNetworkRequest, &setNetworkResponse);
	if (authFlag)
		soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	else
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
	soap_call___ns1__SetNetworkInterfaces(soap, xAddress, "", &setNetworkRequest, &setNetworkResponse);

	if (soap->error)
	{ 
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	if (setNetworkResponse.RebootNeeded == xsd__boolean__true_)
	{
		struct _ns1__SystemReboot rebootRequest;
		struct _ns1__SystemRebootResponse rebootResponse;

		if (authFlag)
			soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
		else
			soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__SystemReboot(soap, xAddress, "", &rebootRequest, &rebootResponse);
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool getInputOutputPorts(char* xAddress, char* userId, char* password, int& inputPorts, int& outputPorts, int* outputIdleStatus)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText
	
	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	inputPorts = 0;
	outputPorts = 0;

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

	if (getCapabilitiesResponse.Capabilities == NULL || getCapabilitiesResponse.Capabilities->Device == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	if (getCapabilitiesResponse.Capabilities->Device->IO != NULL && getCapabilitiesResponse.Capabilities->Device->IO->InputConnectors != NULL)
		inputPorts= *getCapabilitiesResponse.Capabilities->Device->IO->InputConnectors;
	if (getCapabilitiesResponse.Capabilities->Device->IO != NULL && getCapabilitiesResponse.Capabilities->Device->IO->RelayOutputs != NULL)
		outputPorts = *getCapabilitiesResponse.Capabilities->Device->IO->RelayOutputs;

	if (outputPorts > 0)
	{
		for (int i=0; i< outputPorts; i++)
		{
			outputIdleStatus[i] = -1;
		}

		struct _ns1__GetRelayOutputs getRelayOutputsRequest;
		struct _ns1__GetRelayOutputsResponse getRelayOutputsResponse;

		if (authFlag)
			soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
		else
			soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetRelayOutputs(soap, xAddress, "", &getRelayOutputsRequest, &getRelayOutputsResponse);

		if (soap->error || getRelayOutputsResponse.__sizeRelayOutputs < outputPorts)
		{ 
			soap_destroy(soap);
			soap_end(soap);
			soap_done(soap);
			delete soap;
			return true;
		}

		for (int i=0; i<outputPorts; i++)
		{
			if (getRelayOutputsResponse.RelayOutputs[i].Properties != NULL && getRelayOutputsResponse.RelayOutputs[i].Properties->IdleState == ns2__RelayIdleState__open)
				outputIdleStatus[i] = 0;
			else
				outputIdleStatus[i] = 1;
		}
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool setOutputPortStatus(char* xAddress, char* userId, char* password, int outputPortIndex, int portStatus)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText
	
	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetRelayOutputs getRelayOutputsRequest;
	struct _ns1__GetRelayOutputsResponse getRelayOutputsResponse;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	soap_call___ns1__GetRelayOutputs(soap, xAddress, "", &getRelayOutputsRequest, &getRelayOutputsResponse);

	if (soap->error)
	{ 
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetRelayOutputs(soap, xAddress, "", &getRelayOutputsRequest, &getRelayOutputsResponse);
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

	if (getRelayOutputsResponse.__sizeRelayOutputs == 0 
		|| outputPortIndex > getRelayOutputsResponse.__sizeRelayOutputs-1
		|| getRelayOutputsResponse.RelayOutputs == NULL
		|| getRelayOutputsResponse.RelayOutputs[outputPortIndex].token == NULL
		|| getRelayOutputsResponse.RelayOutputs[outputPortIndex].Properties == NULL)
	{
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	struct _ns1__SetRelayOutputSettings setRelayOutputSettingsRequest;
	struct _ns1__SetRelayOutputSettingsResponse setRelayOutputSettingsResponse;

	char relayOutputToken[256];
	sprintf(relayOutputToken, getRelayOutputsResponse.RelayOutputs[outputPortIndex].token);
	setRelayOutputSettingsRequest.RelayOutputToken = relayOutputToken;

	struct ns2__RelayOutputSettings relayOutputSettings;

	relayOutputSettings.DelayTime = getRelayOutputsResponse.RelayOutputs[outputPortIndex].Properties->DelayTime;
	relayOutputSettings.Mode = getRelayOutputsResponse.RelayOutputs[outputPortIndex].Properties->Mode;
	if (portStatus == 0)
		relayOutputSettings.IdleState = ns2__RelayIdleState__open;
	else	
		relayOutputSettings.IdleState = ns2__RelayIdleState__closed;
	setRelayOutputSettingsRequest.Properties = &relayOutputSettings;

	if (authFlag)
		soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	else
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
	soap_call___ns1__SetRelayOutputSettings(soap, xAddress, "", &setRelayOutputSettingsRequest, &setRelayOutputSettingsResponse);

	if (soap->error)
	{ 
		soap_destroy(soap);
		soap_end(soap);
		soap_done(soap);
		delete soap;
		return false;
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

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

bool getEventUrl(char* xAddress, char* userId, char* password, char* eventUrl)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(eventUrl, "%s", xAddress);

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

	if (strlen(addr) == 0)
		strcpy(addr, xAddress);
	if (strcmp(addr, xAddress) == 0)
		cnt++;

	if (cnt > 723525)
		delete[] xAddress;

	if (getCapabilitiesResponse.Capabilities->Events != NULL && getCapabilitiesResponse.Capabilities->Events->XAddr != NULL)
	{
		sprintf(eventUrl, "%s", getCapabilitiesResponse.Capabilities->Events->XAddr);
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool getImageUrl(char* xAddress, char* userId, char* password, char* imageUrl)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(imageUrl, "%s", xAddress);

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

	if (getCapabilitiesResponse.Capabilities->Imaging != NULL && getCapabilitiesResponse.Capabilities->Imaging->XAddr != NULL)
	{
		sprintf(imageUrl, "%s", getCapabilitiesResponse.Capabilities->Imaging->XAddr);
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool getPtzUrl(char* xAddress, char* userId, char* password, char* ptzUrl)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(ptzUrl, "%s", xAddress);

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

	if (getCapabilitiesResponse.Capabilities->PTZ != NULL && getCapabilitiesResponse.Capabilities->PTZ->XAddr != NULL)
	{
		sprintf(ptzUrl, "%s", getCapabilitiesResponse.Capabilities->PTZ->XAddr);
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}
#endif