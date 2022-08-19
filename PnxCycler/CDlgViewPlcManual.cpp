﻿// CDlgViewPlcManual.cpp: 구현 파일
//

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewPlcManual.h"
#include "afxdialogex.h"

#include "MgrConfig.h"
#include "MgrDio.h"
#include "MgrStatus.h"

#include "CPlcProcessSequence1.h"
#include "CPlcProcessSequence2.h"

#define	IO_OPERTAION_STEP_UPDATE_TIMER_ID	20001
#define	TIMER_SET_1S						1000
#define STEP_UI_TIMER 21000

//#define EQP_STAGE_INDEX_S1 = 0

// CDlgViewPlcManual 대화 상자 

IMPLEMENT_DYNAMIC(CDlgViewPlcManual, CDialogEx)

CDlgViewPlcManual::CDlgViewPlcManual(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_PLC_MANUAL, pParent)
{
	m_strBcrBackData = _T("");
}

CDlgViewPlcManual::~CDlgViewPlcManual()
{
}

void CDlgViewPlcManual::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// ----------------------------------------------------------------------------
	//[JMW:220318] PLC Button
	DDX_Control(pDX, IDC_PLC1_SEND_DATA1, m_bPlc1Bit[TYPE_SEND_HEART_BIT]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA2, m_bPlc1Bit[TYPE_SEND_PROCESS_START]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA3, m_bPlc1Bit[TYPE_SEND_SCHEDULE_START]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA4, m_bPlc1Bit[TYPE_SEND_SCHEDULE_COMPLETE]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA7, m_bPlc1Bit[TYPE_SEND_RESULT_PASS]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA5, m_bPlc1Bit[TYPE_SEND_PROCESS_END]);
	DDX_Control(pDX, IDC_PLC1_SEND_DATA6, m_bPlc1Bit[TYPE_SEND_EMO]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA1, m_bPlc1Bit[TYPE_READ_HEART_BIT]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA2, m_bPlc1Bit[TYPE_READ_READY]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA3, m_bPlc1Bit[TYPE_READ_PLC_ALARM_OCCUR]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA4, m_bPlc1Bit[TYPE_PLC_LOAD_COMPLETE]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA5, m_bPlc1Bit[TYPE_PLC_UNLOAD_COMPLETE]);
	DDX_Control(pDX, IDC_PLC1_READ_DATA6, m_bPlc1Bit[TYPE_PLC_EMO]);
	//[JMW:220318] PLC2 Button
	DDX_Control(pDX, IDC_PLC2_SEND_DATA1, m_bPlc2Bit[TYPE_SEND_HEART_BIT]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA2, m_bPlc2Bit[TYPE_SEND_PROCESS_START]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA3, m_bPlc2Bit[TYPE_SEND_SCHEDULE_START]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA7, m_bPlc2Bit[TYPE_SEND_RESULT_PASS]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA4, m_bPlc2Bit[TYPE_SEND_SCHEDULE_COMPLETE]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA5, m_bPlc2Bit[TYPE_SEND_PROCESS_END]);
	DDX_Control(pDX, IDC_PLC2_SEND_DATA6, m_bPlc2Bit[TYPE_SEND_EMO]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA1, m_bPlc2Bit[TYPE_READ_HEART_BIT]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA2, m_bPlc2Bit[TYPE_READ_READY]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA3, m_bPlc2Bit[TYPE_READ_PLC_ALARM_OCCUR]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA4, m_bPlc2Bit[TYPE_PLC_LOAD_COMPLETE]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA5, m_bPlc2Bit[TYPE_PLC_UNLOAD_COMPLETE]);
	DDX_Control(pDX, IDC_PLC2_READ_DATA6, m_bPlc2Bit[TYPE_PLC_EMO]);
}

