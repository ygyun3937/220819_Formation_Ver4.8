// CDlgViewPlc2.cpp: 구현 파일
//
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewPlc2.h"
#include "afxdialogex.h"

#include "MgrConfig.h"
#include "MgrDio.h"
#include "MgrStatus.h"

#include "CDlgViewPlcManual.h"
#include "CPlcProcessSequence2.h"
#include "include\PLC\actutltype.h"
/***************************************************/
#include "include\PLC\ActDefine.h"	// ACT Common Macro Header (For Set/Get Property Value)
/***************************************************/

// CDlgViewPlc2대화 상자

IMPLEMENT_DYNAMIC(CDlgViewPlc2, CDialogEx)

CDlgViewPlc2::CDlgViewPlc2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_PLC, pParent)
{
	PlcDataInit();
	PlcDataBlockInit();

	m_PlcProcessSequence2 = new CPlcProcessSequence2();
	m_PlcProcessSequence2->ThreadRun();
}

CDlgViewPlc2::~CDlgViewPlc2()
{
	PlcDataInit();
	PlcDataBlockInit();

	if (!SetTimer(ScheduleloadTimeID, TIMER_SET_500MS, NULL))
		SetKillScheduleTimer(FALSE);		//[JMW:220325]
		m_PlcProcessSequence2->ThreadStop();

}

BOOL CDlgViewPlc2::Create(CWnd * pParentWnd)
{
	//return 0;
	return CDialogEx::Create(IDD, pParentWnd);
}


void CDlgViewPlc2::OnDestroy()
{
	CDialogEx::OnDestroy();

}

void CDlgViewPlc2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Control(pDX, IDC_ACTUTLTYPE2, m_ActUtlType2);

	PLC_Open();
	Sleep(100);
	if (m_bCheckOpened != TRUE) {
		//AfxMessageBox("PLC2 연결상태를 확인해주세요.", MB_ICONINFORMATION);
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "Disconnect");
	}
	else {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "  Connect   ");
	}
}


BEGIN_MESSAGE_MAP(CDlgViewPlc2, CDialogEx)
	ON_WM_DESTROY()
	//[JMW:220325] 
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgViewPlc2 메시지 처리기


