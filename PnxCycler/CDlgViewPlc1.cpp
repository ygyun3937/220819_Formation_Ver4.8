// CDlgViewPlc1.cpp: 구현 파일
//
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewPlc1.h"
#include "afxdialogex.h"

#include "MgrConfig.h"
#include "MgrDio.h"
#include "MgrStatus.h"

#include "CDlgViewPlcManual.h"
#include "CPlcProcessSequence1.h"
#include "include\PLC\actutltype.h"
/***************************************************/
#include "include\PLC\ActDefine.h"	// ACT Common Macro Header (For Set/Get Property Value)
/***************************************************/

// CDlgViewPlc1 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewPlc1, CDialogEx)

CDlgViewPlc1::CDlgViewPlc1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_PLC, pParent)
{
	PlcDataInit();
	PlcDataBlockInit();

	m_PlcProcessSequence1 = new CPlcProcessSequence1();
	m_PlcProcessSequence1->ThreadRun();

}

CDlgViewPlc1::~CDlgViewPlc1()
{
	PlcDataInit();
	PlcDataBlockInit();

	if (!SetTimer(ScheduleloadTimeID, TIMER_SET_500MS, NULL))
		SetKillScheduleTimer(FALSE);		//[JMW:220325]

	m_PlcProcessSequence1->ThreadStop();

}

BOOL CDlgViewPlc1::Create(CWnd * pParentWnd)
{
	//return 0;
	return CDialogEx::Create(IDD, pParentWnd);
}

void CDlgViewPlc1::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CDlgViewPlc1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Control(pDX, IDC_ACTUTLTYPE1, m_ActUtlType);
	
	PLC_Open();
	Sleep(100);
	if (m_bCheckOpened != TRUE) {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "Disconnect");
	}
	else {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "  Connect   ");
	}
}


BEGIN_MESSAGE_MAP(CDlgViewPlc1, CDialogEx)
	ON_WM_DESTROY()
	//[JMW:220325] 
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CDlgViewPlc1 메시지 처리기


/****************************************************/
/*  Open Communication Route                        */
/****************************************************/
void CDlgViewPlc1::PLC_Open()
{
	m_bCheckOpened = FALSE;
	long lRet;
	CString	MsgStr;

	try {
		//m_ActUtlType.put_ActLogicalStationNumber(1);	// Exec set-property method 
		m_ActUtlType.put_ActLogicalStationNumber(0);	// Exec set-property method 

		lRet = m_ActUtlType.Open();	// Open method exec
		if (lRet == 0x00) {	// Success
			m_iPlcOpenCount = 0;
			m_bCheckOpened = TRUE;
		}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		AfxMessageBox(MsgStr, MB_ICONINFORMATION);
		Exception->Delete();
	}
}


/****************************************************/
/*  Close Communication Route                       */
/****************************************************/
void CDlgViewPlc1::PLC_Close()
{
	long lRet;
	CString	MsgStr;
	try {
		m_iPlcOpenCount = 0;
		m_bCheckOpened = FALSE;
		Sleep(100);

		lRet = m_ActUtlType.Close();	// Exec Close Method
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}
}


/****************************************************/
/*  Get Device Value                                */
/****************************************************/
void CDlgViewPlc1::PLC_ReadDevice(CString strDevice, int nType)
{
	if (m_bCheckOpened != TRUE)
		return;

	long lRet;
	long lValue;
	CString	MsgStr;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();
	
	Sleep(1);
	if (strDevice == "") {
		// Not Enter DeviceName Error
		return;
	}
	try {
		lRet = m_ActUtlType.GetDevice(strDevice, &lValue);// Exec GetDevice Method

		if (lRet == 0x00) {	// Success 
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus("  connect    ");
			m_StPlcDataParam[nType].lPlcData = lValue;
		} 
		//else {	//[JMW:220321]
		//	PLC_ConnectionCheckandReOpen();
		//}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		AfxMessageBox(MsgStr, MB_ICONINFORMATION);
		Exception->Delete();
	}
}

//[JMW:220321]
void CDlgViewPlc1::PLC_ConnectionCheckandReOpen()
{	
	long lRet;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();
	
	lRet = m_ActUtlType.Open();
	if (lRet == 0x00) {
		m_iPlcOpenCount = 0;
		lpDlg->SetPlcConnectingStatus("  connect    ");
	}
	else {
		m_iPlcOpenCount++;
		lpDlg->SetPlcConnectingStatus("   error    ");
		if (m_iPlcOpenCount > PLC_RETRY_MAX_COUNT)
		{
			AfxMessageBox("PLC1 Connection Check Please.", MB_ICONINFORMATION);
			lpDlg->SetPlcConnectingStatus("Disconnect");
			PLC_Close();
		}		
	}
}