BEGIN_MESSAGE_MAP(CDlgViewPlcManual, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MOXA_LOAD, &CDlgViewPlcManual::OnBnClickedMoxaLoad)
	ON_BN_CLICKED(IDC_MOXA_STOP, &CDlgViewPlcManual::OnBnClickedMoxaStop)
	ON_BN_CLICKED(IDC_PLC_OPEN, &CDlgViewPlcManual::OnBnClickedPlcOpen)
	ON_BN_CLICKED(IDC_PLC_CLOSE, &CDlgViewPlcManual::OnBnClickedPlcClose)
	ON_BN_CLICKED(IDC_PLC_OPEN2, &CDlgViewPlcManual::OnBnClickedPlcOpen2)
	ON_BN_CLICKED(IDC_PLC_CLOSE2, &CDlgViewPlcManual::OnBnClickedPlcClose2)
	ON_BN_CLICKED(IDC_PLC_SEND_READDATA, &CDlgViewPlcManual::OnBnClickedPlcSendReaddata)
	ON_BN_CLICKED(IDC_PLC_TEST_START, &CDlgViewPlcManual::OnBnClickedPlcTestStart)
	ON_BN_CLICKED(IDC_PLC_TEST_STOP, &CDlgViewPlcManual::OnBnClickedPlcTestStop)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA2, &CDlgViewPlcManual::OnBnClickedPlc1SendData2)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA3, &CDlgViewPlcManual::OnBnClickedPlc1SendData3)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA4, &CDlgViewPlcManual::OnBnClickedPlc1SendData4)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA5, &CDlgViewPlcManual::OnBnClickedPlc1SendData5)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA6, &CDlgViewPlcManual::OnBnClickedPlc1SendData6)
	ON_BN_CLICKED(IDC_PLC1_READ_DATA2, &CDlgViewPlcManual::OnBnClickedPlc1ReadData2)
	ON_BN_CLICKED(IDC_PLC1_READ_DATA3, &CDlgViewPlcManual::OnBnClickedPlc1ReadData3)
	ON_BN_CLICKED(IDC_PLC1_READ_DATA4, &CDlgViewPlcManual::OnBnClickedPlc1ReadData4)
	ON_BN_CLICKED(IDC_PLC1_READ_DATA5, &CDlgViewPlcManual::OnBnClickedPlc1ReadData5)
	ON_BN_CLICKED(IDC_PLC1_READ_DATA6, &CDlgViewPlcManual::OnBnClickedPlc1ReadData6)
	ON_BN_CLICKED(IDC_PLC1_BIT_RESET, &CDlgViewPlcManual::OnBnClickedPlc1BitReset)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_PLC_TEST_START2, &CDlgViewPlcManual::OnBnClickedPlcTestStart2)
	ON_BN_CLICKED(IDC_PLC2_READ_DATA2, &CDlgViewPlcManual::OnBnClickedPlc2ReadData2)
	ON_BN_CLICKED(IDC_PLC2_READ_DATA3, &CDlgViewPlcManual::OnBnClickedPlc2ReadData3)
	ON_BN_CLICKED(IDC_PLC2_READ_DATA4, &CDlgViewPlcManual::OnBnClickedPlc2ReadData4)
	ON_BN_CLICKED(IDC_PLC2_READ_DATA5, &CDlgViewPlcManual::OnBnClickedPlc2ReadData5)
	ON_BN_CLICKED(IDC_PLC2_READ_DATA6, &CDlgViewPlcManual::OnBnClickedPlc2ReadData6)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA2, &CDlgViewPlcManual::OnBnClickedPlc2SendData2)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA3, &CDlgViewPlcManual::OnBnClickedPlc2SendData3)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA4, &CDlgViewPlcManual::OnBnClickedPlc2SendData4)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA5, &CDlgViewPlcManual::OnBnClickedPlc2SendData5)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA6, &CDlgViewPlcManual::OnBnClickedPlc2SendData6)
	ON_BN_CLICKED(IDC_PLC2_BIT_RESET, &CDlgViewPlcManual::OnBnClickedPlc2BitReset)
	ON_BN_CLICKED(IDC_PLC_TEST_STOP2, &CDlgViewPlcManual::OnBnClickedPlcTestStop2)
	ON_BN_CLICKED(IDC_PLC_SEND_READDATA2, &CDlgViewPlcManual::OnBnClickedPlcSendReaddata2)
	ON_BN_CLICKED(IDC_PLC1_SEND_DATA7, &CDlgViewPlcManual::OnBnClickedPlc1SendData7)
	ON_BN_CLICKED(IDC_PLC2_SEND_DATA7, &CDlgViewPlcManual::OnBnClickedPlc1SendData7)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

// CDlgViewPlcManual 메시지 처리기

void CDlgViewPlcManual::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	InitializeTimers();
}

HBRUSH CDlgViewPlcManual::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (CTLCOLOR_DLG == nCtlColor)
	{
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void CDlgViewPlcManual::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (IO_OPERTAION_STEP_UPDATE_TIMER_ID == nIDEvent)
	{
		//	UpdateManualOperationStep();
		CommPlcAndPc();		//[JMW:220315]
		CommPlcAndPc2();
	}

	if (STEP_UI_TIMER == nIDEvent)
	{
		//UI_Update_IO();

		//BCR 상태 확인
		//CMgrDio* pMgrDio = CMgrDio::GetMgr();
		//if (pMgrDio->GetBcrRead() == FALSE)
		//{
		//	if (pMgrDio->m_strBcrReadData != _T(""))
		//	{
		//		m_strBcrBackData = pMgrDio->m_strBcrReadData;
		//		pMgrDio->m_strBcrReadData = _T("");
		//		SetBcrData(m_strBcrBackData);
		//	}
		//}
		//UI_Step_Stage2_Status();
		//UI_Step_Stage1_Status();
	}

	__super::OnTimer(nIDEvent);
}

void CDlgViewPlcManual::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	KillTimer(IO_OPERTAION_STEP_UPDATE_TIMER_ID);
	KillTimer(STEP_UI_TIMER);

	CDialogEx::OnClose();
}

void CDlgViewPlcManual::OnDestroy()
{
	CDialogEx::OnDestroy();

	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	pDlgViewPlc1->SetPlcThreadStop();
	pDlgViewPlc2->SetPlcThreadStop();
	Sleep(100);
}

// -------------------------------------------------------------------------------------------------

