// CDlgViewProgramStatus.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewProgramStatus.h"
#include "afxdialogex.h"


// CDlgViewProgramStatus 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewProgramStatus, CDialogEx)

CDlgViewProgramStatus::CDlgViewProgramStatus(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PROGRAM_STATUS, pParent)
{

}

CDlgViewProgramStatus::~CDlgViewProgramStatus()
{
}

void CDlgViewProgramStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress_Program);
}

BOOL CDlgViewProgramStatus::Create(CWnd * pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewProgramStatus::OnInitDialog()
{
	__super::OnInitDialog();

	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewProgramStatus::Initialize()
{
	//범위 지정
	m_Progress_Program.SetRange(0, 100);
	m_Progress_Program.SetPos(0);


	CenterWindow();
}

void CDlgViewProgramStatus::InitializeControls()
{
}

void CDlgViewProgramStatus::Set_Progress_Pos(int nPos)
{
	m_Progress_Program.SetPos(nPos);
}



BEGIN_MESSAGE_MAP(CDlgViewProgramStatus, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgViewProgramStatus 메시지 처리기


void CDlgViewProgramStatus::OnDestroy()
{
	__super::OnDestroy();
}
