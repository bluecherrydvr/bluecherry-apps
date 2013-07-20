
struct CAMERAINFO {
	char ipAddress[32];
	char xAddress[256];
};

extern "C" {
	void discovery();
	void check_authority(char* xAddress, char* userName, char* password);
}