/****************************************************/
/*  Open Communication Route                        */
/****************************************************/
void CDlgViewPlc2::PLC_Open()
{
	m_bCheckOpened = FALSE;
	long lRet;
	CString	MsgStr;

	try {
		m_ActUtlType2.put_ActLogicalStationNumber(1);	// Exec set-property method 

		//m_ActUtlType2.put_ActLogicalStationNumber(0);	//[JMW:220318] 삭제예정 Test용

		lRet = m_ActUtlType2.Open();	// Open method exec
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
void CDlgViewPlc2::PLC_Close()
{
	long lRet;
	CString	MsgStr;
	try {
		m_iPlcOpenCount = 0;
		m_bCheckOpened = FALSE;
		Sleep(100);
		//if (m_SelectCntl == 0) {	// ActProgType Control
		//	lRet = m_ActProgType.Close();	// Exec Close Method
		//}
		//else {						// ActUtlType Control
		lRet = m_ActUtlType2.Close();	// Exec Close Method
	//}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}
}


/****************************************************/
/*  Get Device Value                                */
/****************************************************/
void CDlgViewPlc2::PLC_ReadDevice(CString strDevice, int nType)
{
	long lRet;
	long lValue;
	CString	MsgStr;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	Sleep(10);
	if (strDevice == "") {
		// Not Enter DeviceName Error
		return;
	}

	if (m_bCheckOpened != TRUE)
		return;

	try {
		lRet = m_ActUtlType2.GetDevice(strDevice, &lValue);// Exec GetDevice Method

		if (lRet == 0x00) {	// Success
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus2("  connect    ");
			m_StPlcDataParam2[nType].lPlcData = lValue;
		}
		else {
			//PLC_ConnectionCheckandReOpen();
		}

	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		AfxMessageBox(MsgStr, MB_ICONINFORMATION);
		Exception->Delete();
	}
}

void CDlgViewPlc2::PLC_ConnectionCheckandReOpen()
{
	long lRet;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	lRet = m_ActUtlType2.Open();	// Open method exec
	if (lRet == 0x00) {
		m_iPlcOpenCount = 0;
		lpDlg->SetPlcConnectingStatus2("  connect    ");
	}
	else {
		m_iPlcOpenCount++;
		lpDlg->SetPlcConnectingStatus2("   error    ");
		if (m_iPlcOpenCount > PLC_RETRY_MAX_COUNT)
		{
			//AfxMessageBox("PLC2 연결상태를 확인해주세요.", MB_ICONINFORMATION);
			lpDlg->SetPlcConnectingStatus2("Disconnect");
			PLC_Close();
		}
	}
}

/****************************************************/
/*  Set Device Value                                */
/****************************************************/
void CDlgViewPlc2::PLC_WriteDevice(CString strDevice, int nType)
{
	long lValue;
	long lRet;
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	Sleep(10);
	if (strDevice == "") {
		// Not Enter DeviceName Error
		return;
	}

	if (m_bCheckOpened != TRUE)
		return;

	lValue = m_StPlcDataParam2[nType].lPlcData;
	try {
		// ActUtlType Control
		//lRet = m_ActUtlType2.SetDevice(strDevice, lValue);	// Exec SetDevice Method
		lRet = m_ActUtlType2.SetDevice2(strDevice, lValue);	// Exec SetDevice Method
		if (lRet != 0) {	//[JMW:220321]
			//PLC_ConnectionCheckandReOpen();
		}
		else {
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus2("  connect    ");
		}
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}
}

/****************************************************/
/*  Set Device Block Value                          */
/****************************************************/
void CDlgViewPlc2::PLC_RealNumberWriteDevice2(CString strDevice, int nType)
{
	BYTE bValue[4] = { 0, };
	short sDeviceValue[2] = { 0, };
	float m_DeviceValue = 0;
	short SendData[BLOCK_COUNT] = { 0, };

	long lSize = BLOCK_COUNT;
	CString strDeviceBack = _T("");
	LPCTSTR szDevice = _T("");
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	if (m_bCheckOpened != TRUE)
		return;

	for (int i = 0; i < BLOCK_COUNT; i += 2)
	{
		if (i % 2 == 0) {
			m_DeviceValue = m_StPlcDataBlockParam2[nType][i].fPlcDataBlock;
		}
		strDeviceBack.Format(_T("D%s"), strDevice);
		szDevice = strDeviceBack;//_T("D20")

		PLC_DeviceValue_Convert2(m_DeviceValue, bValue, sDeviceValue);

		m_StPlcDataBlockParam2[nType][i].sPlcDataBlock = sDeviceValue[0];
		m_StPlcDataBlockParam2[nType][i + 1].sPlcDataBlock = sDeviceValue[1];

		SendData[i] = m_StPlcDataBlockParam2[nType][i].sPlcDataBlock;
		SendData[i + 1] = m_StPlcDataBlockParam2[nType][i + 1].sPlcDataBlock;
	}

	int result = m_ActUtlType2.WriteDeviceBlock2(szDevice, lSize, SendData);
	if (0 == result) {
		m_iPlcOpenCount = 0;
		lpDlg->SetPlcConnectingStatus2("  connect    ");
		return;
	}
	else {
		//Add Alarm
		//PLC_ConnectionCheckandReOpen();
		return;
	}
}

void CDlgViewPlc2::PLC_WriteDeviceBlock(CString strDevice, int nType)
{
	Sleep(10);
	LPCTSTR szDevice = _T("");
	long lSize = BLOCK_COUNT;
	long SendData[BLOCK_COUNT] = { 0, };
	CString strName = _T("");
	CString strDeviceBack = _T("");
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	if (m_bCheckOpened != TRUE)
		return;

	int nStartCnt = atoi(strDevice);

	strDeviceBack.Format(_T("D%s"), strDevice);
	szDevice = strDeviceBack;//_T("D20")

// Renew ReturnValue 
	CString str = _T(""), str1 = _T("");

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		SendData[i] = m_StPlcDataBlockParam2[nType][i].lPlcDataBlock;

		str1.Format(_T("%d,"), SendData[i]);
		str += str1;
	}
	try {
		long lRet = m_ActUtlType2.WriteDeviceBlock(szDevice, lSize, SendData);	// Exec SetDevice Method

		if (lRet == 0x00) {	// Success
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus2("  connect    ");
		}
		else {
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
void CDlgViewPlc2::PLC_ReadDeviceBlock(CString strDevice, int nType)
{
	Sleep(100);
	long lRet = 0;
	LPCTSTR szDevice = _T("");
	long lSize = BLOCK_COUNT;
	long ReadData[BLOCK_COUNT] = { 0, };
	CString strName = _T("");
	CString strDeviceBack = _T("");
	CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();

	if (m_bCheckOpened != TRUE)
		return;
	strDeviceBack.Format(_T("D%s"), strDevice);
	szDevice = strDeviceBack;//_T("D20")

	try {

		lRet = m_ActUtlType2.ReadDeviceBlock(szDevice, lSize, ReadData);// Exec GetDevice Method

		if (lRet == 0x00) {	// Success
			CString str = _T(""), str1 = _T("");

			for (int i = 0; i < BLOCK_COUNT; i++)
			{
				strName.Format(_T("D%d"), atoi(strDevice) + i);
				m_StPlcDataBlockParam2[nType][i].AddressName = strName;

				m_StPlcDataBlockParam2[nType][i].lPlcDataBlock = ReadData[i];
				str1.Format(_T("R%d S%d,"), m_StPlcDataBlockParam2[nType][i].lPlcDataBlock, ReadData[i]);
				str += str1;
			}
			m_iPlcOpenCount = 0;
			lpDlg->SetPlcConnectingStatus2("  connect    ");
		}
		/*else {
			PLC_ConnectionCheckandReOpen();
		}*/
	}
	catch (COleDispatchException *Exception) {
		// OLE IDispatch Interface Error
		Exception->Delete();
	}

}

void CDlgViewPlc2::PLC_SendResultData2()
{
	PLC_RealNumberWriteDevice2(SEND_V_START_24, PLC_SEND_V_24);

	PLC_RealNumberWriteDevice2(SEND_A_START_24, PLC_SEND_A_24);

	PLC_RealNumberWriteDevice2(SEND_T_START_24, PLC_SEND_T_24);

}
/****************************************************/
/*  Get Device Block Value                           */
/****************************************************/
void CDlgViewPlc2::PLC_SendProcessData()
{
	PLC_WriteDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);
	PLC_WriteDevice(SEND_PROCESS_START, TYPE_SEND_PROCESS_START);
	PLC_WriteDevice(SEND_SCHEDULE_START, TYPE_SEND_SCHEDULE_START);
	PLC_WriteDevice(SEND_SCHEDULE_COMPLETE, TYPE_SEND_SCHEDULE_COMPLETE);
	PLC_WriteDevice(SEND_RESULT_PASS, TYPE_SEND_RESULT_PASS);
	PLC_WriteDevice(SEND_PROCESS_END, TYPE_SEND_PROCESS_END);
	PLC_WriteDevice(SEND_SEND_EMO, TYPE_SEND_EMO);

	/*PLC_WriteDevice(READ_READY, TYPE_READ_READY);
	PLC_WriteDevice(PLC_ALARM_OCCUR, TYPE_READ_PLC_ALARM_OCCUR);
	PLC_WriteDevice(PLC_LOAD_COMPLETE, TYPE_PLC_LOAD_COMPLETE);
	PLC_WriteDevice(PLC_UNLOAD_COMPLETE, TYPE_PLC_UNLOAD_COMPLETE);
	PLC_WriteDevice(PLC_PLC_EMO, TYPE_PLC_EMO);*/
}

void CDlgViewPlc2::PLC_ReadProcessData()
{
	PLC_ReadDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);
	PLC_ReadDevice(SEND_PROCESS_START, TYPE_SEND_PROCESS_START);
	PLC_ReadDevice(SEND_SCHEDULE_START, TYPE_SEND_SCHEDULE_START);
	PLC_ReadDevice(SEND_SCHEDULE_COMPLETE, TYPE_SEND_SCHEDULE_COMPLETE);
	PLC_ReadDevice(SEND_PROCESS_END, TYPE_SEND_PROCESS_END);
	PLC_ReadDevice(SEND_SEND_EMO, TYPE_SEND_EMO);		//[JMW:220325]	//EMO읽기로 수정


	PLC_ReadDevice(READ_HEART_BIT, TYPE_READ_HEART_BIT);
	PLC_ReadDevice(READ_READY, TYPE_READ_READY);
	PLC_ReadDevice(PLC_ALARM_OCCUR, TYPE_READ_PLC_ALARM_OCCUR);
	PLC_ReadDevice(PLC_LOAD_COMPLETE, TYPE_PLC_LOAD_COMPLETE);
	PLC_ReadDevice(PLC_UNLOAD_COMPLETE, TYPE_PLC_UNLOAD_COMPLETE);
	PLC_ReadDevice(PLC_PLC_EMO, TYPE_PLC_EMO);			//[JMW:220325]	//EMO읽기로 수정
}

void CDlgViewPlc2::PLC_ReadResultData()
{
	PLC_ReadDeviceBlock(SEND_V_START_24, PLC_SEND_V_24);

	PLC_ReadDeviceBlock(SEND_A_START_24, PLC_SEND_A_24);

	PLC_ReadDeviceBlock(SEND_T_START_24, PLC_SEND_T_24);
}



void CDlgViewPlc2::PlcDataBlockInit()
{
	for (int y = 0; y < PLC_SEND_MAX; y++)
	{
		for (int x = 0; x < BLOCK_COUNT; x++)
		{
			m_StPlcDataBlockParam2[y][x].AddressName = _T("");
			m_StPlcDataBlockParam2[y][x].lPlcDataBlock = 0;
			m_StPlcDataBlockParam2[y][x].sPlcDataBlock = 0;
			m_StPlcDataBlockParam2[y][x].dlPlcDataBlock = 0.0;
			m_StPlcDataBlockParam2[y][x].fPlcDataBlock = 0.0;
			
//			m_StPlcDataBlockParam[y][x].lPlcGetDataBlock = 0;
		}
	}
}
void CDlgViewPlc2::PlcDataInit()
{
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{
		m_StPlcDataParam2[i].lPlcData = 0;
		m_StPlcDataParam2[i].dlPlcData = 0.0;
		
	}

	ScheduleTime = 0;

	m_StPlcDataParam2[0].ProcessAddName = SEND_HEART_BIT;
	m_StPlcDataParam2[1].ProcessAddName = SEND_PROCESS_START;
	m_StPlcDataParam2[2].ProcessAddName = SEND_SCHEDULE_START;
	m_StPlcDataParam2[3].ProcessAddName = SEND_SCHEDULE_COMPLETE;
	m_StPlcDataParam2[4].ProcessAddName = SEND_PROCESS_END;
	m_StPlcDataParam2[5].ProcessAddName = SEND_SEND_EMO;			//[JMW:220325]  왜없었지???

	m_StPlcDataParam2[6].ProcessAddName = READ_HEART_BIT;
	m_StPlcDataParam2[7].ProcessAddName = READ_READY;
	m_StPlcDataParam2[8].ProcessAddName = PLC_ALARM_OCCUR;
	m_StPlcDataParam2[9].ProcessAddName = PLC_LOAD_COMPLETE;
	m_StPlcDataParam2[10].ProcessAddName = PLC_UNLOAD_COMPLETE;		
	m_StPlcDataParam2[11].ProcessAddName = PLC_PLC_EMO;				//[JMW:220325]   왜없었지???
}

//[+:JMW]
void CDlgViewPlc2::PLC_DeviceValue_Convert2(float fValue, BYTE* bValue, short* sDeviceValue) {

	BYTE bValuetoShort[2];

	*((float *)bValue) = fValue;

	bValuetoShort[0] = bValue[0];
	bValuetoShort[1] = bValue[1];
	sDeviceValue[0] = *((short *)bValuetoShort);

	bValuetoShort[0] = bValue[2];
	bValuetoShort[1] = bValue[3];
	sDeviceValue[1] = *((short *)bValuetoShort);
}

void CDlgViewPlc2::SetPlcProcessStart()
{
	m_PlcProcessSequence2->m_RunStopFlag = TRUE;
	m_PlcProcessSequence2->SetAutoStep(m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_INIT);
}
void CDlgViewPlc2::SetPlcProcessStop()
{
	m_PlcProcessSequence2->m_RunStopFlag = FALSE;
}

void CDlgViewPlc2::SetPlcProcessReset()
{
	m_PlcProcessSequence2->SetAutoStep(m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_INIT);
	m_PlcProcessSequence2->m_RunStopFlag = FALSE;
}

void CDlgViewPlc2::SetPlcThreadStop()
{
	m_PlcProcessSequence2->ThreadStop();
}

void CDlgViewPlc2::OnTimer(UINT_PTR nIDEvent)
{
	//[JMW:220325]
	if (nIDEvent == ScheduleloadTimeID)
	{
		ScheduleTime++;
	}
	__super::OnTimer(nIDEvent);
}

//[JMW:220325]
void CDlgViewPlc2::SetKillScheduleTimer(BOOL timeflag) {
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
