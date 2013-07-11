
struct CAMERAINFO {
	char	macAddress[30];
	char	xAddress[256];
} ;

extern "C" {
	void discoveryOnvifCamera(int& intCount, char** macAddress, char** xAddress);
	void getOnvifCameraList(char** macAddress, char** xAddress);
	int getOnvifCameraSize();

}