#include "Buffer.h"
#include "FFmpegProcess.h"


class CFFmpegMemFile : protected CFFmpegProcess, public CStatus {
public:
	CFFmpegMemFile();
	~CFFmpegMemFile();

	//int Init(
	//	char sGlobalOptions[MAX_NAME_LENGTH],
	//	char sInputFileOptions[MAX_INPUT_NUM][MAX_NAME_LENGTH],
	//	CBuffer* pBufInputFile[MAX_INPUT_NUM],
	//	char sMiddleOptions[MAX_NAME_LENGTH],
	//	char sOutputFileOptions[MAX_OUTPUT_NUM][MAX_NAME_LENGTH],
	//	CBuffer* pBufOutputFile[MAX_OUTPUT_NUM],
	//	char sTailOptions[MAX_NAME_LENGTH]

	//);
};