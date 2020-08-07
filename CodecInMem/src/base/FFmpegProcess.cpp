#include "FFmpegProcess.h"
#include "Utils.h"

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	LOG(INFO) << "CtrlHandler: " << fdwCtrlType;
	switch (fdwCtrlType)
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			return TRUE;
		default:
			LOG(INFO) << "CtrlHandler. Received unknown windows signal " << fdwCtrlType;
			return FALSE;
	}
}


CFFmpegInOut::CFFmpegInOut(bool bInput, string sOption, CBuffer* pBuffer)
{
	char sPipeName[MAX_NAME_LENGTH] = { 0 };
	ostringstream  oStr;

	SetStatus(E_STATUS_INITING);
	m_bInput = bInput;
	m_sOption = sOption;
	if (pBuffer) {
		m_pBuffer = pBuffer;
		m_bInternalBuffer = false;
	}
	else
	{
		m_pBuffer = new CBuffer();
		m_bInternalBuffer = true;
	}

	oStr.clear();
	oStr << PIPE_NAME_PREFIX;
	oStr << FFMPEG_PIPE_PREFIX;
	oStr << m_sNumber;
	m_sNumber++;
	strcat(sPipeName, oStr.str().c_str());
	m_sPipeName.clear();
	m_sPipeName.append(sPipeName);


	m_sUrl.clear();
	SetStatus(E_STATUS_INITED);
	return;
}

CFFmpegInOut::CFFmpegInOut(bool bInput, string sOption, string sUrl)
{
	m_bInput = bInput;
	m_sOption = sOption;
	m_sUrl = sUrl;
	return;
}

CFFmpegInOut::~CFFmpegInOut()
{
	SetStatus(E_STATUS_UNINITING);
	if (m_pBuffer && m_bInternalBuffer)
		delete m_pBuffer;
	SetStatus(E_STATUS_UNINITED);
	return;
}


int CFFmpegInOut::start()
{
	char sName[MAX_NAME_LENGTH] = { 0 };

	if (!m_pBuffer)
		return 0;
	if (pThreadWriteRead)
		return -1;

	SetStatus(E_STATUS_STARTING);

	m_pPipe = new CPipe();
	strcpy(sName, m_sPipeName.c_str());
	m_pPipe->Init(sName, DEFAULT_PIPE_SIZE);
	m_pPipe->Connect();

	if (m_bInput)
		pThreadWriteRead = new thread(mem_fn(&CFFmpegInOut::send), this);
	else
		pThreadWriteRead = new thread(mem_fn(&CFFmpegInOut::receive), this);
	SetStatus(E_STATUS_RUNNING);
	return 0;

}

int CFFmpegInOut::stop()
{
	if (!m_pBuffer)
		return 0;
	if (pThreadWriteRead)
		return -1;

	SetStatus(E_STATUS_STOPPING);
	if (pThreadWriteRead && pThreadWriteRead->joinable()) {
		pThreadWriteRead->join();
		pThreadWriteRead = NULL;
	}
	if (m_pPipe) {
		m_pPipe->Disconnect();
		m_pPipe->Uninit();
		delete m_pPipe;
		m_pPipe = NULL;
	}
	SetStatus(E_STATUS_STOPPED);
	return 0;
}


bool CFFmpegInOut::IsInput()
{
	return m_bInput;
}

CBuffer* CFFmpegInOut::GetBuffer()
{
	return m_pBuffer;
}

bool CFFmpegInOut::IsInternalBuffer()
{
	return m_bInternalBuffer;
}

string CFFmpegInOut::getUrl()
{
	return m_sUrl;
}