void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1RunmodeAuto()
{
	CMgrStatus::GetMgr()->GetSubEqpStageRunMode(m_nSelectedSubEqpNo, 0);
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1RunmodeManual()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1SideCenteringFwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1FrontbackCenteringFwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1TopCylinderDescend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1BottomCylinderAscend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1BottomCylinderDescend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1TopCylinderAcend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1FrontbackCenteringBwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS1SideCenteringBwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// -----------------------------------------------------------------------------

void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2RunmodeAuto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2RunmodeManual()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2SideCenteringFwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2FrontbackCenteringFwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2TopCylinderDescend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2BottomCylinderAscend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2BottomCylinderDescend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2TopCylinderAcend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2FrontbackCenteringBwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewPlcManual::OnBnClickedBtnViewIoManualS2SideCenteringBwd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// -------------------------------------------------------------------------------------------------

BOOL CDlgViewPlcManual::Create(CWnd * pParentWnd)
{
	//return 0;
	return CDialogEx::Create(IDD, pParentWnd);
}

void CDlgViewPlcManual::BitButtonColorAllReset() {

	for (int i = 0; i < 12; i++) {
		m_bPlc1Bit[i].EnableWindowsTheming(FALSE);
		m_bPlc1Bit[i].SetFaceColor(RGB(224, 224, 224));
		m_bPlc2Bit[i].EnableWindowsTheming(FALSE);
		m_bPlc2Bit[i].SetFaceColor(RGB(224, 224, 224));
	}
	//SKINBTN(m_bPlc2Bit[0], IDC_PLC_SEND_DATA22, this, "C:\\Users\\user\\Desktop\\정민우\피닉슨\\0.bmp");
}

BOOL CDlgViewPlcManual::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow();

	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgViewPlcManual::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgViewPlcManual::Initialize()
{	
	InitializeVariables();

	InitializeControls();

	//InitializeTimers();
}

void CDlgViewPlcManual::InitializeVariables()
{
	CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();
	
	BitButtonColorAllReset();
}

void CDlgViewPlcManual::InitializeControls()
{
	//COMBO


	// --------------------------------------------------------------------------
		

}

void CDlgViewPlcManual::InitializeTimers()
{
	SetTimer(IO_OPERTAION_STEP_UPDATE_TIMER_ID, TIMER_SET_1S/2, NULL);
	SetTimer(STEP_UI_TIMER, TIMER_SET_1S/2, NULL);
}

// -----------------------------------------------------------------------------

//void CDlgViewPlcManual::SetOperationStepColorS1(int nSubEqpNo, enumProcessStepManualS1 nOperationStepNo)
//{
	//for (int controlCount = 0; controlCount < OPERATION_MAX_COUNT; controlCount++)
	//{
	//	COLORREF setColor = RGB(240, 240, 240);
	//	m_staticOperationControlS1[controlCount].SetBkColor(setColor, setColor);
	//}

	//// ----------------------------------------------------------------------------

	//switch (nOperationStepNo)
	//{
	//case MANUAL_S1_PORT_INPUT_READY:
	//case MANUAL_S1_PORT_INPUT_READY_CHECK:
	//case MANUAL_S1_TRAY_INPUT:
	//	m_staticOperationControlS1[0].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_TRAY_SENSOR_DETECT_INPUT:
	//case MANUAL_S1_PORT_INPUT_COMPLETE:
	//case MANUAL_S1_PORT_INPUT_COMPLETE_CHECK:
	//case MANUAL_S1_PORT_LOAD_HANDLE_READY:
	//case MANUAL_S1_PORT_LOAD_HANDLE_READY_CHECK:
	//case MANUAL_S1_SIDE_CENTERING_FORWARD:
	//	m_staticOperationControlS1[1].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_SIDE_CENTERING_FORWARD_CHECK:
	//case MANUAL_S1_FRONT_BACK_CENTERING_FORWARD:
	//	m_staticOperationControlS1[2].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_FRONT_BACK_CENTERING_FORWARD_CHECK:
	//case MANUAL_S1_TOP_CYLINDER_DESCEND:
	//	m_staticOperationControlS1[3].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_TOP_CYLINDER_DESCEND_CHECK:
	//case MANUAL_S1_BOTTOM_CYLINDER_ASCEND:
	//	m_staticOperationControlS1[4].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_BOTTOM_CYLINDER_ASCEND_CHECK:
	//case MANUAL_S1_PORT_LOAD_HANDLE_COMPLETE:
	//case MANUAL_S1_PORT_LOAD_HANDLE_COMPLETE_CHECK:
	//	m_staticOperationControlS1[5].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_CHARGE_DISCHARGE_PROCESS_START:
	//case MANUAL_S1_CHARGE_DISCHARGE_PROCESS_START_CHECK:
	//	m_staticOperationControlS1[6].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_CHARGE_DISCHARGE_PROCESS_END:
	//case MANUAL_S1_CHARGE_DISCHARGE_PROCESS_END_CHECK:
	//case MANUAL_S1_PORT_UNLOAD_HANDLE_READY:
	//case MANUAL_S1_PORT_UNLOAD_HANDLE_READY_CHECK:
	//case MANUAL_S1_BOTTOM_CYLINDER_DESCEND:
	//	m_staticOperationControlS1[7].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_BOTTOM_CYLINDER_DESCEND_CHECK:
	//case MANUAL_S1_TOP_CYLINDER_ASCEND:
	//	m_staticOperationControlS1[8].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_TOP_CYLINDER_ASCEND_CHECK:
	//case MANUAL_S1_FRONT_BACK_CENTERING_BACKWARD:
	//	m_staticOperationControlS1[9].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_FRONT_BACK_CENTERING_BACKWARD_CHECK:
	//case MANUAL_S1_SIDE_CENTERING_BACKWARD:
	//	m_staticOperationControlS1[10].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_SIDE_CENTERING_BACKWARD_CHECK:
	//case MANUAL_S1_PORT_UNLOAD_HANDLE_COMPLETE:
	//case MANUAL_S1_PORT_UNLOAD_HANDLE_COMPLETE_CHECK:
	//case MANUAL_S1_TRAY_OUTPUT:
	//	m_staticOperationControlS1[11].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//case MANUAL_S1_TRAY_SENSOR_DETECT_OUTPUT:
	//case MANUAL_S1_PORT_OUTPUT_COMPLETE:
	//case MANUAL_S1_PORT_OUTPUT_COMPLETE_CHECK:
	//	m_staticOperationControlS1[12].SetBkColor(COLOR_BLUE, COLOR_BLUE);
	//	break;
	//default:
	//	break;
	//}

	// ----------------------------------------------------------------------------

//}

//void CDlgViewPlcManual::SetOperationStepColorS2(int nSubEqpNo, enumProcessStepManualS2 nOperationStepNo)
//{
//	//for (int controlCount = 0; controlCount < OPERATION_MAX_COUNT; controlCount++)
//	//{
//	//	COLORREF setColor = RGB(240, 240, 240);
//	//	m_staticOperationControlS2[controlCount].SetBkColor(setColor, setColor);
//	//}
//
//	//// ----------------------------------------------------------------------------
//
//	//switch (nOperationStepNo)
//	//{
//	//case MANUAL_S2_PORT_INPUT_READY:
//	//case MANUAL_S2_PORT_INPUT_READY_CHECK:
//	//case MANUAL_S2_TRAY_INPUT:
//	//	m_staticOperationControlS2[0].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_TRAY_SENSOR_DETECT_INPUT:
//	//case MANUAL_S2_PORT_INPUT_COMPLETE:
//	//case MANUAL_S2_PORT_INPUT_COMPLETE_CHECK:
//	//case MANUAL_S2_PORT_LOAD_HANDLE_READY:
//	//case MANUAL_S2_PORT_LOAD_HANDLE_READY_CHECK:
//	//case MANUAL_S2_SIDE_CENTERING_FORWARD:
//	//	m_staticOperationControlS2[1].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_SIDE_CENTERING_FORWARD_CHECK:
//	//case MANUAL_S2_FRONT_BACK_CENTERING_FORWARD:
//	//	m_staticOperationControlS2[2].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_FRONT_BACK_CENTERING_FORWARD_CHECK:
//	//case MANUAL_S2_TOP_CYLINDER_DESCEND:
//	//	m_staticOperationControlS2[3].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_TOP_CYLINDER_DESCEND_CHECK:
//	//case MANUAL_S2_BOTTOM_CYLINDER_ASCEND:
//	//	m_staticOperationControlS2[4].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_BOTTOM_CYLINDER_ASCEND_CHECK:
//	//case MANUAL_S2_PORT_LOAD_HANDLE_COMPLETE:
//	//case MANUAL_S2_PORT_LOAD_HANDLE_COMPLETE_CHECK:
//	//	m_staticOperationControlS2[5].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_CHARGE_DISCHARGE_PROCESS_START:
//	//case MANUAL_S2_CHARGE_DISCHARGE_PROCESS_START_CHECK:
//	//	m_staticOperationControlS2[6].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_CHARGE_DISCHARGE_PROCESS_END:
//	//case MANUAL_S2_CHARGE_DISCHARGE_PROCESS_END_CHECK:
//	//case MANUAL_S2_PORT_UNLOAD_HANDLE_READY:
//	//case MANUAL_S2_PORT_UNLOAD_HANDLE_READY_CHECK:
//	//case MANUAL_S2_BOTTOM_CYLINDER_DESCEND:
//	//	m_staticOperationControlS2[7].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_BOTTOM_CYLINDER_DESCEND_CHECK:
//	//case MANUAL_S2_TOP_CYLINDER_ASCEND:
//	//	m_staticOperationControlS2[8].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_TOP_CYLINDER_ASCEND_CHECK:
//	//case MANUAL_S2_FRONT_BACK_CENTERING_BACKWARD:
//	//	m_staticOperationControlS2[9].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_FRONT_BACK_CENTERING_BACKWARD_CHECK:
//	//case MANUAL_S2_SIDE_CENTERING_BACKWARD:
//	//	m_staticOperationControlS2[10].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_SIDE_CENTERING_BACKWARD_CHECK:
//	//case MANUAL_S2_PORT_UNLOAD_HANDLE_COMPLETE:
//	//case MANUAL_S2_PORT_UNLOAD_HANDLE_COMPLETE_CHECK:
//	//case MANUAL_S2_TRAY_OUTPUT:
//	//	m_staticOperationControlS2[11].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//case MANUAL_S2_TRAY_SENSOR_DETECT_OUTPUT:
//	//case MANUAL_S2_PORT_OUTPUT_COMPLETE:
//	//case MANUAL_S2_PORT_OUTPUT_COMPLETE_CHECK:
//	//	m_staticOperationControlS2[12].SetBkColor(COLOR_BLUE, COLOR_BLUE);
//	//	break;
//	//default:
//	//	break;
//	//}
//
//	// ----------------------------------------------------------------------------
//}

void CDlgViewPlcManual::UI_Step_Stage1_Blink(int nCurrentStep, bool bBlinkFlag)
{
	static bool bBlink = false;

	if (!bBlinkFlag)
	{
		return;
	}
	if (bBlink)
	{
		m_staticButton_Stage1_Step[nCurrentStep].SetShade(COLOR_GREEN, 8, 10, 62, COLOR_GREEN);
		m_staticButton_Stage1_Step[nCurrentStep].SetFont("Verdana", 16, 1200);
		m_staticButton_Stage1_Step[nCurrentStep].SetTextColor(ColorWhite);
		bBlink = false;
	}
	else
	{
		m_staticButton_Stage1_Step[nCurrentStep].SetShade(COLOR_WAIT_BTN, 8, 10, 62, COLOR_WAIT_BTN);
		m_staticButton_Stage1_Step[nCurrentStep].SetFont("Verdana", 16, 1200);
		m_staticButton_Stage1_Step[nCurrentStep].SetTextColor(ColorBlack);
		bBlink = true;
	}
}

void CDlgViewPlcManual::UI_Step_Stage2_Blink(int nCurrentStep, bool bBlinkFlag)
{
	static bool bBlink = false;

	if (!bBlinkFlag)
	{
		return;
	}
	if (bBlink)
	{
		m_staticButton_Stage2_Step[nCurrentStep].SetShade(COLOR_GREEN, 8, 10, 62, COLOR_GREEN);
		m_staticButton_Stage2_Step[nCurrentStep].SetFont("Verdana", 16, 1200);
		m_staticButton_Stage2_Step[nCurrentStep].SetTextColor(ColorWhite);
		bBlink = false;
	}
	else
	{
		m_staticButton_Stage2_Step[nCurrentStep].SetShade(COLOR_WAIT_BTN, 8, 10, 62, COLOR_WAIT_BTN);
		m_staticButton_Stage2_Step[nCurrentStep].SetFont("Verdana", 16, 1200);
		m_staticButton_Stage2_Step[nCurrentStep].SetTextColor(ColorBlack);
		bBlink = true;
	}
}

void CDlgViewPlcManual::UI_Step_Stage1_Status()
{
	_StageStepStatusInfo nStep = CMgrStatus::GetMgr()->Get_Status_Stage1();
	if (nStep.bInit)
	{
		for (int i = 0; i < CYLINDER_STEP_COUNT; i++)
		{
			m_staticButton_Stage1_Step[i].SetShade(COLOR_WAIT_BTN, 8, 10, 62, COLOR_WAIT_BTN);
			m_staticButton_Stage1_Step[i].SetFont("Verdana", 16, 1200);
			m_staticButton_Stage1_Step[i].SetTextColor(ColorBlack);
			m_staticButton_Stage1_Step[i].EnableWindow(false);
			m_staticButton_Stage1_Step[i].Invalidate(false);
			UI_Step_Stage1_Blink(i, false);
		}
		nStep.bInit = false;
	}
	if (nStep.nCurrentStep == eSequenceCylinderStep_Wait)
	{
		return;
	}
	//Complete
	if (nStep.nStatus == 2)
	{
		//현재 스텝 점등
		m_staticButton_Stage1_Step[nStep.nCurrentStep].SetShade(COLOR_GREEN, 8, 10, 62, COLOR_GREEN);
		m_staticButton_Stage1_Step[nStep.nCurrentStep].Invalidate(false);
		//마지막 스텝 제외
		if (nStep.nCurrentStep != eSequenceCylinderStep_Side_Backward)
		{
			//다음 스텝 Blink
			UI_Step_Stage1_Blink(nStep.nCurrentStep + 1, true);
			//다음 스텝 버튼 활성화
			m_staticButton_Stage1_Step[nStep.nCurrentStep + 1].EnableWindow(true);
			m_staticButton_Stage1_Step[nStep.nCurrentStep + 1].Invalidate(false);
		}
	}
	//Processing
	else if (nStep.nStatus == 1)
	{
		//현재 스텝 Blink
		UI_Step_Stage1_Blink(nStep.nCurrentStep, true);
		//현재 스텝 버튼 비활성화
		m_staticButton_Stage1_Step[nStep.nCurrentStep].EnableWindow(false);
		m_staticButton_Stage1_Step[nStep.nCurrentStep].Invalidate(false);
	}
	//Wait
	else
	{
		//현재 스텝 Blink
		UI_Step_Stage1_Blink(nStep.nCurrentStep, true);
		//현재 스텝 버튼 활성화
		m_staticButton_Stage1_Step[nStep.nCurrentStep].EnableWindow(true);
		m_staticButton_Stage1_Step[nStep.nCurrentStep].Invalidate(false);
	}
}

void CDlgViewPlcManual::UI_Step_Stage2_Status()
{
	_StageStepStatusInfo nStep = CMgrStatus::GetMgr()->Get_Status_Stage2();

	if (nStep.bInit)
	{
		for (int i = 0; i < CYLINDER_STEP_COUNT; i++)
		{
			m_staticButton_Stage2_Step[i].SetShade(COLOR_WAIT_BTN, 8, 10, 62, COLOR_WAIT_BTN);
			m_staticButton_Stage2_Step[i].SetFont("Verdana", 16, 1200);
			m_staticButton_Stage2_Step[i].SetTextColor(ColorBlack);
			m_staticButton_Stage2_Step[i].EnableWindow(false);
			m_staticButton_Stage2_Step[i].Invalidate(false);
			UI_Step_Stage2_Blink(i, false);
		}
		nStep.bInit = false;
	}
	if (nStep.nCurrentStep == eSequenceCylinderStep_Wait)
	{
		return;
	}
	//Complete
	if (nStep.nStatus == 2)
	{
		//현재 스텝 점등
		m_staticButton_Stage2_Step[nStep.nCurrentStep].SetShade(COLOR_GREEN, 8, 10, 62, COLOR_GREEN);
		m_staticButton_Stage2_Step[nStep.nCurrentStep].Invalidate(false);
		//마지막 스텝 제외
		if (nStep.nCurrentStep != eSequenceCylinderStep_Side_Backward)
		{
			//다음 스텝 Blink
			UI_Step_Stage2_Blink(nStep.nCurrentStep + 1, true);
			//다음 스텝 버튼 활성화
			m_staticButton_Stage2_Step[nStep.nCurrentStep + 1].EnableWindow(true);
			m_staticButton_Stage2_Step[nStep.nCurrentStep + 1].Invalidate(false);
		}
	}
	//Processing
	else if (nStep.nStatus == 1)
	{
		//현재 스텝 Blink
		UI_Step_Stage2_Blink(nStep.nCurrentStep, true);
		//현재 스텝 버튼 비활성화
		m_staticButton_Stage2_Step[nStep.nCurrentStep].EnableWindow(false);
		m_staticButton_Stage2_Step[nStep.nCurrentStep].Invalidate(false);
	}
	//Wait
	else
	{
		//현재 스텝 Blink
		UI_Step_Stage2_Blink(nStep.nCurrentStep, true);
		//현재 스텝 버튼 활성화
		m_staticButton_Stage2_Step[nStep.nCurrentStep].EnableWindow(true);
		m_staticButton_Stage2_Step[nStep.nCurrentStep].Invalidate(false);
	}
}

void CDlgViewPlcManual::OnBnClickedMoxaLoad()
{
	//CMgrDio* pMgrDio = CMgrDio::GetMgr();
	//
	//pMgrDio->m_ManualStartStop = FALSE;
	//pMgrDio->m_ManualLoadStartFlag = TRUE;
	//
	//pMgrDio->SetBcrRead(TRUE);
	//pMgrDio->m_ePressRunstepMeasurementCurrent = PC_BCR_CHECK;
}

void CDlgViewPlcManual::OnBnClickedMoxaStop()
{
	//CMgrDio* pMgrDio = CMgrDio::GetMgr();
	//
	//pMgrDio->m_ManualStartStop = TRUE;
	//pMgrDio->m_ManualLoadStartFlag = FALSE;
	//pMgrDio->m_ManualUnloadStartFlag = FALSE;
	//pMgrDio->m_RepeatRunStart = FALSE;
	//pMgrDio->m_ePressRunstepMeasurementCurrent = STEP_END;
}

void CDlgViewPlcManual::OnBnClickedPlcOpen()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	pDlgViewPlc1->PLC_Open();

	Sleep(100);
	if (pDlgViewPlc1->m_bCheckOpened != TRUE) {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "Disconnect");
	}
	else {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "  Connect   ");
	}
}


