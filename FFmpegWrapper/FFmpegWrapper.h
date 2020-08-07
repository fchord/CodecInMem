#include <windows.h>
#include <thread>
#include <mutex>

#define PIPE_WRAPPER_NAME L"\\\\.\\Pipe\\sPipeWrapperName"
using namespace std;


class FFmpegWrapper {
public:
	FFmpegWrapper() { return; };
	~FFmpegWrapper() { return; };
	int creatFFmpegProcess();
	int distoryFFmpegProcess();
	string sFFmpegCmd = string("ffmpeg");
	STARTUPINFO m_ScmStartupInfo;
	PROCESS_INFORMATION m_ScmProcessInfo;
	bool bProcessExistence = false;


private:


	thread* pThreadMonitor = NULL;
	void monitor();

};