#pragma once

#include"Utils.h"

#define MAX_RECEIVER_NUMBER 16

class CInOut {
public:
	// 返回值为写入的数据量。0表示未写入数据。负数表示出错。
	virtual int In(unsigned char* p, int len, E_DATA_TYPE eDataType) { return 0; };
	// 注册一个接收器，以及需要接收的数据类型。
	int RegisterReceiver(CInOut *pInOut, E_DATA_TYPE eDataType);
	int UnregisterReceiver(CInOut *pInOut);
protected:
	int Out(unsigned char* p, int len, E_DATA_TYPE eDataType);
private:
	int receiverNumber = 0;
	CInOut* m_cReceiver[MAX_RECEIVER_NUMBER] = { 0 };
	E_DATA_TYPE m_eReceiverDataType[MAX_RECEIVER_NUMBER] = { E_DATA_TYPE_UNKNOWN };

};