//[JMW:220321] 임시
void CDlgViewPlcManual::SetPlcConnectingStatus(CString str)
{
	SetDlgItemText(IDC_STATIC_VIEW_CONNECT, str);
}

void CDlgViewPlcManual::SetPlcConnectingStatus2(CString str)
{
	SetDlgItemText(IDC_STATIC_VIEW_CONNECT2, str);
}

void CDlgViewPlcManual::OnBnClickedPlcClose()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	pDlgViewPlc1->PLC_Close();
	SetDlgItemText(IDC_STATIC_VIEW_CONNECT, "Disconnect");
}

void CDlgViewPlcManual::ButtonBackgroundValue(int* Rgb, CString bit) {
	if ("1" == bit)	// Green Yellow
	{
		Rgb[0] = 173;
		Rgb[1] = 255;
		Rgb[2] = 47;
	}
	else {	// Gray
		Rgb[0] = 224;
		Rgb[1] = 224;
		Rgb[2] = 224;
	}
}

//[JMW:220314]
void CDlgViewPlcManual::CommPlcAndPc() {
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CString str;
	int RgbColor[3] = { 0, };
	
	//[JMW:220314] PC -> PLC 통신 상태 , PLC -> PC 통신상태
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{
		str.Format("%d", pDlgViewPlc1->m_StPlcDataParam[i].lPlcData);
		m_bPlc1Bit[i].SetWindowText(str);
		ButtonBackgroundValue(RgbColor, str);
		m_bPlc1Bit[i].SetFaceColor(RGB(RgbColor[0], RgbColor[1], RgbColor[2]));
	}
}

