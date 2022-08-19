﻿// CDlgViewIoManual.cpp: 구현 파일
//

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewIoManual.h"
#include "afxdialogex.h"

#include "MgrConfig.h"
#include "MgrDio.h"
#include "MgrStatus.h"

#define	TIMER_SET_1S						1000
#define STEP_UI_TIMER 21000

#define EQP_STAGE_INDEX_S1 = 0

// CDlgViewIoManual 대화 상자 

IMPLEMENT_DYNAMIC(CDlgViewIoManual, CDialogEx)

CDlgViewIoManual::CDlgViewIoManual(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_IO_MANUAL, pParent)
{
	 
}

CDlgViewIoManual::~CDlgViewIoManual()
{
}

void CDlgViewIoManual::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	// ----------------------------------------------------------------------------
	if (pMgr->GetDioIniName() == "DioConfig_PreCharge")
	{
		for (int nInputControlCount = 0; nInputControlCount < PRECHARGE_CONTROL_INPUT_COUNT; nInputControlCount++)
		{
			DDX_Control(pDX, IDC_MOXAIO_PRECHARGE_INPUT1 + nInputControlCount, m_Control_PreCharge_Static_InputIO[nInputControlCount]);

		}
		for (int nOutputControlCount = 0; nOutputControlCount < PRECHARGE_CONTROL_OUTPUT_COUNT; nOutputControlCount++)
		{
			DDX_Control(pDX, IDC_MOXAIO_PRECHARGE_OUTPUT1 + nOutputControlCount, m_Control_PreCharge_Button_OutputIO[nOutputControlCount]);
		}
	}
	if (pMgr->GetDioIniName() == "DioConfig_DCIR")
	{
		for (int nInputControlCount = 0; nInputControlCount < DCIR_CONTROL_INPUT_COUNT; nInputControlCount++)
		{
			DDX_Control(pDX, IDC_MOXAIO_DCIR_INPUT1 + nInputControlCount, m_Control_DCIR_Static_InputIO[nInputControlCount]);

		}
		for (int nOutputControlCount = 0; nOutputControlCount < DCIR_CONTROL_OUTPUT_COUNT; nOutputControlCount++)
		{
			DDX_Control(pDX, IDC_MOXAIO_DCIR_OUTPUT1 + nOutputControlCount, m_Control_DCIR_Button_OutputIO[nOutputControlCount]);
		}
	}
	
}



BEGIN_MESSAGE_MAP(CDlgViewIoManual, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()	


	ON_BN_CLICKED(IDC_MOXA3_OUT1, &CDlgViewIoManual::OnBnClickedMoxa3Out1)
	ON_BN_CLICKED(IDC_MOXA3_OUT2, &CDlgViewIoManual::OnBnClickedMoxa3Out2)
	ON_BN_CLICKED(IDC_MOXA3_OUT3, &CDlgViewIoManual::OnBnClickedMoxa3Out3)
	ON_BN_CLICKED(IDC_MOXA3_OUT4, &CDlgViewIoManual::OnBnClickedMoxa3Out4)
	ON_BN_CLICKED(IDC_MOXA3_OUT5, &CDlgViewIoManual::OnBnClickedMoxa3Out5)
	ON_BN_CLICKED(IDC_MOXA3_OUT6, &CDlgViewIoManual::OnBnClickedMoxa3Out6)
	ON_BN_CLICKED(IDC_MOXA3_OUT7, &CDlgViewIoManual::OnBnClickedMoxa3Out7)
	ON_BN_CLICKED(IDC_MOXA3_OUT8, &CDlgViewIoManual::OnBnClickedMoxa3Out8)

	ON_BN_CLICKED(IDC_MOXA_CONNECT, &CDlgViewIoManual::OnBnClickedMoxaConnect)

	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT9, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput9)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT10, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput10)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT11, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput11)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT12, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput12)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT1, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput1)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT2, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput2)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT3, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput3)
	ON_BN_CLICKED(IDC_MOXAIO_PRECHARGE_OUTPUT4, &CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput4)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT1, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput1)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT2, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput2)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT3, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput3)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT4, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput4)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT5, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput5)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT6, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput6)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT7, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput7)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT8, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput8)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT9, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput9)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT10, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput10)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT17, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput17)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT18, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput18)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT19, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput19)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT20, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput20)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT21, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput21)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT22, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput22)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT23, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput23)
	ON_BN_CLICKED(IDC_MOXAIO_DCIR_OUTPUT24, &CDlgViewIoManual::OnBnClickedMoxaioDcirOutput24)
	ON_STN_CLICKED(IDC_MOXAIO_DCIR_INPUT26, &CDlgViewIoManual::OnStnClickedMoxaioDcirInput26)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

