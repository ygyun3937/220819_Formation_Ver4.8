// CDlgViewEqpChannelStatus.cpp: 구현 파일
//

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewEqpChannelStatus.h"
#include "afxdialogex.h"

#include "DlgViewDAQ.h"
#include "MgrChannel.h"
#include "MgrCommData.h"
#include "MgrConfig.h"
#include "MgrOption.h"
#include "MgrLog.h"
#include "MgrDAQ.h"
#include "MgrStatus.h"
#include "MgrPCan.h"
#include "MgrDio.h"
#include "MgrSerial.h"
#include "MgrPlcInterface.h"
#include "CMgrChamber.h"
#include "PnxUtil.h"

#include "MgrComm.h"
#include "CMgrAccount.h"
#include "CMgrModel.h"
#include "CMgrScheduleHistory.h"
#include "CDlgViewSchedulelist.h"
#include "CDlgPauseOption.h"
#include "CDlgCompleteOption.h"
#include "DlgViewAlarm.h"

#include <Windows.h>
#include <iostream>


#include "CDlgViewPlc1.h"
#include "CDlgViewPlc2.h"


#include "SockProc.h"

#define		EQP_CHANNEL_STATUS_UPDATE_TIMER_ID	20001
#define		EQP_DISK_STATUS_UPDATE_TIMER_ID	20002
#define		TIMER_SET_1S						1000

// CDlgViewEqpChannelStatus 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewEqpChannelStatus, CDialogEx)

CDlgViewEqpChannelStatus::CDlgViewEqpChannelStatus(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_EQP_CHANNEL_STATUS, pParent)
{
	m_nSelectedStageNo = 0;
	m_nBarcodeReadingFocus = IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE;


	memset(m_bStageDataSwitching, FALSE, sizeof(m_bStageDataSwitching));
}

CDlgViewEqpChannelStatus::~CDlgViewEqpChannelStatus()
{
}

void CDlgViewEqpChannelStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	for (int i = 0; i < EQ_CH_INDEX; i++)
	{
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + i, m_staticControl[i]);
		DDX_Control(pDX, IDC_BTN_CH1 + i, m_BtnSelectCh[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_TIME1 + i, m_staticStepTime[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_TOTAL1 + i, m_staticTotalTime[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_TEMP1 + i, m_staticTemp[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_AMP1 + i, m_staticAmp[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_VOLT1 + i, m_staticVolt[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CODE1 + i, m_staticChCode[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_AMP_HOUR1 + i, m_staticCapacity[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE1 + i, m_staticChBarcode[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CYCLE_CH1 + i, m_staticCycle[i]);
		DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_DATA_STEP_CH1 + i, m_staticStep[i]);

	}
	for (int i = 0; i < EQ_STAGE_INDEX; i++)
	{
		DDX_Control(pDX, IDC_BTN_VIEW_EQP_STAGE1_DATA_SWITCHING + i, m_btn_DataSwtching[i]);
	}
	for (int i = 0; i < (int)eCyclerChannelStateMaxCount; i++)
	{
		DDX_Control(pDX, IDC_STATIC_EQSTATUSCOLOR1 + i, m_staticSetStatusColor[i]);
	}

	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_GROUP1, m_BtnSelectStageS1);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_GROUP2, m_BtnSelectStageS2);

	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_SELECT_SCHEDULE, m_Btn_SelectSchedule);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_START, m_Btn_Start);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_PAUSE, m_Btn_Pause);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_STAGE1_STATUS, m_Status_Stage1);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_STAGE2_STATUS, m_Status_Stage2);
	DDX_Control(pDX, IDC_STATIC_VIEW_INVERTER_STATUS, m_Status_Inverter);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_COMPLETE, m_btn_Complete_Option);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_SCHEDULENAME, m_Status_Schedule);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_MODELNAME, m_Status_Schedule_Model);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_CHANNEL2, m_Title_Stage1_CH);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE4, m_Title_Stage1_StepTime);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE5, m_Title_Stage1_TotalTime);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE1, m_Title_Stage1_Volt);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_AMP1, m_Title_Stage1_Current);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_AMPHOUR1, m_Title_Stage1_Cap);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_OPERCODE1, m_Title_Stage1_Code);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_TEMP1, m_Title_Stage1_Temp);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_CHANNEL4, m_Title_Stage2_CH);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE6, m_Title_Stage2_StepTime);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE7, m_Title_Stage2_TotalTime);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VOLTAGE2, m_Title_Stage2_Volt);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_AMP2, m_Title_Stage2_Current);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_AMPHOUR2, m_Title_Stage2_Cap);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_OPERCODE2, m_Title_Stage2_Code);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_TEMP2, m_Title_Stage2_Temp);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_DISK_STATUS, m_Status_Disk);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE, m_staticTrayBarcode);
	DDX_Control(pDX, IDC_STATIC_VIEW_LINE1, m_Line_1);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_START, m_Status_Start_Message);
	DDX_Control(pDX, IDC_STATIC_VIEW_LINE2, m_Line_2);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_COMPLETE, m_btn_Stage2_Complete_Option);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_START, m_Status_Stage2_Start_Message);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_START, m_Btn_Stage2_Start);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_SELECT_STAGE2_SCHEDULE, m_Btn_Stage2_SelectSchedule);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_GROUP, m_BtnSelectStageS2);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_MODELNAME2, m_Status_Schedule_Stage2_Model);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_SCHEDULENAME, m_Status_Stage2_Schedule);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE, m_staticTrayBarcode_Stage2);
	DDX_Control(pDX, IDC_STATIC_VIEW_INVERTER_STATUS2, m_InverterStatus_2);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_STAGE1_MODE, m_Btn_Stage1_Mode);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_STAGE1_INIT, m_Btn_Stage1_Init);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_STAGE2_INIT, m_Btn_Stage2_Init);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_VACCUMVALUE, m_staticVacValue);
	DDX_Control(pDX, IDC_BTN_VIEW_EQP_STAGE1_MODE2, m_Btn_Stage2_Mode);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_DISK_STATUS2, m_Status_Disk2);
	DDX_Control(pDX, IDC_STATIC_VIEW_IOSTATUS, m_Data_IO_Status);
	DDX_Control(pDX, IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_PUMPOPENPERCENT_DATA, m_Data_PumpOpenPercent);
}

BEGIN_MESSAGE_MAP(CDlgViewEqpChannelStatus, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(IDC_BTN_CH1, IDC_BTN_CH72, OnClickTitleButton)
	ON_COMMAND_RANGE(IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE1, IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE48,OnClickChBarcode)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_SELECT_SCHEDULE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1SelectSchedule)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_START, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1Start)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_GROUP1, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1Group1)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_PAUSE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusPause)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_COMPLETE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusComplete)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_GROUP, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Group)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_SELECT_STAGE2_SCHEDULE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusSelectStage2Schedule)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_START, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Start)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_CHANNEL_STATUS_STAGE2_COMPLETE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Complete)
	ON_STN_CLICKED(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE, &CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelStatusTraybarcode)
	ON_STN_CLICKED(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE, &CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelStatusStage2Traybarcode)
	ON_STN_CLICKED(IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE1, &CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelChBarcode1)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE1_MODE, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Mode)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE2_INIT, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage2Init)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE1_INIT, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Init)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE1_MODE2, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Mode2)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE1_DATA_SWITCHING, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1DataSwitching)
	ON_BN_CLICKED(IDC_BTN_VIEW_EQP_STAGE1_DATA_SWITCHING2, &CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1DataSwitching2)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

// CDlgViewEqpChannelStatus 메시지 처리기

void CDlgViewEqpChannelStatus::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgViewEqpChannelStatus::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgViewEqpChannelStatus::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CDlgViewEqpChannelStatus::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	pDC->FillSolidRect(rect, ColorWhite);

	return __super::OnEraseBkgnd(pDC);
}

HBRUSH CDlgViewEqpChannelStatus::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	UINT nID = pWnd->GetDlgCtrlID();

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (CTLCOLOR_DLG == nCtlColor)
	{
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CDlgViewEqpChannelStatus::OnTimer(UINT_PTR nIDEvent)
{
	if (EQP_CHANNEL_STATUS_UPDATE_TIMER_ID == nIDEvent)
	{
		UpdataChStatus();
		UpdateEqpChannelStatus();

		if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
		{
			if (CMgrConfig::GetMgr()->m_strPLCControlType == "Mitsubishi")
			{
				//PLCUse수정필요
				//hsg Read Data
				SetPlcDataParam();
			}
		}
	}
	if (EQP_DISK_STATUS_UPDATE_TIMER_ID == nIDEvent)
	{
		Display_Manage();
	}

	__super::OnTimer(nIDEvent);
}

void CDlgViewEqpChannelStatus::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnClose();
}

void CDlgViewEqpChannelStatus::OnDestroy()
{
	KillTimer(EQP_CHANNEL_STATUS_UPDATE_TIMER_ID);
	KillTimer(EQP_DISK_STATUS_UPDATE_TIMER_ID);

	__super::OnDestroy();
}

// -------------------------------------------------------------------------------------------------

BOOL CDlgViewEqpChannelStatus::Create(CWnd * pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewEqpChannelStatus::OnInitDialog()
{
	__super::OnInitDialog();

	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewEqpChannelStatus::Initialize()
{
	m_nScheduleLoadingType = eScheduleLoadingTypeAdd;

	LoadDrawINI();

	InitializeControls();

	InitializeTimers();

	InitializeChannelInfo();
}

void CDlgViewEqpChannelStatus::InitializeControls()
{
	COLORREF crTempStatusColor;
	CString eqpName = CMgrConfig::GetMgr()->GetSubEqpInfo().strSubEqpId;
	int nChMaxCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageMaxCount;
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	
	if (bSubStage1Use == true)
	{
		for (int nCh = 0; nCh < nStageChCount; nCh++)
		{

			m_staticChBarcode[nCh].SetBkColor(ColorWhite, ColorWhite);
			m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
			m_staticChBarcode[nCh].SetTextColor(COLOR_BLACK);
		}
		UI_Update_Switching(0);

	}
	if (bSubStage2Use == true)
	{
		for (int nCh = nStageChCount; nCh < nStageChCount+ nStageChCount; nCh++)
		{

			m_staticChBarcode[nCh].SetBkColor(ColorWhite, ColorWhite);
			m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
			m_staticChBarcode[nCh].SetTextColor(COLOR_BLACK);
		}
		UI_Update_Switching(1);

	}
	m_Data_IO_Status.SetBkColor(ColorWhite, ColorWhite);
	m_Data_IO_Status.SetFont("Verdana", 14, 1400);
	m_Data_IO_Status.SetTextColor(COLOR_BLACK);

	m_staticTrayBarcode.SetBkColor(ColorWhite, ColorWhite);
	m_staticTrayBarcode.SetFont("Verdana", 14, 1400);
	m_staticTrayBarcode.SetTextColor(COLOR_BLACK);

	m_staticTrayBarcode_Stage2.SetBkColor(ColorWhite, ColorWhite);
	m_staticTrayBarcode_Stage2.SetFont("Verdana", 14, 1400);
	m_staticTrayBarcode_Stage2.SetTextColor(COLOR_BLACK);

	GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_EQPNAME)->SetWindowText(eqpName);

	Set_Stage_UI_Visible();

	for (int i = 0; i < (int)eCyclerChannelStateMaxCount; i++)
	{
		crTempStatusColor = GetDisplayChannelStatusColor(i);

		m_staticSetStatusColor[i].SetBkColor(crTempStatusColor, crTempStatusColor);
		m_staticSetStatusColor[i].SetFont("Verdana", 11, 1200);
		m_staticSetStatusColor[i].SetTextColor(COLOR_BLACK);
	}

	


	// ----------------------------------------------------------------------------

	m_Status_Inverter.SetBkColor(ColorWhite, ColorWhite);
	m_Status_Inverter.SetFont("Verdana", 14, 1400);
	m_Status_Inverter.SetTextColor(COLOR_BLACK);

	m_InverterStatus_2.SetBkColor(ColorWhite, ColorWhite);
	m_InverterStatus_2.SetFont("Verdana", 14, 1400);
	m_InverterStatus_2.SetTextColor(COLOR_BLACK);

	m_Line_1.SetBkColor(ColorLightGray, ColorLightGray);
	m_Line_2.SetBkColor(ColorLightGray, ColorLightGray);


	m_Status_Stage1.SetFont("Verdana", 18, 1800);
	m_Status_Stage1.SetTextColor(COLOR_BLACK);	
	
	

	m_Status_Stage2.SetFont("Verdana", 18, 1800);
	m_Status_Stage2.SetTextColor(COLOR_BLACK);

	m_BtnSelectStageS1.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_FUNC_BTN);
	m_BtnSelectStageS1.SetFont("Verdana", 18, 1800);
	m_BtnSelectStageS1.SetTextColor(COLOR_BLACK);

	m_BtnSelectStageS2.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_FUNC_BTN);
	m_BtnSelectStageS2.SetFont("Verdana", 16, 1400);
	m_BtnSelectStageS2.SetTextColor(COLOR_BLACK);

	m_Btn_Stage1_Mode.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_FUNC_BTN);
	m_Btn_Stage1_Mode.SetFont("Verdana", 16, 1400);
	m_Btn_Stage1_Mode.SetTextColor(COLOR_BLACK);
	
	m_Btn_Stage2_Mode.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_FUNC_BTN);
	m_Btn_Stage2_Mode.SetFont("Verdana", 16, 1400);
	m_Btn_Stage2_Mode.SetTextColor(COLOR_BLACK);

	m_Btn_SelectSchedule.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_SelectSchedule.SetFont("Verdana", 18, 1800);
	m_Btn_SelectSchedule.SetTextColor(COLOR_BLACK);

	m_Btn_Stage2_SelectSchedule.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Stage2_SelectSchedule.SetFont("Verdana", 18, 1800);
	m_Btn_Stage2_SelectSchedule.SetTextColor(COLOR_BLACK);

	m_btn_Complete_Option.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_btn_Complete_Option.SetFont("Verdana", 18, 1800);
	m_btn_Complete_Option.SetTextColor(COLOR_BLACK);

	m_btn_Stage2_Complete_Option.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_btn_Stage2_Complete_Option.SetFont("Verdana", 18, 1800);
	m_btn_Stage2_Complete_Option.SetTextColor(COLOR_BLACK);

	m_Btn_Start.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Start.SetFont("Verdana", 18, 1800);
	m_Btn_Start.SetTextColor(COLOR_BLACK);

	m_Btn_Stage2_Start.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Stage2_Start.SetFont("Verdana", 18, 1800);
	m_Btn_Stage2_Start.SetTextColor(COLOR_BLACK);

	m_Btn_Stage1_Init.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Stage1_Init.SetFont("Verdana", 18, 1800);
	m_Btn_Stage1_Init.SetTextColor(COLOR_BLACK);

	m_Btn_Stage2_Init.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Stage2_Init.SetFont("Verdana", 18, 1800);
	m_Btn_Stage2_Init.SetTextColor(COLOR_BLACK);


	m_Btn_Pause.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_Pause.SetFont("Verdana", 18, 1800);
	m_Btn_Pause.SetTextColor(COLOR_BLACK);


	for (int i = 0; i < EQ_STAGE_INDEX; i++)
	{
		m_btn_DataSwtching[i].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
		m_btn_DataSwtching[i].SetFont("Verdana", 14, 1400);
		m_btn_DataSwtching[i].SetTextColor(COLOR_BLACK);
	}

}

