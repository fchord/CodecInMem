#pragma once

#define STATUS_UNINITED		"StatusUninited"
#define STATUS_INITING		"StatusIniting"
#define STATUS_INITED		"StatusInited"
#define STATUS_STARTING		"StatusStarting"
#define STATUS_RUNNING		"StatusRunning"
#define STATUS_PAUSE		"StatusPause"
#define STATUS_STOPPING		"StatusStopping"
#define STATUS_STOPPED		"StatusStopped"
#define STATUS_UNINITING	"StatusUniniting"
#define STATUS_FAILED		"StatusFailed"
#define STATUS_UNKNOWN		"StatusUnknown"

typedef enum E_STATUS {
	E_STATUS_UNINITED,
	E_STATUS_INITING,
	E_STATUS_INITED,
	E_STATUS_STARTING,
	E_STATUS_RUNNING,
	E_STATUS_PAUSE,
	E_STATUS_STOPPING,
	E_STATUS_STOPPED,
	E_STATUS_UNINITING,
	E_STATUS_FAILED,
	E_STATUS_UNKNOWN
} E_STATUS;

class CStatus {

public:
	CStatus();
	~CStatus();
	virtual int Init() { return 0; }
	virtual int Uninit() { return 0; }
	virtual int Start() { return 0; }
	virtual int Stop() { return 0; }
	virtual int Pause() { return 0; }
	virtual int Resume() { return 0; }
	E_STATUS GetStatus();
	char * GetStatusStr();
protected:
	int SetStatus(E_STATUS status);
private:
	E_STATUS m_eStatus = E_STATUS_UNINITED;
};


#if 0
int Init()
{
	SetStatus(E_STATUS_INITING);
		// ...
	SetStatus(E_STATUS_INITED);
	return 0;
	};

int Uninit()
{
	SetStatus(E_STATUS_UNINITING);

	SetStatus(E_STATUS_UNINITED);
	return 0;
};

int Start()
{
	SetStatus(E_STATUS_STARTING);

	SetStatus(E_STATUS_RUNNING);
	return 0;
};

int Stop()
{
	SetStatus(E_STATUS_STOPPING);

	SetStatus(E_STATUS_STOPPED);
	return 0;
};

int Pause()
{
	SetStatus(E_STATUS_PAUSE);
	// ...
	return 0;
};

int Resume()
{
	// ...
	SetStatus(E_STATUS_RUNNING);
	return 0;
}


#endif