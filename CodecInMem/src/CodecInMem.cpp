// CodecInMem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "FFmpegProcess.h"

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

//#define INPUT_URL "E:\\SwDevelop\\CodecInMem\\20191030T030327.ps"
#define INPUT_URL "E:/Media/love.mkv"
//#define OUTPUT_URL "E:\\SwDevelop\\CodecInMem\\20191030T030327.mkv"
#define OUTPUT_URL "E:/SwDevelop/CodecInMem/love.ts"

using namespace std;

void func1()
{
	int count = 0, ret, length, len;
	string sGlobalOptions, sMiddleOptions, sTailOptions;
	array<CFFmpegInOut*, MAX_INPUT_NUM> arrIn = { NULL }, arrOut = { NULL };
	CFFmpegProcess* pFFmpegProcess = NULL;
	CBuffer* pInputBuf = new CBuffer();
	CBuffer* pOutputBuf = new CBuffer(4 * 1024 * 1024);
	CBuffer* pRawBuf = new CBuffer(1024 * 1024);
	CPipe* pRawPipe = new CPipe();

	const int iSize = 1024 * 1024;
	int n;
	unsigned char* pBuf = new unsigned char[iSize];
	char sName[MAX_NAME_LENGTH] = "\\\\.\\Pipe\\RawPipe";
	time_t time1, time2;
	ret = pRawPipe->Init(sName, 128 * 1024 * 1024);
	LOG(INFO) << "pRawPipe Init: " << ret;
	ret = pRawPipe->Connect();
	LOG(INFO) << "pRawPipe Connect: " << ret;
	Sleep(8000);

	sGlobalOptions.append("-report -loglevel repeat+level+info ");
	pInputBuf->ReadFile(string("E:\\SwDevelop\\CodecInMem\\20191030T030327.ps"), 0);
	arrIn[0] = new CFFmpegInOut(true, string("-probesize 100000"), string("E:\\SwDevelop\\CodecInMem\\20191119T175552.wav.jpg"));
	sMiddleOptions.append("");
	arrOut[0] = new CFFmpegInOut(false, string("-an -f rawvideo -s 720x480 -pix_fmt yuv420p"), pOutputBuf);
	pFFmpegProcess = new CFFmpegProcess(true, false);
	ret = pFFmpegProcess->Init(sGlobalOptions, arrIn, sMiddleOptions, arrOut, sTailOptions);
	LOG(INFO) << "Init. ret: " << ret;
	ret = pFFmpegProcess->Start();
	LOG(INFO) << "Start. ret: " << ret;
	//WinExec("ffplay -f rawvideo -pix_fmt yuv420p  -s 720x576  -i \\\\.\\Pipe\\RawPipe", 1);
	time1 = clock();
	Sleep(50);
	while (1)
	{
		time2 = clock();
		if (time2 - time1 > 3000)
			break;
		if (pOutputBuf->GetLength() > 0)
		{
			length = pOutputBuf->Read(pBuf, iSize);
			if (length <= 0) {
				Sleep(2);
				continue;
			}
			len = 0;
			do {
				n = pRawPipe->Write(pBuf + len, length - len);
				time2 = clock();
				if (time2 - time1 > 3000)
					break;
				if (n < 0) {
					continue;
					//LOG(ERROR) << "pRawPipe->Write. n: " << n;
				}
				len += n;
			} while (len < length);

		}
		else
		{
			Sleep(1);
		}
	}
	ret = pFFmpegProcess->Stop();
	LOG(INFO) << "Stop. ret: " << ret;
	ret = pFFmpegProcess->Uninit();
	LOG(INFO) << "Uninit. ret: " << ret;
	Sleep(10000000);

	return;
}


void func2()
{
	int count = 0, ret, length, len;
	string sGlobalOptions, sMiddleOptions, sTailOptions;
	array<CFFmpegInOut*, MAX_INPUT_NUM> arrIn = { NULL }, arrOut = { NULL };
	CFFmpegProcess* pFFmpegProcess = NULL;
	CBuffer* pInputBuf = new CBuffer();
	CBuffer* pOutputBuf = new CBuffer(4 * 1024 * 1024);
	CBuffer* pRawBuf = new CBuffer(1024 * 1024);
	CPipe* pRawPipe = new CPipe();


	sGlobalOptions.append("-report -loglevel repeat+level+debug ");
	pInputBuf->ReadFile(string("E:\\SwDevelop\\CodecInMem\\love-5s.ts"), 0);
	arrIn[0] = new CFFmpegInOut(true, string("-probesize 100000"), string("E:\\SwDevelop\\CodecInMem\\love-5s.ts"));
	//sMiddleOptions.append("-vcodec libx264 -vb 1000k -acodec libmp3lame -ab 64k");
	sMiddleOptions.append("");
	arrOut[0] = new CFFmpegInOut(false, string("-vcodec h264_qsv -f yuv420p"), pOutputBuf/*string("E:\\SwDevelop\\CodecInMem\\love-5s-new.ts")*/);
	pFFmpegProcess = new CFFmpegProcess(true, false);
	ret = pFFmpegProcess->Init(sGlobalOptions, arrIn, sMiddleOptions, arrOut, sTailOptions);
	LOG(INFO) << "Init. ret: " << ret;
	ret = pFFmpegProcess->Start();
	LOG(INFO) << "Start. ret: " << ret;
	Sleep(2000);
	while (1)
	{
		Sleep(50);
		LOG(INFO) << "count: " << ++count << ", Progress: " << pFFmpegProcess->GetProgress();
		if (pFFmpegProcess->GetProgress() >= 1.0)
			break;
		if (count >= 100)
			break;
	}
	ret = pFFmpegProcess->Stop();
	LOG(INFO) << "Stop. ret: " << ret;
	ret = pFFmpegProcess->Uninit();
	LOG(INFO) << "Uninit. ret: " << ret;
	//pOutputBuf->WriteFile(string("E:\\SwDevelop\\CodecInMem\\love-5s-new.ts"), 0);
	Sleep(10000000);
	//pOutputBuf->WriteFile(string("E:\\SwDevelop\\CodecInMem\\love.mp3"), 0);
	return ;
}

