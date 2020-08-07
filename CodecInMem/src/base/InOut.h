#pragma once

#include"Utils.h"

#define MAX_RECEIVER_NUMBER 16

class CInOut {
public:
	// ����ֵΪд�����������0��ʾδд�����ݡ�������ʾ����
	virtual int In(unsigned char* p, int len, E_DATA_TYPE eDataType) { return 0; };
	// ע��һ�����������Լ���Ҫ���յ��������͡�
	int RegisterReceiver(CInOut *pInOut, E_DATA_TYPE eDataType);
	int UnregisterReceiver(CInOut *pInOut);
protected:
	int Out(unsigned char* p, int len, E_DATA_TYPE eDataType);
private:
	int receiverNumber = 0;
	CInOut* m_cReceiver[MAX_RECEIVER_NUMBER] = { 0 };
	E_DATA_TYPE m_eReceiverDataType[MAX_RECEIVER_NUMBER] = { E_DATA_TYPE_UNKNOWN };

};