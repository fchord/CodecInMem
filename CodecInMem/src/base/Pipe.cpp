#include <Windows.h>
#include <iostream>
#include "Pipe.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"


CPipe::CPipe()
{
	return;
}

CPipe::~CPipe()
{
	return;
}

int CPipe::Init(char sName[MAX_NAME_LENGTH], int iBufSize)
{
	DWORD ReadNum, WriteNum;
	BYTE sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
	SECURITY_ATTRIBUTES sa;
	//int wcSize = 4 * 1024;
	size_t wcCmdSize = 0;
	wchar_t* wcCmd = NULL;
	size_t convertedChars = 0;

	if (iBufSize <= 0 || strlen(sName) <= 0)
		return -1;
	memset(sPipeName, 0, sizeof(sPipeName));
	strncpy(sPipeName, sName, MAX_NAME_LENGTH);
	m_iBufSize = iBufSize;

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

	wcCmdSize = (strlen(sPipeName) + 1) * sizeof(char);
	wcCmd = new wchar_t[wcCmdSize];
	mbstowcs_s(&convertedChars, wcCmd, wcCmdSize, sPipeName, _TRUNCATE);
	m_hPipe = CreateNamedPipe(wcCmd, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, m_iBufSize, &sa);
	if (wcCmd)
		delete wcCmd;
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		LOG(INFO) << "CPipe::Init. CreateNamedPipe failed!" << GetLastError();
		CloseHandle(m_hPipe);
		return -1;
	}
	LOG(INFO) << "CPipe::Init. CreateNamedPipe successfully£¡";

	return 0;
}

int CPipe::Uninit()
{
	//LOG(INFO) << "CPipe::Uninit.";
	if (m_pThreadConnectPipe && m_pThreadConnectPipe->joinable()) {
		m_pThreadConnectPipe->join();
	}
	if (INVALID_HANDLE_VALUE != m_hPipe) {
		CloseHandle(m_hPipe);
		m_hPipe = NULL;
	}
	return 0;
}

int CPipe::Connect()
{
	m_pThreadConnectPipe = new thread(mem_fn(&CPipe::connect), this);
	if (!m_pThreadConnectPipe)
		return -1;
	return 0;

}

int CPipe::connect()
{
	LOG(INFO) << "CPipe::connect.";
	if (ConnectNamedPipe(m_hPipe, NULL) == FALSE)
	{
		LOG(INFO) << "ConnectNamedPipe failed.";
		CloseHandle(m_hPipe);
		return -1;
	}
	LOG(INFO) << "ConnectNamedPipe successfully.";
	return 0;
}

int CPipe::Disconnect()
{
	LOG(INFO) << "CPipe::Disconnect.";
	DisconnectNamedPipe(m_hPipe);
	LOG(INFO) << "DisconnectNamedPipe successfully.";
	return 0;

}

int CPipe::Write(unsigned char *pData, int len)
{
	DWORD n;
	bool bRet;
	bRet = WriteFile(m_hPipe, pData, len, &n, NULL);
	if (bRet) {
		return (int)n;
	}
	else {
		return -1;
	}
}

int CPipe::Read(unsigned char* pData, int len)
{
	DWORD n = 0, dBytesRead = 0, dTotalBytesAvail = 0;
	bool bRet;
	PeekNamedPipe(m_hPipe, NULL, NULL, &dBytesRead, &dTotalBytesAvail, NULL);
	if (0 == dTotalBytesAvail) {
		return 0;
	}
	bRet = ReadFile(m_hPipe, pData, len, &n, NULL);
	if (bRet) {
		return (int)n;
	}
	else {
		return -1;
	}
}

/*int CPipe::GetLength()
{
	return -1;
}*/


HANDLE CPipe::GetHandle()
{
	return m_hPipe;
}

int CPipe::Flush()
{
	FlushFileBuffers(m_hPipe);
	return 0;
}