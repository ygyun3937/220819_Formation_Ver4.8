// CDlgViewEqpLog.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewEqpLog.h"
#include "afxdialogex.h"


// CDlgViewEqpLog 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewEqpLog, CDialogEx)
CDlgViewEqpLog::CDlgViewEqpLog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_EQP_LOG, pParent)
{

	for (int nIndex = 0; nIndex < MAX_LOG_DATA; nIndex++)
	{
		m_strLog[nIndex] = "";
	}

}

CDlgViewEqpLog::~CDlgViewEqpLog()
{
}

void CDlgViewEqpLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTROL, m_ListControl_Log);
}


BEGIN_MESSAGE_MAP(CDlgViewEqpLog, CDialogEx)

	ON_BN_CLICKED(IDC_BTN_LOG_LOAD, &CDlgViewEqpLog::OnBnClickedBtnLogLoad)
	ON_BN_CLICKED(IDC_BTN_LOGLISTUP, &CDlgViewEqpLog::OnBnClickedBtnLoglistup)
END_MESSAGE_MAP()


// CDlgViewEqpLog 메시지 처리기


BOOL CDlgViewEqpLog::Create(CWnd * pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewEqpLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ListBoxFont.CreatePointFont(80, "굴림");
	m_ListControl_Log.SetFont(&m_ListBoxFont);
	
	CRect rt;

	m_ListControl_Log.GetWindowRect(&rt);
	m_ListControl_Log.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_ListControl_Log.InsertColumn(0, TEXT("floor - Unit"), LVCFMT_CENTER, rt.Width()/5);
	m_ListControl_Log.InsertColumn(1, TEXT("Date/Time"), LVCFMT_CENTER, rt.Width() / 5);
	m_ListControl_Log.InsertColumn(2, TEXT("Kind"), LVCFMT_CENTER, rt.Width() / 5);
	m_ListControl_Log.InsertColumn(3, TEXT("Contents"), LVCFMT_CENTER, rt.Width() / 5);
	m_ListControl_Log.InsertColumn(4, TEXT("Mode"), LVCFMT_CENTER, rt.Width() / 5);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}





void CDlgViewEqpLog::OnBnClickedBtnLogLoad()
{
	char m_pszPathname[MAX_PATH];
	CString FileName;
	CString Message;

	//File 설정
	//D:\SystemData\Log\StageMaster\LOG_ALIGN
	static char BASED_CODE szFilter[] = "Files (*.csv)|*.csv|All Files (*.*)|*.*||";
	CFileDialog *sourcefiledialog;

	UpdateData(true);

	//sourcefiledialog = new CFileDialog(TRUE, ".txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	sourcefiledialog = new CFileDialog(TRUE, ".csv", "D:\\SystemData\\Log\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (sourcefiledialog->DoModal() != IDOK) return;

	FileName = sourcefiledialog->GetPathName();

	//

	Invalidate();


	CStdioFile File;

	if (File.Open((LPCTSTR)(FileName), CFile::modeRead | CFile::typeText))
	{
		CString line, str1, str2;
		int nindex;

		nindex = 0;
		m_ListControl_Log.DeleteAllItems();

		while (NULL != File.ReadString(line))
		{
			//Data Format과 길이가 안맞는 경우
			if (line.GetLength() <= 0) /*|| line.GetLength() > MAX_LOG_DATA)*/
				continue;

			m_ListControl_Log.InsertItem(nindex,"");
			for (int i = 0; i < MAX_LOG_DATA; i++)
			{
				AfxExtractSubString(m_strLog[i], line, i, ',');
				m_ListControl_Log.SetItemText(nindex, i, m_strLog[i]);
			}

			line.Remove('\n');
			nindex++;
		}
		
		File.Close();
	}
	else
	{
		return;
	}

}



BOOL CDlgViewEqpLog::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


void CDlgViewEqpLog::OnBnClickedBtnLoglistup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
