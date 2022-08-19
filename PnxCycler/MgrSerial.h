#pragma once

#include "Singleton.h"
#include "QueuingSerial.h"
#include "ZebraBCRControl.h"	//BCR

#include <map>

class CMgrSerial : public CSingleton< CMgrSerial >
{
public:
	CMgrSerial();
	virtual ~CMgrSerial();

	void	StartThread();
	void	StopThread();

	void	AddQueue(int nPrimaryKey, CString strTime, BYTE* pszData, int nDataLenth=0);
	void	AddQueue(int nCommType, BYTE* pszData);
	void	AddQueue(int nPrimaryKey, CString strTime, float fValue);
	void	AddQueue(int nPrimaryKey, CString strTime, float fValue, int nType);
	void	AddQueue(int nPrimaryKey, CString strTime, int nElecType, float fValue);
	void	AddQueue(int nPrimaryKey, CString strTime, char* pData);

	CQueuingSerial*	GetAt(int nPrimaryKey);

	CZebraBCRControl*	GetAtBarcode(int nStationNo);

	   

	void Set_ChBarcodeCheckMode(bool bChBarcodeCheckMode);
	BOOL Get_ChBarcodeCheckMode() { return m_bChBarcodeCheck; }

private:
	std::map< int, CQueuingSerial* > m_mapSerial;
	std::map< int, CZebraBCRControl* > m_mapBarcode;

	BOOL m_bChBarcodeCheck;

private:
	void	RemoveAll();
};

