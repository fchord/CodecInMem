#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>

/*
#include <functional>
#include <fstream>
#include <inttypes.h>
#include <thread>
#include <mutex>
#include <string>
#include <map>
*/

#include "Utils.h"

using namespace std;

char *ResoEnumToStr(E_RESOLUTION resolution)
{
	if (resolution >= E_RESOLUTION_UNKNOWN || resolution < E_RESOLUTION_270P)
		return (char *)RESO_NAME_UNKNOWN;
	switch (resolution)
	{
	case E_RESOLUTION_270P:
		return (char *)RESO_NAME_270P;
	case E_RESOLUTION_360P:
		return (char *)RESO_NAME_360P;
	case E_RESOLUTION_576P:
		return (char *)RESO_NAME_576P;
	case E_RESOLUTION_720P:
		return (char *)RESO_NAME_720P;
	case E_RESOLUTION_1080P:
		return (char *)RESO_NAME_1080P;
	case E_RESOLUTION_2160P:
		return (char *)RESO_NAME_2160P;
	case E_RESOLUTION_UNIVERSAL:
		return (char *)RESO_NAME_UNIVERSAL;
	default:
		return (char *)RESO_NAME_UNKNOWN;
	}
}

int ResoWidth(E_RESOLUTION resolution)
{
	if (resolution >= E_RESOLUTION_UNKNOWN || resolution < E_RESOLUTION_270P)
		return -1;
	switch (resolution)
	{
	case E_RESOLUTION_270P:
		return 480;
	case E_RESOLUTION_360P:
		return 640;
	case E_RESOLUTION_576P:
		return 1024;
	case E_RESOLUTION_720P:
		return 1280;
	case E_RESOLUTION_1080P:
		return 1920;
	case E_RESOLUTION_2160P:
		return 3840;
	case E_RESOLUTION_UNIVERSAL:
		return 10000;
	default:
		return -1;
	}
}

int ResoHeight(E_RESOLUTION resolution)
{
	if (resolution >= E_RESOLUTION_UNKNOWN || resolution < E_RESOLUTION_270P)
		return -1;
	switch (resolution)
	{
	case E_RESOLUTION_270P:
		return 270;
	case E_RESOLUTION_360P:
		return 360;
	case E_RESOLUTION_576P:
		return 576;
	case E_RESOLUTION_720P:
		return 720;
	case E_RESOLUTION_1080P:
		return 1080;
	case E_RESOLUTION_2160P:
		return 2160;
	case E_RESOLUTION_UNIVERSAL:
		return 10000;
	default:
		return -1;
	}
}


int PixelBytes(E_PIX_FMT pixFmt)
{
	if (pixFmt < E_PIX_FMT_MIN || pixFmt > E_PIX_FMT_MAX)
		return -1;
	switch (pixFmt)
	{
	case E_PIX_FMT_YUV420:
		return 0;
	case E_PIX_FMT_ARGB:
		return 4;
	case E_PIX_FMT_RGB:
		return 3;
	}
	return 0;
}

LONGLONG getMsTime()
{
#if 0
	time_t timep;
	SYSTEMTIME sysTime;

	time(&timep);
	GetSystemTime(&sysTime);
	return (timep * 1000 + sysTime.wMilliseconds);
#else
	return clock();
#endif
}

LONGLONG getMilliseconds(SYSTEMTIME time)
{
	LONGLONG mTime;
	mTime = (((((time.wYear - 1970) * 365 + (time.wMonth - 1) * 31 + time.wDay) * 24 + time.wHour) * 60 + time.wMinute) * 60 + time.wSecond) * 1000 + time.wMilliseconds;
	return mTime;
}

int IsFileExistence(char *filePath, int* fileType)
{
	struct stat s;
	char *path = NULL;
	int i;

	if (!filePath || !fileType || (strlen(filePath) <= 0))
		return -1;

	path = (char *)malloc(strlen(filePath) + 1);
	memset(path, 0, strlen(filePath) + 1);
	for (i = 0; i < strlen(filePath); i++) {
		if (filePath[i] != '\\')
			path[i] = filePath[i];
		else
			path[i] = '/';
	}
	path[i] = '\0';
	//cout << "IsFileExistence. path: " << path;

	if (0 == stat(path, &s)) {
		if (s.st_mode & S_IFDIR) {
			*fileType = ST_MODE_DIR;
		}
		else if (s.st_mode & S_IFREG) {
			*fileType = ST_MODE_FILE;
		}
		else {
			*fileType = ST_MODE_OTHER;
		}
	}
	else {
		*fileType = ST_MODE_NOT_EXISTENCE;
	}
	//cout << "IsFileExistence. fileType: " << *fileType;
	if (path)
		free(path);
	return 0;
}

