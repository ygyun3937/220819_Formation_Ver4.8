#pragma once

#include "include\PLC\actutltype.h"
#include "include\PLC\ActDefine.h"	// ACT Common Macro Header (For Set/Get Property Value)
// CDlgViewPlc1 대화 상자




//PLC Block
struct StPlcDataBlockParam
{
	CString AddressName;
	long lPlcDataBlock;
	double dlPlcDataBlock;
	float fPlcDataBlock;	//[+:JMW]
	short sPlcDataBlock;	//[+:JMW]
	//long lPlcGetDataBlock;
};

extern StPlcDataBlockParam m_StPlcDataBlockParam[PLC_SEND_MAX][BLOCK_COUNT];

//PLC
struct StPlcDataParam
{
	//	CString SendAddName;
	CString ProcessAddName;
	long lPlcData;
	double dlPlcData;
	//	long lPlcGetData;
};

extern StPlcDataParam m_StPlcDataParam[TYPE_PLC_MAX];

class CPlcProcessSequence1;
class CDlgViewPlc1 : public CDialogEx, public CSingleton< CDlgViewPlc1 >
{
	DECLARE_DYNAMIC(CDlgViewPlc1)


	enum { IDD = IDD_DLG_VIEW_PLC };
public:
	CDlgViewPlc1(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewPlc1();


	CPlcProcessSequence1* m_PlcProcessSequence1;

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	afx_msg void OnDestroy();

	CActutltype m_ActUtlType;

	CString	m_RetVal;
	CString	m_RetVal2;
	CString	m_RetVal3;

	//Connection
	BOOL m_bCheckOpened;
	int m_iPlcOpenCount;	//[JMW:220321]

	//[JMW:220325]
	#define		ScheduleloadTimeID					1027
	#define		TIMER_SET_500MS						500

	int ScheduleTime;	//[JMW:220325]
	BOOL Timerflag;		//[JMW:220325]

	void SetKillScheduleTimer(BOOL timeflag);	//[JMW:220325]
	
    //PLC
	void PlcDataBlockInit();
	void PlcDataInit();

	void PLC_Open();
	void PLC_Close();

	//[JMW:220321]
	void PLC_ConnectionCheckandReOpen();

	void PLC_ReadDevice(CString strDevice, int nType);
	void PLC_WriteDevice(CString strDevice, int nType);

	void PLC_RealNumberWriteDevice(CString strDevice, int nType);

	void PLC_ReadDeviceBlock(CString strDevice, int nType);
	void PLC_WriteDeviceBlock(CString strDevice, int nType);

	void PLC_DeviceValue_Convert(float fValue, BYTE* bValue, short* sDeviceValue);
	void PLC_SendProcessData();
	void PLC_ReadProcessData();
	void PLC_SendResultData();
	void PLC_ReadResultData();
	void SetPlcAlarmReset();

	void SetPlcProcessStart();
	void SetPlcProcessStop();
	void SetPlcProcessReset();
	void SetPlcThreadStop();

	StPlcDataBlockParam m_StPlcDataBlockParam[PLC_SEND_MAX][BLOCK_COUNT];
	StPlcDataParam m_StPlcDataParam[TYPE_PLC_MAX];

	#define PLC_RETRY_MAX_COUNT 3

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_PLC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.



	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
