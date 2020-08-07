#pragma once
#include "SenderEventBody.h"
#include "CameraEventBody.h"
#include "HonkSystemSdkEventBody.h"
#include "PingerEventBody.h"
#include "SonarEventBody.h"

typedef enum E_EVENT_TYPE {
	E_EVENT_TYPE_UNKNOWN = 0,
	E_EVENT_TYPE_SDK = 1,
	E_EVENT_TYPE_CAMERA_MAIN = 2,
	E_EVENT_TYPE_CAMERA_AUX = 4,
	E_EVENT_TYPE_SONAR = 8,
	E_EVENT_TYPE_EVIDENCE = 16,
	E_EVENT_TYPE_PINGER = 32,
	E_EVENT_TYPE_SENDER = 64,
} E_EVENT_TYPE;

class CEventBody {
public:
	CEventBody() { return; };
	~CEventBody() { return; };
	CEventBody(CSenderEventBody body)
	{
		eEventType = E_EVENT_TYPE_SENDER;
		SenderEventBody = body;
		return;
	};
	CEventBody(CSdkEventBody body)
	{
		eEventType = E_EVENT_TYPE_SDK;
		SdkEventBody = body;
		return;
	};
	CEventBody(CPingerEventBody body)
	{
		eEventType = E_EVENT_TYPE_PINGER;
		PingerEventBody = body;
		return;
	};
	CEventBody(CCameraEventBody body)
	{
		eEventType = E_EVENT_TYPE_CAMERA_MAIN;
		CameraEventBody = body;
		return;
	};
	CEventBody(CCameraEventBody body, bool bIsMain)
	{
		if(bIsMain)
			eEventType = E_EVENT_TYPE_CAMERA_MAIN;
		else
			eEventType = E_EVENT_TYPE_CAMERA_AUX;
		CameraEventBody = body;
		return;
	};
	CEventBody(CSonarEventBody body)
	{
		eEventType = E_EVENT_TYPE_SONAR;
		SonarEventBody = body;
		return;
	};

	E_EVENT_TYPE eEventType;
	//union {
		CSenderEventBody SenderEventBody;
		CSdkEventBody SdkEventBody;
		CCameraEventBody CameraEventBody;
		CPingerEventBody PingerEventBody;
		CSonarEventBody SonarEventBody; 
	//};
};