void CDlgViewPlcManual::CommPlcAndPc2() {
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString str;
	int RgbColor[3] = { 0, };

	//[JMW:220314] PC -> PLC 통신 상태 , PLC -> PC 통신상태
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{
		str.Format("%d", pDlgViewPlc2->m_StPlcDataParam2[i].lPlcData);
		m_bPlc2Bit[i].SetWindowText(str);
		ButtonBackgroundValue(RgbColor, str);
		m_bPlc2Bit[i].SetFaceColor(RGB(RgbColor[0], RgbColor[1], RgbColor[2]));
	}
}

void CDlgViewPlcManual::OnBnClickedPlc1BitReset()
{	
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//[JMW:220315]
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{	
		if ((i == TYPE_SEND_HEART_BIT) || (i == TYPE_READ_HEART_BIT))
			continue;
		pDlgViewPlc1->m_StPlcDataParam[i].lPlcData = BIT_OFF;
	}

	pDlgViewPlc1->PLC_SendProcessData();

	//[JMW:220325] 시퀀스도 처음로 Return
	pDlgViewPlc1->m_PlcProcessSequence1->SetAutoStep(pDlgViewPlc1->m_PlcProcessSequence1->enumProcessStepAuto::AUTO_INIT);
}

void CDlgViewPlcManual::SetEditSendData()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();

	CString str = _T(""), strBack = _T("");
	//PLC -> PC READ
	for (int x = 0; x < TYPE_PLC_MAX / 2; x++)
	{
		str.Format(_T("D%d:%d, "), (x + 1), pDlgViewPlc1->m_StPlcDataParam[x].lPlcData);
		strBack += str;
	}
}