/****************************************************/
/*  Set Device Value                                */
/****************************************************/
void CDlgViewPlc1::PLC_WriteDevice(CString strDevice, int nType)
{
	if (m_bCheckOpened != TRUE)
		return;

	long lValue;
	long lRet;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	Sleep(10);
	if (strDevice == "") {
		// Not Enter DeviceName Error
		return;
	}
	lValue = m_StPlcDataParam[nType].lPlcData;
	try {
		// ActUtlType Control
		//lRet = m_ActUtlType.SetDevice(strDevice, lValue);	// Exec SetDevice Method
		lRet = m_ActUtlType.SetDevice2(strDevice, lValue);	// Exec SetDevice Method
		if (lRet != 0) {	//[JMW:220321]
			//PLC_ConnectionCheckandReOpen();
		}
		else {
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus("  connect    ");
		}

	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}
}

/****************************************************/
/*  Set Device Value                                */
/****************************************************/
void CDlgViewPlc1::PLC_RealNumberWriteDevice(CString strDevice, int nType)
{
	if (m_bCheckOpened != TRUE)
		return;
	BYTE bValue[4] = { 0, };
	short sDeviceValue[2] = { 0, };
	float m_DeviceValue = 0;
	short SendData[BLOCK_COUNT] = { 0, };

	long lSize = BLOCK_COUNT;
	CString strDeviceBack = _T("");
	LPCTSTR szDevice = _T("");
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	for (int i = 0; i < BLOCK_COUNT; i += 2)
	{
		if (i % 2 == 0) {
			m_DeviceValue = m_StPlcDataBlockParam[nType][i].fPlcDataBlock;
		}
		strDeviceBack.Format(_T("D%s"), strDevice);
		szDevice = strDeviceBack;//_T("D20")

		PLC_DeviceValue_Convert(m_DeviceValue, bValue, sDeviceValue);

		m_StPlcDataBlockParam[nType][i].sPlcDataBlock = sDeviceValue[0];
		m_StPlcDataBlockParam[nType][i + 1].sPlcDataBlock = sDeviceValue[1];


		SendData[i] = m_StPlcDataBlockParam[nType][i].sPlcDataBlock;
		SendData[i + 1] = m_StPlcDataBlockParam[nType][i + 1].sPlcDataBlock;
	}
	int result = m_ActUtlType.WriteDeviceBlock2(szDevice, lSize, SendData);
	if (0 == result) {
		m_iPlcOpenCount = 0;
		lpDlg->SetPlcConnectingStatus("  connect    ");
	}
	else {
		//Add Alarm
		//PLC_ConnectionCheckandReOpen();
		return;
	}
}


/****************************************************/
/*  Set Device Block Value                          */
/****************************************************/
void CDlgViewPlc1::PLC_WriteDeviceBlock(CString strDevice, int nType)
{
	if (m_bCheckOpened != TRUE)
		return;

	Sleep(10);
	LPCTSTR szDevice = _T("");
	long lSize = BLOCK_COUNT;
	long SendData[BLOCK_COUNT] = { 0, };
	CString strName = _T("");
	CString strDeviceBack = _T("");
	int nStartCnt = atoi(strDevice);
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	strDeviceBack.Format(_T("D%s"), strDevice);
	szDevice = strDeviceBack;//_T("D20")

// Renew ReturnValue 
	CString str = _T(""), str1 = _T("");

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		SendData[i] = m_StPlcDataBlockParam[nType][i].lPlcDataBlock;

		str1.Format(_T("%d,"), SendData[i]);
		str += str1;
	}
	try {
		long lRet = m_ActUtlType.WriteDeviceBlock(szDevice, lSize, SendData);	// Exec SetDevice Method

		if (lRet == 0x00) {	// Success
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus("  connect    ");
		}
		else {	//	[JMW:220321]
			//PLC_ConnectionCheckandReOpen();
		}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}


}


/****************************************************/
/*  Get Device Block Value                           */
/****************************************************/
void CDlgViewPlc1::PLC_ReadDeviceBlock(CString strDevice, int nType)
{
	if (m_bCheckOpened != TRUE)
		return;

	Sleep(1);
	long lRet = 0;
	LPCTSTR szDevice = _T("");
	long lSize = BLOCK_COUNT;
	long ReadData[BLOCK_COUNT] = { 0, };
	CString strName = _T("");
	CString strDeviceBack = _T("");
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	strDeviceBack.Format(_T("D%s"), strDevice);
	szDevice = strDeviceBack;//_T("D20")

	try {

		lRet = m_ActUtlType.ReadDeviceBlock(szDevice, lSize, ReadData);// Exec GetDevice Method

		if (lRet == 0x00) {	// Success
			CString str = _T(""), str1 = _T("");

			for (int i = 0; i < BLOCK_COUNT; i++)
			{
				strName.Format(_T("D%d"), atoi(strDevice) + i);
				m_StPlcDataBlockParam[nType][i].AddressName = strName;

				m_StPlcDataBlockParam[nType][i].lPlcDataBlock = ReadData[i];
				str1.Format(_T("R%d S%d,"), m_StPlcDataBlockParam[nType][i].lPlcDataBlock, ReadData[i]);
				str += str1;
			}
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus("  connect    ");
		}
		else {	//[JMW:220321]
			//PLC_ConnectionCheckandReOpen();
		}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}

}