void CDlgViewEqpChannelStatus::InitializeTimers()
{
	SetTimer(EQP_CHANNEL_STATUS_UPDATE_TIMER_ID, TIMER_SET_1S / 5, NULL);
	SetTimer(EQP_DISK_STATUS_UPDATE_TIMER_ID, TIMER_SET_1S, NULL);
}

bool CDlgViewEqpChannelStatus::LoadDrawINI()
{
	CStringArray strAry;
	CString strFilePath; strFilePath.Empty();
	CString strAppName; strAppName.Empty();

	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), _T("Draw.ini"));
	strAppName.Format(_T("ViewChannel_FormationEtc"));


	CString tempValue = INIReadStr(strAppName, _T("GridTitle"), strFilePath);
	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitle"), strFilePath)), ',', m_strArrTitle);

	return TRUE;
}

bool CDlgViewEqpChannelStatus::InitializeChannelInfo()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	int nChannelCnt = listChannel.GetCount();
	POSITION Pos = listChannel.GetHeadPosition();

	int nRow = 1;
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		CStringArray arr;
		CString tempItemName;
		CString tempItemValue;

		pChannel->ChannelFormation2Array(arr, this->m_strArrTitle);

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			tempItemName = m_strArrTitle[i];
			tempItemValue = arr.GetAt(i);

			int tempChannelIndexNo = nRow - 1;

			if (tempItemName.CompareNoCase(_T("Ch")) == 0)
			{
				SetDisplayChannelNo(tempChannelIndexNo, tempItemValue);
			}
			else if (tempItemName.CompareNoCase(_T("Voltage")) == 0)
			{
				SetDisplayChannelVoltage(tempChannelIndexNo, tempItemValue);
			}
			else if (tempItemName.CompareNoCase(_T("Current")) == 0)
			{
				SetDisplayChannelAmpare(tempChannelIndexNo, tempItemValue);
			}
			else if (tempItemName.CompareNoCase(_T("TotalAh")) == 0)
			{
				SetDisplayChannelAmpareHour(tempChannelIndexNo, tempItemValue);
			}
			else if (tempItemName.CompareNoCase(_T("OpCode")) == 0)
			{
				SetDisplayChannelCode(tempChannelIndexNo, tempItemValue);
			}
			else if (tempItemName.CompareNoCase(_T("Temperature")) == 0)
			{
				SetDisplayChannelTemperature(tempChannelIndexNo, tempItemValue);
			}
			//Step Time
			else if (m_strArrTitle[i].CompareNoCase(_T("StepTime")) == 0)
			{

				SetDisplayChannelStepTime(tempChannelIndexNo, tempItemValue);

			}
			//Total Time
			else if (m_strArrTitle[i].CompareNoCase(_T("TotalTime")) == 0)
			{
				SetDisplayChannelTotalTime(tempChannelIndexNo, tempItemValue);
			}
		}

		nRow++;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------