int CFFmpegInOut::send()
{
	bool bLimited = true, bFinish = false;
	const int iBlockSize = BLOCK_SIZE;
	int ret = MAX_INT, n, len, length, iTotalLength = 0, iBufLen;
	unsigned char* pBlock = NULL;
	while (GetStatus() < E_STATUS_RUNNING)
		Sleep(5);
	if (GetStatus() >= E_STATUS_STOPPING)
		return ret;
	pBlock = new unsigned char[iBlockSize];
	while (1)
	{
		if (E_STATUS_RUNNING != GetStatus())
			break;

		length = m_pBuffer->Read(pBlock, iBlockSize);
		if (length < 0) {
			LOG(ERROR) << "send. Read m_pBuffer ERROR: " << length;
			ret = -1;
			break;
		}
		if (0 == length) {
			//Sleep(1);
			//continue;
			if (bLimited && m_pBuffer->GetLength() <= 0)
				bFinish = true;
		}
				
		len = 0;
		do {
			n = m_pPipe->Write(pBlock + len, length - len);
			if (n < 0) {
				LOG(ERROR) << "send. Write m_pPipe ERROR: " << n;
				ret = -2;
				break;
			}
			len += n;
			if (len >= length) {
				break;
			}
			if (E_STATUS_RUNNING != GetStatus())
				break;
		} while (1);
		iTotalLength += len;
		if (ret != MAX_INT)
			break;
		if (bFinish) {
			m_pPipe->Flush();
			LOG(INFO) << "send. Flushed!";
			break;
		}
			
	}
	if (pBlock)
		delete pBlock;
	
	return ret;
}

int CFFmpegInOut::sendTiny()
{
	bool bLimited = true, bFinish = false;
	const int iBlockSize = BLOCK_SIZE;
	int ret = MAX_INT, n, len, length, iTotalLength = 0, iBufLen;
	unsigned char* pBlock = NULL;
	pBlock = new unsigned char[iBlockSize];
	memset(pBlock, 0, iBlockSize);

	while (GetStatus() < E_STATUS_RUNNING)
		Sleep(5);
	if (GetStatus() >= E_STATUS_STOPPING) {
		delete pBlock;
		return ret;
	}
	if(m_pPipe)
		n = m_pPipe->Write(pBlock, iBlockSize);
	delete pBlock;
	return n;
}

int CFFmpegInOut::receive()
{
	const int iBlockSize = BLOCK_SIZE;
	int ret = MAX_INT, n, len, length, iBufLen;
	unsigned char* pBlock = NULL;
	while (GetStatus() < E_STATUS_RUNNING)
		Sleep(5);
	if (GetStatus() >= E_STATUS_STOPPING)
		return ret;
	pBlock = new unsigned char[iBlockSize];
	while (1)
	{
		if (E_STATUS_RUNNING != GetStatus())
			break;
		length = m_pPipe->Read(pBlock, iBlockSize);
		if (length < 0) {
			LOG(ERROR) << "receive. Read m_pPipe ERROR: " << length;
			ret = -1;
			break;
		}
		if (0 == length) {
			Sleep(1);
			continue;
		}
		//LOG(INFO) << "CFFmpegInOut::receive. length: " << length;

		len = 0;
		do {
			n = m_pBuffer->Write(pBlock + len, length - len);
			if (CBUFFER_ERROR_NOT_ENOUGH_STORAGE == n)
			{
				if (m_bAllowToClean) {
					LOG(INFO) << "receive. Clean m_pBuffer";
					m_pBuffer->Clean();
				}
				else {
					LOG(INFO) << "receive. EnlargeBuffer2Times m_pBuffer";
					m_pBuffer->EnlargeBuffer2Times();
				}
				continue;
			}
			else if (n < 0) {
				LOG(ERROR) << "receive. Write m_pBuffer ERROR: " << n;
				ret = -2;
				break;
			}
			len += n;
			if (len >= length) {
				break;
			}
			if (E_STATUS_RUNNING != GetStatus())
				break;
		} while (1);
		if (ret != MAX_INT)
			break;
	}
	if (pBlock)
		delete pBlock;
	return ret;
}

int CFFmpegInOut::setAllowToCleanBuf(bool bAllow)
{
	if (m_bInput)
		return -1;
	m_bAllowToClean = bAllow;
	return 0;
}

int CFFmpegInOut::m_sNumber = 0;

CFFmpegProcess::CFFmpegProcess(bool bLimited, bool bAllowToDrop)
{

	SetStatus(E_STATUS_UNINITED);
	m_bLimited = bLimited;
	m_bAllowToDrop = bAllowToDrop;
	//m_pFFmpegLogBuf = new CBuffer(FFMPEG_LOG_SIZE);
	//m_pFFmpegLogPipe = new CPipe();

	ostringstream  oStr;
	char sPipeName[MAX_NAME_LENGTH] = { 0 };
	oStr.clear();
	oStr << PIPE_NAME_PREFIX;
	oStr << FFMPEG_LOG_PIPE_PREFIX;
	oStr << m_sNumber;
	m_sFFmpegLogPipeName.clear();
	m_sFFmpegLogPipeName = oStr.str();
	m_sNumber++;

	m_pPipeWrapper = new CPipe();
	//SetConsoleMode(NULL, ENABLE_PROCESSED_INPUT);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	return;
}

