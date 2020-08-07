// FFmpegWrapper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <csignal>

#include "Pipe.h"
#include "FFmpegWrapper.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"


int FFmpegWrapper::creatFFmpegProcess()
{
	wchar_t* wcCmd = NULL;
	size_t wcCmdSize = 0, convertedChars = 0;

	LOG(INFO) << "creatFFmpegProcess.";
	ZeroMemory(&m_ScmStartupInfo, sizeof(m_ScmStartupInfo));
	//m_ScmStartupInfo.cb = sizeof(STARTUPINFO);
	//m_ScmStartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
	//m_ScmStartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	//m_ScmStartupInfo.dwFlags |= CREATE_NEW_CONSOLE;
	//m_ScmStartupInfo.dwFlags |= CREATE_NEW_PROCESS_GROUP;
	//m_ScmStartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	//m_ScmStartupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	//m_ScmStartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	//m_ScmStartupInfo.hStdOutput = m_pFFmpegLogPipe->GetHandle();
	//m_ScmStartupInfo.hStdError = m_pFFmpegLogPipe->GetHandle();
	//m_ScmStartupInfo.hStdInput = m_pFFmpegLogPipe->GetHandle();
	//m_ScmStartupInfo.wShowWindow = SW_HIDE;
	ZeroMemory(&m_ScmProcessInfo, sizeof(m_ScmProcessInfo));
	wcCmdSize = (strlen(sFFmpegCmd.c_str()) + 1) * sizeof(char);
	wcCmd = new wchar_t[wcCmdSize];
	ZeroMemory(wcCmd, wcCmdSize);
	mbstowcs_s(&convertedChars, wcCmd, wcCmdSize, sFFmpegCmd.c_str(), _TRUNCATE);
	LOG(INFO) << "creatFFmpegProcess. FFmpeg command is: " << sFFmpegCmd;

	if (CreateProcess(
		NULL,		//    指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
		wcCmd,		//    命令行字符串  
		NULL,		//    指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。
		NULL,		//    如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>  
		false,		//    指示新进程是否从调用进程处继承了句柄。  
		0,			//    指定附加的、用来控制优先类和进程的创建的标  
					//    CREATE_NEW_CONSOLE  新控制台打开子进程  
					//    CREATE_SUSPENDED    子进程创建后挂起，直到调用ResumeThread函数  
		NULL,		//    指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
		NULL,		//    指定子进程的工作路径  //L"E:\\SwDevelop\\CodecInMem\\"
		&m_ScmStartupInfo,	// 决定新进程的主窗体如何显示的STARTUPINFO结构体  
		&m_ScmProcessInfo	// 接收新进程的识别信息的PROCESS_INFORMATION结构体  
	))
	{
		LOG(INFO) << "creatFFmpegProcess successfully.";
		bProcessExistence = true;
		pThreadMonitor = new thread(mem_fn(&FFmpegWrapper::monitor), this);
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

int FFmpegWrapper::distoryFFmpegProcess()
{
	TerminateProcess(m_ScmProcessInfo.hProcess, 300);
	return 0;
}


void FFmpegWrapper::monitor()
{
	Sleep(100);
	LOG(INFO) << "Start monitor.";
	WaitForSingleObject(m_ScmProcessInfo.hProcess, INFINITE);
	LOG(INFO) << "monitor. FFmpeg process quit.";
	bProcessExistence = false;
	//ExitProcess(0);
	return;
}


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
			LOG(INFO) << "CtrlHandler. Received windows signal " << fdwCtrlType;
			//ExitProcess(0);
			return TRUE;
		default:
			LOG(INFO) << "CtrlHandler. Received unknown windows signal " << fdwCtrlType;
			return FALSE;
	}
}

void signalHandler(int signum)
{
	cout << "Interrupt signal (" << signum << ") received.\n";

	// 清理并关闭
	// 终止程序 

	//exit(signum);

}