bool CDlgViewEqpChannelStatus::SetDisplayChannelNo(int nEqpChannelIndexNo, CString sValue)
{
	GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + nEqpChannelIndexNo)->SetWindowText(_T(sValue));
	GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + nEqpChannelIndexNo)->Invalidate();


	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelVoltage(int nEqpChannelIndexNo, CString sValue)
{
	m_staticVolt[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticVolt[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelAmpare(int nEqpChannelIndexNo, CString sValue)
{
	m_staticAmp[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticAmp[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelAmpareHour(int nEqpChannelIndexNo, CString sValue)
{
	m_staticCapacity[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticCapacity[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelCode(int nEqpChannelIndexNo, CString sValue)
{
	int nChannelOpCode = CMgrChannel::GetMgr()->GetChannel(nEqpChannelIndexNo)->GetOperationMode();

	COLORREF crTempOpCodeColor1 = GetDisplayChannelOpCodeColor(nChannelOpCode);
	if (_tcsicmp(m_strChOpCode[nEqpChannelIndexNo], sValue) != 0)
	{
		m_staticChCode[nEqpChannelIndexNo].SetBkColor(crTempOpCodeColor1, crTempOpCodeColor1);
		m_staticChCode[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
		m_strChOpCode[nEqpChannelIndexNo] = sValue;
		m_staticChCode[nEqpChannelIndexNo].Invalidate(false);
	}
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelTemperature(int nEqpChannelIndexNo, CString sValue)
{
	m_staticTemp[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticTemp[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelStepTime(int nEqpChannelIndexNo, CString sValue)
{
	m_staticStepTime[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticStepTime[nEqpChannelIndexNo].Invalidate(false);

	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelTotalTime(int nEqpChannelIndexNo, CString sValue)
{
	m_staticTotalTime[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticTotalTime[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelCycleNumber(int nEqpChannelIndexNo, CString sValue)
{
	m_staticCycle[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticCycle[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

bool CDlgViewEqpChannelStatus::SetDisplayChannelStepNumber(int nEqpChannelIndexNo, CString sValue)
{
	m_staticStep[nEqpChannelIndexNo].SetWindowTextA(_T(sValue));
	m_staticStep[nEqpChannelIndexNo].Invalidate(false);
	return true;
}

void CDlgViewEqpChannelStatus::SetInverterStatus(int nStation)
{
	CMgrPCan* pMgr = CMgrPCan::GetMgr();
	int nInverterStatus = 0;

	nInverterStatus = CMgrPCan::GetMgr()->GetStation(nStation)->GetInverterStatus();

	if (nStation == 0)
	{

		m_Status_Inverter.SetWindowTextA(_T(g_lpszInverterStatusString[nInverterStatus]));
		m_Status_Inverter.Invalidate(false);
	}
	else
	{
		m_InverterStatus_2.SetWindowTextA(_T(g_lpszInverterStatusString[nInverterStatus]));
		m_InverterStatus_2.Invalidate(false);
	}

}

// -------------------------------------------------------------------------------------------------

bool CDlgViewEqpChannelStatus::SetDisplayEqpId(CString eqpId)
{
	m_strSubEqpId = eqpId;
	GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_EQPNAME)->SetWindowText(_T(eqpId));
	GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_EQPNAME)->Invalidate();

	return true;
}

/**
 * .
 *
 * METHOD_NAME : SetDisplayChannelStatus
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
//220113 YGY 매개변수 Ch No 제거
COLORREF CDlgViewEqpChannelStatus::GetDisplayChannelStatusColor(int nStatusValue)
{
	COLORREF crStatusColor;

	eCyclerChannelState statusValue = (eCyclerChannelState)nStatusValue;
	switch (statusValue)
	{
	case eCyclerChannelStateUnknown:
		crStatusColor = ColorWhite;
		break;
	case eCyclerChannelStateIdle:
		crStatusColor = COLOR_CYAN_128;
		break;
	case eCyclerChannelStateReady:
		crStatusColor = ColorLightGray;
		break;
	case eCyclerChannelStateStart:
		crStatusColor = ColorLightGreen;
		break;
	case eCyclerChannelStatePause:
		crStatusColor = COLOR_WHITE_RED;
		break;
	case eCyclerChannelStatePauseFromDwell:
		crStatusColor = COLOR_WAIT_BTN;
		break;
	case eCyclerChannelStatePauseFromWaitChamber:
		crStatusColor = ColorSlateblue;
		break;
	case eCyclerChannelStatePauseFromTempSync:
		crStatusColor = COLOR_TOPBAR;
		break;
	case eCyclerChannelStatePauseFromStepSync:
		crStatusColor = ColorPurple;
		break;
	case eCyclerChannelStateComplete:
		crStatusColor = COLOR_MAGENTA_128;
		break;
	case eCyclerChannelStateError:
		crStatusColor = ColorLightRed;
		break;
	case eCyclerChannelStateCommuicationError:
		crStatusColor = ColorLightRed;
		break;
	default:
		break;
	}

	return crStatusColor;
}

COLORREF CDlgViewEqpChannelStatus::GetDisplayChannelOpCodeColor(int nOpCodeValue)
{
	COLORREF crOpCodeColor;

	eIFBoardOpCode statusValue = (eIFBoardOpCode)nOpCodeValue;
	switch (statusValue)
	{
	case eIFBoardOpCodeUnknown:
		crOpCodeColor = ColorWhite;
		break;
	case eIFBoardOpCodeInputRelayOn:
		crOpCodeColor = ColorLightGray;
		break;
	case eIFBoardOpCodeOutputReplyOn:
		crOpCodeColor = ColorBlue;
		break;
	case eIFBoardOpCodeChargeCC:
		crOpCodeColor = ColorLightGreen;
		break;
	case eIFBoardOpCodeChargeCV:
		crOpCodeColor = ColorGreen;
		break;
	case eIFBoardOpCodeDischargeCC:
		crOpCodeColor = ColorYellow;
		break;
	case eIFBoardOpCodeDischargeCV:
		crOpCodeColor = ColorWhiteYellow;
		break;
	case eIFBoardOpCodeChargeCP:
		crOpCodeColor = COLOR_CYAN;
		break;
	case eIFBoardOpCodeDischargeCP:
		crOpCodeColor = COLOR_CYAN_RIGHT;
		break;
	default:
		break;
	}

	return crOpCodeColor;
}

bool CDlgViewEqpChannelStatus::SetStaticControlColor(UINT nID, COLORREF crColor)
{
	return true;
}

// -------------------------------------------------------------------------------------------------

/**
 * .
 *
 * METHOD_NAME : UpdateEqpChannelStatus
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
bool CDlgViewEqpChannelStatus::UpdateEqpChannelStatus()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	CString strValue;

	int nChannelNo = 1;
	int nState = 0;
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel)
			continue;

 		if (pChannel->GetEqMode() == EqModeSerial)
		{
			CStringArray arr;
			//m_strArrTitle : 데이터 파싱 기준 문자 배열
			pChannel->ChannelFormation2Array(arr, this->m_strArrTitle);
			nChannelNo = pChannel->GetChannelNumber() + 1;
			nState = pChannel->GetChannelStatus();
			for (int i = 0; i < arr.GetSize(); ++i)
			{			

				if (nState == eCyclerChannelStateError)
				{				
					// TODO 확인필요
					//if (m_Grid.GetItemText(0, i) != "StepTime" && m_Grid.GetItemText(0, i) != "TitalTime")  // step time과  total time
					//	m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i));

					//if (m_Grid.GetItemText(0, i) == "Status")
					//	m_Grid.SetItemBkColour(nChannelNo, i, Status2Color(_T("Error")));			
				}
				else
				{
					int nEqpChannelIndexNo = pChannel->GetChannelNumber();
									   

					if (m_strArrTitle[i].CompareNoCase(_T("Ch")) == 0)
					{
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("Voltage")) == 0)
					{
						SetDisplayChannelVoltage(nEqpChannelIndexNo, arr.GetAt(i));
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("Current")) == 0)
					{					
						SetDisplayChannelAmpare(nEqpChannelIndexNo, arr.GetAt(i));
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("TotalAh")) == 0)
					{
						SetDisplayChannelAmpareHour(nEqpChannelIndexNo, arr.GetAt(i));
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("OpCode")) == 0)
					{
						SetDisplayChannelCode(nEqpChannelIndexNo, arr.GetAt(i));
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("Temperature")) == 0)
					{
						SetDisplayChannelTemperature(nEqpChannelIndexNo, arr.GetAt(i));
					}
					//Step Time
					else if (m_strArrTitle[i].CompareNoCase(_T("StepTime")) == 0)
					{
						SetDisplayChannelStepTime(nEqpChannelIndexNo, arr.GetAt(i));
					}
					//Total Time
					else if (m_strArrTitle[i].CompareNoCase(_T("TotalTime")) == 0)
					{
						SetDisplayChannelTotalTime(nEqpChannelIndexNo, arr.GetAt(i));
					}
					//CycleNumber
					else if (m_strArrTitle[i].CompareNoCase(_T("CycleNumber")) == 0)
					{
						SetDisplayChannelCycleNumber(nEqpChannelIndexNo, arr.GetAt(i));
					}
					//CycleNumber
					else if (m_strArrTitle[i].CompareNoCase(_T("StepNumber")) == 0)
					{
						//m_staticTotalTime[nEqpChannelIndexNo].GetWindowTextA(_T(strValue));
						//if (strValue != arr.GetAt(i))
						//{
						SetDisplayChannelStepNumber(nEqpChannelIndexNo, arr.GetAt(i));
						//}
					}
					else
					{
						//m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i))
					}
				}
			}
		}
		else if (pChannel->GetEqMode() == EQModeParallel)
		{

		}
	}

	return true;
}

void CDlgViewEqpChannelStatus::Init_Stage(int nStageNo)
{
	//Barcode 초기화
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	POSITION Pos = listChannel.GetHeadPosition();

	CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();
	pMgrSerial->GetAtBarcode(nStageNo)->SetTrayBarcode("");
	CMgrModel* pMgrModel = CMgrModel::GetMgr();
	//Automode or Manaul Mode 구분 필요
	CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetChannelNumber() / CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount != nStageNo)
			continue;

		pChannel->SetChBarcode("");
	}

	if (nStageNo == 0)
	{
		//상태 초기화
		COLORREF crTempStatusColor1 = GetDisplayChannelStatusColor(eCyclerChannelStateIdle);

		if (_tcsicmp(m_strStage1, g_lpszCyclerStateString[eCyclerChannelStateIdle]) != 0)
		{
			m_Status_Stage1.SetBkColor(crTempStatusColor1, crTempStatusColor1);
			m_Status_Stage1.SetWindowText(g_lpszCyclerStateString[eCyclerChannelStateIdle]);
			m_Status_Stage1.Invalidate(false);
			m_strStage1 = g_lpszCyclerStateString[eCyclerChannelStateIdle];
		}
	}
	if (nStageNo == 1)
	{
		//상태 초기화
		COLORREF crTempStatusColor2 = GetDisplayChannelStatusColor(eCyclerChannelStateIdle);

		if (_tcsicmp(m_strStage2, g_lpszCyclerStateString[eCyclerChannelStateIdle]) != 0)
		{
			m_Status_Stage2.SetBkColor(crTempStatusColor2, crTempStatusColor2);
			m_Status_Stage2.SetWindowText(g_lpszCyclerStateString[eCyclerChannelStateIdle]);
			m_Status_Stage2.Invalidate(false);
			m_strStage2 = g_lpszCyclerStateString[eCyclerChannelStateIdle];
		}
	}	
	//스케줄 초기화
	pMgrModel->SetModelIndex(nStageNo, -1);
	pMgrModel->SetScheduleIndex(nStageNo, -1);

	if (pMgrStatus->GetEQp_Finish(nStageNo) == true)
	{
		pMgrStatus->SetEQp_Finish(nStageNo, false);
	}

	UI_Clear(nStageNo);

}

// -------------------------------------------------------------------------------------------------

void CDlgViewEqpChannelStatus::SelectSchedule()
{
	SYSLOG(Operation, _T("The operator has chosen to start the schedule"));

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	if (true == IsSystemError())
	{
		AfxMessageBox("Unable to proceed with system safety condition not set");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		//AfxMessageBox("선택된 채널 정보가 없습니다.");
		//return;

		AfxMessageBox("No channel is selected in that list.\n[ TARGET CHANNEL -> ALL CHANNEL ]");
		SetSelectEQChAll(m_nSelectedStageNo);
	}

	if (false == pMgr->IsCyclerChannelStartEnable())
		return;

	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	SelectMasterSlave();

	if (lpDlg)
	{
		lpDlg->SetEqpProcessMode("");
		lpDlg->SetScheduleLoadingType(eScheduleLoadingTypeAdd);

		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void CDlgViewEqpChannelStatus::SelectSchedule(int nStageNo)
{
	SYSLOG(Operation, _T("The operator has chosen to start the schedule ( Stage %d )",nStageNo));

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	if (true == IsSystemError())
	{
		AfxMessageBox("Unable to proceed with system safety condition not set");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected(nStageNo))
	{
		AfxMessageBox("No channel is selected in that list.\n[ TARGET CHANNEL -> ALL CHANNEL ]");
		SetSelectEQChAll(nStageNo);
	}

	////Tray Barcode Check

	//CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();

	//if (pMgrSerial->GetAtBarcode(nStageNo)->GetTrayBarcode() == "")
	//{
	//	AfxMessageBox("Tray Barcode Check Please.");
	//	return;
	//}

	//if (false == pMgr->IsChannelBarcodeCheck(nStageNo))
	//{
	//	AfxMessageBox("Channel Barcode Check Please.");
	//	return;
	//}

	//if (false == pMgr->IsCyclerChannelStartEnable(nStageNo))
	//	return;


	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	SelectMasterSlave();

	if (lpDlg)
	{
		lpDlg->SetEqpProcessMode("");
		lpDlg->SetStageNo(nStageNo);
		lpDlg->SetScheduleLoadingType(eScheduleLoadingTypeAdd);

		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void CDlgViewEqpChannelStatus::SetSelectChannelAll()
{
	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	int nTotalChannelCount = CMgrChannel::GetMgr()->GetChannelList().GetCount();

	//체널 No는 0 부터 시작. 
	for (int nChannelCount = 0; nChannelCount < nTotalChannelCount; nChannelCount++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelCount);
		pChannel->SetSelected(true);
	}
}

void CDlgViewEqpChannelStatus::SelectMasterSlave()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount();		// 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard();	// 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (pChannel->GetSelected() == true)
			{
				int nChannelStartNumber = nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel); //현재 선택 된 체널에 첫번째 체널 넘버

				//체널 No는 0 부터 시작. 
				for (int i = nChannelStartNumber; i < (nChannelStartNumber + nChannelCountInParallel); i++)
				{
					CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(i);

					pChannel_1->SetSelected(true);
				}
			}
		}

		Sleep(1);
	}
}

void CDlgViewEqpChannelStatus::SelectCalibration()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	CString strChannelName ="";
	CString strSubStageName = "";
	int nChannelCountInParallel = pMgr->GetParallelChannelCount();		// 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard();	// 한개의 인터페이스 보드당 체널 갯수

	if (pMgr->GetMeasureModeNo() != elFBoradStepModeMasterAutocalCurrent)
		return;

	//in the case Current Auto Calibration
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		int nMasterChannel;
		int nSlaveChannel;		

		if (pChannel->GetSelected() == true)
		{			
			if (pChannel->GetStepMode() == elFBoradStepModeSlaveAutocalCurrent)
				continue;

			//짝수
			if (pChannel->GetChannelNumber() % nChannelCountInParallel == 0)
			{
				nMasterChannel = pChannel->GetChannelNumber();
				nSlaveChannel = pChannel->GetChannelNumber() + 1;
			}
			//홀수
			else
			{
				nMasterChannel = pChannel->GetChannelNumber();
				nSlaveChannel = pChannel->GetChannelNumber() - 1;
			}

			CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(nSlaveChannel);

			pChannel->SetStepMode(elFBoradStepModeMasterAutocalCurrent);

			m_BtnSelectCh[pChannel->GetChannelNumber()].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_PUSH_BTN);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetTextColor(ColorWhite);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetWindowText("MASTER");
			m_BtnSelectCh[pChannel->GetChannelNumber()].Invalidate(false);


			pChannel_1->SetSelected(true);
			pChannel_1->SetStepMode(elFBoradStepModeSlaveAutocalCurrent);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_PUSH_BTN);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetTextColor(ColorWhite);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetWindowText("SLAVE");	
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].Invalidate(false);
		}
		else
		{
			//짝수
			if (pChannel->GetChannelNumber() % nChannelCountInParallel == 0)
			{
				nMasterChannel = pChannel->GetChannelNumber();
				nSlaveChannel = pChannel->GetChannelNumber() + 1;
			}
			//홀수
			else
			{
				nMasterChannel = pChannel->GetChannelNumber();
				nSlaveChannel = pChannel->GetChannelNumber() - 1;
			}
			if (pChannel->GetChannelNumber() / pMgr->GetSubEqpInfo().nSubEqpStageChannelCount == 0)
			{
				strSubStageName = pMgr->GetSubEqpInfo().strStage1Name;
			}
			else
			{
				strSubStageName = pMgr->GetSubEqpInfo().strStage2Name;
			}
			pChannel->SetStepMode(eIFBoardStepModeDefault);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetTextColor(COLOR_BLACK);	
			strChannelName.Format("%s-%d", strSubStageName, pChannel->GetChannelNumber() + 1);
			m_BtnSelectCh[pChannel->GetChannelNumber()].SetWindowText(strChannelName);
			m_BtnSelectCh[pChannel->GetChannelNumber()].Invalidate(false);

			CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(nSlaveChannel);

			pChannel_1->SetStepMode(eIFBoardStepModeDefault);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetTextColor(COLOR_BLACK);
			strChannelName.Format("%s-%d", strSubStageName, pChannel_1->GetChannelNumber() + 1);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].SetWindowText(strChannelName);
			m_BtnSelectCh[pChannel_1->GetChannelNumber()].Invalidate(false);

		}

		Sleep(1);
	}

}

bool CDlgViewEqpChannelStatus::IsSystemError()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (!pMgr)
	{
		return true;
	}

	return pMgr->IsErrorSystemSafety();
}

void CDlgViewEqpChannelStatus::SetSelectEQChAll(int nEqpStageIndexNo)
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	int nSubEqpStageChannelCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;

	int nStart_Ch = 0;
	int nEnd_Ch = 0;

	nStart_Ch = nEqpStageIndexNo * nSubEqpStageChannelCount + 0;
	nEnd_Ch = nEqpStageIndexNo * nSubEqpStageChannelCount + nSubEqpStageChannelCount;

	for (int nCh = nStart_Ch; nCh < nEnd_Ch; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		//if (pChannel->GetSelected() == true)
		//{
		//	pChannel->SetSelected(false);
		//	m_BtnSelectCh[nCh].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
		//	//m_BtnSelectCh[nCh].SetFont("Verdana", 10, 1000);
		//	m_BtnSelectCh[nCh].SetTextColor(COLOR_BLACK);
		//}
		//else
		//{
			if (pChannel->GetChannelStatus() == eCyclerChannelStateReady)
			{
				pChannel->SetSelected(true);
				m_BtnSelectCh[nCh].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_PUSH_BTN);
				m_BtnSelectCh[nCh].SetTextColor(ColorWhite);
			}
			else if (pChannel->GetChannelStatus() < eCyclerChannelStateReady)
			{
				pChannel->SetSelected(false);
				m_BtnSelectCh[nCh].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
				m_BtnSelectCh[nCh].SetTextColor(COLOR_BLACK);
			}
		//}
	}
	Invalidate(FALSE);
}

// -------------------------------------------------------------------------------------------------

void CDlgViewEqpChannelStatus::ExecProcScheduleStart()
{
	SYSLOG(Operation, _T("Execute the worker schedule start command."));

#ifndef _DEBUG
	if (false == IsEnableScheduleStart())
	{
		AfxMessageBox("It cannot start because the charger/discharger is not connected.");

		return;
	}
#endif

	// 없어서는 안되는 항목들 체크 하고 넘어가자

	int nStartCycle = GetStartCycle();		// FIX=1
	int nStartStep = GetStartStep();		// FIX=1

	if (nStartCycle <= 0 || nStartStep <= 0)
	{
		AfxMessageBox("It cannot be started because the start position of the recipe is not specified.");
		return;
	}

	// 여기서 동기화 선택 여부에 따라서 첫번째 예외처리를 한다.
	int nSyncMode = GetSyncMode();

	if (eScheduleSyncModeStep == nSyncMode)
	{
		if (false == IsStepSyncValidate())
		{
			AfxMessageBox("During step sync, you can proceed with two or more channels selected.");
			return;
		}
	}
	else if (eScheduleSyncModeTemperature == nSyncMode)
	{
		if (false == IsTemperatureSyncValidate())
		{
			AfxMessageBox("During Temp sync, it is possible to proceed with the chamber temperature change history.");
			return;
		}
	}

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (pMgrAccount)
	{
		sAccount* lpAccount = pMgrAccount->GetLoginedAccount();

		if (lpAccount)
		{
			CString strFilePath;

			//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_FILE_PATH)->GetWindowText(strFilePath);
			strFilePath = pMgrConfig->GetLogFilePath();		// FORMATION 

			_tcscpy_s(lpAccount->szSaveFilePath, strFilePath);

			pMgrAccount->SaveAccount();
		}
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		SaveHistory();

		SStartSchedule kParser;

		kParser.nStartCycleNumber = nStartCycle;
		kParser.nStartStepNumber = nStartStep;
		kParser.nSyncMode = nSyncMode;
		kParser.strEQPID = GetEqpId();
		kParser.strScheduleFilePath = GetScheduleFilePath();
		kParser.strScheduleName = GetScheduleName();
		kParser.strUserID = GetLoginUserName();
		kParser.strWorkName = GetWorkName();
		kParser.strLogFilePath = GetLogFilePath();
		kParser.bScheduleName = IsDlgButtonChecked(IDC_CHECK_SCHNAME);
		kParser.bTime = IsDlgButtonChecked(IDC_CHECK_TIME);

		GetCellID(kParser.vecCellID);

		GetCyclerChannelNumber(kParser.vecCyclerChannelNumber);

		//중복 불가하게 수정해야함 pnx_hr 2021.04.29
		if (eScheduleLoadingTypeAdd == m_nScheduleLoadingType)
		{
			
			pMgr->StartSchedule(kParser);		// START PROCESS
		}
		else
			pMgr->ScheduleAppend(kParser);
		TRACE("m_nScehduleLoadingType = %d", m_nScheduleLoadingType);

		CWnd* pWnd = AfxGetApp()->GetMainWnd();
		if (pWnd)
		{
			auto iterE = kParser.vecCyclerChannelNumber.end();
			iterE--;
			::PostMessage(pWnd->GetSafeHwnd(), WM_VIEW_PATTERN, 0, *iterE);
		}

		//ShowWindow(SW_HIDE);
	}
}

void CDlgViewEqpChannelStatus::ExecProcScheduleStart(int nStageNo)
{
	SYSLOG(Operation, _T("Execute the worker schedule start command."));

#ifndef _DEBUG
	if (false == IsEnableScheduleStart())
	{
		AfxMessageBox("It cannot start because the charger/discharger is not connected.");

		return;
	}
#endif

	// 없어서는 안되는 항목들 체크 하고 넘어가자

	int nStartCycle = GetStartCycle();		// FIX=1
	int nStartStep = GetStartStep();		// FIX=1

	if (nStartCycle <= 0 || nStartStep <= 0)
	{
		AfxMessageBox("It cannot be started because the start position of the recipe is not specified.");
		return;
	}

	// 여기서 동기화 선택 여부에 따라서 첫번째 예외처리를 한다.
	int nSyncMode = GetSyncMode();

	if (eScheduleSyncModeStep == nSyncMode)
	{
		if (false == IsStepSyncValidate())
		{
			AfxMessageBox("During step sync, you can proceed with two or more channels selected.");
			return;
		}
	}
	else if (eScheduleSyncModeTemperature == nSyncMode)
	{
		if (false == IsTemperatureSyncValidate())
		{
			AfxMessageBox("During Temp sync, it is possible to proceed with the chamber temperature change history.");
			return;
		}
	}

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (pMgrAccount)
	{
		sAccount* lpAccount = pMgrAccount->GetLoginedAccount();

		if (lpAccount)
		{
			CString strFilePath;

			//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_FILE_PATH)->GetWindowText(strFilePath);
			strFilePath = pMgrConfig->GetLogFilePath();		// FORMATION 

			_tcscpy_s(lpAccount->szSaveFilePath, strFilePath);

			pMgrAccount->SaveAccount();
		}
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		SaveHistory();

		SStartSchedule kParser;

		kParser.nStartCycleNumber = nStartCycle;
		kParser.nStartStepNumber = nStartStep;
		kParser.nSyncMode = nSyncMode;
		kParser.strEQPID = GetEqpId();
		kParser.strScheduleFilePath = GetScheduleFilePath(nStageNo);
		kParser.strScheduleName = GetScheduleName(nStageNo);
		kParser.strUserID = GetLoginUserName();
		kParser.strWorkName = GetWorkName();
		kParser.strLogFilePath = GetLogFilePath();
		kParser.bScheduleName = IsDlgButtonChecked(IDC_CHECK_SCHNAME);
		kParser.bTime = IsDlgButtonChecked(IDC_CHECK_TIME);

		GetCellID(kParser.vecCellID);

		//Stage 구분 활용 필요 할듯
		GetCyclerChannelNumber(nStageNo, kParser.vecCyclerChannelNumber);

		//중복 불가하게 수정해야함 pnx_hr 2021.04.29
		if (eScheduleLoadingTypeAdd == m_nScheduleLoadingType)
		{

			pMgr->StartSchedule(kParser);		// START PROCESS
		}
		else
			pMgr->ScheduleAppend(kParser);
		TRACE("m_nScehduleLoadingType = %d", m_nScheduleLoadingType);

		CWnd* pWnd = AfxGetApp()->GetMainWnd();
		if (pWnd)
		{
			auto iterE = kParser.vecCyclerChannelNumber.end();
			iterE--;
			::PostMessage(pWnd->GetSafeHwnd(), WM_VIEW_PATTERN, 0, *iterE);
		}

		//ShowWindow(SW_HIDE);
	}
}

int CDlgViewEqpChannelStatus::GetStartCycle()
{
	CString strValue;
	strValue = _T("1");

	return atoi(strValue);
}

int CDlgViewEqpChannelStatus::GetStartStep()
{
	CString strValue;
	strValue = _T("1");

	return atoi(strValue);
}

int CDlgViewEqpChannelStatus::GetSyncMode()
{
	return eScheduleSyncModeNotUse;
}

bool CDlgViewEqpChannelStatus::IsStepSyncValidate()
{
	// 스텝 동기화를 할때는 2개 이상의 채널이 선택되어 있어야 한다.
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return false;

	int nSelectCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

	if (nSelectCyclerChannelCount <= 1)
		return false;

	int nChamberKey = -1;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (nullptr == lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		CMgrComm* pMgrComm = CMgrComm::GetMgr();

		if (!pMgrComm)
			continue;

		if (pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber()) < 0)
			return false;

		if (nChamberKey == -1)
			nChamberKey = pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber());
		else
		{
			if (nChamberKey != pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber()))
				return true;
		}
	}

	return true;
}

bool CDlgViewEqpChannelStatus::IsTemperatureSyncValidate()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return false;

	CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();

	if (!lpCyclerChannel)
		return false;

	CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

	if (!lpScheduleInfo)
		return false;

	// 지금 선택한 채널이 1개인 경우 -> 나머지 채널이 진행중인지 체크
	// 3. 나머지 채널 진행중이다 and 지금 채널이 온도 변경이 있다 = false;		ok
	// 4. 나머지 채널 진행중이다 and 직므 채널이 온도 변경이 없다  = true;		ok

	int nSelectCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

	if (1 == nSelectCyclerChannelCount)
	{
		CMgrComm* pMgrComm = CMgrComm::GetMgr();

		if (!pMgrComm)
			return false;

		int nExecCyclerChannel = pMgrComm->GetCyclerChannelFromDevice(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber());

		if (nExecCyclerChannel < 0)
			return false;

		CChannel* lpExeCyclerChannel = pMgr->GetChannel(nExecCyclerChannel);

		if (!lpExeCyclerChannel)
			return false;

		if (true == lpExeCyclerChannel->GetScheduleStart())
		{
			{
				CMgrSchedule* lpExeSchedule = lpExeCyclerChannel->GetMgr();

				if (!lpExeSchedule)
					return false;

				CList< CStep*, CStep* >& listExeStep = lpExeSchedule->GetStepList();

				auto Pos = listExeStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listExeStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					// 얘는 이미 돌고 있는 스케줄이 온도 변경하는 놈이다.
					if (fValue != 0.000f)
						return false;
				}
			}

			{
				CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
				lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath());

				CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

				if (listStep.GetSize() <= 0)
				{
					delete lpModifyScheduleInfo;
					lpModifyScheduleInfo = nullptr;

					return false;
				}

				auto Pos = listStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					if (fValue != 0.000f)
					{
						delete lpModifyScheduleInfo;
						lpModifyScheduleInfo = nullptr;

						return false;
					}
				}

				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;
			}
		}
	}

	{
		CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
		lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath());

		CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

		if (listStep.GetSize() <= 0)
		{
			delete lpModifyScheduleInfo;
			lpModifyScheduleInfo = nullptr;

			return false;
		}

		auto Pos = listStep.GetHeadPosition();

		while (Pos)
		{
			CStep* lpStep = listStep.GetNext(Pos);

			if (!lpStep)
				continue;

			float fValue = lpStep->GetChamberSetTemp();

			if (fValue != 0.000f)
			{
				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;

				return true;
			}
		}

		delete lpModifyScheduleInfo;
		lpModifyScheduleInfo = nullptr;

		return false;
	}
}

bool CDlgViewEqpChannelStatus::IsTemperatureSyncValidate(int nStageNo)
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return false;

	CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();

	if (!lpCyclerChannel)
		return false;

	CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

	if (!lpScheduleInfo)
		return false;

	// 지금 선택한 채널이 1개인 경우 -> 나머지 채널이 진행중인지 체크
	// 3. 나머지 채널 진행중이다 and 지금 채널이 온도 변경이 있다 = false;		ok
	// 4. 나머지 채널 진행중이다 and 직므 채널이 온도 변경이 없다  = true;		ok

	int nSelectCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

	if (1 == nSelectCyclerChannelCount)
	{
		CMgrComm* pMgrComm = CMgrComm::GetMgr();

		if (!pMgrComm)
			return false;

		int nExecCyclerChannel = pMgrComm->GetCyclerChannelFromDevice(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber());

		if (nExecCyclerChannel < 0)
			return false;

		CChannel* lpExeCyclerChannel = pMgr->GetChannel(nExecCyclerChannel);

		if (!lpExeCyclerChannel)
			return false;

		if (true == lpExeCyclerChannel->GetScheduleStart())
		{
			{
				CMgrSchedule* lpExeSchedule = lpExeCyclerChannel->GetMgr();

				if (!lpExeSchedule)
					return false;

				CList< CStep*, CStep* >& listExeStep = lpExeSchedule->GetStepList();

				auto Pos = listExeStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listExeStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					// 얘는 이미 돌고 있는 스케줄이 온도 변경하는 놈이다.
					if (fValue != 0.000f)
						return false;
				}
			}

			{
				CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
				lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath(nStageNo));

				CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

				if (listStep.GetSize() <= 0)
				{
					delete lpModifyScheduleInfo;
					lpModifyScheduleInfo = nullptr;

					return false;
				}

				auto Pos = listStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					if (fValue != 0.000f)
					{
						delete lpModifyScheduleInfo;
						lpModifyScheduleInfo = nullptr;

						return false;
					}
				}

				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;
			}
		}
	}

	{
		CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
		lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath(nStageNo));

		CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

		if (listStep.GetSize() <= 0)
		{
			delete lpModifyScheduleInfo;
			lpModifyScheduleInfo = nullptr;

			return false;
		}

		auto Pos = listStep.GetHeadPosition();

		while (Pos)
		{
			CStep* lpStep = listStep.GetNext(Pos);

			if (!lpStep)
				continue;

			float fValue = lpStep->GetChamberSetTemp();

			if (fValue != 0.000f)
			{
				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;

				return true;
			}
		}

		delete lpModifyScheduleInfo;
		lpModifyScheduleInfo = nullptr;

		return false;
	}
}

void CDlgViewEqpChannelStatus::SaveHistory()
{
	CMgrScheduleHistory* pMgrScheduleHistory = CMgrScheduleHistory::GetMgr();

	if (pMgrScheduleHistory)
	{
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeUserName, GetLoginUserName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeSampleName, GetWorkName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeEquipName, GetEqpId());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeScheduleName, GetScheduleName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeSchedulePath, GetScheduleFilePath());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeLogFilePath, GetLogFilePath());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeDescription, GetDescription());
	}
}

void CDlgViewEqpChannelStatus::GetCellID(std::vector<CString>& vecValue)
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		int nSelectedCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

		for (int i = 0; i < nSelectedCyclerChannelCount; ++i)
		{
			CString strValue;
			//strValue = m_listMaterial.GetItemText(i, 2); CELL_ID		// 확인필요
			strValue.Format(_T("TempCellId_%d"), i);
			vecValue.push_back(strValue);
		}
	}
}

void CDlgViewEqpChannelStatus::GetCyclerChannelNumber(std::vector<int>& vecValue)
{
	vecValue.clear();

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (!lpCyclerChannel->GetSelected())
			continue;

		vecValue.push_back(lpCyclerChannel->GetChannelNumber());
	}
}

void CDlgViewEqpChannelStatus::GetCyclerChannelNumber(int nStageNo, std::vector<int>& vecValue)
{
	vecValue.clear();

	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	if (!pMgr)
		return;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (!lpCyclerChannel->GetSelected())
			continue;

		vecValue.push_back(lpCyclerChannel->GetChannelNumber());
	}




}

// -----------------------------------------------------------------------------

/**
 * .
 * 
 * METHOD_NAME : GetScheduleFilePath
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetScheduleFilePath()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex());

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex());

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleFilePath;
	}

	return "";
}

CString CDlgViewEqpChannelStatus::GetScheduleFilePath(int nStageNo)
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex(nStageNo));

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex(nStageNo));

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleFilePath;
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetScheduleName
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetScheduleName()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex());

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex());

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleName;
	}

	return "";
}

CString CDlgViewEqpChannelStatus::GetScheduleName(int nStageNo)
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex(nStageNo));

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex(nStageNo));

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleName;
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetEQPID
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetEqpId()
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig)
	{
		return pMgrConfig->GetCyclerEQPID();
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetLoginUserName
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetLoginUserName()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_WORKER)->GetWindowText(strValue);

	if (strValue.IsEmpty())
	{
		// MES 연동 부분에 작업자, 설명이 있다. 이걸로 넣어야 하나??
		CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
		if (pMgrAccount)
		{
			return pMgrAccount->GetLoginID();
		}
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetWorkName
 * METHOD_TYPE : -
 * DESCRIPTION : 작업명(?)
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetWorkName()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_WORK_NAME)->GetWindowText(strValue);

	return strValue;
}

/**
 * .
 * 
 * METHOD_NAME : GetLogFilePath
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetLogFilePath()
{
	CString strValue;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig)
	{
		strValue = pMgrConfig->GetLogFilePath();
	}

	return strValue;
}

/**
 * .
 * 
 * METHOD_NAME : GetDescription
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewEqpChannelStatus::GetDescription()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_DESCRIPTION)->GetWindowText(strValue);
	return strValue;
}

void CDlgViewEqpChannelStatus::SetSelectStage(int nStageNo, bool bSelect)
{
	m_bSelectStage[nStageNo] = bSelect;
	SetSelectEQChAll(nStageNo);
}

bool CDlgViewEqpChannelStatus::GetSelectStage(int nStageNo)
{
	return m_bSelectStage[nStageNo];
}


void CDlgViewEqpChannelStatus::UpdataChStatus()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CString strStage1Status = "";
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	if (bSubStage1Use == true)
	{
		int nMgrStatus1 = CMgrStatus::GetMgr()->GetEqpRunStatus(0);
		COLORREF crTempStatusColor1 = GetDisplayChannelStatusColor(nMgrStatus1);
		
		if (_tcsicmp(m_strStage1, g_lpszCyclerStateString[nMgrStatus1]) != 0)
		{
			m_Status_Stage1.SetBkColor(crTempStatusColor1, crTempStatusColor1);
			m_Status_Stage1.SetWindowText(g_lpszCyclerStateString[nMgrStatus1]);
			m_Status_Stage1.Invalidate(false);
			m_strStage1 = g_lpszCyclerStateString[nMgrStatus1];
		}
		if (CMgrPCan::GetMgr()->GetStation(0) != NULL)
		{
			SetInverterStatus(0);
		}

		if (CMgrStatus::GetMgr()->GetEqInitFlag(0) == true)
		{
			m_Btn_Stage1_Init.SetWindowText("Stage Initial OK");
			//m_Btn_Stage1_Init.EnableWindow(false);
		}
		else
		{
			m_Btn_Stage1_Init.SetWindowText("Stage Initial Please");
			//m_Btn_Stage1_Init.EnableWindow(true);
		}
		int nSubEqpStageChannelMaxCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
		CString strChStatus = "";
		for (int nCh = 0; nCh < nSubEqpStageChannelMaxCount; nCh++)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();

			COLORREF crTempStatusColor = GetDisplayChannelStatusColor(nChannelStatus);
			GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + nCh)->GetWindowTextA(_T(strChStatus));

			if (_tcsicmp(m_strChStatus[nCh], g_lpszCyclerStateString[nChannelStatus]) != 0 || _tcsicmp(m_staticControl[nCh].GetControlText(), g_lpszCyclerStateString[nChannelStatus]) != 0)
			{
				m_staticControl[nCh].SetBkColor(crTempStatusColor, crTempStatusColor);
				m_staticControl[nCh].SetWindowText(g_lpszCyclerStateString[nChannelStatus]);
				m_staticControl[nCh].Invalidate(false);
				m_strChStatus[nCh] = g_lpszCyclerStateString[nChannelStatus];
			}
		}
		UI_BarcodeUpdate(0);

	}

	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	if (bSubStage2Use == true)
	{
		int nMgrStatus2 = CMgrStatus::GetMgr()->GetEqpRunStatus(1);
		COLORREF crTempStatusColor2 = GetDisplayChannelStatusColor(nMgrStatus2);
		if (_tcsicmp(m_strStage2, g_lpszCyclerStateString[nMgrStatus2]) != 0)
		{
			m_Status_Stage2.SetBkColor(crTempStatusColor2, crTempStatusColor2);
			m_Status_Stage2.SetWindowText(g_lpszCyclerStateString[nMgrStatus2]);
			m_Status_Stage2.Invalidate(false);
			m_strStage2 = g_lpszCyclerStateString[nMgrStatus2];
		}
		if (CMgrPCan::GetMgr()->GetStation(1) != NULL)
		{
			SetInverterStatus(1);
		}

		if (CMgrStatus::GetMgr()->GetEqInitFlag(1) == true)
		{
			m_Btn_Stage2_Init.SetWindowText("Stage Initial OK");
			//m_Btn_Stage2_Init.EnableWindow(false);
		}
		else
		{
			m_Btn_Stage2_Init.SetWindowText("Stage Initial Please");
			//m_Btn_Stage2_Init.EnableWindow(true);
		}
		UI_BarcodeUpdate(1);
		int nSubEqpStageChannelMaxCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
		CString strChStatus = "";
		for (int nCh = nSubEqpStageChannelMaxCount; nCh < nSubEqpStageChannelMaxCount+ nSubEqpStageChannelMaxCount; nCh++)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();

			COLORREF crTempStatusColor = GetDisplayChannelStatusColor(nChannelStatus);
			GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + nCh)->GetWindowTextA(_T(strChStatus));

			if (_tcsicmp(m_strChStatus[nCh], g_lpszCyclerStateString[nChannelStatus]) != 0 || _tcsicmp(m_staticControl[nCh].GetControlText(), g_lpszCyclerStateString[nChannelStatus]) != 0)
			{
				m_staticControl[nCh].SetBkColor(crTempStatusColor, crTempStatusColor);
				m_staticControl[nCh].SetWindowText(g_lpszCyclerStateString[nChannelStatus]);
				m_staticControl[nCh].Invalidate(false);
				m_strChStatus[nCh] = g_lpszCyclerStateString[nChannelStatus];
			}
		}
	}

	if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
	{
		//20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		if (CMgrConfig::GetMgr()->m_strPLCControlType == "MOXA")
		{
			if (bSubStage1Use == true)
			{
				if (CMgrStatus::GetMgr()->GetEqInitFlag(0) == true)
				{
					m_Status_Start_Message.ShowWindow(SW_SHOW);
					m_Btn_Stage1_Init.ShowWindow(SW_HIDE);
				}
				else
				{
					m_Status_Start_Message.ShowWindow(SW_HIDE);
					m_Btn_Stage1_Init.ShowWindow(SW_SHOW);
				}
				if (pMgrDio->GetMoxa_PC_Busy() == true)
				{
					m_Status_Start_Message.SetWindowText("Schedule Proceeding status...");

					m_Btn_Start.EnableWindow(false);
				}
				else if (pMgrDio->GetMoxa_PLC_Start() != true)
				{
					m_Status_Start_Message.SetWindowText("Waiting for PLC Ready status...");

					m_Btn_Start.EnableWindow(false);
				}
				else if (pMgrDio->GetMoxa_PLC_Start() == true)
				{
					m_Status_Start_Message.SetWindowText("Ready to start charging/discharging schedule");
					m_Btn_Start.EnableWindow(true);
				}
			}
		}
		else if (CMgrConfig::GetMgr()->m_strPLCControlType == "Mitsubishi")
		{
			if (bSubStage1Use == true)
			{
				if (CMgrStatus::GetMgr()->GetEqInitFlag(0) == false)
				{
					m_Status_Start_Message.ShowWindow(SW_HIDE);
					m_Btn_Stage1_Init.ShowWindow(SW_SHOW);
				}

				else
				{
					m_Status_Start_Message.ShowWindow(SW_SHOW);
					m_Btn_Stage1_Init.ShowWindow(SW_HIDE);
				}
				if (pDlgViewPlc1 != NULL)
				{
					if (pDlgViewPlc1->m_StPlcDataParam[TYPE_PLC_LOAD_COMPLETE].lPlcData == BIT_ON)
					{
						m_Status_Start_Message.SetWindowText("Waiting for PLC Load Complete status...");
						m_Btn_Start.EnableWindow(false);
					}
					else
					{
						m_Status_Start_Message.SetWindowText("Ready to start charging/discharging schedule");
						m_Btn_Start.EnableWindow(true);
					}
				}
			}
			if (bSubStage2Use == true)
			{
				if (CMgrStatus::GetMgr()->GetEqInitFlag(1) == false)
				{
					m_Status_Stage2_Start_Message.ShowWindow(SW_HIDE);
					m_Btn_Stage2_Init.ShowWindow(SW_SHOW);
				}
				else
				{
					m_Status_Stage2_Start_Message.ShowWindow(SW_SHOW);
					m_Btn_Stage2_Init.ShowWindow(SW_HIDE);
				}
				if (pDlgViewPlc2 != NULL)
				{
					if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_LOAD_COMPLETE].lPlcData != BIT_ON)
					{
						m_Status_Stage2_Start_Message.SetWindowText("Waiting for PLC Load Complete status...");
						m_Btn_Stage2_Start.EnableWindow(false);
					}
					else
					{
						m_Status_Stage2_Start_Message.SetWindowText("Ready to start charging/discharging schedule");
						m_Btn_Stage2_Start.EnableWindow(true);
					}
				}
			}		
		
		}
	}
	else
	{
		if (bSubStage1Use == true)
		{
			m_Btn_Stage1_Init.ShowWindow(SW_SHOW);
			m_Status_Start_Message.ShowWindow(SW_HIDE);
		}
		if (bSubStage2Use == true)
		{
			m_Btn_Stage2_Init.ShowWindow(SW_SHOW);
			m_Status_Stage2_Start_Message.ShowWindow(SW_HIDE);
		}

		CMgrStatus* pCMgrStatus = CMgrStatus::GetMgr();

		if (pCMgrStatus == NULL)
		{
			return;
		}

	/*	if (pCMgrStatus->GetEqp_Stage1_RunMode() == EQP_RUN_MODE_MANUAL)
		{
			m_Btn_Stage1_Init.EnableWindow(TRUE);
		}
		else if (pCMgrStatus->GetEqp_Stage1_RunMode() == EQP_RUN_MODE_AUTO)
		{
			m_Btn_Stage1_Init.EnableWindow(FALSE);
		}*/
	}	
}

void CDlgViewEqpChannelStatus::OnClickTitleButton(UINT nID)
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	//220331 YGYUN Test 용
	// 임시로 막음
	int nCh = nID - IDC_BTN_CH1;

	if (pMgr->GetMeasureModeNo() != elFBoradStepModeMasterAutocalCurrent)
	{
		////220331 YGYUN 컨셉 상 하나씩 채널 선택 안되게 막음
		AfxMessageBox("It's not the right approach. It is not possible to select a single channel. Please use Select All Channels.");

		return;

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{

			pChannel->SetSelected(false);
			m_BtnSelectCh[nCh].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
			m_BtnSelectCh[nCh].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[nCh].SetTextColor(COLOR_BLACK);
		}
		else
		{
			pChannel->SetSelected(true);
			m_BtnSelectCh[nCh].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_PUSH_BTN);
			m_BtnSelectCh[nCh].SetFont("Verdana", 10, 1000);
			m_BtnSelectCh[nCh].SetTextColor(ColorWhite);
		}
	}

	//220818 YGY Calibration Test 용
	int nMasterChannel = 0;
	int nSlaveChannel = 0;

	//짝수
	if (nCh % 2 == 0)
	{
		nMasterChannel = nCh;
		nSlaveChannel = nCh + 1;
	}
	//홀수
	else
	{
		nMasterChannel = nCh;
		nSlaveChannel = nCh - 1;
	}

	CChannel* pChannel_Master = CMgrChannel::GetMgr()->GetChannel(nMasterChannel);
	CChannel* pChannel_Slave = CMgrChannel::GetMgr()->GetChannel(nSlaveChannel);

	if (pChannel_Master->GetSelected() == false)
	{
		//Master
		pChannel_Master->SetSelected(true);
		//Calibration Mode
		SelectCalibration();
	}
	else
	{
		pChannel_Master->SetSelected(false);
		pChannel_Slave->SetSelected(false);

		SelectCalibration();

	}

}

void CDlgViewEqpChannelStatus::OnClickChBarcode(UINT nID)
{
	UI_Update_Focus(nID);
	Set_BarcodeReadingFocus(nID);
}


// -----------------------------------------------------------------------------
BOOL CDlgViewEqpChannelStatus::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusPause()
{
	CDlgPauseOption* lpDlg = CDlgPauseOption::GetMgr();
	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);

}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusComplete()
{
	CDlgCompleteOption* lpDlg = CDlgCompleteOption::GetMgr();
	if (lpDlg)
	{
		lpDlg->SetStageNo(0);
		lpDlg->ShowWindow(SW_SHOW);
	}
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusAlarmClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}




void CDlgViewEqpChannelStatus::Set_Stage_UI_Visible()
{
	CString strChannelName;
	COLORREF crTempStatusColor;

	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	if (bSubStage1Use == true)
	{

		m_Title_Stage1_CH.SetBkColor(ColorWhite);
		m_Title_Stage1_StepTime.SetBkColor(ColorWhite);
		m_Title_Stage1_TotalTime.SetBkColor(ColorWhite);
		m_Title_Stage1_Volt.SetBkColor(ColorWhite);
		m_Title_Stage1_Current.SetBkColor(ColorWhite);
		m_Title_Stage1_Cap.SetBkColor(ColorWhite);
		m_Title_Stage1_Code.SetBkColor(ColorWhite);
		m_Title_Stage1_Temp.SetBkColor(ColorWhite);

		UI_Ch_Init(0);
	}
	else
	{
		//GroupBox
		GetDlgItem(IDC_BTN_VIEW_EQP_STAGE1_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_STAGE1_MODE)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_BARCODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_MODELNAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_SCHEDULENAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_CHBARCODE3)->ShowWindow(SW_HIDE);

			   		 
		//GroupBox
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_GRP_STACK1)->ShowWindow(SW_HIDE);
					
		m_Line_1.ShowWindow(SW_HIDE);
		m_btn_Complete_Option.ShowWindow(SW_HIDE);
		m_Status_Start_Message.ShowWindow(SW_HIDE);
		m_Btn_Start.ShowWindow(SW_HIDE);
		m_Btn_SelectSchedule.ShowWindow(SW_HIDE);
		m_BtnSelectStageS1.ShowWindow(SW_HIDE);
		m_Status_Schedule_Model.ShowWindow(SW_HIDE);
		m_Status_Schedule.ShowWindow(SW_HIDE);
		m_staticTrayBarcode.ShowWindow(SW_HIDE);
		m_Btn_Stage1_Init.ShowWindow(SW_HIDE);

		//Main Status
		m_Status_Stage1.ShowWindow(SW_HIDE);
		m_Btn_Stage1_Mode.ShowWindow(SW_HIDE);

		//All Ch Select Button
		m_BtnSelectStageS1.ShowWindow(SW_HIDE);
			   

		m_btn_DataSwtching[0].ShowWindow(SW_HIDE);
		m_Title_Stage1_CH.ShowWindow(SW_HIDE);
		m_Title_Stage1_StepTime.ShowWindow(SW_HIDE);
		m_Title_Stage1_TotalTime.ShowWindow(SW_HIDE);
		m_Title_Stage1_Volt.ShowWindow(SW_HIDE);
		m_Title_Stage1_Current.ShowWindow(SW_HIDE);
		m_Title_Stage1_Cap.ShowWindow(SW_HIDE);
		m_Title_Stage1_Code.ShowWindow(SW_HIDE);
		m_Title_Stage1_Temp.ShowWindow(SW_HIDE);

		for (int i = 0; i < 24; i++)
		{
			//CH
			m_BtnSelectCh[i].ShowWindow(SW_HIDE);

			//Status
			m_staticControl[i].ShowWindow(SW_HIDE);

			//Ch Barcode
			m_staticChBarcode[i].ShowWindow(SW_HIDE);

			//Step Time
			m_staticStepTime[i].ShowWindow(SW_HIDE);

			//Total Time
			m_staticTotalTime[i].ShowWindow(SW_HIDE);

			//Volt
			m_staticVolt[i].ShowWindow(SW_HIDE);
			
			//Current
			m_staticAmp[i].ShowWindow(SW_HIDE);

			//Cap
			m_staticCapacity[i].ShowWindow(SW_HIDE);
			
			//Code
			m_staticChCode[i].ShowWindow(SW_HIDE);

			//TEMP
			m_staticTemp[i].ShowWindow(SW_HIDE);

			//Cycle 
			m_staticCycle[i].ShowWindow(SW_HIDE);

			//Step
			m_staticStep[i].ShowWindow(SW_HIDE);
		}
		m_Status_Inverter.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_EQPNAME3)->ShowWindow(SW_HIDE);
	}

	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	if (bSubStage2Use == true)
	{
		CString strSubStageName = CMgrConfig::GetMgr()->GetSubEqpInfo().strStage2Name;

		m_Title_Stage2_CH.SetBkColor(ColorWhite);
		m_Title_Stage2_StepTime.SetBkColor(ColorWhite);
		m_Title_Stage2_TotalTime.SetBkColor(ColorWhite);
		m_Title_Stage2_Volt.SetBkColor(ColorWhite);
		m_Title_Stage2_Current.SetBkColor(ColorWhite);
		m_Title_Stage2_Cap.SetBkColor(ColorWhite);
		m_Title_Stage2_Code.SetBkColor(ColorWhite);
		m_Title_Stage2_Temp.SetBkColor(ColorWhite);


		UI_Ch_Init(1);
	}
	else
	{
		//GroupBox
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_GRP_STACK2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_STAGE1_MODE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_LBL_BARCODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_LBL_MODELNAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_LBL_SCHEDULENAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_CHBARCODE2)->ShowWindow(SW_HIDE);


		m_btn_DataSwtching[1].ShowWindow(SW_HIDE);

		m_Line_2.ShowWindow(SW_HIDE);
		m_btn_Stage2_Complete_Option.ShowWindow(SW_HIDE);
		m_Status_Stage2_Start_Message.ShowWindow(SW_HIDE);
		m_Btn_Stage2_Start.ShowWindow(SW_HIDE);
		m_Btn_Stage2_SelectSchedule.ShowWindow(SW_HIDE);
		m_BtnSelectStageS2.ShowWindow(SW_HIDE);
		m_Status_Schedule_Stage2_Model.ShowWindow(SW_HIDE);
		m_Status_Stage2_Schedule.ShowWindow(SW_HIDE);
		m_staticTrayBarcode_Stage2.ShowWindow(SW_HIDE);
		m_Btn_Stage2_Init.ShowWindow(SW_HIDE);

		//Main Status
		m_Status_Stage2.ShowWindow(SW_HIDE);
		m_Btn_Stage2_Mode.ShowWindow(SW_HIDE);

		//All Ch Select Button
		m_BtnSelectStageS2.ShowWindow(SW_HIDE);

		m_Title_Stage2_CH.ShowWindow(SW_HIDE);
		m_Title_Stage2_StepTime.ShowWindow(SW_HIDE);
		m_Title_Stage2_TotalTime.ShowWindow(SW_HIDE);
		m_Title_Stage2_Volt.ShowWindow(SW_HIDE);
		m_Title_Stage2_Current.ShowWindow(SW_HIDE);
		m_Title_Stage2_Cap.ShowWindow(SW_HIDE);
		m_Title_Stage2_Code.ShowWindow(SW_HIDE);
		m_Title_Stage2_Temp.ShowWindow(SW_HIDE);

		for (int i = 24; i < 48; i++)
		{
			//CH
			m_BtnSelectCh[i].ShowWindow(SW_HIDE);

			//Status
			m_staticControl[i].ShowWindow(SW_HIDE);

			//Ch Barcode
			m_staticChBarcode[i].ShowWindow(SW_HIDE);

			//Step Time
			m_staticStepTime[i].ShowWindow(SW_HIDE);

			//Total Time
			m_staticTotalTime[i].ShowWindow(SW_HIDE);

			//Volt
			m_staticVolt[i].ShowWindow(SW_HIDE);

			//Current
			m_staticAmp[i].ShowWindow(SW_HIDE);

			//Cap
			m_staticCapacity[i].ShowWindow(SW_HIDE);

			//Code
			m_staticChCode[i].ShowWindow(SW_HIDE);

			//TEMP
			m_staticTemp[i].ShowWindow(SW_HIDE);

			//Cycle 
			m_staticCycle[i].ShowWindow(SW_HIDE);

			//Step
			m_staticStep[i].ShowWindow(SW_HIDE);
		}


		m_InverterStatus_2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_EQPNAME4)->ShowWindow(SW_HIDE);
	}

	if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
	{
		if (CMgrDio::GetMgr()->GetEQType() != enumEQType_DFS)
		{
			CMgrPlcInterface* pMgrPlcInterface = CMgrPlcInterface::GetMgr();

			if (pMgrPlcInterface == NULL)
			{
				return;
			}
			CPlcInterface* lpDevicePlcInterface = pMgrPlcInterface->GetDevice(0);

			if (lpDevicePlcInterface == NULL)
			{
				return;
			}
			GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_PLC_INFO)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_VACCUMVALUE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_LBL_PUMPOPENPERCENT)->ShowWindow(SW_SHOW);

			m_staticVacValue.ShowWindow(SW_SHOW);
			m_Data_PumpOpenPercent.ShowWindow(SW_SHOW);
		}
	}
	

}

void CDlgViewEqpChannelStatus::UI_Update_Focus(int nID)
{
	int nChMaxCount = 0;
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	int nFocusCh = 0;

	nFocusCh = nID - IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE1;
	   
	if (bSubStage1Use == true)
	{
		nChMaxCount++;
	}
	if (bSubStage1Use == true)
	{
		nChMaxCount++;
	}
	if (nID == IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE || nID == IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE)
	{
		if (nID == IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE)
		{
			//Stage1
			m_staticTrayBarcode.SetBkColor(COLOR_GREEN, COLOR_GREEN);
			m_staticTrayBarcode.SetFont("Verdana", 14, 1400);
			m_staticTrayBarcode.SetTextColor(ColorWhite);
		}
		if (nID == IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE)
		{
			//Stage2
			m_staticTrayBarcode_Stage2.SetBkColor(COLOR_GREEN, COLOR_GREEN);
			m_staticTrayBarcode_Stage2.SetFont("Verdana", 14, 1400);
			m_staticTrayBarcode_Stage2.SetTextColor(ColorWhite);
		}

		for (int nCh = 0; nCh < nStageChCount; nCh++)
		{
			m_staticChBarcode[nCh].SetBkColor(ColorWhite, ColorWhite);
			m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
			m_staticChBarcode[nCh].SetTextColor(COLOR_BLACK);
		}
	}		
	else
	{
		if (nFocusCh < nStageChCount)
		{
			m_staticTrayBarcode.SetBkColor(ColorWhite, ColorWhite);
			m_staticTrayBarcode.SetFont("Verdana", 14, 1400);
			m_staticTrayBarcode.SetTextColor(COLOR_BLACK);
		}
		else
		{
			m_staticTrayBarcode_Stage2.SetBkColor(ColorWhite, ColorWhite);
			m_staticTrayBarcode_Stage2.SetFont("Verdana", 14, 1400);
			m_staticTrayBarcode_Stage2.SetTextColor(COLOR_BLACK);
		}
		for (int nCh = nFocusCh / nStageChCount; nCh < (nFocusCh / nStageChCount)*nStageChCount + nStageChCount; nCh++)
		{

			if (nCh == nFocusCh)
			{
				m_staticChBarcode[nCh].SetBkColor(COLOR_GREEN, COLOR_GREEN);
				m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
				m_staticChBarcode[nCh].SetTextColor(ColorWhite);
			}
			else
			{
				m_staticChBarcode[nCh].SetBkColor(ColorWhite, ColorWhite);
				m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
				m_staticChBarcode[nCh].SetTextColor(COLOR_BLACK);
			}
		}
	}

}

void CDlgViewEqpChannelStatus::Display_Manage()
{
	ULARGE_INTEGER avail, total, free;
	avail.QuadPart = 0L;
	total.QuadPart = 0L;
	free.QuadPart = 0L;

	int m_avail, m_total, m_free;
	CString strFilePath;
	CString strDisk;
	unsigned __int64 i64FreeBytesToCaller,i64TotalBytes,i64FreeBytes;
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		strFilePath = pMgr->GetLogFilePath();
		AfxExtractSubString(strFilePath, strFilePath, 0, ':');
		strFilePath.Format(_T("%s:\\"),strFilePath);
	}

	
	// 첫번째 파라메터로 NULL을 넘기면 CurrentDir의 디스크 용량
	if (GetDiskFreeSpaceEx(TEXT(strFilePath), &avail, &total, &free)	== FALSE)
	{
		return;
	}

	m_total = (int)(total.QuadPart >> 30);
	m_free = (int)(free.QuadPart >> 30);

	strDisk.Format(_T("Total :\t%dGB"), m_total);

	m_Status_Disk.SetBkColor(COLOR_BLACK, COLOR_BLACK);
	m_Status_Disk.SetFont("Verdana", 14, 1400);
	m_Status_Disk.SetTextColor(ColorWhite);
	m_Status_Disk.SetWindowText(strDisk);
	m_Status_Disk.Invalidate(false);

	strDisk.Format(_T("Free :\t%dGB"), m_free);

	m_Status_Disk2.SetBkColor(ColorLightBlue, ColorLightBlue);
	m_Status_Disk2.SetFont("Verdana", 14, 1400);
	m_Status_Disk2.SetTextColor(ColorWhite);
	m_Status_Disk2.SetWindowText(strDisk);
	m_Status_Disk2.Invalidate(false);

	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	if(pMgrDio == NULL)
	{ 
		return;
	}

	if (pMgrDio->GetConnect() == true)
	{
		m_Data_IO_Status.SetBkColor(ColorWhite, ColorWhite);
		m_Data_IO_Status.SetFont("Verdana", 14, 1400);
		m_Data_IO_Status.SetTextColor(COLOR_BLACK);
		m_Data_IO_Status.SetWindowTextA(_T("Connect"));

	}
	else
	{
		m_Data_IO_Status.SetBkColor(ColorRed, ColorRed);
		m_Data_IO_Status.SetFont("Verdana", 14, 1400);
		m_Data_IO_Status.SetTextColor(ColorWhite);
		m_Data_IO_Status.SetWindowTextA(_T("Disconnect"));

	}
	if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
	{
		if (CMgrDio::GetMgr()->GetEQType() != enumEQType_DFS)
		{
			CMgrPlcInterface* pMgrPlcInterface = CMgrPlcInterface::GetMgr();

			if (pMgrPlcInterface == NULL)
			{
				return;
			}
			CPlcInterface* lpDevicePlcInterface = pMgrPlcInterface->GetDevice(0);

			if (lpDevicePlcInterface == NULL)
			{
				return;
			}
			if (pMgrPlcInterface->Get_VacuumValue() > 0)
			{
				m_staticVacValue.SetFont("Verdana", 22, 2800);
				m_staticVacValue.SetTextColor(COLOR_BLACK);
				m_staticVacValue.SetWindowText(pMgrPlcInterface->Get_VaccumValueKPAStr());
				m_staticVacValue.Invalidate(false);

			}
			if (pMgrPlcInterface->Get_PumpOpenPercent() > 0)
			{
				m_Data_PumpOpenPercent.SetFont("Verdana", 22, 2800);
				m_Data_PumpOpenPercent.SetTextColor(COLOR_BLACK);
				m_Data_PumpOpenPercent.SetWindowText(pMgrPlcInterface->Get_PumpOpenPercentStr());
				m_Data_PumpOpenPercent.Invalidate(false);

			}

			if (pMgrPlcInterface->Get_AutoMode() == true)
			{
				m_Btn_Stage1_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage1_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage1_Mode.SetWindowText("AUTO");
				m_Btn_Stage1_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(0, TRUE);
				//CMgrStatus::GetMgr()->SetEqInitFlag(0, TRUE);
			}
			else
			{
				m_Btn_Stage1_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage1_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage1_Mode.SetWindowText("MANUAL");
				m_Btn_Stage1_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(0, false);
				//CMgrStatus::GetMgr()->SetEqInitFlag(0, false);
			}			
		}
		else
		{
			if (CMgrDio::GetMgr()->GetAjinInputData(InputList_DFS_AJIN::INPUT_DFS_STAGE1_AUTO_MODE) == true && CMgrStatus::GetMgr()->GetEqInitFlag(0) == true)
			{
				m_Btn_Stage1_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage1_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage1_Mode.SetWindowText("AUTO");
				m_Btn_Stage1_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(0, true);
			}
			else
			{
				m_Btn_Stage1_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage1_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage1_Mode.SetWindowText("MANUAL");
				m_Btn_Stage1_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(0, false);
			}
			if (CMgrDio::GetMgr()->GetAjinInputData(InputList_DFS_AJIN::INPUT_DFS_STAGE1_AUTO_MODE) == true && CMgrStatus::GetMgr()->GetEqInitFlag(1) == true)
			{
				m_Btn_Stage2_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage2_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage2_Mode.SetWindowText("AUTO");
				m_Btn_Stage2_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(1, true);
			}
			else
			{
				m_Btn_Stage2_Mode.SetFont("Verdana", 16, 1400);
				m_Btn_Stage2_Mode.SetTextColor(COLOR_BLACK);
				m_Btn_Stage2_Mode.SetWindowText("MANUAL");
				m_Btn_Stage2_Mode.Invalidate(false);
				CMgrStatus::GetMgr()->SetEqAutoMode(1, false);
			}
		}

	}



	
}

void CDlgViewEqpChannelStatus::SetPlcDataParam()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	int nChannelCnt = listChannel.GetCount();
	auto Pos = listChannel.GetHeadPosition();

	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();

	int nRow = 0;
	int nBlockCount = (BLOCK_COUNT / 2);
	if (pDlgViewPlc1 != NULL && pDlgViewPlc2 != NULL) {
		while (Pos)
		{
			CChannel* pChannel = listChannel.GetNext(Pos);

			if (NULL == pChannel)
				continue;

			CStringArray arr;
			CString tempItemName;
			CString tempItemValue;

			pChannel->ChannelFormation2Array(arr, this->m_strArrTitle);

			for (int i = 0; i < arr.GetSize(); ++i)
			{
				tempItemName = m_strArrTitle[i];
				tempItemValue = arr.GetAt(i);

				int tempChannelIndexNo = nRow;

				if (tempChannelIndexNo < nBlockCount)//PLC1
				{
					if (tempItemName.CompareNoCase(_T("Voltage")) == 0) {
						pDlgViewPlc2->m_StPlcDataBlockParam2[PLC_SEND_V_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
					else if (tempItemName.CompareNoCase(_T("Current")) == 0) {
						pDlgViewPlc2->m_StPlcDataBlockParam2[PLC_SEND_A_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("Temperature")) == 0) {

						CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();

						if (pMgrDaQ)
						{
							tempItemValue = pMgrDaQ->GetDAQTemperaturestr(tempChannelIndexNo);
							//SetDisplayChannelTemperature(tempChannelIndexNo, strCyclerValue);
							pDlgViewPlc2->m_StPlcDataBlockParam2[PLC_SEND_T_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
						}
					}
					else if (m_strArrTitle[i].CompareNoCase(_T("Ah")) == 0) {
						pDlgViewPlc2->m_StPlcDataBlockParam2[PLC_SEND_Ah_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
				}
				else//PLC2
				{
					tempChannelIndexNo = (nRow - nBlockCount);
					if (tempItemName.CompareNoCase(_T("Voltage")) == 0) {
						pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_V_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
					else if (tempItemName.CompareNoCase(_T("Current")) == 0) {
						pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_A_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
					else if (tempItemName.CompareNoCase(_T("Temperature")) == 0) {
						CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();

						if (pMgrDaQ)
						{
							tempItemValue = pMgrDaQ->GetDAQTemperaturestr(tempChannelIndexNo);
							//SetDisplayChannelTemperature(tempChannelIndexNo, strCyclerValue);
							pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_T_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
						}
					}
					else if (tempItemName.CompareNoCase(_T("Ah")) == 0) {
						pDlgViewPlc1->m_StPlcDataBlockParam[PLC_SEND_Ah_24][tempChannelIndexNo * 2].fPlcDataBlock = atof(tempItemValue);
					}
				}
			}
			nRow++;
		}
	}
}


void CDlgViewEqpChannelStatus::Set_BarcodeReadingFocus(int nID)
{
	m_nBarcodeReadingFocus = nID;

}
void CDlgViewEqpChannelStatus::UI_Ch_Init(int nStageNo)
{
	CString strSubStageName = CMgrConfig::GetMgr()->GetSubEqpInfo().strStage1Name;
	CString strChannelName;
	COLORREF crTempStatusColor;
	int nStageCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	for (int i = nStageNo * nStageCount; i < nStageNo * nStageCount + nStageCount; i++)
	{
		// CH
		strChannelName.Format("%s-%d", strSubStageName, i + 1);

		crTempStatusColor = GetDisplayChannelStatusColor(0);

		m_staticControl[i].SetBkColor(crTempStatusColor, crTempStatusColor);
		m_staticControl[i].SetFont("Verdana", 11, 1200);
		m_staticControl[i].SetTextColor(COLOR_BLACK);

		m_BtnSelectCh[i].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
		m_BtnSelectCh[i].SetFont("Verdana", 10, 1000);
		m_BtnSelectCh[i].SetTextColor(COLOR_BLACK);
		m_BtnSelectCh[i].SetWindowText(strChannelName);
	}
}
void CDlgViewEqpChannelStatus::UI_Clear(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	if (nStageNo == 1)
	{
		m_staticTrayBarcode_Stage2.SetBkColor(ColorWhite, ColorWhite);
		m_staticTrayBarcode_Stage2.SetFont("Verdana", 14, 1400);
		m_staticTrayBarcode_Stage2.SetTextColor(COLOR_BLACK);
		m_staticTrayBarcode_Stage2.SetWindowText("");

		m_Status_Schedule_Stage2_Model.SetBkColor(ColorWhite, ColorWhite);
		m_Status_Schedule_Stage2_Model.SetFont("Verdana", 14, 1400);
		m_Status_Schedule_Stage2_Model.SetTextColor(COLOR_BLACK);
		m_Status_Schedule_Stage2_Model.SetWindowText("");

		m_Status_Stage2_Schedule.SetBkColor(ColorWhite, ColorWhite);
		m_Status_Stage2_Schedule.SetFont("Verdana", 14, 1400);
		m_Status_Stage2_Schedule.SetTextColor(COLOR_BLACK);
		m_Status_Stage2_Schedule.SetWindowText("");
	}
	else
	{
		m_staticTrayBarcode.SetBkColor(ColorWhite, ColorWhite);
		m_staticTrayBarcode.SetFont("Verdana", 14, 1400);
		m_staticTrayBarcode.SetTextColor(COLOR_BLACK);
		m_staticTrayBarcode.SetWindowText("");

		m_Status_Schedule_Model.SetBkColor(ColorWhite, ColorWhite);
		m_Status_Schedule_Model.SetFont("Verdana", 14, 1400);
		m_Status_Schedule_Model.SetTextColor(COLOR_BLACK);
		m_Status_Schedule_Model.SetWindowText("");

		m_Status_Schedule.SetBkColor(ColorWhite, ColorWhite);
		m_Status_Schedule.SetFont("Verdana", 14, 1400);
		m_Status_Schedule.SetTextColor(COLOR_BLACK);
		m_Status_Schedule.SetWindowText("");

	}

	for (int nCh = nStageNo * nStageChCount; nCh < nStageNo*nStageChCount + nStageChCount; nCh++)
	{
		m_staticChBarcode[nCh].SetBkColor(ColorWhite, ColorWhite);
		m_staticChBarcode[nCh].SetFont("Verdana", 14, 1400);
		m_staticChBarcode[nCh].SetTextColor(COLOR_BLACK);
		m_staticChBarcode[nCh].SetWindowText("");
	}


}
//220610 YGYUN NorthVolt Formation
void CDlgViewEqpChannelStatus::UI_BarcodeUpdate(int nStageNo)
{
	CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();

	int nTrayFocus = 0;
	if (pMgrSerial == NULL)
		return;
	
	if (nStageNo == 1)
	{
		nTrayFocus = IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE;
	}
	else
	{
		nTrayFocus = IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE;
	}
	int nCh = Get_BarcodeReadingFocus() - IDC_STATIC_VIEW_EQP_CHANNEL_CH_BARCODE1;
	
	if (Get_BarcodeReadingFocus() != nTrayFocus )
	{
		if (nCh / 24 != nStageNo)
		{
			return;
		}
	}

 	if (pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode() != "" )
	{
		if (pMgrSerial->Get_ChBarcodeCheckMode() == true)
		{
			//case : Channel Barcode
			if (Get_BarcodeReadingFocus() != nTrayFocus)
			{
				CCommData*	pCommData;

				pCommData = CMgrCommData::GetMgr()->GetAt(nCh);
				pCommData->SetChBarcode(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());

				CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
				pChannel->SetChBarcode(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());
			}
			if (Get_BarcodeReadingFocus() == nTrayFocus)
			{
				pMgrSerial->GetAtBarcode(nStageNo)->SetTrayBarcode(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());

			}
			GetDlgItem(Get_BarcodeReadingFocus())->SetWindowText(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());

			pMgrSerial->GetAtBarcode(nStageNo)->SetBarcode("");
			GetDlgItem(Get_BarcodeReadingFocus())->Invalidate(false);
		}
		else
		{		
			//case : Tray Barcode
			pMgrSerial->GetAtBarcode(nStageNo)->SetTrayBarcode(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());
			GetDlgItem(nTrayFocus)->SetWindowText(pMgrSerial->GetAtBarcode(nStageNo)->GetBarcode());
			pMgrSerial->GetAtBarcode(nStageNo)->SetBarcode("");
			GetDlgItem(nTrayFocus)->Invalidate(false);
		}		
		
	}	
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1Start()
{	


	//Auto Mode
	if (CMgrStatus::GetMgr()->GetEqAutoMode(0) == TRUE)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		if (pMgrDio == NULL)
			return;

		if (pMgrDio->GetEQType() == enumEQType_Precharger)
		{
			//Model 정보 확인
			CMgrModel* pMgrModel = CMgrModel::GetMgr();
			if (pMgrModel)
			{
				int nModelIndex = pMgrModel->GetModelIndex(0);
				if (nModelIndex < 0)
				{
					AfxMessageBox("Can't proceed because no model has been selected");

					return;
				}
			}

			//IO Connect Status
			if (pMgrDio->GetConnect() != true)
			{
				AfxMessageBox("IO Board not connected");
				return;
			}

			//Init Check
			if (CMgrStatus::GetMgr()->GetEqInitFlag(0) != TRUE)
			{
				AfxMessageBox("Please proceed with equipment initialization (Stage 1)");
				return;
			}

			//Chamber Run 상태 확인
			CMgrChamber* pMgrChmaber = CMgrChamber::GetMgr();

			if (!pMgrChmaber)
				return;

			SChamber* lpDeviceChamber = pMgrChmaber->GetDevice(0);

			if (lpDeviceChamber->bChamberExec != true)
			{
				AfxMessageBox("Chamber Ruuning Status Check Please");

				return;
			}			

			//Moxa PLC Ready Status
			if (pMgrDio->GetMoxa_PLC_Ready() != true)
			{
				AfxMessageBox("Moxa Board plc not ready.");

				return;
			}
			
			ExecProcScheduleStart(0);

		}
		else if (pMgrDio->GetEQType() == enumEQType_DFS)
		{
			//IO Connect Status
			if (pMgrDio->GetConnect() != true)
			{
				AfxMessageBox("IO Board not connected");
				return;
			}

			//Init Check
			if (CMgrStatus::GetMgr()->GetEqInitFlag(0) != TRUE)
			{
				AfxMessageBox("Please proceed with equipment initialization (Stage 1)");
				return;
			}

			//Model 정보 확인
			CMgrModel* pMgrModel = CMgrModel::GetMgr();
			if (pMgrModel)
			{
				int nModelIndex = pMgrModel->GetModelIndex(0);
				if (nModelIndex < 0)
				{
					AfxMessageBox("Can't proceed because no model has been selected");

					return;
				}
			}

			CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
			if (pDlgViewPlc1 != NULL)
				return;


			pDlgViewPlc1->SetPlcProcessStart();

		}
		//DCIR, Formation
		else
		{
			CMgrStatus::GetMgr()->SetEqStartFlag(0, TRUE);
		}
	}
	//Manual Mode
	else
	{
		////Chamber Run 상태 확인
		//CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

		//if (!pMgrChamber)
		//	return;

		//SChamber* lpDeviceChamber = pMgrChamber->GetDevice(0);

		//if (lpDeviceChamber->bChamberExec != true)
		//{
		//	AfxMessageBox("Chamber Ruuning Status Check Please");

		//	return;
		//}

		//Model 정보 확인
		CMgrModel* pMgrModel = CMgrModel::GetMgr();
		if (pMgrModel)
		{
			int nModelIndex = pMgrModel->GetModelIndex(0);
			if (nModelIndex < 0)
			{
				AfxMessageBox("Can't proceed because no model has been selected");

				return;
			}
		}
		//Init Check
		if (CMgrStatus::GetMgr()->GetEqInitFlag(0) != TRUE)
		{
			AfxMessageBox("Please proceed with equipment initialization (Stage 1)");
			return;
		}

		CMgrStatus::GetMgr()->SetEqp_RunMode(0, EQP_RUN_MODE_MANUAL);
		ExecProcScheduleStart(0);
	}
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1Group1()
{
	//Stage 1 선택
	m_nSelectedStageNo = 0;
	if (GetSelectStage(m_nSelectedStageNo) == false)
	{
		SetSelectStage(m_nSelectedStageNo, true);
	}
	else
	{
		SetSelectStage(m_nSelectedStageNo, false);
	}
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Group()
{
	//Stage 2 선택
	m_nSelectedStageNo = 1;
	if (GetSelectStage(m_nSelectedStageNo) == false)
	{
		SetSelectStage(m_nSelectedStageNo, true);
	}
	else
	{
		SetSelectStage(m_nSelectedStageNo, false);
	}
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage1SelectSchedule()
{
	SelectCalibration();

	//SelectSchedule();
	//220526 YYG Added function for selecting by stage
	SelectSchedule(0);
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusSelectStage2Schedule()
{
	//SelectSchedule();
	//220526 YYG Added function for selecting by stage
	SelectSchedule(1);
}

void CDlgViewEqpChannelStatus::Set_UI_Data_Switching(int nStageNo, bool bSwitching)
{
	m_bStageDataSwitching[nStageNo] = bSwitching;
}

void CDlgViewEqpChannelStatus::UI_Update_Switching(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	//기존 Status 데이터
	if (Get_UI_Data_Switching(nStageNo) == false)
	{
		for (int i = nStageNo * nStageChCount; i < (nStageNo*nStageChCount) + nStageChCount; i++)
		{
			m_staticControl[i].ShowWindow(SW_SHOW);
			m_staticCycle[i].ShowWindow(SW_HIDE);
			m_staticStep[i].ShowWindow(SW_HIDE);
		}
		m_btn_DataSwtching[nStageNo].SetWindowText("STATUS");
	
	}
	//Step Cycle 정보
	else
	{
		for (int i = nStageNo * nStageChCount; i < (nStageNo*nStageChCount) + nStageChCount; i++)
		{
			m_staticControl[i].ShowWindow(SW_HIDE);
			m_staticCycle[i].ShowWindow(SW_SHOW);
			m_staticStep[i].ShowWindow(SW_SHOW);

		}
		m_btn_DataSwtching[nStageNo].SetWindowText("Cycle   /   Step");
	}
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Start()
{	

	//Auto Mode 경우 Start
	if (CMgrStatus::GetMgr()->GetEqAutoMode(1) == TRUE)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		if (pMgrDio == NULL)
			return;

		//DFS 장비
		if (pMgrDio->GetEQType() == enumEQType_DFS)
		{
			//IO Connect Status
			if (pMgrDio->GetConnect() != true)
			{
				//AfxMessageBox("IO Board not connected");

				return;
			}

			//Init Check
			if (CMgrStatus::GetMgr()->GetEqInitFlag(1) != TRUE)
			{
				AfxMessageBox("Please proceed with equipment initialization (Stage 2)");
				return;
			}

			//PLC  통신 상태 확인
			CDlgViewPlc1* pDlgViewPlc2 = CDlgViewPlc1::GetMgr();
			if (pDlgViewPlc2 != NULL)
				return;

			//Run
			pDlgViewPlc2->SetPlcProcessStart();

		}
		//한화기계 
		else
		{
			//챔버 런 상태 확인
			CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

			if (!pMgrChamber)
				return;

			SChamber* lpDeviceChamber = pMgrChamber->GetDevice(1);

			if (lpDeviceChamber->bChamberExec != true)
			{
				AfxMessageBox("Chamber Running Status Check Please");

				return;
			}
			CMgrStatus::GetMgr()->SetEqStartFlag(1, TRUE);
		}
	}
	//Manual Mode
	else
	{
		//챔버 런 상태 확인
		CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

		if (!pMgrChamber)
			return;

		SChamber* lpDeviceChamber = pMgrChamber->GetDevice(1);

		if (lpDeviceChamber->bChamberExec != true)
		{
			AfxMessageBox("Chamber Running Status Check Please");

			return;
		}
		//Model 정보 확인
		CMgrModel* pMgrModel = CMgrModel::GetMgr();
		if (pMgrModel)
		{
			int nModelIndex = pMgrModel->GetModelIndex(1);
			if (nModelIndex < 0)
			{
				AfxMessageBox("Can't proceed because no model has been selected");

				return;
			}
		}
		//Init Check
		if (CMgrStatus::GetMgr()->GetEqInitFlag(1) != TRUE)
		{
			AfxMessageBox("Please proceed with equipment initialization (Stage 2)");
			return;
		}
		//Run Mode Manual로 지정
		CMgrStatus::GetMgr()->SetEqp_RunMode(1,EQP_RUN_MODE_MANUAL);
		//Run
		ExecProcScheduleStart(1);
	}
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpChannelStatusStage2Complete()
{
	CDlgCompleteOption* lpDlg = CDlgCompleteOption::GetMgr();
	if (lpDlg)
	{
		lpDlg->SetStageNo(1);
		lpDlg->ShowWindow(SW_SHOW);
	}
}


void CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelStatusTraybarcode()
{

	UI_Update_Focus(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE);

	Set_BarcodeReadingFocus(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_TRAYBARCODE);
}


void CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelStatusStage2Traybarcode()
{
	UI_Update_Focus(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE);

	Set_BarcodeReadingFocus(IDC_STATIC_VIEW_EQP_CHANNEL_STATUS_STAGE2_TRAYBARCODE);
}


void CDlgViewEqpChannelStatus::OnStnClickedStaticViewEqpChannelChBarcode1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Mode()
{	
	if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
	{
		return;
	}
	
	if (CMgrStatus::GetMgr()->GetEqAutoMode(0) == FALSE)
	{
		if (CMgrStatus::GetMgr()->GetEqInitFlag(0) != true)
		{
			AfxMessageBox("Please proceed with equipment initialization (Stage 1)");
			return;
		}

		CMgrStatus::GetMgr()->SetEqAutoMode(0, true);
		CMgrStatus::GetMgr()->SetEqp_RunMode(0,EQP_RUN_MODE_AUTO);
		m_Btn_Stage1_Mode.SetWindowText("AUTO");
	}
	else 
	{
		CMgrStatus::GetMgr()->SetEqAutoMode(0, FALSE);
		CMgrStatus::GetMgr()->SetEqp_RunMode(0, EQP_RUN_MODE_MANUAL);
		m_Btn_Stage1_Mode.SetWindowText("MANUAL");
	}	
}



void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage2Init()
{
	if (CMgrStatus::GetMgr()->GetEqAutoMode(1) == FALSE)
	{
		if (AfxMessageBox("Eq Initialize?", MB_YESNO) == IDYES)
		{
			CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

			if (NULL == pDlg)
				return;

			pDlg->ClearSysAlarmHisotry(1);

			Init_Stage(1);

			CMgrStatus::GetMgr()->SetEqInitFlag(1, TRUE);

		}
	}
	else
	{
		if (CMgrDio::GetMgr()->GetConnect() != TRUE)
		{
			AfxMessageBox("Please check the connection status of IO Board");
			return;
		}



		if (AfxMessageBox("Eq Initialize?", MB_YESNO) == IDYES)
		{
			CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

			if (NULL == pDlg)
				return;

			pDlg->ClearSysAlarmHisotry(1);

			CMgrStatus::GetMgr()->SetEqp_RunMode(1, EQP_RUN_MODE_INIT);

			CMgrStatus::GetMgr()->SetEqStartFlag(1, TRUE);
			Init_Stage(1);
		}
	}
}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Init()
{	
	if (CMgrStatus::GetMgr()->GetEqAutoMode(0) == FALSE)
	{
		if (AfxMessageBox("Eq Initialize?", MB_YESNO) == IDYES)
		{
			CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

			if (NULL == pDlg)
				return;

			pDlg->ClearSysAlarmHisotry(0);

			Init_Stage(0);		
			
			CMgrStatus::GetMgr()->SetEqInitFlag(0, TRUE);
		}
	}
	else
	{

		if (CMgrDio::GetMgr()->GetConnect() != TRUE)
		{
			AfxMessageBox("Please check the connection status of IO Board");
			return;
		}

		if (AfxMessageBox("Eq Initialize?", MB_YESNO) == IDYES)
		{
			CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

			if (NULL == pDlg)
				return;

			pDlg->ClearSysAlarmHisotry(0);

			CMgrStatus::GetMgr()->SetEqp_RunMode(0, EQP_RUN_MODE_INIT);

			CMgrStatus::GetMgr()->SetEqStartFlag(0, TRUE);
			Init_Stage(0);
		}
	}
	
}

void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1Mode2()
{
	if (CMgrConfig::GetMgr()->m_bPLCControlUse == true)
	{
		return;
	}

	if (CMgrStatus::GetMgr()->GetEqAutoMode(1) == FALSE)
	{
		if (CMgrStatus::GetMgr()->GetEqInitFlag(1) != true)
		{
			AfxMessageBox("Please proceed with equipment initialization (Stage 2)");
			return;
		}
		CMgrStatus::GetMgr()->SetEqAutoMode(1, true);
		CMgrStatus::GetMgr()->SetEqp_RunMode(1, EQP_RUN_MODE_AUTO);
		m_Btn_Stage2_Mode.SetWindowText("AUTO");
	}
	else
	{
		CMgrStatus::GetMgr()->SetEqAutoMode(1, FALSE);
		CMgrStatus::GetMgr()->SetEqp_RunMode(1, EQP_RUN_MODE_MANUAL);
		m_Btn_Stage2_Mode.SetWindowText("MANUAL");
	}
}




void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1DataSwitching()
{
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	if (bSubStage1Use != true)
	{
		return;
	}
	if (Get_UI_Data_Switching(0) == FALSE)
	{
		Set_UI_Data_Switching(0, true);
	}
	else
	{
		Set_UI_Data_Switching(0, FALSE);
	}
	UI_Update_Switching(0);


}


void CDlgViewEqpChannelStatus::OnBnClickedBtnViewEqpStage1DataSwitching2()
{
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	if (bSubStage2Use != true)
	{
		return;
	}

	if (Get_UI_Data_Switching(1) == FALSE)
	{
		Set_UI_Data_Switching(1, true);
	}
	else
	{
		Set_UI_Data_Switching(1, FALSE);
	}
	UI_Update_Switching(1);
}