/****************************************************/
/*  Get Device Block Value                           */
/****************************************************/
void CDlgViewPlc1::PLC_SendProcessData()
{
	//PLC_RealNumberWriteDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);
	//PLC_RealNumberWriteDevice(SEND_PROCESS_START, TYPE_SEND_HEART_BIT);
	PLC_WriteDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);
	PLC_WriteDevice(SEND_PROCESS_START, TYPE_SEND_PROCESS_START);
	PLC_WriteDevice(SEND_SCHEDULE_START, TYPE_SEND_SCHEDULE_START);
	PLC_WriteDevice(SEND_SCHEDULE_COMPLETE, TYPE_SEND_SCHEDULE_COMPLETE);
	PLC_WriteDevice(SEND_RESULT_PASS, TYPE_SEND_RESULT_PASS);
	PLC_WriteDevice(SEND_PROCESS_END, TYPE_SEND_PROCESS_END);
	PLC_WriteDevice(SEND_SEND_EMO, TYPE_SEND_EMO);


	//Test
	//PLC_WriteDevice(READ_HEART_BIT, TYPE_READ_HEART_BIT);
	//PLC_WriteDevice(READ_READY, TYPE_READ_READY);
	//PLC_WriteDevice(PLC_ALARM_OCCUR, TYPE_READ_PLC_ALARM_OCCUR);
	//PLC_WriteDevice(PLC_LOAD_COMPLETE, TYPE_PLC_LOAD_COMPLETE);
	//PLC_WriteDevice(PLC_UNLOAD_COMPLETE, TYPE_PLC_UNLOAD_COMPLETE);
	//PLC_WriteDevice(PLC_PLC_EMO, TYPE_PLC_EMO);
}

void CDlgViewPlc1::PLC_ReadProcessData()
{
	PLC_ReadDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);
	PLC_ReadDevice(SEND_PROCESS_START, TYPE_SEND_PROCESS_START);
	PLC_ReadDevice(SEND_SCHEDULE_START, TYPE_SEND_SCHEDULE_START);
	PLC_ReadDevice(SEND_SCHEDULE_COMPLETE, TYPE_SEND_SCHEDULE_COMPLETE);
	PLC_ReadDevice(SEND_RESULT_PASS, TYPE_SEND_RESULT_PASS);
	PLC_ReadDevice(SEND_PROCESS_END, TYPE_SEND_PROCESS_END);
	PLC_ReadDevice(SEND_SEND_EMO, TYPE_SEND_EMO);		//[JMW:220325]	//EMO읽기로 수정

	PLC_ReadDevice(READ_HEART_BIT, TYPE_READ_HEART_BIT);
	PLC_ReadDevice(READ_READY, TYPE_READ_READY);
	PLC_ReadDevice(PLC_ALARM_OCCUR, TYPE_READ_PLC_ALARM_OCCUR);
	PLC_ReadDevice(PLC_LOAD_COMPLETE, TYPE_PLC_LOAD_COMPLETE);
	PLC_ReadDevice(PLC_UNLOAD_COMPLETE, TYPE_PLC_UNLOAD_COMPLETE);
	PLC_ReadDevice(PLC_PLC_EMO, TYPE_PLC_EMO);			//[JMW:220325]	//EMO읽기로 수정
}

void CDlgViewPlc1::PLC_SendResultData()
{
	PLC_RealNumberWriteDevice(SEND_V_START_24, PLC_SEND_V_24);

	PLC_RealNumberWriteDevice(SEND_A_START_24, PLC_SEND_A_24);

	PLC_RealNumberWriteDevice(SEND_T_START_24, PLC_SEND_T_24);
}

void CDlgViewPlc1::PLC_ReadResultData()
{
	PLC_ReadDeviceBlock(SEND_V_START_24, PLC_SEND_V_24);

	PLC_ReadDeviceBlock(SEND_A_START_24, PLC_SEND_A_24);

	PLC_ReadDeviceBlock(SEND_T_START_24, PLC_SEND_T_24);
}



