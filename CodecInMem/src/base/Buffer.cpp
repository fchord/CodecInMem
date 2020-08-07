
#include "Buffer.h"

CBuffer::CBuffer()
{
	m_pBuffer = new unsigned char[m_iSize];
	memset(m_pBuffer, 0, m_iSize);
	return;
}

CBuffer::CBuffer(int iInitialSize)
{
	m_iSize = iInitialSize < DEFAULT_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : iInitialSize;
	m_pBuffer = new unsigned char[m_iSize];
	memset(m_pBuffer, 0, m_iSize);
	return;
}


CBuffer::~CBuffer()
{
	m_mtxBuffer.lock();
	if (m_pBuffer)
		delete m_pBuffer;
	m_pBuffer = NULL;
	m_mtxBuffer.unlock();
	return;
}

int CBuffer::GetSize()
{
	return m_iSize;
}

int CBuffer::GetLength()
{
	return m_iLength;
}

int CBuffer::EnlargeBuffer2Times()
{
	int times = 2;
	unsigned char* p = { 0 };
	m_mtxBuffer.lock();
	if (!m_pBuffer || m_iSize <= 0) {
		m_mtxBuffer.unlock();
		return CBUFFER_ERROR_NO_BUFFER;
	}
	p = new unsigned char[m_iSize];
	memcpy(p, m_pBuffer, m_iSize);
	delete m_pBuffer;
	m_pBuffer = new unsigned char[m_iSize * times];
	memset(m_pBuffer, 0, m_iSize * times);
	memcpy(m_pBuffer, p, m_iSize);
	m_iSize = m_iSize * times;
	delete p;
	m_mtxBuffer.unlock();
	return 0;
}

int CBuffer::Write(unsigned char* pData, int len)
{
	unsigned char* p;
	int n;
	if (!pData || len < 0) {
		return CBUFFER_ERROR_PARA_ERROR;
	}
	if (0 == len)
		return 0;
	m_mtxBuffer.lock();
	if (m_iSize - m_iLength <= 0) {
		m_mtxBuffer.unlock();
		return CBUFFER_ERROR_NOT_ENOUGH_STORAGE;
	}
	n = (len > (m_iSize - m_iLength) ? (m_iSize - m_iLength) : len);
	if (n > m_iSize - m_iLength - m_iStart) {
		if (m_iLength > 0) {
			p = new unsigned char[m_iLength];
			memcpy(p, m_pBuffer + m_iStart, m_iLength);
			m_iStart = 0;
			memcpy(m_pBuffer, p, m_iLength);
			delete p;
		}
		else {
			m_iStart = 0;
			m_iLength = 0;
		}
	}
	memcpy(m_pBuffer + m_iStart + m_iLength, pData, n);
	m_iLength += n;
	m_mtxBuffer.unlock();
	return n;

}

int CBuffer::Read(unsigned char* pData, int len)
{
	int n;
	if (!pData || len < 0) {
		return CBUFFER_ERROR_PARA_ERROR;
	}
	if (0 == len)
		return 0;
	m_mtxBuffer.lock();
	if (m_iLength <= 0) {
		m_mtxBuffer.unlock();
		return 0;
	}
	n = (len > m_iLength) ? m_iLength : len;
	memcpy(pData, m_pBuffer + m_iStart, n);
	m_iStart += n;
	m_iLength -= n;
	if (0 == m_iLength)
		m_iStart = 0;
	m_mtxBuffer.unlock();
	return n;
}

int CBuffer::Peek(unsigned char* pData, int len)
{
	int n;
	if (!pData || len <= 0) {
		return CBUFFER_ERROR_PARA_ERROR;
	}
	m_mtxBuffer.lock();
	if (m_iLength <= 0) {
		m_mtxBuffer.unlock();
		return CBUFFER_ERROR_NOT_ENOUGH_DATA;
	}
	n = (len > m_iLength) ? m_iLength : len;
	memcpy(pData, m_pBuffer + m_iStart, n);
	m_mtxBuffer.unlock();
	return n;
}

bool CBuffer::Clean()
{
	if (!m_pBuffer)
		return false;
	m_mtxBuffer.lock();
	m_iStart = 0;
	m_llDroped += m_iLength;
	m_iLength = 0;
	m_mtxBuffer.unlock();
	return true;
}

bool CBuffer::Copy(CBuffer& Buffer)
{
	if (Buffer.GetLength() <= 0)
		return false;
	m_mtxBuffer.lock();
	if (m_pBuffer)
		delete m_pBuffer;
	m_iSize = Buffer.GetSize();
	m_pBuffer = new unsigned char[m_iSize];
	m_iLength = Buffer.GetLength();
	m_iStart = 0;
	Buffer.Peek(m_pBuffer, m_iLength);
	m_mtxBuffer.unlock();
	return true;
}

