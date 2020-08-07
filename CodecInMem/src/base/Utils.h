#pragma once
#include<windows.h>

#define MAX_NAME_LENGTH 128
#define MAX_STRING_LENGTH (32 * MAX_NAME_LENGTH)

typedef enum E_DATA_TYPE {
	E_DATA_TYPE_UNKNOWN,
	E_DATA_TYPE_MPEGTS,
	E_DATA_TYPE_MPEGPS,
	E_DATA_TYPE_H264,
	E_DATA_TYPE_RAWVIDEO,
	E_DATA_TYPE_MP3,
	E_DATA_TYPE_PCM,
	E_DATA_TYPE_POSITION,
	E_DATA_TYPE_EVENT
}E_DATA_TYPE;

typedef enum E_PIX_FMT {
	E_PIX_FMT_MIN,
	E_PIX_FMT_YUV420,
	E_PIX_FMT_ARGB,
	E_PIX_FMT_RGB,
	E_PIX_FMT_MAX
}E_PIX_FMT;

#define RESO_NAME_270P		"480x270p"
#define RESO_NAME_360P		"640x360p"
#define RESO_NAME_576P		"1024x576p"
#define RESO_NAME_720P		"1280x720p"
#define RESO_NAME_1080P		"1920x1080p"
#define RESO_NAME_2160P		"3840x2160p"
#define RESO_NAME_UNIVERSAL	"UniversalResolution"  // 10000x10000
#define RESO_NAME_UNKNOWN	"UnknowResolution"

typedef enum E_RESOLUTION {
	E_RESOLUTION_270P,
	E_RESOLUTION_360P,
	E_RESOLUTION_576P,
	E_RESOLUTION_720P,
	E_RESOLUTION_1080P,
	E_RESOLUTION_2160P,
	E_RESOLUTION_UNIVERSAL,
	E_RESOLUTION_UNKNOWN
}E_RESOLUTION;


char* ResoEnumToStr(E_RESOLUTION resolution);
int ResoWidth(E_RESOLUTION resolution);
int ResoHeight(E_RESOLUTION resolution);
//int PositionConverse(E_RESOLUTION srcReso, int srcX, int srcY, E_RESOLUTION dstReso, int* dstX, int* dstY);
int PixelBytes(E_PIX_FMT pixFmt);



#define ST_MODE_NOT_EXISTENCE 0
#define ST_MODE_DIR 1
#define ST_MODE_FILE 2
#define ST_MODE_OTHER 3

LONGLONG getMsTime();
LONGLONG getMilliseconds(SYSTEMTIME time);
int IsFileExistence(char *filePath, int* fileType);
int IsFileExistenceWithOutime(char *filePath, int* fileType, int millisecond);
int GetFileSize(char *filePath, int* fileSize);
int CopyLocalFile(char *srcFilePath, char* dstPath);
int DeleteFileOrDir(char* pDir);
