
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "OnvifMgm.h"
#include "Discovery/discovery.h"
#include "DeviceManagement/devmng.h"
#include "Media/media.h"
#include "service/onvif_service.h"

/************************************************************************************/
/*  Function Name    :    GetCamerasCallBack                                        */
/*  Function Desc    :    scan the onvif cameras using ws-discovery service         */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int GetCamerasCallBack(int* count, char** ipAddr, char** xAddr, char** make, char** model, char** firmware)
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

		bool isDuplicated = false;		
		for (int j=0; j<*count; j++)
		{
			if (strcmp(ipAddress[i], ipAddress[j]) == 0)
			{
				isDuplicated = true;
				break;
			}
				
		}
		if (isDuplicated)
			continue;

		// set the camera's information
		sprintf(ipAddr[*count], "%s", ipAddress[i]);
		sprintf(xAddr[*count], "%s", chrXAddress);
		
		// get the make, model, firmware
		getDeviceInfo(chrXAddress, "", "", make[*count], model[*count], firmware[*count]);

		printf("count = %d\n", *count);
		printf("ipAddr = %s\n", ipAddr[*count]);
		printf("xAddr = %s\n", xAddr[*count]);
		printf("make = %s\n", make[*count]);
		printf("model = %s\n", model[*count]);
		printf("firmware = %s\n", firmware[*count]);

		*count = (*count) + 1;
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
/*  Function Name    :    GetCamerainfoCallBack                                     */
/*  Function Desc    :    1.check the authority.                                    */ 
/*                        2.get the streaming uri&port and snapshot uri&port.       */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
int GetCamerainfoCallBack(char* xAddress, char* username, char* password, char* rtspUri, int* rtspPort, char* snapshotUri, int* snapshotPort)
{
	bool bRet = false;
	char mediaUrl[256];
	char streamUri[512];
	char snapUri[512];

	memset(mediaUrl, 0, 256);
	memset(streamUri, 0, 512);
	memset(snapUri, 0, 512);

	getMediaUrl(xAddress, username, password, mediaUrl);
	
	bRet = getStreamInfo(mediaUrl, username, password, streamUri, snapUri);

	if (bRet == false || strlen(streamUri) == 0)
	{
		return -1;
	}

	printf("fullStreamUri=%s\n", streamUri);
	printf("fullSnapshotUri=%s\n", snapUri);

	char* startStr = strstr(streamUri, "://");
	if (startStr == NULL)
	{
		sprintf(rtspUri, "%s", streamUri);
		*rtspPort = 554;
	}
	else
	{
		startStr += 3;	

		char* endStr = strstr(startStr, ":");

		if (endStr == NULL)
		{
			*rtspPort = 554;
		}
		else
		{
			char* temp = strstr(startStr, "/");
			int length = temp - endStr;
			char strRtspPort[6];
			memset(strRtspPort, 0, 6);
			strncpy(strRtspPort, endStr+1, length);
			*rtspPort = atoi(strRtspPort);
		}

		endStr = strstr(startStr, "/");
		if (endStr == NULL)
		{
			sprintf(rtspUri, "%s", endStr);
		}
		else
		{
			sprintf(rtspUri, "%s", endStr);
		}
	}
	
	startStr = strstr(snapUri, "://");
	if (startStr == NULL)
	{
		sprintf(snapshotUri, "%s", snapUri);
		*rtspPort = 80;
	}
	else
	{
		startStr += 3;	

		char* endStr = strstr(startStr, ":");

		if (endStr == NULL)
		{
			*snapshotPort = 80;
		}
		else
		{
			char* temp = strstr(startStr, "/");
			int length = temp - endStr;
			char strSnapshotPort[6];
			memset(strSnapshotPort, 0, 6);
			strncpy(strSnapshotPort, endStr+1, length);
			*snapshotPort = atoi(strSnapshotPort);
		}

		endStr = strstr(startStr, "/");
		if (endStr == NULL)
		{
			sprintf(snapshotUri, "%s", endStr);
		}
		else
		{
			sprintf(snapshotUri, "%s", endStr);
		}
	}

	printf("rtspUri=%s\n", rtspUri);
	printf("rtspPort=%d\n", *rtspPort);
	printf("snapshotUri=%s\n", snapshotUri);
	printf("snapshotPort=%d\n", *snapshotPort);

	return 0;
}

/************************************************************************************/
/*  Function Name    :    rpcserver_start                                           */
/*  Function Desc    :    start the xml-rpc server on the selected Port             */
/*  Author           :    ruminsam                                                  */
/************************************************************************************/
bool start_onvifserver(int port)
{
	set_GetCameras_callback(GetCamerasCallBack);
	set_GetCamerainfo_callback(GetCamerainfoCallBack);

	start_rpcserver(port);
	return true;
}

