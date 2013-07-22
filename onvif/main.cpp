#include "./OnvifMgm.h"
#include <stdio.h>

int main()
{
	// discovery for the onvif cameras
	discovery();

	// check authority
	char* xAddress = "http://192.168.0.110:80/onvif/device_service";
	char* username = "root";
	char* password = "admin";
	if (check_authority(xAddress, username, password) == false)
	{
		printf("authorization fail\n");
	}
	else
	{
		printf("succcess\n");
	}

	
}