int main(int argc, char* *argv)
{
	int i = 0;
	DWORD n;
	HANDLE m_hPipe;
	unsigned char buf = 0x0;
	bool bRet;
	FFmpegWrapper* pFFmpegWrapper = NULL;
	HANDLE hProcessHandle;
	DWORD dProcessID;

	//SetConsoleCtrlHandler(NULL, FALSE);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),
		ENABLE_PROCESSED_INPUT 
		| ENABLE_LINE_INPUT
		| ENABLE_ECHO_INPUT
		| ENABLE_WINDOW_INPUT);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),
		ENABLE_PROCESSED_OUTPUT
		| ENABLE_WRAP_AT_EOL_OUTPUT);
	SetConsoleMode(GetStdHandle(STD_ERROR_HANDLE),
		ENABLE_PROCESSED_OUTPUT
		| ENABLE_WRAP_AT_EOL_OUTPUT);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)NULL, TRUE); //CtrlHandler

	signal(SIGINT, signalHandler);

    std::cout << "Hello World! argc: " << argc << " \n";


	if (WaitNamedPipe(PIPE_WRAPPER_NAME, NMPWAIT_WAIT_FOREVER) == FALSE)
	{
		LOG(INFO) << "等待命名管道实例失败！" << endl;
		return -1;
	}
	m_hPipe = CreateFile(PIPE_WRAPPER_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		LOG(INFO) << "创建命名管道失败！" << endl;
		CloseHandle(m_hPipe);
		return -2;
	}
	LOG(INFO) << "与服务器连接成功！" << endl;

	pFFmpegWrapper = new FFmpegWrapper();
	for (i = 1; i < argc; i++)
	{
		pFFmpegWrapper->sFFmpegCmd.append(string(" ") + string(argv[i]));
	}
	pFFmpegWrapper->creatFFmpegProcess();

	LOG(INFO) << "pFFmpegWrapper creatFFmpegProcess finish.";
	do {
		bRet = ReadFile(m_hPipe, &buf, 1, &n, NULL);
		//LOG(INFO) << "buf: " << buf;
		if (0xFF == buf)
			break;
		else if (0x1 == buf) {
			bRet = ReadFile(m_hPipe, &hProcessHandle, sizeof(hProcessHandle), &n, NULL);
			LOG(INFO) << "Read process handle. n: " << n;
			bRet = ReadFile(m_hPipe, &dProcessID, sizeof(dProcessID), &n, NULL);
			LOG(INFO) << "Read process id. n: " << n << ", dProcessID: " << dProcessID;
		}
	} while (1);
	LOG(INFO) << "FFmpegWrapper receive quit command!";


	//FreeConsole();
	//GetProcessId();
	//GetCurrentProcessId()
	//pFFmpegWrapper->m_ScmProcessInfo.dwProcessId

	//raise(SIGINT);
	//LOG(INFO) << "CFFmpegProcess::Stop. raise  SIGINT";

	//Sleep(1000);
	//dProcessID
	//

	if (!pFFmpegWrapper->bProcessExistence) {
		goto Exit;
	}
	bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pFFmpegWrapper->m_ScmProcessInfo.dwProcessId);
	LOG(INFO) << "CFFmpegProcess::Stop. 1 GenerateConsoleCtrlEvent: " << bRet;
	if (!bRet) {
		LOG(INFO) << "Error code: " << GetLastError();
	}
	Sleep(10);
	if (!pFFmpegWrapper->bProcessExistence) {
		goto Exit;
	}

	while (pFFmpegWrapper->bProcessExistence) {
		buf = 0xFE; //FFmpeg 进程还在，需要继续送空数据
		bRet = WriteFile(m_hPipe, &buf, 1, &n, NULL);
		Sleep(50);
		LOG(INFO) << "bProcessExistence: " << pFFmpegWrapper->bProcessExistence;
		if (pFFmpegWrapper->bProcessExistence) {
			bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pFFmpegWrapper->m_ScmProcessInfo.dwProcessId);
			LOG(INFO) << "CFFmpegProcess::Stop. 1 GenerateConsoleCtrlEvent: " << bRet;
			if (!bRet) {
				LOG(INFO) << "Error code: " << GetLastError();
			}
		}
		else {
			break;
		}
		
	}
	LOG(INFO) << "bProcessExistence: " << pFFmpegWrapper->bProcessExistence;
	buf = 0xFD; //FFmpeg 进程已经退出了
	bRet = WriteFile(m_hPipe, &buf, 1, &n, NULL);
	FlushFileBuffers(m_hPipe);
	Sleep(10);

Exit:
	CloseHandle(m_hPipe);
	DisconnectNamedPipe(m_hPipe);
	LOG(INFO) << "FFmpegWrapper Quit!";

#if 0
	Sleep(1000);
	bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pFFmpegWrapper->m_ScmProcessInfo.dwProcessId);
	LOG(INFO) << "CFFmpegProcess::Stop. 2 GenerateConsoleCtrlEvent: " << bRet;
	if (!bRet) {
		LOG(INFO) << "Error code: " << GetLastError();
	}
	Sleep(1000);
	bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pFFmpegWrapper->m_ScmProcessInfo.dwProcessId);
	LOG(INFO) << "CFFmpegProcess::Stop. 3 GenerateConsoleCtrlEvent: " << bRet;
	if (!bRet) {
		LOG(INFO) << "Error code: " << GetLastError();
	}
	Sleep(1000);
	bRet = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pFFmpegWrapper->m_ScmProcessInfo.dwProcessId);
	LOG(INFO) << "CFFmpegProcess::Stop. 4 GenerateConsoleCtrlEvent: " << bRet;
	if (!bRet) {
		LOG(INFO) << "Error code: " << GetLastError();
	}
	//pFFmpegWrapper->distoryFFmpegProcess();
#endif
	//Sleep(100 * 1000);
	//system("PAUSE");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
