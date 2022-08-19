// DlgEQMode.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgEQMode.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "MgrConfig.h"
#include "MgrPCan.h"
#include "DlgMainTitle.h"

// CDlgEQMode ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgEQMode, CDialogEx)

CDlgEQMode::CDlgEQMode(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEQMode::IDD, pParent)
	, m_nEQMode(0)
{

}

CDlgEQMode::~CDlgEQMode()
{

}

void CDlgEQMode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RDO_EQ_MODE_SERIAL, m_nEQMode);
	DDX_Control(pDX, IDC_CHECK_CVMODE, m_check_CVMode);			//kw--- check �ڽ� ��������� ���� �߰�
	DDX_Control(pDX, IDC_CHECK_CVMODE2, m_check_CVMode2);
}


BEGIN_MESSAGE_MAP(CDlgEQMode, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_EQ_MODE_SAVE, &CDlgEQMode::OnBnClickedBtnEqModeSave)
	ON_BN_CLICKED(IDC_BTN_EQ_MODE_EXIT, &CDlgEQMode::OnBnClickedBtnEqModeExit)
	ON_BN_CLICKED(IDC_RDO_EQ_MODE_SERIAL, &CDlgEQMode::OnBnClickedRdoEqModeSerial)
	ON_BN_CLICKED(IDC_RDO_EQ_MODE_PARALLEL, &CDlgEQMode::OnBnClickedRdoEqModeParallel)
	ON_BN_CLICKED(IDC_CHECK_CVMODE, &CDlgEQMode::OnBnClickedCVMode)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_CVMODE2, &CDlgEQMode::OnBnClickedCheckCvmode2)
END_MESSAGE_MAP()


// CDlgEQMode �޽��� ó�����Դϴ�.


BOOL CDlgEQMode::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgEQMode::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgEQMode::OnInitDialog()
{
	__super::OnInitDialog();

	CenterWindow();

	InitStatic();

	InitButton();

	LoadEQMode();
	m_bCVMode = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CDlgEQMode::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	pDC->GetClipBox(&rt);
	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

HBRUSH CDlgEQMode::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}


	return hbr;
}

void CDlgEQMode::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	for (int i = 0; i < EQModeButtonMaxCount; ++i)
	{
		m_btnEQMode[i].SetBkDC(&dc);
	}
}


void	CDlgEQMode::InitStatic()
{
	LPCTSTR lpszName[] = { _T("Save"), _T("Exit") };

	for (int i = 0; i < EQModeStaticMaxCount; ++i)
	{
		CST_DEFAULT(m_ctrlEQMode[i], IDC_STATIC_EQ_MODE_SAVE + i, this, 14, 0, TRUE, ColorWhite, ColorBlack, lpszName[i]);
	}
}

void	CDlgEQMode::InitButton()
{
	LPCTSTR lpszName[] = { _T("\\res\\Serial.png"), _T("\\res\\Parallel.png"), _T("\\res\\CVMode.png"), _T("\\res\\PowerMap.png"), _T("\\res\\Apply.png"), _T("\\res\\Close.png"), };

	for (int i = 0; i < EQModeButtonMaxCount; ++i)
	{
		SKINBTN(m_btnEQMode[i], IDC_BTN_EQ_MODE_SERIAL + i, this, GetExePath() + lpszName[i], GetExePath() + lpszName[i]);
	}
}

void	CDlgEQMode::LoadEQMode()
{
	m_nEQMode = CMgrConfig::GetMgr()->GetEQMode();

	switch (m_nEQMode)
	{
	case EqModeSerial: ((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_PARALLEL))->SetCheck(FALSE);	((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_SERIAL))->SetCheck(TRUE); break;
	case EQModeParallel: ((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_PARALLEL))->SetCheck(TRUE);		((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_SERIAL))->SetCheck(FALSE); break;
	}

	UpdateData(FALSE);
}

void CDlgEQMode::OnBnClickedBtnEqModeSave()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (TRUE == pChannel->GetScheduleStart())
		{
#ifdef _LANG_KOR
			AfxMessageBox(_T("Schedule ���� �߿��� ���� �� �� �����ϴ�"));
#endif
#ifdef _LANG_ENG
			AfxMessageBox(_T("EQ Mode Can Not Change During Schedule Started"));
#endif
			return;
		}
	}

	UpdateData(TRUE);

	CMgrConfig::GetMgr()->SetEQMode(m_nEQMode);
	CMgrConfig::GetMgr()->SaveCommon();

	//���⼭ Mode �� ���� Master, Slave ���� ����
	Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		int nChannelNumber = pChannel->GetChannelNumber();

		if (EQModeParallel == m_nEQMode)
		{
			if (nChannelNumber % 2 == 0)
			{
				pChannel->SetParallelObj(ParallelMaster);
				pChannel->SetMasterChannel(nChannelNumber);
				pChannel->SetSlaveChannel(nChannelNumber + 1);
				//pChannel->SendCommand(0x91);
			}
			else
			{
				pChannel->SetParallelObj(ParallelSlave);
				pChannel->SetMasterChannel(nChannelNumber - 1);
				pChannel->SetSlaveChannel(nChannelNumber);
				//pChannel->SendCommand(0x91);
			}
		}
		else
		{
			pChannel->SetParallelObj(ParallelMaster);
			pChannel->SetMasterChannel(nChannelNumber);
			pChannel->SetSlaveChannel(nChannelNumber);
			pChannel->SendCommand(0x92);
		}
	}

	//kw--------- �߰� -------------//
//	CDlgMainTitle::GetMgr()->CVModeIconChange(m_check_CVMode.GetCheck());		//kw--- title�� CV ��� ������ ���� �߰�

	AfxMessageBox(_T("���� �Ϸ�"));

	ShowWindow(SW_HIDE);
	//kw---------------------------//
}

