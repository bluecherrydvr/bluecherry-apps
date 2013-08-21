#ifndef _USER_TYPE_H_
#define _USER_TYPE_H_

#define MIN(a,b)   (((a) < (b)) ? (a) : (b))
#define MAX(a,b)   (((a) > (b)) ? (a) : (b))

#ifdef LINUX_NAS
#include <pthread.h>
#endif
#define	MAX_QUALITY_LEVEL	3

#define CODEC_MPEG4 0
#define CODEC_MJPEG 1
#define CODEC_3GP   2
#define CODEC_H264  3
#define CODEC_MPNG  4

#define CODEC_G711 5
#define CODEC_G726 6

#define LIMIT_RECORD_FIFO_COUNT			1024
#define LIMIT_LIVE_FIFO_COUNT			512
#define LIMIT_QUERY_TOTAL_SIZE			2 * 1024 * 1024		// ImageFifo	-- Query size
#define LIMIT_RECORD_FIFO_TOTAL_SIZE	2 * 1024 * 1024		// RecordFifo	-- Total buffer size
#define LIMIT_LIVE_FIFO_TOTAL_SIZE		512 * 1024			// LiveFifo		-- Total buffer size
#define LIMIT_IMAGE_SIZE				300 * 1024			// Video		-- One frame size
#define LIMIT_AUDIO_FRAME_SIZE			4 * 1024			// Audio		-- One frame size

#define LIMIT_CHANNEL_COUNT				36

#ifdef LINUX_NAS
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stddef.h>
#ifndef __cplusplus
# include <stdbool.h>
#endif

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <wchar.h>
#include <time.h>
#include <string>

#ifndef NULL
#define NULL 0
#endif

#ifndef INT
#define INT int
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#ifndef INT64
#define INT64 int64_t
#endif

#ifndef BOOL
#define BOOL bool
#endif

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef ULONG
#define ULONG unsigned long
#endif

#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef CHAR
#define CHAR char
#endif

#ifndef DOUBLE
#define DOUBLE double
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef LONG
#define LONG long
#endif

#ifndef WORD
#define WORD unsigned short int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define False false
#define True true
#define MAX_PATH          260

typedef int64_t __int64;
typedef int32_t __int32;
typedef wchar_t WCHAR;
typedef void    *LPVOID;
typedef unsigned char byte;

typedef int WPARAM;
typedef int LPARAM;

//typedef struct timeb _timeb;
#define _ftime        ftime;

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

typedef struct _SYSTEMTIME { 
	WORD wYear; 
	WORD wMonth; 
	WORD wDayOfWeek; 
	WORD wDay; 
	WORD wHour; 
	WORD wMinute; 
	WORD wSecond; 
	WORD wMilliseconds; 
} SYSTEMTIME, *LPSYSTEMTIME;

static int inline IsBadReadPtr(void* lp, int cb)
{
	unsigned char b1;
	int bRet = 0;
	if (lp == NULL)
	{
		return 1;
	}
	try
	{
		for (int i = 0; i < cb; i ++)
			b1 = ((unsigned char*)lp)[i];
		bRet = 0;
	}
	catch(char* str)
	{
		bRet = 1;
	}

	return bRet;
}


inline void GetLocalTime(SYSTEMTIME* lpSysTime)
{
	if(!lpSysTime)
		return;

	struct timeb tp;
	ftime(&tp);

	struct tm *newtime;
	newtime = localtime(&tp.time);

	if(!newtime)
		return;

	lpSysTime->wYear = newtime->tm_year + 1900; 
	lpSysTime->wMonth = newtime->tm_mon + 1; 
	lpSysTime->wDayOfWeek = newtime->tm_wday; 
	lpSysTime->wDay = newtime->tm_mday; 
	lpSysTime->wHour = newtime->tm_hour; 
	lpSysTime->wMinute = newtime->tm_min; 
	lpSysTime->wSecond = newtime->tm_sec; 
	lpSysTime->wMilliseconds = tp.millitm; 
}

typedef struct _FILETIME 
{
	DWORD dwLowDateTime; 
	DWORD dwHighDateTime; 
} FILETIME; 

void inline FileTimeToSystemTime(FILETIME* lpFileTime, SYSTEMTIME *lpSysTime)
{
	if(!lpSysTime || !lpFileTime)
		return;

	time_t timet;
	timet = *((INT64*)(lpFileTime)) / 10000;

	struct tm* newtime;
	newtime = localtime(&timet);

	if(!newtime)
		return;

	lpSysTime->wYear = newtime->tm_year + 1900; 
	lpSysTime->wMonth = newtime->tm_mon + 1; 
	lpSysTime->wDayOfWeek = newtime->tm_wday; 
	lpSysTime->wDay = newtime->tm_mday; 
	lpSysTime->wHour = newtime->tm_hour; 
	lpSysTime->wMinute = newtime->tm_min; 
	lpSysTime->wSecond = newtime->tm_sec; 
	lpSysTime->wMilliseconds = timet % 1000; 
}

