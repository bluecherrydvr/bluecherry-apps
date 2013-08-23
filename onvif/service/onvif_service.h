
typedef int (*get_cameras_callback)(int* count, char** ipAddr, char** xAddr, char** make, char** model, char** firmware);
typedef int (*get_camerainfo_callback)(char* xAddress, char* username, char* password, char* rtspUri, int* rtspPort, char* snapshotUri, int* snapshotPort);

extern "C" {
	void start_rpcserver(int port);
	void set_GetCameras_callback(get_cameras_callback getCamerasFunc);
	void set_GetCamerainfo_callback(get_camerainfo_callback getCamerainfoFunc);
}
