#pragma once
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <array>
#include "InOut.h"
#include "Pipe.h"
#include "Status.h"
#include "Buffer.h"

#define MAX_INPUT_NUM 8
#define MAX_OUTPUT_NUM MAX_INPUT_NUM
#define MAX_INOUT_NUM (MAX_OUTPUT_NUM + MAX_INPUT_NUM )
constexpr auto FFMPEG_PIPE_PREFIX = "FFmpegPipe_";
constexpr auto FFMPEG_LOG_PIPE_PREFIX = "FFmpegLOGPipe_";
constexpr auto DEFAULT_PIPE_SIZE = (64*1024);
constexpr auto MAX_INT = ((~(unsigned int)0) >> 1);
constexpr auto BLOCK_SIZE = 64 * 1024;
constexpr auto FFMPEG_LOG_SIZE = 1024 * 1024;

using namespace std;

class CFFmpegInOut : public CStatus {
public:
	friend class CFFmpegProcess;

	CFFmpegInOut(bool bInput, string sOption, CBuffer* pBuffer);
	CFFmpegInOut(bool bInput, string sOption, string sUrl);
	~CFFmpegInOut();
	bool IsInput();
	CBuffer* GetBuffer();
	bool IsInternalBuffer();
private:
	int start();
	int stop();
	int send();
	int sendTiny();
	int receive();
	int setAllowToCleanBuf(bool bAllow);
	string getUrl();

	static int m_sNumber;
	bool m_bInput;
	string m_sOption;
	string m_sUrl;

	CBuffer* m_pBuffer = NULL;
	bool m_bInternalBuffer = false;
	bool m_bAllowToClean = false;
	thread* pThreadWriteRead = NULL;

	string m_sPipeName;
	CPipe* m_pPipe;
};

class CFFmpegProcess : public CInOut , public CStatus{
public:
	/*
		bLimited: 数据是否是有限多。文件、固定大小内存数据为true，网络流为false；
		bAllowToDrop: 当输出CBuffer空间不足，是否允许清空CBuffer。为true时清空CBuffer再写入，为false时扩大CBuffer空间再写入，可能引起内存泄漏！
		应当还有第三种处理方式——延时写入。
	*/
	CFFmpegProcess(bool bLimited, bool bAllowToDrop);
	~CFFmpegProcess();

	// ffmpeg [global_options] {[input_file_options] -i input_url} ... {[output_file_options] output_url} ...
	int Init(
		string sGlobalOptions,
		array<CFFmpegInOut*, MAX_INPUT_NUM> arrInputPtrs,
		string sMiddleOptions,
		array<CFFmpegInOut*, MAX_OUTPUT_NUM> arrOutputPtrs,
		string sTailOptions
	);
	/*int Init(
		char sGlobalOptions[MAX_NAME_LENGTH],
		char sInputFileOptions[MAX_INPUT_NUM][MAX_NAME_LENGTH],
		char sInputUrl[MAX_INPUT_NUM][MAX_NAME_LENGTH],
		char sMiddleOptions[MAX_NAME_LENGTH],
		char sOutputFileOptions[MAX_OUTPUT_NUM][MAX_NAME_LENGTH],
		char sOutputUrl[MAX_OUTPUT_NUM][MAX_NAME_LENGTH],
		char sTailOptions[MAX_NAME_LENGTH],
		int iPipeBufSize[MAX_INOUT_NUM]
	);*/
	int Uninit();
	int Start();
	int Stop();
	float GetProgress();
private:
	array<CFFmpegInOut*, MAX_INPUT_NUM> m_arrInputs;
	array<CFFmpegInOut*, MAX_OUTPUT_NUM> m_arrOutputs;

	static int m_sNumber;
	bool m_bLimited;
	bool m_bAllowToDrop;
	__int64 m_llTotalLen = 0;
	string m_sFFmpegCmd;
	//char m_sFFmpegCmd[MAX_STRING_LENGTH] = { 0 };
	//int m_iFFmpegPipeNum = 0;
	char m_sFFmpegPipe[MAX_INOUT_NUM][MAX_NAME_LENGTH] = { 0 };
	int m_iPipeBufSize[MAX_INOUT_NUM] = { 0 };
	//CPipe* m_pPipe[MAX_INOUT_NUM] = { 0 };

	STARTUPINFO m_ScmStartupInfo;
	PROCESS_INFORMATION m_ScmProcessInfo;
	bool bProcessExistence = false;
	thread* pThreadMonitor = NULL;
	char m_sPipeWrapperName[MAX_NAME_LENGTH] = { 0 };
	CPipe* m_pPipeWrapper = NULL;
	int creatFFmpegProcess();
	void monitor();

	CBuffer* m_pFFmpegLogBuf = NULL;
	CPipe* m_pFFmpegLogPipe = NULL;
	string m_sFFmpegLogPipeName;
	thread* pThreadLog = NULL;
	void log();

};