int IsFileExistenceWithOutime(char *filePath, int* fileType, int millisecond)
{
	struct stat s;
	char *path = NULL;
	int i, iSleepTime = 0;

	if (!filePath || !fileType || (strlen(filePath) <= 0) || millisecond < 0)
		return -1;

	path = (char *)malloc(strlen(filePath) + 1);
	memset(path, 0, strlen(filePath) + 1);
	for (i = 0; i < strlen(filePath); i++) {
		if (filePath[i] != '\\')
			path[i] = filePath[i];
		else
			path[i] = '/';
	}
	path[i] = '\0';
	cout << "IsFileExistence. path: " << path;

	do {
		if (0 == stat(path, &s)) {
			if (s.st_mode & S_IFDIR) {
				*fileType = ST_MODE_DIR;
			}
			else if (s.st_mode & S_IFREG) {
				*fileType = ST_MODE_FILE;
			}
			else {
				*fileType = ST_MODE_OTHER;
			}
		}
		else {
			*fileType = ST_MODE_NOT_EXISTENCE;
		}
		if (*fileType != ST_MODE_NOT_EXISTENCE)
			break;
		Sleep(100);
		iSleepTime += 100;
	} while (iSleepTime < millisecond);

	cout << "IsFileExistence. fileType: " << *fileType;
	if (path)
		free(path);
	return 0;
}

int GetFileSize(char *filePath, int* fileSize)
{
	struct stat s;
	char *path = NULL;
	int i, fileType;

	if (!filePath || !fileSize || (strlen(filePath) <= 0))
		return -1;

	path = (char *)malloc(strlen(filePath) + 1);
	memset(path, 0, strlen(filePath) + 1);
	for (i = 0; i < strlen(filePath); i++) {
		if (filePath[i] != '\\')
			path[i] = filePath[i];
		else
			path[i] = '/';
	}
	path[i] = '\0';
	cout << "GetFileSize. path: " << path;

	if (0 == stat(path, &s)) {
		if (s.st_mode & S_IFDIR) {
			fileType = ST_MODE_DIR;
		}
		else if (s.st_mode & S_IFREG) {
			fileType = ST_MODE_FILE;
			*fileSize = s.st_size;
		}
		else {
			fileType = ST_MODE_OTHER;
		}
	}
	else {
		fileType = ST_MODE_NOT_EXISTENCE;
	}
	cout << "GetFileSize. fileType: " << fileType << ", fileSize:" << *fileSize;
	if (path)
		free(path);
	if (ST_MODE_FILE == fileType) {
		return 0;
	}
	else
		return -2;

}