CFFmpegProcess::~CFFmpegProcess()
{
	if (m_pFFmpegLogBuf)
		delete m_pFFmpegLogBuf;
	if (m_pFFmpegLogPipe)
		delete m_pFFmpegLogPipe;

	return;
}

int CFFmpegProcess::Init(
	string sGlobalOptions,
	array<CFFmpegInOut*, MAX_INPUT_NUM> arrInputPtrs,
	string sMiddleOptions,
	array<CFFmpegInOut*, MAX_OUTPUT_NUM> arrOutputPtrs,
	string sTailOptions
)
{
	int i = 0;
	if (GetStatus() != E_STATUS_UNINITED)
	{
		return -1;
	}
	SetStatus(E_STATUS_INITING);
	for (i = 0; i < arrInputPtrs.size(); i++)
	{
		if (arrInputPtrs[i] && !arrInputPtrs[i]->IsInput())
		{
			SetStatus(E_STATUS_UNINITED);
			return -1;
		}
	}
	for (i = 0; i < arrOutputPtrs.size(); i++)
	{
		if (arrOutputPtrs[i] && arrOutputPtrs[i]->IsInput())
		{
			SetStatus(E_STATUS_UNINITED);
			return -2;
		}
	}
	for (i = 0; i < arrInputPtrs.size(); i++)
	{
		if (arrInputPtrs[i] && !arrInputPtrs[i]->GetBuffer() && arrInputPtrs[i]->getUrl().empty())
		{
			SetStatus(E_STATUS_UNINITED);
			return -3;
		}
	}
	for (i = 0; i < arrOutputPtrs.size(); i++)
	{
		if (arrOutputPtrs[i] && !arrOutputPtrs[i]->GetBuffer() && arrOutputPtrs[i]->getUrl().empty())
		{
			SetStatus(E_STATUS_UNINITED);
			return -4;
		}
	}	
	m_arrInputs = arrInputPtrs;
	m_arrOutputs = arrOutputPtrs;

	m_sFFmpegCmd.clear();
	//m_sFFmpegCmd.append("ffmpeg ");
	m_sFFmpegCmd.append("FFmpegWrapper ");
	m_sFFmpegCmd.append(sGlobalOptions + " ");

	for (i = 0; i < m_arrInputs.size(); i++)
	{
		if (m_arrInputs[i])
		{
			m_sFFmpegCmd.append(m_arrInputs[i]->m_sOption + " ");
			m_sFFmpegCmd.append("-i ");
			if (m_arrInputs[i]->GetBuffer()) {
				m_sFFmpegCmd.append(m_arrInputs[i]->m_sPipeName + " ");
				m_llTotalLen += m_arrInputs[i]->GetBuffer()->GetLength();
			}
			else {
				m_sFFmpegCmd.append(m_arrInputs[i]->m_sUrl + " ");
			}
		}
	}
	m_sFFmpegCmd.append(sMiddleOptions + " ");
	for (i = 0; i < m_arrOutputs.size(); i++)
	{
		if (m_arrOutputs[i])
		{
			m_sFFmpegCmd.append(m_arrOutputs[i]->m_sOption + " ");
			m_sFFmpegCmd.append("-y ");
			if (m_arrOutputs[i]->GetBuffer()) {
				m_sFFmpegCmd.append(m_arrOutputs[i]->m_sPipeName + " ");
			}
			else {
				m_sFFmpegCmd.append(m_arrOutputs[i]->m_sUrl + " ");
			}
		}
	}
	m_sFFmpegCmd.append(sTailOptions + " ");
	LOG(INFO) << "Init. m_sFFmpegCmd: " << m_sFFmpegCmd;

	SetStatus(E_STATUS_INITED);
	return 0;
}

