#include "stdafx.h"
#ifdef LINUX_NAS
#include "media.h"
#include "gsoap/soapH.h"
#include "gsoap/DeviceBinding.nsmap"
#include "gsoap/smdevp.h"
#include "gsoap/wsseapi.h"
#include "math.h"


bool getMediaInfo(char* xAddress, char* userId, char* password, char* streamUri, char* subStreamUri, int& streamCodec, int& subStreamCodec)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(streamUri,"%s", "");
	sprintf(subStreamUri,"%s", "");

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

	bool blnFindPrimaryProfile = false;
	bool blnFindSecondProfile = false;

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

		if (intProfileIndex == 0)
		{
			strcpy(streamUri, streamResponse.MediaUri->Uri);
			streamCodec = resultProfile.VideoEncoderConfiguration->Encoding;
			intProfileIndex++;
		}
		else if (intProfileIndex == 1)
		{
			strcpy(subStreamUri, streamResponse.MediaUri->Uri);
			subStreamCodec = resultProfile.VideoEncoderConfiguration->Encoding;
			break;
		}

	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

bool getConfiguration(char* xAddress, char* userId, char* password, int encodingType, int& resolCount, int* width, int* heigth, int &minFps, int &maxFps)
{
	return true;
}


bool setConfiguration(char* xAddress, char* userId, char* password, int streamNo, int encodingType, int width, int heigth, int fps, int bitrate)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);

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

	int streamIndex = 0;

	for (int i=0; i<count; i++)
	{
		resultProfile = result.Profiles[i];

		if (resultProfile.VideoEncoderConfiguration == NULL || resultProfile.VideoEncoderConfiguration->Encoding != ns2__VideoEncoding__H264)
			continue;

		if (streamIndex == streamNo)
		{
			struct _ns1__SetVideoEncoderConfiguration setConfigRequest;
			struct _ns1__SetVideoEncoderConfigurationResponse setConfigResponse;

			struct ns2__VideoRateControl setRateControl;
			memset(&setRateControl, 0, sizeof(struct ns2__VideoRateControl));

			setConfigRequest.ForcePersistence = xsd__boolean__true_;
			setConfigRequest.Configuration = resultProfile.VideoEncoderConfiguration;

			setConfigRequest.Configuration->Resolution->Width = width;
			setConfigRequest.Configuration->Resolution->Height = heigth;

			if (setConfigRequest.Configuration->RateControl == NULL)
			{
				setConfigRequest.Configuration->RateControl = &setRateControl;
			}

			setConfigRequest.Configuration->RateControl->FrameRateLimit = fps;

			if (encodingType == ns2__VideoEncoding__H264)
			{
				setConfigRequest.Configuration->RateControl->BitrateLimit = bitrate;
				if (setConfigRequest.Configuration->H264 != NULL)
				{
					setConfigRequest.Configuration->H264 = NULL;
					//setConfigRequest.Configuration->H264->GovLength = (int) ceil((double) fps * 0.5);
				}
			}
			else
			{
				setConfigRequest.Configuration->Quality = bitrate;
			}

			if (authFlag)
				soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
			else
				soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);

			soap_call___ns1__SetVideoEncoderConfiguration(soap, xAddress, "", &setConfigRequest, &setConfigResponse);

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

		streamIndex++;
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return false;
}

bool getVideoSourceToken(char* xAddress, char* userId, char* password, int encodingType, char* videoSourceToken)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(videoSourceToken, "%s", "");

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetProfiles request;
	struct _ns1__GetProfilesResponse result;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
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

	for (int i=0; i<count; i++)
	{
		resultProfile = result.Profiles[i];

		if (resultProfile.VideoEncoderConfiguration == NULL)
			continue;

		if (resultProfile.VideoEncoderConfiguration->Encoding == encodingType && resultProfile.VideoSourceConfiguration->SourceToken != NULL)
		{
			strcpy(videoSourceToken, resultProfile.VideoSourceConfiguration->SourceToken);
			break;
		}
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;

	if (strlen(videoSourceToken) == 0)
		return false;

	return true;
}

bool getPTZProfileToken(char* xAddress, char* userId, char* password, char* profileToken)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	sprintf(profileToken, "%s", "");

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetProfiles request;
	struct _ns1__GetProfilesResponse result;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
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

	for (int i=0; i<count; i++)
	{
		resultProfile = result.Profiles[i];

		if (resultProfile.PTZConfiguration == NULL || resultProfile.token == NULL)
			continue;

		strcpy(profileToken, resultProfile.token);
		break;
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;

	if (strlen(profileToken) == 0)
		return false;

	return true;
}

