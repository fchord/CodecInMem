#pragma once
#include <windows.h>
#include <string>

using namespace std;

class CDeviceInfo {
public:
	string sDeviceName;
	string sIpAddr;
	WORD iPort;
	string sUserName;
	string sUserPassword;
};