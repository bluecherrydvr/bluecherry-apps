#include "stdafx.h"
#ifdef LINUX_NAS
#include "discovery.h"
#include "NVRArrayList.h"
#include "gsoap/soapH.h"
#include "gsoap/wsdd.nsmap"
#include "gsoap/wsddapi.h"

//CNVRArrayList cameraList;

void discoveryOnvifCamera(int& intCount, char** macAddress, char** xAddress)
{
	int port = 3702;
	struct soap *soap = soap_new();

	soap_init1(soap, SOAP_IO_UDP); 

	soap->bind_flags = SO_REUSEADDR;
	/*
	if (!soap_valid_socket(soap_bind(soap, NULL, port, 100)))
	{ 
	soap_print_fault(soap, stderr);
	return;
	}
	*/

	soap->recv_timeout = 7;
	intCount = 0;

	char id[100];
	memset(id, 0, 100);
	sprintf(id, "uuid:%s", soap_wsa_rand_uuid(soap));

	soap_wsdd_Probe(soap,
		SOAP_WSDD_MANAGED,      // ad-hoc mode
		SOAP_WSDD_TO_TS,      // to a TS
		"soap.udp://239.255.255.250:3702",         // address of TS
		id,                   // message ID
		NULL,                 // ReplyTo
		"",
		"",
		NULL,
		intCount,
		macAddress,
		xAddress);

	//	soap_wsdd_listen(serv, 3);	

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
}

void getOnvifCameraList(char** macAddress, char** xAddress)
{
	/*
	for (int i=0; i<cameraList.Size(); i++)
	{
	sprintf(macAddress[i], "%s", ((CAMERAINFO*)cameraList.Get(i))->macAddress);
	sprintf(xAddress[i], "%s", ((CAMERAINFO*)cameraList.Get(i))->xAddress);
	}
	*/
}

int getOnvifCameraSize()
{

	//return cameraList.Size();
	return 0;
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	return SOAP_WSDD_ADHOC;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
	/*
	struct CAMERAINFO* camInfo = new struct CAMERAINFO();
	strcpy(camInfo->macAddress, matches->ProbeMatch->wsa5__EndpointReference.Address);
	strcpy(camInfo->xAddress, matches->ProbeMatch->XAddrs);

	cameraList.Add((void*)camInfo);
	*/
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
	return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
}
#endif