void func3()
{
	int count = 0, ret, length, len;
	string sGlobalOptions, sMiddleOptions, sTailOptions;
	array<CFFmpegInOut*, MAX_INPUT_NUM> arrIn = { NULL }, arrOut = { NULL };
	CFFmpegProcess* pFFmpegProcess = NULL;
	CBuffer* pInputBuf = new CBuffer();
	CBuffer* pOutputBuf = new CBuffer(4 * 1024 * 1024);
	CBuffer* pRawBuf = new CBuffer(1024 * 1024);
	CPipe* pRawPipe = new CPipe();

	sGlobalOptions.append("-report");
	arrIn[0] = new CFFmpegInOut(true, string(""), string(INPUT_URL));
	sMiddleOptions.append("-vcodec libx264 -preset ultrafast -vb 1000k");
	arrOut[0] = new CFFmpegInOut(false, string(""), string(OUTPUT_URL));
	pFFmpegProcess = new CFFmpegProcess(true, false);
	ret = pFFmpegProcess->Init(sGlobalOptions, arrIn, sMiddleOptions, arrOut, sTailOptions);
	LOG(INFO) << "Init. ret" << ret;
	ret = pFFmpegProcess->Start();
	LOG(INFO) << "Start. ret" << ret;
	while (1)
	{
		Sleep(1000);
		LOG(INFO) << "" << ++count;
	}
	return ;
}

void func4()
{
	int count = 0, ret, length, len;
	string sGlobalOptions, sMiddleOptions, sTailOptions;
	array<CFFmpegInOut*, MAX_INPUT_NUM> arrIn = { NULL }, arrOut = { NULL };
	CFFmpegProcess* pFFmpegProcess = NULL;
	CBuffer* pInputBuf = new CBuffer();
	CBuffer* pOutputBuf = new CBuffer(4 * 1024 * 1024);
	CBuffer* pRawBuf = new CBuffer(1024 * 1024);
	CPipe* pRawPipe = new CPipe();


	return;
}


int main()
{
	func1();
	return 0;
}

#if 0
int main()
{
	char sGlobalOptions[MAX_NAME_LENGTH] = { 0 };
	char sInputFileOptions[MAX_INPUT_NUM][MAX_NAME_LENGTH] = { 0 };
	char sInputUrl[MAX_INPUT_NUM][MAX_NAME_LENGTH] = { 0 };
	char sMiddleOptions[MAX_NAME_LENGTH] = { 0 };
	char sOutputFileOptions[MAX_OUTPUT_NUM][MAX_NAME_LENGTH] = { 0 };
	char sOutputUrl[MAX_OUTPUT_NUM][MAX_NAME_LENGTH] = { 0 };
	char sTailOptions[MAX_NAME_LENGTH] = { 0 };
	int iPipeBufSize[MAX_INOUT_NUM] = { 0 };
	CFFmpegProcess* pFFmpegProcess = NULL;
	int ret = 0;

    LOG(INFO) << "Hello World!\n"; 

	pFFmpegProcess = new CFFmpegProcess(true, false);
	strcat(sGlobalOptions, "-report");
	strcat(sInputUrl[0], INPUT_URL);
	strcat(sOutputFileOptions[0], "-vcodec libx264 -vb 1000k");
	strcat(sOutputUrl[0], OUTPUT_URL);
	ret = pFFmpegProcess->Init(
		sGlobalOptions,
		sInputFileOptions,
		sInputUrl,
		sMiddleOptions,
		sOutputFileOptions,
		sOutputUrl,
		sTailOptions,
		iPipeBufSize);
	LOG(INFO) << "pFFmpegProcess->Init ret:" << ret << "\n";

	ret = pFFmpegProcess->Start();
	LOG(INFO) << "pFFmpegProcess->Start ret:" << ret << "\n";

	Sleep(10 * 1000);
	return 0;
}
#endif

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件


#if 0
待完成：
1、arrFFmpegOut可以有阻塞的选项；
2、提供实时的CProgressInfo查询，包括转码进度、转码速度等等；
	转码进度： 对于文件，使用Duration。对于流数据，使用数据量的消耗量；
3、重定向FFmpeg log回来，利用正则表达式过滤出有用信息；
4、
#endif