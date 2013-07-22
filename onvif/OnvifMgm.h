
struct CAMERAINFO {
	char ipAddress[256];
	char xAddress[256];
};

extern "C" {
	void discovery();
	bool check_authority(char* xAddress, char* userName, char* password);
}
