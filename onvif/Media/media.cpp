#include "stdafx.h"
#ifdef LINUX_NAS
#include "media.h"
#include "gsoap/soapH.h"
#include "gsoap/DeviceBinding.nsmap"
#include "gsoap/smdevp.h"
#include "gsoap/wsseapi.h"
#include "math.h"

/************************************************************************************/
/*  Function Name	:	getStreamInfo												*/
/*  Function Desc	:	get rtsp uri and snapshot uri								*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
bool getStreamInfo(char* xAddress, char* userId, char* password, char* streamUri, char* snapshotUri)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(streamUri,"%s", "");
	sprintf(snapshotUri,"%s", "");

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetProfiles request;
	struct _ns1__GetProfilesResponse result;

	soap_call___ns1__GetProfiles(soap, xAddress, "", &request, &result);

	if (soap->error)
	{ 
		soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);
		soap_call___ns1__GetProfiles(soap, xAddress, "", &request, &result);
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

	int count = result.__sizeProfiles;

	struct ns2__Profile resultProfile;

	int intProfileIndex = 0;
	for (int i=0; i<count; i++)
	{
		resultProfile = result.Profiles[i];

		if (resultProfile.VideoEncoderConfiguration == NULL || resultProfile.VideoEncoderConfiguration->Encoding != ns2__VideoEncoding__H264)
			continue;

		struct _ns1__GetStreamUri streamRequest;
		struct _ns1__GetStreamUriResponse streamResponse;

		char profileToken[256];
		strcpy(profileToken, resultProfile.token);
		streamRequest.ProfileToken = profileToken;

		struct ns2__StreamSetup streamSetup;

		streamSetup.Stream = ns2__StreamType__RTP_Unicast;

		struct ns2__Transport transport;
		transport.Protocol = ns2__TransportProtocol__UDP;
		streamSetup.Transport = &transport;

		streamSetup.__size = 0;
		streamSetup.__any = 0;
		streamSetup.__anyAttribute = 0;

		streamRequest.StreamSetup = &streamSetup;

		if (authFlag)
			soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
		else
			soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);

		soap_call___ns1__GetStreamUri(soap, xAddress, "", &streamRequest, &streamResponse);

		if (soap->error || streamResponse.MediaUri == NULL || streamResponse.MediaUri->Uri == NULL)
		{ 
			continue;
		}

		// get the stream uri
		strcpy(streamUri, streamResponse.MediaUri->Uri);

		struct _ns1__GetSnapshotUri snapshotUriRequest;
		struct _ns1__GetSnapshotUriResponse snapshotUriResponse;

		snapshotUriRequest.ProfileToken = profileToken;

		if (authFlag)
			soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
		else
			soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);

		soap_call___ns1__GetSnapshotUri(soap, xAddress, "", &snapshotUriRequest, &snapshotUriResponse);

		if (soap->error || snapshotUriResponse.MediaUri == NULL || snapshotUriResponse.MediaUri->Uri == NULL)
		{ 
			break;
		}

		// get the snapshot uri
		strcpy(snapshotUri, snapshotUriResponse.MediaUri->Uri);

		break;
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;

	return true;
}

#endif
