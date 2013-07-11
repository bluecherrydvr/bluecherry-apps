
struct CAMERAINFO {
	char ipAddress[32];
	char xAddress[256];
};

class COnvifMgm
{
public:
	COnvifMgm();
	~COnvifMgm();

	void discovery();
	void check_authority();
}