void CDlgViewPlc1::PlcDataBlockInit()
{
	for (int y = 0; y < PLC_SEND_MAX; y++)
	{
		for (int x = 0; x < BLOCK_COUNT; x++)
		{
			m_StPlcDataBlockParam[y][x].AddressName = _T("");
			m_StPlcDataBlockParam[y][x].lPlcDataBlock = 0;
			m_StPlcDataBlockParam[y][x].sPlcDataBlock = 0;
			m_StPlcDataBlockParam[y][x].dlPlcDataBlock = 0.0;
			m_StPlcDataBlockParam[y][x].fPlcDataBlock = 0.0;

			//			m_StPlcDataBlockParam[y][x].lPlcGetDataBlock = 0;
		}
	}
}
void CDlgViewPlc1::PlcDataInit()
{
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{
		m_StPlcDataParam[i].lPlcData = 0;
		m_StPlcDataParam[i].dlPlcData = 0.0;

	}

	m_iPlcOpenCount = 0;
	ScheduleTime = 0; //[JMW:220325]

	m_StPlcDataParam[0].ProcessAddName = SEND_HEART_BIT;
	m_StPlcDataParam[1].ProcessAddName = SEND_PROCESS_START;
	m_StPlcDataParam[2].ProcessAddName = SEND_SCHEDULE_START;
	m_StPlcDataParam[3].ProcessAddName = SEND_SCHEDULE_COMPLETE;
	m_StPlcDataParam[4].ProcessAddName = SEND_PROCESS_END;
	m_StPlcDataParam[5].ProcessAddName = SEND_SEND_EMO;		//[JMW:220325]  왜없었지???

	m_StPlcDataParam[6].ProcessAddName = READ_HEART_BIT;
	m_StPlcDataParam[7].ProcessAddName = READ_READY;
	m_StPlcDataParam[8].ProcessAddName = PLC_ALARM_OCCUR;
	m_StPlcDataParam[9].ProcessAddName = PLC_LOAD_COMPLETE;
	m_StPlcDataParam[10].ProcessAddName = PLC_UNLOAD_COMPLETE;
	m_StPlcDataParam[11].ProcessAddName = PLC_PLC_EMO;		//[JMW:220325]   왜없었지???
}


//[+:JMW]
void CDlgViewPlc1::PLC_DeviceValue_Convert(float fValue, BYTE* bValue, short* sDeviceValue)
{

	BYTE bValuetoShort[2];

	*((float *)bValue) = fValue;

	bValuetoShort[0] = bValue[0];
	bValuetoShort[1] = bValue[1];
	sDeviceValue[0] = *((short *)bValuetoShort);

	bValuetoShort[0] = bValue[2];
	bValuetoShort[1] = bValue[3];
	sDeviceValue[1] = *((short *)bValuetoShort);
}

void CDlgViewPlc1::SetPlcAlarmReset()
{
	//m_PlcProcessSequence1->m_PLCAlarmFlag = FALSE;
	m_PlcProcessSequence1->m_ReadAlarmCount = 0;
}

void CDlgViewPlc1::SetPlcProcessStart()
{
	//m_PlcProcessSequence1->m_RunStopFlag = TRUE;

	CMgrStatus::GetMgr()->SetEqStartFlag(1, TRUE);

	m_PlcProcessSequence1->SetAutoStep(m_PlcProcessSequence1->enumProcessStepAuto::AUTO_INIT);
}

void CDlgViewPlc1::SetPlcProcessStop()
{
	CMgrStatus::GetMgr()->SetEqStartFlag(1, FALSE);

	//m_PlcProcessSequence1->m_RunStopFlag = FALSE;
}

void CDlgViewPlc1::SetPlcProcessReset()
{
	m_PlcProcessSequence1->SetAutoStep(m_PlcProcessSequence1->enumProcessStepAuto::AUTO_INIT);
	CMgrStatus::GetMgr()->SetEqStartFlag(1, FALSE);

	//m_PlcProcessSequence1->m_RunStopFlag = FALSE;
}

void CDlgViewPlc1::SetPlcThreadStop()
{
	m_PlcProcessSequence1->ThreadStop();
}

void CDlgViewPlc1::OnTimer(UINT_PTR nIDEvent)
{
	//[JMW:220325]
	if (nIDEvent == ScheduleloadTimeID)
	{
		ScheduleTime++;
	}
	__super::OnTimer(nIDEvent);
}

//[JMW:220325]
void CDlgViewPlc1::SetKillScheduleTimer(BOOL timeflag) {
	if (timeflag == TRUE)
	{
		SetTimer(ScheduleloadTimeID, TIMER_SET_500MS, NULL);
	}
	else {
		KillTimer(ScheduleloadTimeID);
		ScheduleTime = 0;
		Timerflag = TRUE;
	}
}