int CopyLocalFile(char *srcFilePath, char* dstPath)
{
	bool bRet;
	int i, j, ret, fileType = 0, requiredSize, fileNameLen = 0, dstFilePathLen = 0;
	char *pFileName = NULL, *dstFilePath = NULL;
	wchar_t *pwcSrcFilePath = NULL, *pwcDstFilePath = NULL;

	ret = IsFileExistence(srcFilePath, &fileType);
	if (ret < 0 || ST_MODE_FILE != fileType)
		return -1;
	ret = IsFileExistence(dstPath, &fileType);
	if (ret < 0 || ST_MODE_DIR != fileType)
		return -2;

	for (i = strlen(srcFilePath) - 1; i >= 0; i--) {
		if ('\\' == srcFilePath[i] || '/' == srcFilePath[i])
			break;
	}
	if (i < 0)
		return -3;
	fileNameLen = strlen(srcFilePath) - 1 - i;
	dstFilePathLen = strlen(dstPath) + fileNameLen;
	dstFilePath = (char *)malloc((dstFilePathLen + 1) * sizeof(char));
	memset(dstFilePath, 0, (dstFilePathLen + 1) * sizeof(char));
	strncpy(dstFilePath, dstPath, strlen(dstPath));
	for (j = strlen(dstFilePath), i += 1; j < dstFilePathLen; j++, i++) {
		dstFilePath[j] = srcFilePath[i];
	}
	cout << "CopyLocalFile. DstFilePath: " << dstFilePath;
	/*pFileName = (char *)malloc((fileNameLen + 1) * sizeof(char));
	if (!pFileName) {
		return -99;
	}
	memset(pFileName, 0, (fileNameLen + 1) * sizeof(char));
	strncpy(pFileName, srcFilePath + i + 1, fileNameLen);*/

	requiredSize = mbstowcs(NULL, srcFilePath, 0);
	pwcSrcFilePath = (wchar_t *)malloc((requiredSize + 1) * sizeof(wchar_t));
	if (!pwcSrcFilePath) {
		return -99;
	}
	memset(pwcSrcFilePath, 0, (requiredSize + 1) * sizeof(wchar_t));
	mbstowcs(pwcSrcFilePath, srcFilePath, requiredSize + 1);

	requiredSize = mbstowcs(NULL, dstFilePath, 0);
	pwcDstFilePath = (wchar_t *)malloc((requiredSize + 1) * sizeof(wchar_t));
	if (!pwcDstFilePath) {
		return -99;
	}
	memset(pwcDstFilePath, 0, (requiredSize + 1) * sizeof(wchar_t));
	mbstowcs(pwcDstFilePath, dstFilePath, requiredSize + 1);

	bRet = CopyFile(pwcSrcFilePath, pwcDstFilePath, false);
	cout << "CopyLocalFile. Src file " << pwcSrcFilePath << "copy to " << pwcDstFilePath << ", ret: " << bRet;
	if (true == bRet)
		return 0;
	else
		return -4;
}


int DeleteFileOrDir(char* pDir)
{
	struct _finddata_t fb;
	string check_path = "", circle_path = "", remove_path = "";
	LONGLONG handle = 0;
	int  noFile = 0, i = 0, fileType, ret, retValue;
	char *path = NULL;

	//先将字符串中的\转成/
	if (!pDir || strlen(pDir) <= 0)
		return -1;

	path = (char *)malloc(strlen(pDir) + 1);
	memset(path, 0, strlen(pDir) + 1);
	for (i = 0; i < strlen(pDir); i++) {
		if (pDir[i] != '\\')
			path[i] = pDir[i];
		else
			path[i] = '/';
	}
	path[i] = '\0';
	remove_path = std::string(path);

	ret = IsFileExistence(path, &fileType);
	if (ret < 0) {
		retValue = -2;
		goto delete_file;
	}
	if (ST_MODE_NOT_EXISTENCE == fileType) {
		retValue = -3;
		goto delete_file;
	}
	else if (ST_MODE_OTHER == fileType) {
		retValue = -4;
		goto delete_file;
	}
	else if (ST_MODE_FILE == fileType) {
		remove(path);
		retValue = 0;
		goto delete_file;
	}

	//开始递归删除目录
	if (remove_path.back() == '/') {
		check_path = remove_path + "*";
	}
	else
	{
		check_path = remove_path + "/*";
	}

	handle = _findfirst(check_path.c_str(), &fb);

	//cout << "the check_path = " << check_path << ", handle = " << handle << ",fb.name = " << fb.name << "fb.attrib = " << fb.attrib;

	//find the first matching file
	if (handle != -1)
	{
		//find next matching file
		while (0 == _findnext(handle, &fb))
		{
			// "." and ".." are not processed
			noFile = strcmp(fb.name, "..");

			if (0 != noFile)
			{
				if (remove_path.back() == '/') {
					circle_path = remove_path + fb.name;
				}
				else
				{
					circle_path = remove_path + +"/" + fb.name;
				}

				//fb.attrib == 16 means folder
				if (fb.attrib == 16)
				{
					DeleteFileOrDir((char *)circle_path.c_str());
				}
				else
				{
					remove(circle_path.c_str());
				}
			}
		}
	}
	else
	{
		cout << "no file in remove_path = " << remove_path.data();
	}
	_findclose(handle);
	_rmdir(remove_path.c_str());
	return 0;

delete_file:
	if (path) {
		free(path);
		path = NULL;
	}
	return retValue;
}
