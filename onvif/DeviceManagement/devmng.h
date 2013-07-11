extern "C" {
bool getDeviceInfo(char* xAddress, char* userId, char* password, char* ipAddr, char* serialNum, char* firmware);
bool setNetworkInterface(char* xAddress, char* userId, char* password, char* ipAddr, bool isDHCP);
bool getInputOutputPorts(char* xAddress, char* userId, char* password, int& inputPorts, int& outputPorts, int* outputIdleStatus);
bool setOutputPortStatus(char* xAddress, char* userId, char* password, int outputPortIndex, int portStatus);
bool getMediaUrl(char* xAddress, char* userId, char* password, char* mediaUrl);
bool getEventUrl(char* xAddress, char* userId, char* password, char* eventUrl);
bool getImageUrl(char* xAddress, char* userId, char* password, char* imageUrl);
bool getPtzUrl(char* xAddress, char* userId, char* password, char* ptzUrl);
}