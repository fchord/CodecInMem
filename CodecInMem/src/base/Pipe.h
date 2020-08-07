#pragma once
#include <windows.h>
#include <thread>
#include <mutex>
#include "Utils.h"

#define PIPE_NAME_PREFIX "\\\\.\\Pipe\\"
#define PIPE_NAME L"\\\\.\\Pipe\\StreamerPipe"   //  \\.\Pipe\StreamerPipe

using namespace std;

class CPipe {
public:
	CPipe();
	~CPipe();
	int Init(char sName[MAX_NAME_LENGTH], int iBufSize);
	int Uninit();
	int Connect();
	int Disconnect();
	int Write(unsigned char* pData, int len);
	int Read(unsigned char* pData, int len);
	int Flush();
	HANDLE GetHandle();
	//int GetLength();

	char sPipeName[MAX_NAME_LENGTH] = {0};

private:
	HANDLE m_hPipe;
	int m_iBufSize = 0;


	thread *m_pThreadConnectPipe = NULL;
	int connect();
};