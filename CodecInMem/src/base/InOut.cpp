
#include <iostream>
#include "InOut.h"

using namespace std;

int CInOut::RegisterReceiver(CInOut* pInOut, E_DATA_TYPE eDataType)
{
	int i;
	for ( i = 0; i < MAX_RECEIVER_NUMBER; i++)
	{
		if (m_cReceiver[i] == nullptr) {
			m_cReceiver[i] = pInOut;
			m_eReceiverDataType[i] = eDataType;
			cout << "CDistributor::RegisterReceiver.\n";
			receiverNumber++;
			break;
		}
	}
	if (MAX_RECEIVER_NUMBER == i) {
		cout << "CInOut::RegisterReceiver. Register failed, receivers is full.\n";
		return -1;
	}
	return 0;
};

int CInOut::UnregisterReceiver(CInOut* pInOut)
{
	int i;
	for ( i = 0; i < MAX_RECEIVER_NUMBER; i++)
	{
		if (m_cReceiver[i] == pInOut) {
			m_cReceiver[i] = nullptr;
			m_eReceiverDataType[i] = E_DATA_TYPE_UNKNOWN;
			receiverNumber--;
			break;
		}
	}
	if (MAX_RECEIVER_NUMBER == i) {
		cout << "CInOut::RegisterReceiver. Unregister failed, receiver not found.\n";
		return -1;
	}
	return 0;
}


int CInOut::Out(unsigned char* pData, int len, E_DATA_TYPE eDataType)
{
	int length, n;
	for (int i = 0; i < MAX_RECEIVER_NUMBER; i++)
	{
		if (m_cReceiver[i] != nullptr && eDataType == m_eReceiverDataType[i]) {
			length = 0;
			n = 0;
			do {
				n = m_cReceiver[i]->In(pData + length, len - length, eDataType);
				if (n >= 0)
					length += n;
				else {
					cout << "CInOut::Out. Write data ERROR.";
					break;
				}
			} while (length < len);
		}
	}
	return 0;
}