void CDlgEQMode::OnBnClickedBtnEqModeExit()
{
	ShowWindow(SW_HIDE);
}

void CDlgEQMode::OnBnClickedRdoEqModeSerial()
{
	((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_PARALLEL))->SetCheck(FALSE);

	m_nEQMode = EqModeSerial;

	UpdateData(FALSE);
}


void CDlgEQMode::OnBnClickedRdoEqModeParallel()
{
	((CButton*)GetDlgItem(IDC_RDO_EQ_MODE_SERIAL))->SetCheck(FALSE);

	m_nEQMode = EQModeParallel;

	UpdateData(FALSE);
}

void CDlgEQMode::OnBnClickedCVMode()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_CVMODE))->GetCheck() == FALSE)
	{
		m_bCVMode = FALSE;
		CMgrChannel* pMgr = CMgrChannel::GetMgr();

		CChannel* pChannel = pMgr->GetChannel(0);
		if (NULL == pChannel)
			return;

		if (TRUE == pChannel->GetScheduleStart())
		{
#ifdef _LANG_KOR
			AfxMessageBox(_T("Schedule ���� �߿��� ���� �� �� �����ϴ�"));
#endif
#ifdef _LANG_ENG
			AfxMessageBox(_T("EQ Mode Can Not Change During Schedule Started"));
#endif
			return;
		}
		CPCanCom* pCan = CMgrPCan::GetMgr()->GetAt(pChannel->GetChannelNumber());

		if (pCan == NULL)
			return;
		/*pCan->m_fnVoltSenserOnOff(pChannel->GetChannelNumber(), m_bCVMode);*/
		pChannel->SetCVMode(m_bCVMode);
		pChannel->SetCVMode2(0);
		pCan->m_fnCVmodeChange(pChannel->GetChannelNumber(), m_bCVMode);
	}
	else if (((CButton*)GetDlgItem(IDC_CHECK_CVMODE))->GetCheck() == TRUE)
	{
		m_bCVMode = TRUE;
		CMgrChannel* pMgr = CMgrChannel::GetMgr();

		CChannel* pChannel = pMgr->GetChannel(0);
		if (NULL == pChannel)
			return;

		if (TRUE == pChannel->GetScheduleStart())
		{
#ifdef _LANG_KOR
			AfxMessageBox(_T("Schedule ���� �߿��� ���� �� �� �����ϴ�"));
#endif
#ifdef _LANG_ENG
			AfxMessageBox(_T("EQ Mode Can Not Change During Schedule Started"));
#endif
			return;
		}
		CPCanCom* pCan = CMgrPCan::GetMgr()->GetAt(pChannel->GetChannelNumber());

		if (pCan == NULL)
			return;
		pChannel->SetCVMode(m_bCVMode);
		pChannel->SetCVMode2(1);
		pCan->m_fnCVmodeChange(pChannel->GetChannelNumber(), m_bCVMode);
	}


	UpdateData(FALSE);
}

void CDlgEQMode::OnBnClickedCheckCvmode2()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_CVMODE2))->GetCheck() == FALSE)
	{
		m_bCVMode = FALSE;
		CMgrChannel* pMgr = CMgrChannel::GetMgr();

		CChannel* pChannel = pMgr->GetChannel(1);
		if (NULL == pChannel)
			return;

		if (TRUE == pChannel->GetScheduleStart())
		{
#ifdef _LANG_KOR
			AfxMessageBox(_T("Schedule ���� �߿��� ���� �� �� �����ϴ�"));
#endif
#ifdef _LANG_ENG
			AfxMessageBox(_T("EQ Mode Can Not Change During Schedule Started"));
#endif
			return;
		}
		CPCanCom* pCan = CMgrPCan::GetMgr()->GetAt(pChannel->GetChannelNumber());

		if (pCan == NULL)
			return;
		/*pCan->m_fnVoltSenserOnOff(pChannel->GetChannelNumber(), m_bCVMode);*/
		pChannel->SetCVMode(m_bCVMode);
		pChannel->SetCVMode2(0);
		pCan->m_fnCVmodeChange(pChannel->GetChannelNumber(), m_bCVMode);
	}
	else if (((CButton*)GetDlgItem(IDC_CHECK_CVMODE2))->GetCheck() == TRUE)
	{
		m_bCVMode = TRUE;
		CMgrChannel* pMgr = CMgrChannel::GetMgr();

		CChannel* pChannel = pMgr->GetChannel(1);
		if (NULL == pChannel)
			return;

		if (TRUE == pChannel->GetScheduleStart())
		{
#ifdef _LANG_KOR
			AfxMessageBox(_T("Schedule ���� �߿��� ���� �� �� �����ϴ�"));
#endif
#ifdef _LANG_ENG
			AfxMessageBox(_T("EQ Mode Can Not Change During Schedule Started"));
#endif
			return;
		}
		CPCanCom* pCan = CMgrPCan::GetMgr()->GetAt(pChannel->GetChannelNumber());

		if (pCan == NULL)
			return;
		pChannel->SetCVMode(m_bCVMode);
		pChannel->SetCVMode2(1);
		pCan->m_fnCVmodeChange(pChannel->GetChannelNumber(), m_bCVMode);
	}


	UpdateData(FALSE);
}

void CDlgEQMode::SetCVModeCheckButton(int nCh, BOOL bOn)
{
	if (nCh == 0)
	{
		((CButton*)GetDlgItem(IDC_CHECK_CVMODE))->SetCheck(bOn);
	}
	else if (nCh == 1)
	{
		((CButton*)GetDlgItem(IDC_CHECK_CVMODE2))->SetCheck(bOn);
	}
}