void inline SystemTimeToFileTime( SYSTEMTIME *lpSysTime, FILETIME* lpFileTime )
{
	if(!lpSysTime || !lpFileTime)
		return;

	struct tm newtime;
	newtime.tm_year = lpSysTime->wYear - 1900;
	newtime.tm_mon = lpSysTime->wMonth - 1; 
	newtime.tm_wday = lpSysTime->wDayOfWeek; 
	newtime.tm_mday = lpSysTime->wDay; 
	newtime.tm_hour = lpSysTime->wHour; 
	newtime.tm_min = lpSysTime->wMinute; 
	newtime.tm_sec = lpSysTime->wSecond; 
	newtime.tm_isdst = -1;

	time_t timet = mktime(&newtime);
	*((INT64*)lpFileTime) = (lpSysTime->wMilliseconds + timet * 1000) * 10000;
}

static int inline GetTickCount()
{
	int tc;
	struct timeval t;
	gettimeofday( &t, NULL );
	tc = ((t.tv_sec * 1000) + (t.tv_usec / 1000));

	return tc;
}



#define TRACE 		printf

inline void CW2A(const wchar_t* src, std::string& dest)
{
	if(src == NULL)
		return;

	char destChars[2048];
	memset(destChars, 0, 2048);
	wcstombs(destChars, src, 2048);
	dest = destChars;
}

int inline GetLastError()
{
	return errno;
}

#define _T(x) L##x
#define TEXT(x) L##x
#endif

typedef struct tagAUDIO {
	unsigned char audioData[LIMIT_AUDIO_FRAME_SIZE];
	int audioLength;
	int flag;

#ifdef LINUX_NAS
	pthread_mutex_t lock;
#else
	bool lock; // use in display
#endif

	double  audioTotalSize; // KB
	__int64 timeStamp;

} AUDIOFRAME;

extern AUDIOFRAME ipCameraAudios[LIMIT_CHANNEL_COUNT];
extern AUDIOFRAME ipCameraRecordAudios[LIMIT_CHANNEL_COUNT];

#ifdef LINUX_NAS
void inline InitLock(pthread_mutex_t& lock)
{
	pthread_mutexattr_t attr;
	int i_result;

	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
	i_result = pthread_mutex_init( &lock, &attr );
	pthread_mutexattr_destroy( &attr );
}

void inline Lock(pthread_mutex_t& lock)
{
	pthread_mutex_lock( &lock );
}

void inline UnLock(pthread_mutex_t& lock)
{
	pthread_mutex_unlock( &lock );
}

bool inline TryLock(pthread_mutex_t& lock)
{
	return pthread_mutex_trylock( &lock );
}

void inline EnterCritical(pthread_mutex_t& lock)
{
	Lock(lock);
}
void inline LeaveCritical(pthread_mutex_t& lock)
{
	UnLock(lock);
}

void inline DeleteCritical(pthread_mutex_t& lock)
{
	pthread_mutex_destroy(&lock);
}

void inline InitCritical(pthread_mutex_t& lock)
{
	pthread_mutexattr_t attr;
	int i_result;

	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
	i_result = pthread_mutex_init( &lock, &attr );
	pthread_mutexattr_destroy( &attr );
}
#else

void inline InitLock(bool& lock)
{
	lock = false;
}

void inline Lock(bool& lock)
{
	InterlockedIncrement((long*)&lock);
}

void inline gobal_Lock(bool& lock)
{
	Lock(lock);
}

void inline UnLock(bool& lock)
{
	InterlockedDecrement((long*)&lock);
}

void inline global_UnLock(bool& lock)
{
	UnLock(lock);
}
void inline Lock(long& lock)
{
	InterlockedIncrement(&lock);
}
void inline UnLock(long& lock)
{
	InterlockedDecrement(&lock);
}

bool inline TryLock(bool& lock)
{
	return lock;
}

void inline EnterCritical(CRITICAL_SECTION& lock)
{
	EnterCriticalSection(&lock);
}
void inline LeaveCritical(CRITICAL_SECTION& lock)
{
	LeaveCriticalSection(&lock);
}

void inline DeleteCritical(CRITICAL_SECTION& lock)
{
	DeleteCriticalSection(&lock);
}

