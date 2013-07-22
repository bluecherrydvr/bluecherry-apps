#include "OnvifMgm.h"
#include "Discovery/discovery.h"
#include "DeviceManagement/devmng.h"
#include "Media/media.h"
#include <memory.h>
#include <stdio.h>

/************************************************************************************/
/*  Function Name	:	discovery													*/
/*  Function Desc	:	scan the onvif cameras using ws-discovery service			*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
void discovery()
{
	int cameraSize;

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

	discoveryOnvifCamera(cameraSize, macAddress, xAddress);

	for (int i=0; i<cameraSize; i++)
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

		printf(chrXAddress);
		printf("\n");
		printf(ipAddress[i]);
		printf("\n");

		// to-do area
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
/*  Function Name	:	check_authority												*/
/*  Function Desc	:	1.check the authority. 										*/ 
/*						2.get the streaming uri&port and snapshot uri&port.			*/
/*  Author			:	ruminsam													*/
/************************************************************************************/
bool check_authority(char* xAddress, char* userName, char* password)
{
	bool bRet = false;
	char mediaUrl[256];
	char streamUri[256];
	char snapshotUri[256];

	memset(mediaUrl, 0, 256);
	memset(streamUri, 0, 256);
	memset(snapshotUri, 0, 256);

	getMediaUrl(xAddress, userName, password, mediaUrl);
	bRet = getStreamInfo(mediaUrl, userName, password, streamUri, snapshotUri);

	// get the ports
	if (bRet == false || strlen(streamUri) == 0)
	{
		return false;
	}

	printf("streamUri=%s\n", streamUri);
	printf("snapshotUri=%s\n", snapshotUri);

	return true;
}