void CDlgViewPlcManual::SetEditSendData2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();

	CString str = _T(""), strBack = _T("");
	//PLC -> PC READ
	for (int x = 0; x < TYPE_PLC_MAX / 2; x++)
	{
		str.Format(_T("D%d:%d, "), (x + 1), pDlgViewPlc2->m_StPlcDataParam2[x].lPlcData);
		strBack += str;
	}
}

void CDlgViewPlcManual::SetPlcSend()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	if (pDlgViewPlc1->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc1->PLC_SendProcessData();
	Sleep(100);
	GetPlcReceive();
}

void CDlgViewPlcManual::SetPlcSend2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	if (pDlgViewPlc2->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc2->PLC_SendProcessData();
	Sleep(100);
	GetPlcReceive2();
}

void CDlgViewPlcManual::GetPlcReceive()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	if (pDlgViewPlc1->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc1->PLC_ReadProcessData();

	CString str = _T(""), strBack = _T("");
	//PLC -> PC READ
	for (int x = 0; x < TYPE_PLC_MAX; x++)
	{
		str.Format(_T("D%d:%d, "), (x + 1), pDlgViewPlc1->m_StPlcDataParam[x].lPlcData);
		strBack += str;
	}
}

void CDlgViewPlcManual::GetPlcReceive2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	if (pDlgViewPlc2->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc2->PLC_ReadProcessData();

	CString str = _T(""), strBack = _T("");
	//PLC -> PC READ
	for (int x = 0; x < TYPE_PLC_MAX; x++)
	{
		str.Format(_T("D%d:%d, "), (x + 1), pDlgViewPlc2->m_StPlcDataParam2[x].lPlcData);
		strBack += str;
	}
}