// CDlgViewIoManual 메시지 처리기

void CDlgViewIoManual::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (TRUE == bShow)
	{
		KillTimer(STEP_UI_TIMER);
		SetTimer(STEP_UI_TIMER, TIMER_SET_1S / 10, NULL);
	}
	else
	{
		KillTimer(STEP_UI_TIMER);
	}
}

HBRUSH CDlgViewIoManual::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CDlgViewIoManual::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.



	if (STEP_UI_TIMER == nIDEvent)
	{
		UI_Update_IO();

		//UI_Step_Stage2_Status();
		//UI_Step_Stage1_Status();
	}
	__super::OnTimer(nIDEvent);
}

void CDlgViewIoManual::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	KillTimer(STEP_UI_TIMER);

	CDialogEx::OnClose();
}

void CDlgViewIoManual::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------

BOOL CDlgViewIoManual::Create(CWnd * pParentWnd)
{
	//return 0;
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgViewIoManual::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow();

	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgViewIoManual::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgViewIoManual::Initialize()
{
	InitializeVariables();

	InitializeControls();

	InitializeTimers();
}

void CDlgViewIoManual::InitializeVariables()
{
	CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();
	
}

void CDlgViewIoManual::InitializeControls()
{
	UI_SetEQID();
	

	// --------------------------------------------------------------------------
		

}

void CDlgViewIoManual::InitializeTimers()
{

}

// -----------------------------------------------------------------------------

void CDlgViewIoManual::SetManualOperationButton(int nEqpStageIndexNo, bool usableValue)
{
	
}



//HeartBeat PC
void CDlgViewIoManual::OnBnClickedMoxa3Out1()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 8);
	pMgrDio->SetMoxa_PC_HeartBit(!bIo);
}

//Ready PC
void CDlgViewIoManual::OnBnClickedMoxa3Out2()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 9);
	pMgrDio->SetMoxa_PC_Ready(!bIo);
}

//Busy PC
void CDlgViewIoManual::OnBnClickedMoxa3Out3()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 10);
	pMgrDio->SetMoxa_PC_Busy(!bIo);
}

//WorkComplete Req
void CDlgViewIoManual::OnBnClickedMoxa3Out4()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 11);
	pMgrDio->SetMoxa_PC_WorkComplete(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxa3Out5()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 12);
	pMgrDio->SetIO(0, 12, !bIo);

	pMgrDio->MoxaIoWrite(0);
}


void CDlgViewIoManual::OnBnClickedMoxa3Out6()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 13);
	pMgrDio->SetIO(0, 13, !bIo);

	pMgrDio->MoxaIoWrite(0);
}


void CDlgViewIoManual::OnBnClickedMoxa3Out7()
{

	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 14);
	pMgrDio->SetIO(0, 14, !bIo);

	pMgrDio->MoxaIoWrite(0);
}


void CDlgViewIoManual::OnBnClickedMoxa3Out8()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(0, 15);
	pMgrDio->SetIO(0, 15, !bIo);

	pMgrDio->MoxaIoWrite(0);

}


void CDlgViewIoManual::OnBnClickedMoxaConnect()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	pMgrDio->MoxaIOConnect();
}

