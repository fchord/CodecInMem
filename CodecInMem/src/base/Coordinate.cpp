
#include "Coordinate.h"

CCoordinate::CCoordinate()
{
	return;
}

CCoordinate::CCoordinate(E_RESOLUTION resolution, int iWidth, int iHeight)
{
	if (resolution >= E_RESOLUTION_UNKNOWN || resolution < E_RESOLUTION_270P) {
		this->resolution = E_RESOLUTION_UNKNOWN;
		m_iWidth = 0;
		m_iHeight = 0;
		return;
	}
	this->resolution = resolution;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	return;
}

CCoordinate::~CCoordinate()
{
	return;
}

int CCoordinate::ChangeResolution(E_RESOLUTION resolution)
{
	if (resolution >= E_RESOLUTION_UNKNOWN || resolution < E_RESOLUTION_270P)
		return -1;
	if (this->resolution == resolution)
		return 0;

	m_iWidth = m_iWidth * ResoWidth(resolution) / ResoWidth(this->resolution);
	m_iHeight = m_iHeight * ResoHeight(resolution) / ResoHeight(this->resolution);
	this->resolution = resolution;
	return 0;
}

E_RESOLUTION CCoordinate::GetResolution()
{
	return resolution;
}

int CCoordinate::GetWidth()
{
	return m_iWidth;
}

int CCoordinate::GetHeight()
{
	return m_iHeight;
}