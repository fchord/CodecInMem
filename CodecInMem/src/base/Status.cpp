
#include "Status.h"


CStatus::CStatus()
{
	return;
}

CStatus::~CStatus()
{
	return;
}

E_STATUS CStatus::GetStatus()
{
	return m_eStatus;
}

char * CStatus::GetStatusStr()
{
	switch (m_eStatus)
	{
		case E_STATUS_UNINITED:
			return (char *)STATUS_UNINITED;
		case E_STATUS_INITING:
			return (char *)STATUS_INITING;
		case E_STATUS_INITED:
			return (char *)STATUS_INITED;
		case E_STATUS_STARTING:
			return (char *)STATUS_STARTING;
		case E_STATUS_RUNNING:
			return (char *)STATUS_RUNNING;
		case E_STATUS_PAUSE:
			return (char *)STATUS_PAUSE;
		case E_STATUS_STOPPING:
			return (char *)STATUS_STOPPING;
		case E_STATUS_STOPPED:
			return (char *)STATUS_STOPPED;
		case E_STATUS_UNINITING:
			return (char *)STATUS_UNINITING;
		case E_STATUS_FAILED:
			return (char *)E_STATUS_FAILED;
		case E_STATUS_UNKNOWN:
			return (char *)E_STATUS_UNKNOWN;
		default:
			return (char *)E_STATUS_UNKNOWN;

	}
}


int CStatus::SetStatus(E_STATUS status)
{
	if (status < E_STATUS_UNINITED || status > E_STATUS_UNKNOWN)
		return -1;
	m_eStatus = status;
	return 0;
}