void CDlgViewPlcManual::GetPlcReceiveBlock()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	if (pDlgViewPlc1->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc1->PLC_ReadProcessData();

	CString str = _T(""), strBack = _T("");
	
	//SEND_V_START1_24
	for (int x = 0; x < BLOCK_COUNT; x++)
	{
		str.Format(_T("D%d:%d,"), (x + atoi(SEND_V_START_24)), pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_V_24][x].lPlcDataBlock);
		strBack += str;
	}

	//SEND_A_START1_24
	str = _T(""), strBack = _T("");
	for (int x = 0; x < BLOCK_COUNT; x++)
	{
		str.Format(_T("D%d:%d,"), (x + atoi(SEND_A_START_24)), pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_A_24][x].lPlcDataBlock);
		strBack += str;
	}

	//SEND_T_START1_24
	str = _T(""), strBack = _T("");
	for (int x = 0; x < BLOCK_COUNT; x++)
	{
		str.Format(_T("D%d:%d,"), (x + atoi(SEND_T_START_24)), pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_T_24][x].lPlcDataBlock);
		strBack += str;
	}
}

void CDlgViewPlcManual::SetBcrData(CString strBcrData)
{
	SetDlgItemText(IDC_BCR_NAME, strBcrData);
}

void CDlgViewPlcManual::OnBnClickedPlcOpen2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	pDlgViewPlc2->PLC_Open();

	Sleep(100);
	if (pDlgViewPlc2->m_bCheckOpened != TRUE) {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT2, "Disconnect");
	}
	else {
		SetDlgItemText(IDC_STATIC_VIEW_CONNECT2, "  Connect   ");
	}
}

void CDlgViewPlcManual::OnBnClickedPlcClose2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	pDlgViewPlc2->PLC_Close();
	SetDlgItemText(IDC_STATIC_VIEW_CONNECT2, "Disconnect");
}

void CDlgViewPlcManual::OnBnClickedPlcSendReaddata()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int Block_cnt = 0;		//[+:JMW]

	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();

	if (pDlgViewPlc1->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc1->PlcDataBlockInit();

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		if (i / 2 == 0)
			Block_cnt++;

		pDlgViewPlc1->m_StPlcDataBlockParam[0][i].fPlcDataBlock = 50.01 + (i*0.05) + i;
		pDlgViewPlc1->m_StPlcDataBlockParam[1][i].fPlcDataBlock = 60.01 + (i*0.06) + i;
		pDlgViewPlc1->m_StPlcDataBlockParam[2][i].fPlcDataBlock = 70.01 + (i*0.07) + i;
	}

	pDlgViewPlc1->PLC_SendResultData();
}

void CDlgViewPlcManual::OnBnClickedPlcTestStart()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	pDlgViewPlc1->SetPlcProcessStart();
	
}

void CDlgViewPlcManual::OnBnClickedPlcTestStart2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	pDlgViewPlc2->SetPlcProcessStart();
}


void CDlgViewPlcManual::OnBnClickedPlcTestStop()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	pDlgViewPlc1->SetPlcProcessStop();
}

// PLC -> PC PLC Unload Complete Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1ReadData5()
{
	//CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_UNLOAD_COMPLETE].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_LOAD_COMPLETE].lPlcData = BIT_OFF;
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_UNLOAD_COMPLETE].lPlcData = lBitStatus;
}

// PLC -> PC Read Ready Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1ReadData2()
{	
	//CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_READ_READY].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc1->m_StPlcDataParam[TYPE_READ_READY].lPlcData = lBitStatus;
}

// PLC -> PC PLC Alarm Occur Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1ReadData3()
{
	//CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_READ_PLC_ALARM_OCCUR].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc1->m_StPlcDataParam[TYPE_READ_PLC_ALARM_OCCUR].lPlcData = lBitStatus;
}

// PLC -> PC PLC Load Complete Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1ReadData4()
{
	//CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_LOAD_COMPLETE].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	pDlgViewPlc1->m_StPlcDataParam[TYPE_READ_READY].lPlcData = BIT_OFF;
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_LOAD_COMPLETE].lPlcData = lBitStatus;
}

// PLC -> PC PLE EMO Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1ReadData6()
{
	//CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_EMO].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_EMO].lPlcData = lBitStatus;
}

// PC -> PLC Send Process Start Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1SendData2()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CPlcProcessSequence1* pPlcProSequence1 = CPlcProcessSequence1::GetMgr();

	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_PROCESS_START].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc1->m_PlcProcessSequence1->SetAutoStep(pDlgViewPlc1->m_PlcProcessSequence1->enumProcessStepAuto::AUTO_PC_PROCESS_START);
	}

	pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_PROCESS_START].lPlcData = lBitStatus;
}

// PC -> PLC Send Schedule Start Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1SendData3()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CPlcProcessSequence1* pPlcProSequence1 = CPlcProcessSequence1::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_SCHEDULE_START].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc1->PLC_SendProcessData();
		//pPlcProSequence1->SetAutoStep(pPlcProSequence1->enumProcessStepAuto::AUTO_PC_SCHEDULE_START_CHECK);
		pDlgViewPlc1->m_PlcProcessSequence1->SetAutoStep(pDlgViewPlc1->m_PlcProcessSequence1->enumProcessStepAuto::AUTO_PC_SCHEDULE_START_CHECK);
	}

	pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_SCHEDULE_START].lPlcData = lBitStatus;
}

// PC -> PLC Send Schedule End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1SendData4()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CPlcProcessSequence1* pPlcProSequence1 = CPlcProcessSequence1::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc1->PLC_SendProcessData();
		//pPlcProSequence1->SetAutoStep(pPlcProSequence1->enumProcessStepAuto::AUTO_PC_SCHEDULE_COMPLETE_CHECK);
		pDlgViewPlc1->m_PlcProcessSequence1->SetAutoStep(pDlgViewPlc1->m_PlcProcessSequence1->enumProcessStepAuto::AUTO_PC_SCHEDULE_COMPLETE);
	}

	pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData = lBitStatus;
}

// PC -> PLC Send Process End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1SendData5()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	//CPlcProcessSequence1* pPlcProSequence1 = CPlcProcessSequence1::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_PROCESS_END].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc1->PLC_SendProcessData();
		pDlgViewPlc1->m_PlcProcessSequence1->SetAutoStep(pDlgViewPlc1->m_PlcProcessSequence1->enumProcessStepAuto::AUTO_PLC_PROCESS_END_CHECK);
		//pPlcProSequence1->SetAutoStep(pPlcProSequence1->enumProcessStepAuto::AUTO_PLC_PROCESS_END_CHECK);
	}

	pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_PROCESS_END].lPlcData = lBitStatus;
}