#if 0
int CFFmpegProcess::Init(
		char sGlobalOptions[MAX_NAME_LENGTH], 
		char sInputFileOptions[MAX_INPUT_NUM][MAX_NAME_LENGTH],
		char sInputUrl[MAX_INPUT_NUM][MAX_NAME_LENGTH],
		char sMiddleOptions[MAX_NAME_LENGTH],
		char sOutputFileOptions[MAX_OUTPUT_NUM][MAX_NAME_LENGTH],
		char sOutputUrl[MAX_OUTPUT_NUM][MAX_NAME_LENGTH],
		char sTailOptions[MAX_NAME_LENGTH],
		int iPipeBufSize[MAX_INOUT_NUM]
)
{
	int i = 0;
	SetStatus(E_STATUS_INITING);
	memset(m_sFFmpegCmd, 0, sizeof(m_sFFmpegCmd));
	strcat(m_sFFmpegCmd, "ffmpeg ");
	strcat(m_sFFmpegCmd, sGlobalOptions);
	strcat(m_sFFmpegCmd, " ");
	for (i = 0; i < MAX_INPUT_NUM; i++)
	{
		if (NULL != sInputFileOptions[i][0])
		{
			strcat(m_sFFmpegCmd, sInputFileOptions[i]);
			strcat(m_sFFmpegCmd, " ");
		}
		if (NULL != sInputUrl[i][0])
		{
			strcat(m_sFFmpegCmd, "-i ");
			strcat(m_sFFmpegCmd, sInputUrl[i]);
			strcat(m_sFFmpegCmd, " ");
			if (strstr(sInputUrl[i], PIPE_NAME_PREFIX) && m_iFFmpegPipeNum < MAX_INOUT_NUM) {
				strcat(m_sFFmpegPipe[m_iFFmpegPipeNum], sInputUrl[i]);
				m_iFFmpegPipeNum++;
			}
		}
	}
	strcat(m_sFFmpegCmd, " ");
	strcat(m_sFFmpegCmd, sMiddleOptions);
	strcat(m_sFFmpegCmd, " ");
	for (i = 0; i < MAX_OUTPUT_NUM; i++)
	{
		if (NULL != sOutputFileOptions[i][0])
		{
			strcat(m_sFFmpegCmd, sOutputFileOptions[i]);
			strcat(m_sFFmpegCmd, " ");
		}
		if (NULL != sOutputUrl[i][0])
		{
			strcat(m_sFFmpegCmd, sOutputUrl[i]);
			strcat(m_sFFmpegCmd, " ");
			if (strstr(sOutputUrl[i], PIPE_NAME_PREFIX) && m_iFFmpegPipeNum < MAX_INOUT_NUM) {
				strcat(m_sFFmpegPipe[m_iFFmpegPipeNum], sOutputUrl[i]);
				m_iFFmpegPipeNum++;
			}
		}
	}
	LOG(INFO) << "CFFmpegProcess::Init. m_sFFmpegCmd: " << m_sFFmpegCmd;
	for (int i = 0; i < m_iFFmpegPipeNum; i++)
	{
		m_iPipeBufSize[i] = iPipeBufSize[i];
		m_pPipe[i] = new CPipe();
		if (!m_pPipe[i]) {
			LOG(ERROR) << "Init. new CPipe failed.";
			SetStatus(E_STATUS_FAILED);
			return -1;
		}
		if (m_pPipe[i]->Init(m_sFFmpegPipe[i], m_iPipeBufSize[i]) < 0) {
			LOG(ERROR) << "Init. Init CPipe failed.";
			SetStatus(E_STATUS_FAILED);
			return -2;
		}
	}
	SetStatus(E_STATUS_INITED);
	return 0;
}
#endif

int CFFmpegProcess::Uninit()
{
	if (GetStatus() != E_STATUS_STOPPED)
	{
		return -1;
	}
	SetStatus(E_STATUS_UNINITING);

	m_arrInputs = { 0 };
	m_arrOutputs = { 0 };
	m_sFFmpegCmd.clear();
	SetStatus(E_STATUS_UNINITED);
	return 0;
}

