#include <windows.h>
#include <mutex>


constexpr auto DEFAULT_BUFFER_SIZE = (128*1024);

#define CBUFFER_ERROR_NOT_ENOUGH_STORAGE	-1
#define CBUFFER_ERROR_NOT_ENOUGH_DATA		-2
#define CBUFFER_ERROR_NO_BUFFER				-3
#define CBUFFER_ERROR_PARA_ERROR			-4

using namespace std;

class CBuffer {
public:
	CBuffer();
	CBuffer(int iInitialSize);
	~CBuffer();
	int GetSize();
	int GetLength();
	int EnlargeBuffer2Times();
	int Write(unsigned char* pData, int len);
	int Read(unsigned char* pData, int len);
	int Peek(unsigned char* pData, int len);
	bool Clean();
	bool Copy(CBuffer& Buffer);
	// 读取文件到buffer。
	// iReadSize为正表示读取文件前iReadSize字节。
	// iReadSize为0时表示整个文件。
	// iReadSize为负时表示读取文件末尾iReadSize字节。
	// 返回后原有数据被覆盖。
	int ReadFile(string sFileName, int iReadSize);
	// 写buffer数据到文件。
	// iWriteSize为正表示取buffer前iWriteSize字节，
	// iWriteSize为0时表示整个buffer。
	// iWriteSize为负时表示取buffer末尾iWriteSize字节。
	// 返回后原有数据不变。
	int WriteFile(string sFileName, int iWriteSize);
private:
	unsigned char* m_pBuffer = { 0 };
	int m_iInitialSize;
	int m_iSize = DEFAULT_BUFFER_SIZE;
	int m_iStart = 0;
	int m_iLength = 0;
	__int64 m_llDroped = 0;
	mutex m_mtxBuffer;
};