// PC -> PLC Send End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc1SendData6()
{
	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_EMO].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
	}

	pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_EMO].lPlcData = lBitStatus;
}


void CDlgViewPlcManual::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	if (nIDCtl == IDC_PLC1_SEND_DATA2) {
		dc.Attach(lpDrawItemStruct->hDC);
		{
			RECT rtBtn = lpDrawItemStruct->rcItem;

			dc.FillSolidRect(&rtBtn, RGB(175, 255, 47));
		}
		dc.Detach();
	}

	__super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// PLC2 -> PC Read Ready Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2ReadData2()
{
	//CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_READY].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_READY].lPlcData = lBitStatus;
}

// PLC2 -> PC PLC Alarm Occur Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2ReadData3()
{
	//CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_PLC_ALARM_OCCUR].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}
	//pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_PLC_ALARM_OCCUR].lPlcData = lBitStatus;
}

// PLC2 -> PC PLC Load Complete Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2ReadData4()
{
	//CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_LOAD_COMPLETE].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_READY].lPlcData = BIT_OFF;
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_LOAD_COMPLETE].lPlcData = lBitStatus;
}

// PLC2 -> PC PLC Unload Complete Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2ReadData5()
{
	//CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_UNLOAD_COMPLETE].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_LOAD_COMPLETE].lPlcData = BIT_OFF;
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_UNLOAD_COMPLETE].lPlcData = lBitStatus;
}

// PLC2 -> PC PLE EMO Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2ReadData6()
{
	//CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CString str;
	//long lBitStatus;

	//lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_EMO].lPlcData;

	//if (lBitStatus == BIT_ON)
	//{
	//	lBitStatus = BIT_OFF;
	//}
	//else {
	//	lBitStatus = BIT_ON;
	//}

	//pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_EMO].lPlcData = lBitStatus;
}


// PC -> PLC2 Send Process Start Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2SendData2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//CPlcProcessSequence1* pPlcProSequence1 = CPlcProcessSequence1::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_START].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc2->m_PlcProcessSequence2->SetAutoStep(pDlgViewPlc2->m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_PC_PROCESS_START);
	}

	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_START].lPlcData = lBitStatus;
	pDlgViewPlc2->PLC_SendProcessData();
}

// PC -> PLC2 Send Schedule Start Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2SendData3()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_START].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc2->m_PlcProcessSequence2->SetAutoStep(pDlgViewPlc2->m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_PC_SCHEDULE_START_CHECK);
	}

	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_START].lPlcData = lBitStatus;
	pDlgViewPlc2->PLC_SendProcessData();
}

// PC -> PLC2 Send Schedule End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2SendData4()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else
	{
		lBitStatus = BIT_ON;
		pDlgViewPlc2->m_PlcProcessSequence2->SetAutoStep(pDlgViewPlc2->m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_PC_SCHEDULE_COMPLETE);
	}
	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData = lBitStatus;
	pDlgViewPlc2->PLC_SendProcessData();
}

// PC -> PLC2 Send Process End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2SendData5()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_END].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
		pDlgViewPlc2->m_PlcProcessSequence2->SetAutoStep(pDlgViewPlc2->m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_PLC_PROCESS_END_CHECK);
	}

	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_END].lPlcData = lBitStatus;
	pDlgViewPlc2->PLC_SendProcessData();
}

// PC -> PLC2 Send End Bit Button
void CDlgViewPlcManual::OnBnClickedPlc2SendData6()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString str;
	long lBitStatus;

	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData;

	if (lBitStatus == BIT_ON)
	{
		lBitStatus = BIT_OFF;
	}
	else {
		lBitStatus = BIT_ON;
	}

	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData = lBitStatus;
}


void CDlgViewPlcManual::OnBnClickedPlc2BitReset()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	//[JMW:220315]
	for (int i = 0; i < TYPE_PLC_MAX; i++)
	{
		if ((i == TYPE_SEND_HEART_BIT) || (i == TYPE_READ_HEART_BIT))
			continue;
		pDlgViewPlc2->m_StPlcDataParam2[i].lPlcData = BIT_OFF;
	}

	pDlgViewPlc2->PLC_SendProcessData();

	//[JMW:220325]
	pDlgViewPlc2->m_PlcProcessSequence2->SetAutoStep(pDlgViewPlc2->m_PlcProcessSequence2->enumProcessStepAuto2::AUTO_INIT);
}


void CDlgViewPlcManual::OnBnClickedPlcTestStop2()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	pDlgViewPlc2->SetPlcProcessStop();
}


void CDlgViewPlcManual::OnBnClickedPlcSendReaddata2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int Block_cnt = 0;		//[+:JMW]

	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();

	if (pDlgViewPlc2->m_bCheckOpened != TRUE)
		return;

	pDlgViewPlc2->PlcDataBlockInit();

	for (int i = 0; i < BLOCK_COUNT; i++)
	{
		if (i / 2 == 0)
			Block_cnt++;

		pDlgViewPlc2->m_StPlcDataBlockParam2[0][i].fPlcDataBlock = 10.01 + (i*0.01) + i;
		pDlgViewPlc2->m_StPlcDataBlockParam2[1][i].fPlcDataBlock = 20.01 + (i*0.01) + i;
		pDlgViewPlc2->m_StPlcDataBlockParam2[2][i].fPlcDataBlock = 30.01 + (i*0.01) + i;
	}

	pDlgViewPlc2->PLC_SendResultData2();
}


void CDlgViewPlcManual::OnBnClickedPlc1SendData7()
{
	// TODO: Add your control notification handler code here
}
