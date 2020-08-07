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
	// ��ȡ�ļ���buffer��
	// iReadSizeΪ����ʾ��ȡ�ļ�ǰiReadSize�ֽڡ�
	// iReadSizeΪ0ʱ��ʾ�����ļ���
	// iReadSizeΪ��ʱ��ʾ��ȡ�ļ�ĩβiReadSize�ֽڡ�
	// ���غ�ԭ�����ݱ����ǡ�
	int ReadFile(string sFileName, int iReadSize);
	// дbuffer���ݵ��ļ���
	// iWriteSizeΪ����ʾȡbufferǰiWriteSize�ֽڣ�
	// iWriteSizeΪ0ʱ��ʾ����buffer��
	// iWriteSizeΪ��ʱ��ʾȡbufferĩβiWriteSize�ֽڡ�
	// ���غ�ԭ�����ݲ��䡣
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
