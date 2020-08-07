#pragma once
#include "EventBody.h"
#include "Utils.h"

#define MAX_EVENT_RECEIVER_NUMBER 64

class CEvent {
public:
	std::string sModuleName;
	// �ϱ��¼�
	virtual int Report(CEventBody body) { return 0; };
	// ֪ͨ��ע��ģ��һ����ע�����͵��¼�
	virtual int Notify(CEventBody body) { return 0; };
	// ע��һ�����������Լ���Ҫ���յ��¼����͡�
	virtual int RegisterReceiver(CEvent *pEvent, int eEventType) { return 0; };
	virtual int UnregisterReceiver(CEvent *pEvent) { return 0; };
protected:
	int m_iReceiverNumber = 0;
	CEvent* m_pcReceiver[MAX_EVENT_RECEIVER_NUMBER] = { nullptr };
	int m_eReceiverEventType[MAX_EVENT_RECEIVER_NUMBER] = { 0 };

private:

};
