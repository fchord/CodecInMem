#pragma once
#include "Utils.h"

class CCoordinate {
public:
	CCoordinate();
	CCoordinate(E_RESOLUTION resolution, int iWidth, int iHeight);
	~CCoordinate();
	int ChangeResolution(E_RESOLUTION resolution);
	E_RESOLUTION GetResolution();
	int GetWidth();
	int GetHeight();

private:
	E_RESOLUTION resolution = E_RESOLUTION_UNKNOWN;
	int m_iWidth = 0, m_iHeight = 0;
};