static int nInputIOCh = 0;
void CDlgViewIoManual::UI_SetEQID()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	int nIOCount = CMgrDio::GetMgr()->GetIOTotalCount();
	int nFirstInputIO = CMgrDio::GetMgr()->GetFirstInputDioNo() - 1;
	int nInputCount = CMgrDio::GetMgr()->GetInputDioCount();
	int nFirstOutputIO = CMgrDio::GetMgr()->GetFirstOutputDioNo() - 1;
	int nOutputCount = CMgrDio::GetMgr()->GetOutputDioCount();
	int nChCount = CMgrDio::GetMgr()->GetChCount();
	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	
	if (pMgr == NULL)
		return;
	//1. PreCharge
	if (pMgr->GetDioIniName() == "DioConfig_PreCharge")
	{
		for (int nInputNo = nFirstInputIO; nInputNo < nFirstInputIO + nInputCount; nInputNo++)
		{
		
			nChCount = vecValue[nInputNo]->nChCount;

			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				
				m_Control_PreCharge_Static_InputIO[nInputIOCh + nChNo].SetWindowText(CMgrDio::GetMgr()->m_strIoName[nInputNo][IO_NAME][nChNo]);
				m_Control_PreCharge_Static_InputIO[nInputIOCh + nChNo].Invalidate(false);
				m_Control_PreCharge_Static_InputIO[nInputIOCh + nChNo].ShowWindow(true);
			}
			nInputIOCh = nInputIOCh + nChCount;

		}
		for (int nOutputNo = nFirstOutputIO; nOutputNo < nFirstOutputIO + nOutputCount; nOutputNo++)
		{
			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetWindowText(CMgrDio::GetMgr()->m_strIoName[nOutputNo][IO_NAME][nChNo]);
				m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].Invalidate(false);;
				m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].ShowWindow(true);
			}
		}

		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP16)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP17)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP18)->ShowWindow(true);
	}
	//2. DCIR
	if (pMgr->GetDioIniName() == "DioConfig_DCIR")
	{
		for (int nInputNo = nFirstInputIO; nInputNo < nFirstInputIO + nInputCount; nInputNo++)
		{
			nChCount = vecValue[nInputNo]->nChCount;

			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				m_Control_DCIR_Static_InputIO[nInputIOCh + nChNo].SetWindowText(CMgrDio::GetMgr()->m_strIoName[nInputNo][IO_NAME][nChNo]);
				m_Control_DCIR_Static_InputIO[nInputIOCh + nChNo].Invalidate(false);
				m_Control_DCIR_Static_InputIO[nInputIOCh + nChNo].ShowWindow(true);
			}
			nInputIOCh = nInputIOCh + nChCount;

		}
		for (int nOutputNo = nFirstOutputIO; nOutputNo < nFirstOutputIO + nOutputCount; nOutputNo++)
		{
			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetWindowText(CMgrDio::GetMgr()->m_strIoName[nOutputNo][IO_NAME][nChNo]);
				m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].Invalidate(false);;
				m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].ShowWindow(true);
			}
		}

		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP11)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP12)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP13)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP14)->ShowWindow(true);
		GetDlgItem(IDC_STATIC_VIEW_IO_MANUAL_GRP15)->ShowWindow(true);

	}
}