int CBuffer::ReadFile(string sFileName, int iReadSize)
{
	char* pPath;
	int iSize, i, iFileType;
	__int64 llFileSize, llOffset;
	struct stat s;
	FILE* pf = NULL;

	m_mtxBuffer.lock();
	iSize = sFileName.size() + 1;
	pPath = (char *)malloc(iSize);
	memset(pPath, 0, iSize);
	strcpy(pPath, sFileName.c_str());
	for (i = 0; i < iSize; i++) {
		if (pPath[i] == '\\')
			pPath[i] = '/';
	}
	pPath[i] = '\0';
	if (0 == stat(pPath, &s)) {
		if (s.st_mode & S_IFDIR) {
			iFileType = 1; //ST_MODE_DIR
		}
		else if (s.st_mode & S_IFREG) {
			iFileType = 2; //ST_MODE_FILE
		}
		else {
			iFileType = 3; //ST_MODE_OTHER
		}
	}
	else {
		iFileType = 0; //ST_MODE_NOT_EXISTENCE
	}
	if (2 == iFileType)
		pf = fopen(pPath, "rb");
	if (2 != iFileType || NULL == pf)
	{
		if (pPath)
			delete pPath;
		//m_iSize = (abs(iReadSize) > m_iSize) ? abs(iReadSize) : m_iSize;
		//m_pBuffer = new unsigned char[m_iSize];
		//memset(m_pBuffer, 0, m_iSize);
		m_mtxBuffer.unlock();
		return -1;
	}

	fseek(pf, 0L, SEEK_END);
	llFileSize = ftell(pf);
	//LOG(INFO) << "CBuffer::ReadFile. llFileSize:" << llFileSize;
	if (0 == iReadSize || abs(iReadSize) >= (int)llFileSize)
		iReadSize = (int)llFileSize;
	if (iReadSize >= 0)
		llOffset = 0;
	else
		llOffset = llFileSize - abs(iReadSize);
	fseek(pf, llOffset, SEEK_SET);

	m_iSize = (abs(iReadSize) > m_iSize) ? abs(iReadSize) : m_iSize;
	if (m_pBuffer)
		delete m_pBuffer;
	m_pBuffer = new unsigned char[m_iSize];
	memset(m_pBuffer, 0, m_iSize);
	m_iLength = fread(m_pBuffer, 1, abs(iReadSize), pf);
	fclose(pf);
	if (pPath)
		delete pPath;
	m_mtxBuffer.unlock();
	return m_iLength;
}

int CBuffer::WriteFile(string sFileName, int iWriteSize)
{
	char* pPath;
	int iSize, i, iFileType, iStart, iLength, n;
	__int64 llFileSize, llOffset;
	struct stat s;
	FILE* pf = NULL;

	m_mtxBuffer.lock();
	iSize = sFileName.size() + 1;
	pPath = (char *)malloc(iSize);
	memset(pPath, 0, iSize);
	strcpy(pPath, sFileName.c_str());
	for (i = 0; i < iSize; i++) {
		if (pPath[i] == '\\')
			pPath[i] = '/';
	}
	pPath[i] = '\0';
	if (0 == stat(pPath, &s)) {
		if (s.st_mode & S_IFDIR) {
			iFileType = 1; //ST_MODE_DIR
		}
		else if (s.st_mode & S_IFREG) {
			iFileType = 2; //ST_MODE_FILE
		}
		else {
			iFileType = 3; //ST_MODE_OTHER
		}
	}
	else {
		iFileType = 0; //ST_MODE_NOT_EXISTENCE
	}
	if (2 == iFileType)
		pf = fopen(pPath, "wb");
	if (2 != iFileType || NULL == pf)
	{
		if (pPath)
			delete pPath;
		//m_iSize = (abs(iReadSize) > m_iSize) ? abs(iReadSize) : m_iSize;
		//m_pBuffer = new unsigned char[m_iSize];
		//memset(m_pBuffer, 0, m_iSize);
		m_mtxBuffer.unlock();
		return -1;
	}

	if (0 == iWriteSize)
		iLength = m_iLength;
	else
		iLength = (abs(iWriteSize) > m_iLength) ? m_iLength : abs(iWriteSize);
	if (iWriteSize >= 0)
		iStart = 0;
	else
		iStart = m_iLength - iLength;
	iLength;
	n = fwrite(m_pBuffer + m_iStart + iStart, 1, iLength, pf);
	fclose(pf);
	if (pPath)
		delete pPath;
	m_mtxBuffer.unlock();
	return n;
}