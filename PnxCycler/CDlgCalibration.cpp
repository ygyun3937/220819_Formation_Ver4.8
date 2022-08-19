// CDlgCalibration.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgCalibration.h"
#include "afxdialogex.h"
#include "MgrConfig.h"
#include "CCalibratorVisaProcess.h"


// CDlgCalibration 대화 상자

IMPLEMENT_DYNAMIC(CDlgCalibration, CDialogEx)

CDlgCalibration::CDlgCalibration(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CALIBRATION, pParent)
	, m_Radio(0)
{

}

CDlgCalibration::~CDlgCalibration()
{
}

void CDlgCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_CALMODE_NONE, m_Radio);
}

BOOL CDlgCalibration::Create(CWnd * pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgCalibration::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Init_Control();
	Init_Data();

	return 0;
}

BOOL CDlgCalibration::PreTranslateMessage(MSG * pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
			return TRUE;

		if (VK_ESCAPE == pMsg->wParam)
		{
			ShowWindow(SW_HIDE);

			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}



void CDlgCalibration::Init_Control()
{
	m_Radio = 0;


}

void CDlgCalibration::Init_Data()
{

}


BEGIN_MESSAGE_MAP(CDlgCalibration, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CALMODE_APPLY, &CDlgCalibration::OnBnClickedButtonCalmodeApply)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CALTEST_START, &CDlgCalibration::OnBnClickedButtonCaltestStart)
END_MESSAGE_MAP()


// CDlgCalibration 메시지 처리기


void CDlgCalibration::OnBnClickedButtonCalmodeApply()
{
	UpdateData(TRUE);

	switch (m_Radio)
	{
	case 0:
		CMgrConfig::GetMgr()->SetMeasureMode("None");
		break;
	case 1:
		CMgrConfig::GetMgr()->SetMeasureMode("VoltCalibration");
		break;
	case 2:
		CMgrConfig::GetMgr()->SetMeasureMode("CurrCalibration");
		break;
	}
	UpdateData(FALSE);


	
}


void CDlgCalibration::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgCalibration::OnBnClickedButtonCaltestStart()
{
	CCalibratorVisaProcess* pMgr = CCalibratorVisaProcess::GetMgr();
	CString strDelayTime;
	if (pMgr == NULL)
		return;
	
	GetDlgItemText(IDC_EDIT_DELAY_TIME, strDelayTime);

	if (strDelayTime == "")
	{
		strDelayTime = "0";
	}

	pMgr->Set_DelayTime(atoi(strDelayTime));

	pMgr->Set_MeasureRun(true);

	//case : Auto Calibration Volt Mode 
	if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeAutocalVoltage)
	{
		//CMgrLog::GetMgr()->WriteLogClaibrationVolt(m_nChannelNumber, pCommData);
	}
}
