#pragma once
#include "EventBody.h"
#include "Utils.h"

#define MAX_EVENT_RECEIVER_NUMBER 64

class CEvent {
public:
	std::string sModuleName;
	// 上报事件
	virtual int Report(CEventBody body) { return 0; };
	// 通知已注册模块一个已注册类型的事件
	virtual int Notify(CEventBody body) { return 0; };
	// 注册一个接收器，以及需要接收的事件类型。
	virtual int RegisterReceiver(CEvent *pEvent, int eEventType) { return 0; };
	virtual int UnregisterReceiver(CEvent *pEvent) { return 0; };
protected:
	int m_iReceiverNumber = 0;
	CEvent* m_pcReceiver[MAX_EVENT_RECEIVER_NUMBER] = { nullptr };
	int m_eReceiverEventType[MAX_EVENT_RECEIVER_NUMBER] = { 0 };

private:

};