int CFFmpegProcess::Start()
{
	int ret; 
	if (GetStatus() != E_STATUS_INITED)
	{
		return -1;
	}
	SetStatus(E_STATUS_STARTING);

	memset(m_sPipeWrapperName, 0, sizeof(m_sPipeWrapperName));
	strcat(m_sPipeWrapperName, "\\\\.\\Pipe\\sPipeWrapperName");
	m_pPipeWrapper->Init(m_sPipeWrapperName, 1024);
	m_pPipeWrapper->Connect();

	char sPipeName[MAX_NAME_LENGTH] = { 0 };
	strcpy(sPipeName, m_sFFmpegLogPipeName.c_str());
	if (m_pFFmpegLogPipe && (ret = m_pFFmpegLogPipe->Init(sPipeName, DEFAULT_PIPE_SIZE) < 0))
	{
		LOG(ERROR) << "Start. m_pFFmpegLogPipe Init failed. ret: " << ret;
	}
	if (m_pFFmpegLogPipe && (ret = m_pFFmpegLogPipe->Connect() < 0)) {
		LOG(ERROR) << "Start. m_pFFmpegLogPipe Connect failed. ret: " << ret;
	}
	if(m_pFFmpegLogBuf)
		m_pFFmpegLogBuf->Clean();

	for (int i = 0; i < m_arrInputs.size(); i++)
		if(m_arrInputs[i])
			m_arrInputs[i]->start();
	for (int i = 0; i < m_arrOutputs.size(); i++)
		if(m_arrOutputs[i])
			m_arrOutputs[i]->start();
	if (creatFFmpegProcess() < 0)
	{
		LOG(ERROR) << "Start. creatFFmpegProcess failed.";
		SetStatus(E_STATUS_FAILED);
		return -2;
	}	

	unsigned char* pBuf = new unsigned char[1 + sizeof(HANDLE) + sizeof(DWORD)];
	HANDLE hProcessHandle;
	DWORD dProcessID;
	hProcessHandle = m_ScmProcessInfo.hProcess;
	dProcessID = m_ScmProcessInfo.dwProcessId;
	memset(pBuf, 1, 0x01);
	memcpy(pBuf + 1, &hProcessHandle, sizeof(HANDLE));
	memcpy(pBuf + 1 + sizeof(HANDLE), &dProcessID, sizeof(DWORD));
	m_pPipeWrapper->Write(pBuf, 1 + sizeof(HANDLE) + sizeof(DWORD));
	if (pBuf)
	{
		//delete pBuf;
		//pBuf = NULL;
	}

	SetStatus(E_STATUS_RUNNING);
	return 0;
}

int CFFmpegProcess::Stop()
{
	bool bRet;
	int n;
	string sKillCmd;
	unsigned char buf = 0xFF;
	if (GetStatus() != E_STATUS_RUNNING)
	{
		return -1;
	}
	SetStatus(E_STATUS_STOPPING);
	LOG(INFO) << "CFFmpegProcess::Stop";
	//while (bProcessExistence)

	LOG(INFO) << "CFFmpegProcess::Stop. To write quit FFmpeg signal.";
	m_pPipeWrapper->Write(&buf, sizeof(buf));

	do {
		n = m_pPipeWrapper->Read(&buf, sizeof(buf));
		if (n <= 0)
		{
			Sleep(1);
			continue;
		}
		if (buf == 0xFE) {
			LOG(INFO) << "CFFmpegProcess::Stop. FFmpegWrapper has sent Ctrl+Break signal, but not quit.";
			for (int i = 0; i < m_arrInputs.size(); i++)
				if (m_arrInputs[i]) {
					m_arrInputs[i]->sendTiny();
				}
			Sleep(5);
			continue;
		}
		else if (buf == 0xFD) {
			LOG(INFO) << "CFFmpegProcess::Stop. FFmpegWrapper has quit.";
			break;
		}
	} while (1);
	//Sleep(50);
	Sleep(50);
	for (int i = 0; i < m_arrInputs.size(); i++)
	{

		if (m_arrInputs[i]) {
			m_arrInputs[i]->stop();
		}
	}
	for (int i = 0; i < m_arrOutputs.size(); i++)
	{
		if (m_arrOutputs[i])
			m_arrOutputs[i]->stop();
	}
	
	LOG(INFO) << "111";

	if (0)  //终止子进程 
	{
		//GetConsoleCursorInfo();

#if 0
		//&& !TerminateProcess(m_ScmProcessInfo.hProcess, 300)
		//bRet = AttachConsole(m_ScmProcessInfo.dwProcessId);
		//LOG(INFO) << "CFFmpegProcess::Stop. AttachConsole: " << bRet;
		bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0);
		LOG(INFO) << "CFFmpegProcess::Stop. GenerateConsoleCtrlEvent: " << bRet;
		Sleep(500);
		bRet = TerminateProcess(m_ScmProcessInfo.hProcess, 300);
		LOG(INFO) << "CFFmpegProcess::Stop. TerminateProcess: " << bRet;

#elif 0
		sKillCmd.append(string("taskkill /T /pid  ") + to_string(m_ScmProcessInfo.dwProcessId));
		LOG(INFO) << "sKillCmd: " << sKillCmd.c_str();
		system(sKillCmd.c_str());
		Sleep(500);
		bRet = TerminateProcess(m_ScmProcessInfo.hProcess, 300);
		LOG(INFO) << "CFFmpegProcess::Stop. TerminateProcess: " << bRet;
#endif
		SetStatus(E_STATUS_FAILED);
		return -2;
	}
	LOG(INFO) << "222";

	/*if (pThreadMonitor && pThreadMonitor->joinable())
	{
		pThreadMonitor->join();
		pThreadMonitor = NULL;
	}*/
	LOG(INFO) << "333";
	m_pPipeWrapper->Disconnect();
	m_pPipeWrapper->Uninit();

	//bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_ScmProcessInfo.dwProcessId);

	if(m_pFFmpegLogPipe)
		m_pFFmpegLogPipe->Disconnect();
	if(m_pFFmpegLogPipe)
		m_pFFmpegLogPipe->Uninit();

	if(m_pFFmpegLogBuf)
		m_pFFmpegLogBuf->WriteFile(string("E:\\SwDevelop\\CodecInMem\\love.log"), 0);
	SetStatus(E_STATUS_STOPPED);
	return  0;
}