bool getStreamInfo(char* xAddress, char* userId, char* password, 
							   int& intStreamCount, int* intStreamCodec, int* intResDefaultWidth, int* intResDefaultHeight, 
							   int* intFpsDefault, int* intBitrateDefault, int* intQuality, int* intResCount,
							   int** intResWidth, int** intResHeight, int* intFpsMin, int* intFpsMax, int* intQualityMin, int* intQualityMax)
{
	int authFlag = 1;	// 1 : UsernameTokenDigest, 0: UsernameTokenText

	struct soap* soap = new struct soap;

	soap_init2(soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap->connect_timeout = 7;
	soap->recv_timeout = 7;

	struct _ns1__GetProfiles request;
	struct _ns1__GetProfilesResponse result;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
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

	intStreamCount = 0;

	for (int i = 0; i < result.__sizeProfiles; i++)
	{
		struct ns2__Profile* resultProfile = &(result.Profiles[i]);

		if (resultProfile->VideoEncoderConfiguration == NULL || resultProfile->VideoEncoderConfiguration->Encoding != ns2__VideoEncoding__H264)
			continue;

		intStreamCodec[intStreamCount] = resultProfile->VideoEncoderConfiguration->Encoding;

		intResDefaultWidth[intStreamCount] = resultProfile->VideoEncoderConfiguration->Resolution->Width;
		intResDefaultHeight[intStreamCount] = resultProfile->VideoEncoderConfiguration->Resolution->Height;
		if (resultProfile->VideoEncoderConfiguration->RateControl == NULL)
		{
			intFpsDefault[intStreamCount] = 30;
			intBitrateDefault[intStreamCount] = 2000;
		}
		else
		{
			intFpsDefault[intStreamCount] = resultProfile->VideoEncoderConfiguration->RateControl->FrameRateLimit;
			intBitrateDefault[intStreamCount] = resultProfile->VideoEncoderConfiguration->RateControl->BitrateLimit;
		}

		intQuality[intStreamCount] = resultProfile->VideoEncoderConfiguration->Quality;

		struct _ns1__GetVideoEncoderConfigurationOptions configOptionsRequest;
		struct _ns1__GetVideoEncoderConfigurationOptionsResponse configOptionsResponse;

		memset(&configOptionsRequest, 0, sizeof(struct _ns1__GetVideoEncoderConfigurationOptions));

		char profileToken[100];
		strcpy(profileToken, resultProfile->token);
		configOptionsRequest.ProfileToken = profileToken;

		if (authFlag)
			soap_wsse_add_UsernameTokenDigest(soap, NULL, userId, password);
		else
			soap_wsse_add_UsernameTokenText(soap, NULL, userId, password);

		soap_call___ns1__GetVideoEncoderConfigurationOptions(soap, xAddress, "", &configOptionsRequest, &configOptionsResponse);

		if (soap->error || configOptionsResponse.Options == NULL)
		{ 
			continue;
		}

		intQualityMin[intStreamCount] = configOptionsResponse.Options->QualityRange->Min;
		intQualityMax[intStreamCount] = configOptionsResponse.Options->QualityRange->Max;

		if (intStreamCodec[intStreamCount] == ns2__VideoEncoding__H264)
		{
			if (configOptionsResponse.Options->H264 == NULL)
			{
				continue;
			}

			intResCount[intStreamCount] = configOptionsResponse.Options->H264->__sizeResolutionsAvailable;

			for (int j=0; j<intResCount[intStreamCount]; j++)
			{
				intResWidth[intStreamCount][j] = configOptionsResponse.Options->H264->ResolutionsAvailable[j].Width;
				intResHeight[intStreamCount][j] = configOptionsResponse.Options->H264->ResolutionsAvailable[j].Height;
			}

			intFpsMin[intStreamCount] = configOptionsResponse.Options->H264->FrameRateRange->Min;
			intFpsMax[intStreamCount] = configOptionsResponse.Options->H264->FrameRateRange->Max;
		}
		else if (intStreamCodec[intStreamCount] == ns2__VideoEncoding__JPEG)
		{
			if (configOptionsResponse.Options->JPEG == NULL)
			{
				continue;
			}

			intResCount[intStreamCount] = configOptionsResponse.Options->JPEG->__sizeResolutionsAvailable;

			for (int j=0; j<intResCount[intStreamCount]; j++)
			{
				intResWidth[intStreamCount][j] = configOptionsResponse.Options->JPEG->ResolutionsAvailable[j].Width;
				intResHeight[intStreamCount][j] = configOptionsResponse.Options->JPEG->ResolutionsAvailable[j].Height;
			}

			intFpsMin[intStreamCount] = configOptionsResponse.Options->JPEG->FrameRateRange->Min;
			intFpsMax[intStreamCount] = configOptionsResponse.Options->JPEG->FrameRateRange->Max;
		}

		if (intFpsMin[intStreamCount] == 0)
			intFpsMin[intStreamCount] = 1;
		if (intFpsMax[intStreamCount] == 0)
			intFpsMax[intStreamCount] = 30;

		if (intFpsDefault[intStreamCount] < intFpsMin[intStreamCount] || intFpsDefault[intStreamCount] > intFpsMax[intStreamCount])
			intFpsDefault[intStreamCount] = intFpsMax[intStreamCount];

		intStreamCount++;

		if (intStreamCount == 2)
			break;
	}

	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	delete soap;
	return true;
}

#endif