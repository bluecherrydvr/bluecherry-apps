extern "C" {
	bool getMediaInfo(char* xAddress, char* userId, char* password, char* streamUri, char* subStreamUri, int& streamCodec, int& subStreamCodec);
	bool getConfiguration(char* xAddress, char* userId, char* password, int encodingType, int& resolCount, int* width, int* heigth, int &minFps, int &maxFps);
	bool setConfiguration(char* xAddress, char* userId, char* password, int streamNo, int encodingType, int width, int heigth, int fps, int bitrate);
	bool getVideoSourceToken(char* xAddress, char* userId, char* password, int encodingType, char* videoSourceToken);
	bool getPTZProfileToken(char* xAddress, char* userId, char* password, char* profileToken);

	bool getStreamInfo(char* xAddress, char* userId, char* password, 
		int& intStreamCount, int* intStreamCodec, int* intResDefaultWidth, int* intResDefaultHeight, 
		int* intFpsDefault, int* intBitrateDefault, int* intQuality, int* intResCount,
		int** intResWidth, int** intResHeight, int* intFpsMin, int* intFpsMax, int* intQualityMin, int* intQualityMax);
}