
#include <memory.h>
#include <stdio.h>

#include "OnvifMgm.h"
#include "Discovery/discovery.h"
#include "DeviceManagement/devmng.h"
#include "Media/media.h"
#include "rpcserver/onvifserver/xmlrpc_onvif_server.h"

/************************************************************************************/
/*  Function Name	:	GetCamerasCallBack											*/
/*  Function Desc	:	scan the onvif cameras using ws-discovery service			*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
int GetCamerasCallBack(int* count, char** cameras)
{
	int cameraCount;

	char* ipAddress[100];
	char* macAddress[100];
	char* xAddress[100];

	for (int i=0; i<100; i++)
	{
		ipAddress[i] = new char[256];
		memset(ipAddress[i], 0, 256);
		macAddress[i] = new char[256];
		memset(macAddress[i], 0, 256);
		xAddress[i] = new char[256];
		memset(xAddress[i], 0, 256);
	}

	*count = 0;

	discoveryOnvifCamera(cameraCount, macAddress, xAddress);

	for (int i=0; i<cameraCount; i++)
	{
		char chrXAddress[256];
		sprintf(chrXAddress, "%s", xAddress[i]);

		if (strstr(chrXAddress, "http:///") != NULL)
		{
			continue;
		}

		char* chrTemp = strstr(chrXAddress, " ");
		if ( chrTemp != NULL)
		{
			char addrTemp[100];
			memset(addrTemp, 0, 100);
			strncpy(addrTemp, chrXAddress, chrTemp - chrXAddress);
			char* chrTemp1 = strstr(addrTemp, "192.168.");
			if (chrTemp1 != NULL)
			{
				sprintf(chrXAddress, "%s", addrTemp);
			}
			else
			{
				chrTemp1 = strstr(chrTemp + 1, "192.168.");
				if (chrTemp1 != NULL)
				{
					sprintf(chrXAddress, "%s", chrTemp + 1);
				}
				else
				{
					sprintf(chrXAddress, "%s", addrTemp);
				}
			}

		}

		char* startStr = strstr(chrXAddress, "://");
		if (startStr == NULL)
		{
			continue;
		}
		
		startStr += 3;
		
		char* endStr = strstr(startStr, ":");

		if (endStr == NULL)
			endStr = strstr(startStr, "/");

		if (endStr == NULL)
		{
			continue;
		}

		int length = (int)(endStr - startStr);
		
		if (length == 0 || length > 16)
		{
			continue;
		}

		strncpy(ipAddress[i], startStr, length);

		// set the camera's information
		sprintf(cameras[*count], "%s,%s", ipAddress[i], chrXAddress);

		printf("count = %d\n", *count);
		printf("xAddress = %s\n", cameras[*count]);

		*count = (*count) + 1;

		printf("count = %d\n", *count);
	}

	// free 
	for (int i=0; i<100; i++)
	{
		delete ipAddress[i];
		delete macAddress[i];
		delete xAddress[i];
	}
}

/************************************************************************************/
/*  Function Name	:	GetCamerainfoCallBack										*/
/*  Function Desc	:	1.check the authority. 										*/ 
/*						2.get the streaming uri&port and snapshot uri&port.			*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
int GetCamerainfoCallBack(char* xAddress, char* username, char* password, char* make, char* model, char* firmware, char* rtspUri, int* rtspPort, char* snapshotUri, int* snapshotPort)
{
	bool bRet = false;
	char mediaUrl[256];
	char streamUri[512];
	char snapUri[512];

	memset(mediaUrl, 0, 256);
	memset(streamUri, 0, 512);
	memset(snapUri, 0, 512);

	getMediaUrl(xAddress, username, password, mediaUrl);
	
	bRet = getDeviceInfo(xAddress, username, password, make, model, firmware);

	if (bRet == false || strlen(model) == 0)
	{
		return -1;
	}

	bRet = getStreamInfo(mediaUrl, username, password, streamUri, snapUri);

	if (bRet == false || strlen(streamUri) == 0)
	{
		return -1;
	}

	printf("make=%s\n", make);
	printf("model=%s\n", model);
	printf("firmware=%s\n", firmware);
	printf("streamUri=%s\n", streamUri);
	printf("snapshotUri=%s\n", snapUri);

	return 0;
}

/************************************************************************************/
/*  Function Name	:	rpcserver_start												*/
/*  Function Desc	:	start the xml-rpc server on the selected Port				*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
bool start_onvifserver(int port)
{
	set_GetCameras_callback(GetCamerasCallBack);
	set_GetCamerainfo_callback(GetCamerainfoCallBack);

	start_rpcserver(port);
	return true;
}