void inline InitCritical(CRITICAL_SECTION& lock)
{
	InitializeCriticalSection(&lock);
}

void inline EnterCritical(bool& lock)
{
	Lock(lock);
}
void inline LeaveCritical(bool& lock)
{
	UnLock(lock);
}

void inline InitCritical(bool& lock)
{
	InitLock(lock);
}

#endif



typedef struct tagIMAGEFRAME {
	UCHAR	imageData[LIMIT_IMAGE_SIZE];
	INT		imageLength;
	INT		flag;		// 0: free  1: writing  2: recording/displaying

#ifdef LINUX_NAS
	pthread_mutex_t lock;
#else
	bool lock;		// use in recording/displaying
#endif

	BOOL	keyFrame;	// keyFrame 
	INT64	timeStamp;
	INT64	livetimeStamp;
	INT		count;
} IMAGEFRAME, RECORDINGIMAGE, KEYFRAMEIMAGE;

typedef struct tagCAMERARECEIVERINFO {
	INT		nModel;				// 카메라의 모델 : AXIS, KNY, CNB, GENOSYS, ...
	INT		nSourceNo;			// Source Number
	INT		nCameraStatus;		// 카메라의 상태 : UNKNOWN_STATUS, CONNECT_STATUS, DISCONNECT_STATUS
	INT		nVideoInIndex;		// 비데오인덱스
	CHAR	lpVideoName[128];	// 비데오이름

	INT		nHighLevel;			// 고해상도이미지
	INT		nMediumLevel;		// 중해상도이미지
	INT		nLowLevel;			// 저해상도이미지
	INT		nMotionLevel;		// 모션검출이미지
	INT		nRecordingLevel;	// 레코딩이미지
	INT		nThumbnailLevel;	// 썸네일이미지
	INT		nKeyFrameLevel;
	INT		nEventLevel;
} CAMERARECEIVERINFO, *LPCAMERARECEIVERINFO;

typedef struct tagRecImageFIFO {
	IMAGEFRAME		imageObject;
	SYSTEMTIME		recTime;
} RecImageFIFO;

class CRecordingFiFo
{
public:
	CRecordingFiFo(void);
	~CRecordingFiFo(void);

private:
	int				m_nSize;
	UCHAR			m_imageTotal[LIMIT_RECORD_FIFO_TOTAL_SIZE];
	UCHAR			m_imageTemp[LIMIT_IMAGE_SIZE];
	DWORD			m_imagePotion[LIMIT_RECORD_FIFO_COUNT];
	BYTE			m_imageStatus[LIMIT_RECORD_FIFO_COUNT];
	RecImageFIFO	m_popImage;

	int				m_nPushIndex;
	int				m_nPopIndex;
public:
#ifndef LINUX_NAS
	CRITICAL_SECTION	m_csFlag;
#else
	pthread_mutex_t     m_csFlag;
#endif

public:
	void push_back(DWORD dwImageLen, UCHAR* pImageData, BOOL bKeyFrame);
	RecImageFIFO* pop_front();
	BOOL isEmpty();
	void clear();
	int size();
};

class CLiveFiFo
{
public:
	CLiveFiFo(void);
	~CLiveFiFo(void);

private:
	int				m_nSize;
	UCHAR			m_imageTotal[LIMIT_LIVE_FIFO_TOTAL_SIZE];
	UCHAR			m_imageTemp[LIMIT_IMAGE_SIZE];
	UCHAR			m_imageTickCount[16];
	DWORD			m_imagePotion[LIMIT_LIVE_FIFO_COUNT];
	BYTE			m_imageStatus[LIMIT_LIVE_FIFO_COUNT];

	int				m_nChannelIndex;
	int				m_nPushIndex;
	int				m_nPopIndex;
	SYSTEMTIME		m_stLastTime;

	LONG			m_uFlag;
public:
#ifndef LINUX_NAS
	CRITICAL_SECTION	m_csFlag;
#else
	pthread_mutex_t     m_csFlag;
#endif

public:
	INT64			m_timestamp;
	INT64			m_livetimestamp;

	void push_back(UINT dwImageLen, UCHAR* pImageData, BOOL bKeyFrame);
	void pop_front(int& popIndex, UCHAR* fto, UINT* fimagelength);
	BOOL isEmpty();
	void clear();
	int size();
	void SetChannelIndex(int nIndex);
};

typedef struct tagLIVEIMAGE {
	CLiveFiFo channelData[MAX_QUALITY_LEVEL];
	DOUBLE	motionRate;
} LIVEIMAGE;

#ifdef LINUX_NAS
#define  Sleep(x) usleep(x*1000)
#define	 _timeb	  timeb
#endif
#endif