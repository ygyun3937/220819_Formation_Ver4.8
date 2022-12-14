// CDlgViewLogIn.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewLogIn.h"
#include "afxdialogex.h"
#include "CMgrAccount.h"
#include "CDlgEditUser.h"

#include "DlgMainTitle.h"
#include "CDlgMainTitleFormation.h"
// CDlgViewLogIn 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewLogIn, CDialogEx)

CDlgViewLogIn::CDlgViewLogIn(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_LOGIN, pParent)
	, m_strLoginID(_T(""))
	, m_strLoginPW(_T(""))
{

}

CDlgViewLogIn::~CDlgViewLogIn()
{
}

void CDlgViewLogIn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strLoginID);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strLoginPW);
	DDX_Control(pDX, IDC_LIST_USER, m_List_User);
	DDX_Control(pDX, IDC_BTN_LOGIN_LOGIN, m_BtnLogin);
	DDX_Control(pDX, IDC_BTN_LOGIN_CLOSE, m_BtnLogOut);
	DDX_Control(pDX, IDC_BTN_LOGIN_ADD, m_BtnUserAdd);
}


BEGIN_MESSAGE_MAP(CDlgViewLogIn, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_LOGIN_ADD, &CDlgViewLogIn::OnBnClickedBtnLoginAdd)
	ON_BN_CLICKED(IDC_BTN_LOGIN_LOGIN, &CDlgViewLogIn::OnBnClickedBtnLoginLogin)
	ON_BN_CLICKED(IDC_BTN_LOGIN_CLOSE, &CDlgViewLogIn::OnBnClickedBtnLoginClose)
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


// CDlgViewLogIn 메시지 처리기
BOOL CDlgViewLogIn::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgViewLogIn::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rt;

	InitStatic();

	m_ListBoxFont.CreatePointFont(80, "굴림");
	m_List_User.SetFont(&m_ListBoxFont);


	m_List_User.GetWindowRect(&rt);
	m_List_User.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_List_User.InsertColumn(0, TEXT("User ID"), LVCFMT_CENTER, rt.Width() / 2);
	m_List_User.InsertColumn(1, TEXT("User Level"), LVCFMT_CENTER, rt.Width() / 2);


	SetUserList();
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgViewLogIn::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			ShowWindow(SW_HIDE);

			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

void CDlgViewLogIn::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// Add By JSK - 2020.06.09
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (TRUE == bShow)
	{
		if (pMgr)
		{
			if (pMgr->IsLogin())
			{
				m_BtnLogin.EnableWindow(false);
				m_BtnLogOut.EnableWindow(true);
			}
			else
			{
				m_BtnLogin.EnableWindow(true);
				m_BtnLogOut.EnableWindow(false);
			}
			m_strLoginID = pMgr->GetLoginID();
			m_strLoginPW = pMgr->GetLoginPW();

			UpdateData(FALSE);
		}
	}
	else
	{
		if (pMgr)
		{
			//메인화면 로그인 표시
			//CDlgMainTitle* pDlgMainTitle = CDlgMainTitle::GetMgr();
			//pDlgMainTitle->SetLoginStatus(pMgr->GetLoginedAccount()); //lyw - 로그인 상태 표시

			CDlgMainTitleFormation* pDlgMainTitleFormation = CDlgMainTitleFormation::GetMgr();
			pDlgMainTitleFormation->SetLoginStatus(pMgr->GetLoginedAccount());
		}
	}
}


void CDlgViewLogIn::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CDlgViewLogIn::InitStatic()
{
	LPCTSTR lpszText[] = { "Login Info Input Window", "Worker ID", "Password" };

	for (int i = 0; i < eLogInObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_LOGIN_TITLE + i, this, 16, 0, TRUE, ColorLightGray, ColorWhite, lpszText[i]);
}

void CDlgViewLogIn::OnBnClickedBtnLoginAdd()
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();
	sAccount * lpAccount;
	lpAccount = pMgr->GetLoginedAccount();

	if (lpAccount == nullptr)
	{
		AfxMessageBox("You can add workers only at Admin User Level.");
		return;
	}
	if (_tcsicmp(lpAccount->szUserLevel, "Admin") != 0)
	{
		AfxMessageBox("You can add workers only at Admin User Level.");

		return;
	}
	CDlgEditUser* lpDlg = CDlgEditUser::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void CDlgViewLogIn::OnBnClickedBtnLoginLogin()
{
	UpdateData(TRUE);

	if (m_strLoginID.IsEmpty())
	{
		AfxMessageBox("Unable to log in because worker ID is blank");

		return;
	}

	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return;

	sAccount* lpAccount = pMgr->LogInAccount(m_strLoginID, m_strLoginPW);

	if (nullptr == lpAccount)
	{
		AfxMessageBox(_T("The worker ID and PW registered in the system are not"));

		return;
	}	
	
	if (pMgr->IsLogin())
	{
		m_BtnLogin.EnableWindow(false);
		m_BtnLogOut.EnableWindow(true);
	}

	m_strLoginID = pMgr->GetLoginID();
	m_strLoginPW = pMgr->GetLoginPW();

	UpdateData(FALSE);

	ShowWindow(SW_HIDE);
}


void CDlgViewLogIn::OnBnClickedBtnLoginClose()
{
	UpdateData(TRUE);

	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return;

	pMgr->Logout();

	m_BtnLogin.EnableWindow(true);
	m_BtnLogOut.EnableWindow(false);

	UpdateData(FALSE);

	ShowWindow(SW_HIDE);
}

void CDlgViewLogIn::SetUserList()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\Users.ini"), (LPCSTR)GetExePath());

	m_List_User.DeleteAllItems();

	int nAccountSize = INIReadInt("UserCount", "Count", strFilePath);

	for (int i = 0; i < nAccountSize; ++i)
	{
		CString strAppName;
		strAppName.Format("USERS%d", i);

		sAccount* lpAccount = new sAccount;
		m_List_User.InsertItem(i, "");

		m_List_User.SetItemText(i, 0, INIReadStr(strAppName, "Name", strFilePath));
		m_List_User.SetItemText(i, 1, INIReadStr(strAppName, "UserLevel", strFilePath));
	}
}


void CDlgViewLogIn::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