int CFFmpegProcess::creatFFmpegProcess()
{
	wchar_t* wcCmd = NULL;
	wchar_t wTitle[128] = L"ModiFFmpegWrapper";
	size_t wcCmdSize = 0, convertedChars = 0;

	LOG(INFO) << "creatFFmpegProcess.";
	ZeroMemory(&m_ScmStartupInfo, sizeof(m_ScmStartupInfo));
	//m_ScmStartupInfo.cb = sizeof(STARTUPINFO);
	m_ScmStartupInfo.lpTitle = (LPWSTR)&wTitle;
	m_ScmStartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
	m_ScmStartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	//m_ScmStartupInfo.dwFlags |= CREATE_NEW_CONSOLE;
	m_ScmStartupInfo.dwFlags |= CREATE_NEW_PROCESS_GROUP;
	m_ScmStartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	m_ScmStartupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	m_ScmStartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	//m_ScmStartupInfo.hStdOutput = m_pFFmpegLogPipe->GetHandle();
	//m_ScmStartupInfo.hStdError = m_pFFmpegLogPipe->GetHandle();
	//m_ScmStartupInfo.hStdInput = m_pFFmpegLogPipe->GetHandle();
	m_ScmStartupInfo.wShowWindow = SW_SHOW;
	ZeroMemory(&m_ScmProcessInfo, sizeof(m_ScmProcessInfo));
	wcCmdSize = (strlen(m_sFFmpegCmd.c_str()) + 1) * sizeof(char);
	wcCmd = new wchar_t[wcCmdSize];
	ZeroMemory(wcCmd, wcCmdSize);
	mbstowcs_s(&convertedChars, wcCmd, wcCmdSize, m_sFFmpegCmd.c_str(), _TRUNCATE);
	LOG(INFO) << "creatFFmpegProcess. FFmpeg command is: " << m_sFFmpegCmd;

	if (CreateProcess(
		NULL,		//    指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
		wcCmd,		//    命令行字符串  
		NULL,		//    指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。
		NULL,		//    如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>  
		false,		//    指示新进程是否从调用进程处继承了句柄。  
		CREATE_NEW_CONSOLE,			//    指定附加的、用来控制优先类和进程的创建的标  
					//    CREATE_NEW_CONSOLE  新控制台打开子进程  
					//    CREATE_SUSPENDED    子进程创建后挂起，直到调用ResumeThread函数  
		NULL,		//    指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
		NULL,		//    指定子进程的工作路径  //L"E:\\SwDevelop\\CodecInMem\\"
		&m_ScmStartupInfo,	// 决定新进程的主窗体如何显示的STARTUPINFO结构体  
		&m_ScmProcessInfo	// 接收新进程的识别信息的PROCESS_INFORMATION结构体  
	))
	{
		LOG(INFO) << "creatFFmpegProcess successfully. Process ID: " << m_ScmProcessInfo.dwProcessId;
		bProcessExistence = true;
		pThreadMonitor = new thread(mem_fn(&CFFmpegProcess::monitor), this);
		//pThreadLog = new thread(mem_fn(&CFFmpegProcess::log), this);
	}
	else {
		LOG(ERROR) << "creatFFmpegProcess failed.";
		bProcessExistence = false;
		if (wcCmd) {
			delete wcCmd;
			wcCmd = NULL;
		}
		return -1;
	}
	if (wcCmd) {
		delete wcCmd;
		wcCmd = NULL;
	}
	return 0;
}