static int nInputIOStartCh = 0;
void CDlgViewIoManual::UI_Update_IO()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	int nIOCount = CMgrDio::GetMgr()->GetIOTotalCount();
	int nFirstInputIO = CMgrDio::GetMgr()->GetFirstInputDioNo() - 1;
	int nInputCount = CMgrDio::GetMgr()->GetInputDioCount();
	int nFirstOutputIO = CMgrDio::GetMgr()->GetFirstOutputDioNo() - 1;
	int nOutputCount = CMgrDio::GetMgr()->GetOutputDioCount();
	int nChCount = CMgrDio::GetMgr()->GetChCount();
	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	nInputIOStartCh = 0;
	bool bIo = false;
	//1. PreCharge
	if (pMgr->GetDioIniName() == "DioConfig_PreCharge")
	{
	//INPUT IO
		//0,1,2
					
		for (int nInputNo = nFirstInputIO; nInputNo < nFirstInputIO + nInputCount; nInputNo++)
		{
			nChCount = vecValue[nInputNo]->nChCount;

			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				bIo = pMgrDio->GetIOStatus(nInputNo, nChNo);
				if (bIo)
				{
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetBkColor(COLOR_GREEN, COLOR_GREEN);
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetFont("Verdana", 11, 1200);
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetTextColor(ColorWhite);
				}
				else {
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetBkColor(COLOR_WAIT_BTN, COLOR_WAIT_BTN);
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetFont("Verdana", 11, 1200);
					m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].SetTextColor(ColorBlack);
				}
				m_Control_PreCharge_Static_InputIO[nInputIOStartCh + nChNo].Invalidate(false);
			}
			nInputIOStartCh = nInputIOStartCh + nChCount;
		}
		for (int nOutputNo = nFirstOutputIO; nOutputNo < nFirstOutputIO + nOutputCount; nOutputNo++)
		{
			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				bIo = pMgrDio->GetIOStatus(nOutputNo -2, nChNo+8);
				if (bIo)
				{
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_GREEN);
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetFont("Verdana", 11, 1200);
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetTextColor(ColorWhite);
				}
				else
				{
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetFont("Verdana", 11, 1200);
					m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetTextColor(ColorBlack);
				}
				m_Control_PreCharge_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].Invalidate(false);;
			}
		}
	}
	//2. DCIR
	if (pMgr->GetDioIniName() == "DioConfig_DCIR")
	{
		for (int nInputNo = nFirstInputIO; nInputNo < nFirstInputIO + nInputCount; nInputNo++)
		{
			nChCount = vecValue[nInputNo]->nChCount;

			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				bIo = pMgrDio->GetIOStatus(nInputNo, nChNo);
				if (bIo)
				{
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetBkColor(COLOR_GREEN, COLOR_GREEN);
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetFont("Verdana", 11, 1200);
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetTextColor(ColorWhite);
				}
				else {
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetBkColor(COLOR_WAIT_BTN, COLOR_WAIT_BTN);
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetFont("Verdana", 11, 1200);
					m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].SetTextColor(ColorBlack);
				}
				m_Control_DCIR_Static_InputIO[nInputIOStartCh + nChNo].Invalidate(false);
			}
			nInputIOStartCh = nInputIOStartCh + nChCount;
		}
		for (int nOutputNo = nFirstOutputIO; nOutputNo < nFirstOutputIO + nOutputCount; nOutputNo++)
		{
			for (int nChNo = 0; nChNo < nChCount; nChNo++)
			{
				bIo = pMgrDio->GetIOStatus(nOutputNo - 3, nChNo + 8);
				if (bIo)
				{
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_GREEN);
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetFont("Verdana", 11, 1200);
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetTextColor(ColorWhite);
				}
				else
				{
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetFont("Verdana", 11, 1200);
					m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].SetTextColor(ColorBlack);
				}
				m_Control_DCIR_Button_OutputIO[nChNo + ((nOutputNo - nFirstOutputIO)*nChCount)].Invalidate(false);;
			}
		}
	}

}



void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput9()
{
	
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput10()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(2, 9);
	pMgrDio->SetMoxa_PowerOffLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput11()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(2, 10);
	pMgrDio->SetMoxa_PowerResetLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput12()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(2, 11);
	pMgrDio->SetMoxa_PowerOnLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput1()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(1, 8);
	pMgrDio->SetMoxa_PC_HeartBit(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput2()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(1, 9);
	pMgrDio->SetMoxa_PC_Ready(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput3()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(1, 10);
	pMgrDio->SetMoxa_PC_Busy(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioPrechargeOutput4()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(1, 11);
	pMgrDio->SetMoxa_PC_WorkComplete(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput1()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Pusher_UpDown(false);

}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput2()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Pusher_UpDown(true);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput3()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Probe_UpDown(false);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput4()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Probe_UpDown(true);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput5()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetJobChangeCylinder(0, false);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput6()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetJobChangeCylinder(0, true);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput7()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Tray_Centering_ForBack(false);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput8()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Tray_Centering_ForBack(true);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput9()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Tray_Clamp_ForBack(false);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput10()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetMoxa_DCIR_Tray_Clamp_ForBack(true);
}






void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput17()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 8);
	pMgrDio->SetMoxa_PowerOnLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput18()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 9);
	pMgrDio->SetMoxa_PowerOffLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput19()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 10);
	pMgrDio->SetMoxa_PowerResetLamp(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput20()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 11);
	pMgrDio->SetMoxa_TowerLamp_Red(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput21()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 12);
	pMgrDio->SetMoxa_TowerLamp_YELLOW(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput22()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bIo = false;
	bIo = pMgrDio->GetIOStatus(4, 13);
	pMgrDio->SetMoxa_TowerLamp_GREEN(!bIo);
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput23()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewIoManual::OnBnClickedMoxaioDcirOutput24()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewIoManual::OnStnClickedMoxaioDcirInput26()
{
	// TODO: Add your control notification handler code here
}