#if 0
int CFFmpegProcess::Write(char sInputPipe[MAX_NAME_LENGTH], unsigned char* pData, int len)
{
	int iPipeNum = 0;
	if (E_STATUS_RUNNING != GetStatus())
		return 0;
	for (iPipeNum = 0; iPipeNum < MAX_INOUT_NUM; iPipeNum++) {
		if(0 == strcmp(m_sFFmpegPipe[iPipeNum], sInputPipe))
			break;
	}
	if (iPipeNum < 0 || iPipeNum >= m_iFFmpegPipeNum)
		return -1;
	if (!pData || len < 0)
		return -2;
	return m_pPipe[iPipeNum]->Write(pData, len);
}

int CFFmpegProcess::Read(char sOutputPipe[MAX_NAME_LENGTH], unsigned char* pData, int len)
{
	int iPipeNum = 0;
	if (E_STATUS_RUNNING != GetStatus())
		return 0;
	for (iPipeNum = 0; iPipeNum < MAX_INOUT_NUM; iPipeNum++) {
		if (0 == strcmp(m_sFFmpegPipe[iPipeNum], sOutputPipe))
			break;
	}
	if (iPipeNum < 0 || iPipeNum >= m_iFFmpegPipeNum)
		return -1;
	if (!pData || len < 0)
		return -2;
	return m_pPipe[iPipeNum]->Read(pData, len);
}
#endif

float CFFmpegProcess::GetProgress()
{
	int iCurLen = 0, iPipeSize = 0;
	float fProgress = 0.0;
	if (!m_bLimited)
		return -1.0;
	for (int i = 0; i < m_arrInputs.size(); i++)
	{
		if (m_arrInputs[i] && m_arrInputs[i]->m_pBuffer) {
			iCurLen += m_arrInputs[i]->m_pBuffer->GetLength();
		}
		if (m_arrInputs[i] && m_arrInputs[i]->m_pPipe) {
			iPipeSize += DEFAULT_PIPE_SIZE;
		}
	}
	if (0 != iPipeSize) {
		/*if (iCurLen < iPipeSize) {
			iCurLen = iPipeSize;
		}*/
		if(0 != m_llTotalLen)
			fProgress = 1.0 - (float)iCurLen / m_llTotalLen;
		if (fProgress >= 1.0)
		{
			for (int i = 0; i < m_arrInputs.size(); i++)
				if (m_arrInputs[i] && m_arrInputs[i]->m_pPipe) {
					m_arrInputs[i]->m_pPipe->Flush();
				}
		}
	}
	if (!bProcessExistence)
		fProgress = 1.0;
	return fProgress;
}

void CFFmpegProcess::monitor()
{
	Sleep(100);
	WaitForSingleObject(m_ScmProcessInfo.hProcess, INFINITE);
	LOG(INFO) << "monitor. FFmpeg process quit.";
	bProcessExistence = false;
	return;
}

int CFFmpegProcess::m_sNumber = 0;

void CFFmpegProcess::log()
{
	const int iBufSize = 128;
	int n, length, len;
	unsigned char buf[iBufSize];
	LOG(INFO) << "CFFmpegProcess::log";
	while (!bProcessExistence)
		Sleep(1);
	while (1)
	{
		if (!bProcessExistence)
			break;
		LOG(INFO) << "CFFmpegProcess::log. 1";

		if (m_pFFmpegLogPipe && m_pFFmpegLogBuf)
		{
			LOG(INFO) << "CFFmpegProcess::log. 2";

			memset(buf, 0, iBufSize);
			n = m_pFFmpegLogPipe->Read(buf, iBufSize);
			if (n <= 0) {
				Sleep(1);
				continue;
			}
			LOG(INFO) << "FFmpegLog. " << buf;
			length = 0;
			do {
				len = m_pFFmpegLogBuf->Write(buf + length, n - length);
				if (CBUFFER_ERROR_NOT_ENOUGH_STORAGE == len)
				{
					m_pFFmpegLogBuf->Clean();
					continue;
				}
				else if (len < 0)
				{
					break;
				}
				length += len;
				if (length >= n)
					break;
			} while (1);
		}
		else {
			Sleep(1);
		}
	}
}