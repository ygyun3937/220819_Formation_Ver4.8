// DlgTotalSafetyContition.cpp : implementation file
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgTotalSafetyCondition.h"
#include "afxdialogex.h"

#include "NewCellTypes/GridCellCheck.h"

#include "MgrConfig.h"
#include "MgrChannel.h"

#include "DlgAuth.h"

#include <vector>


// CDlgTotalSafetyContition dialog

IMPLEMENT_DYNAMIC(CDlgTotalSafetyCondition, CDialogEx)

CDlgTotalSafetyCondition::CDlgTotalSafetyCondition(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgTotalSafetyCondition::IDD, pParent)
{

}

CDlgTotalSafetyCondition::~CDlgTotalSafetyCondition()
{
}

void CDlgTotalSafetyCondition::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CDlgTotalSafetyCondition, CDialogEx)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_BN_CLICKED(IDOK, &CDlgTotalSafetyCondition::OnBnClickedOk)
END_MESSAGE_MAP()



BOOL CDlgTotalSafetyCondition::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFont font;
	font.CreatePointFont(10, "바탕");
	GetDlgItem(IDC_STATIC_WARRING)->SetFont(&font);

	InitializeGrid();

	return TRUE;
}

void CDlgTotalSafetyCondition::InitializeGrid()
{
	m_Grid.SetRowCount(13);
	m_Grid.SetColumnCount(9);

	DWORD dwTextStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	for (int row = 0; row < m_Grid.GetRowCount(); row++) {
		for (int col = 0; col < m_Grid.GetColumnCount(); col++) {
			m_Grid.SetCellType(row, col, RUNTIME_CLASS(CGridCell));
			m_Grid.SetItemState(row, col, m_Grid.GetItemState(row, col) | GVIS_READONLY);
			m_Grid.SetItemFormat(row, col, dwTextStyle);
		}
	}

	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < m_Grid.GetColumnCount(); col++) {
			m_Grid.SetItemState(row, col, m_Grid.GetItemState(row, col) | GVIS_READONLY);
			m_Grid.SetItemBkColour(row, col, RGB(0x80, 0x80, 0x80));
			m_Grid.SetItemFgColour(row, col, RGB(0xff, 0xff, 0xff));
		}
	}

	m_Grid.MergeCells(CCellRange(0, 0, 1, 0));
	m_Grid.MergeCells(CCellRange(0, 1, 1, 1));
	m_Grid.MergeCells(CCellRange(0, 2, 1, 2));
	m_Grid.MergeCells(CCellRange(0, 3, 0, 4));
	m_Grid.MergeCells(CCellRange(0, 5, 0, 6));
	m_Grid.MergeCells(CCellRange(0, 7, 0, 8));

	m_Grid.SetItemText(0, 0, "No");
	m_Grid.SetItemText(0, 1, "Apply\nCheck");
	m_Grid.SetItemText(0, 2, "Item");
	m_Grid.SetItemText(0, 3, "Standard");
	m_Grid.SetItemText(1, 3, "Value");
	m_Grid.SetItemText(1, 4, "Unit");
	m_Grid.SetItemText(0, 5, "Set");
	m_Grid.SetItemText(1, 5, "Set Value");
	m_Grid.SetItemText(1, 6, "Unit");
	m_Grid.SetItemText(0, 7, "Safety Condition");
	m_Grid.SetItemText(1, 7, "Value");
	m_Grid.SetItemText(1, 8, "Unit");

	m_Grid.SetColumnWidth(0, 40);
	for (int row = 2; row < m_Grid.GetRowCount(); row++) {
		m_Grid.SetItemTextFmt(row, 0, "%d", row - 1);
	}

	m_Grid.SetColumnWidth(1, 40);
	for (int row = 2; row < m_Grid.GetRowCount(); row++) {
		m_Grid.SetItemState(row, 1, m_Grid.GetItemState(row, 1) & ~GVIS_READONLY);
		m_Grid.SetCellType(row, 1, RUNTIME_CLASS(CGridCellCheck));

		CGridCellCheck* cell = (CGridCellCheck*)m_Grid.GetCell(row, 1);
		cell->SetCheck();
	}

	m_Grid.SetColumnWidth(2, 200);
	char* items[] = { "Capacity", "Voltage", "Current", "Test End Time(Add Time)", "No voltage variation", "Voltage variation(CC)",
		"Charge capacity variation", "Discharge capacity variation", "Chamber Max Temp.", "Sample Max Temp.", "SOH" };
	for (int row = 2; row < m_Grid.GetRowCount(); row++) {
		m_Grid.SetItemText(row, 2, items[(row - 2)]);
	}

	char* default[] = { "105", "+/- 0.050", "+/- 50.000", "01:30", "00:03", "+/- 100", "+/- 5", "+/- 5", "+/- 2", "", "60" };
	for (int row = 2; row < m_Grid.GetRowCount(); row++) {
		m_Grid.SetItemText(row, 3, default[(row - 2)]);
		m_Grid.SetItemState(row, 5, m_Grid.GetItemState(row, 1) & ~GVIS_READONLY);
		//m_Grid.SetItemBkColour(row, 5, RGB(0xFF, 0xFF, 0xE0));
		m_Grid.SetItemText(row, 5, default[(row - 2)]);
	}

	char* units[] = { "%", "V", "mA", "HR:MIN", "HR:MIN", "mV", "%", "%", "°C", "°C", "%" };
	for (int row = 2; row < m_Grid.GetRowCount(); row++) {
		m_Grid.SetItemText(row, 4, units[(row - 2)]);
		m_Grid.SetItemText(row, 6, units[(row - 2)]);
	}

	// sepcial set
	m_Grid.SetItemText(2, 4, "mAh");
	m_Grid.SetItemText(2, 8, "mAh");
	m_Grid.SetItemText(12, 8, "mAh");

	// 챔버조건 미사용
	CGridCellCheck* cell = (CGridCellCheck*)m_Grid.GetCell(10, 1);
	cell->SetCheck(FALSE);
	cell->SetState(GVIS_READONLY);
	m_Grid.SetItemFgColour(10, 2, RGB(0xA0, 0xA0, 0xA0));

	// charge capacity variation 기본체크 false
	cell = (CGridCellCheck*)m_Grid.GetCell(8, 1);
	cell->SetCheck(FALSE);
	for (int i = 0; i < 6; i++) {
		m_Grid.SetItemFgColour(8, 2 + i, RGB(0xA0, 0xA0, 0xA0));
		m_Grid.RedrawCell(8, 2 + i);
	}

	// capacity 조건은 체크박스 없애기
	m_Grid.SetCellType(2, 1, RUNTIME_CLASS(CGridCell));
	m_Grid.SetItemState(2, 1, m_Grid.GetItemState(2, 1) | GVIS_READONLY);

	// capacity 용량 입력 가능하게
	m_Grid.SetItemText(2, 3, "0");
	m_Grid.SetItemState(2, 3, m_Grid.GetItemState(2, 3) & ~GVIS_READONLY);
	m_Grid.SetItemBkColour(2, 3, RGB(0, 128, 0));
	m_Grid.SetItemFgColour(2, 3, RGB(255, 255, 255));

	// sample temp
	m_Grid.SetItemBkColour(11, 5, RGB(0, 128, 0));
	m_Grid.SetItemFgColour(11, 5, RGB(255, 255, 255));


	m_Grid.ExpandToFit();
}

// GVN_BEGINLABELEDIT
void CDlgTotalSafetyCondition::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	int row = pItem->iRow;
	if (row == 3 || row == 4 || row == 8 || row == 9) {
		CString text = m_Grid.GetItemText(pItem->iRow, pItem->iColumn);
		text.Replace("+/- ", "");
		m_Grid.SetItemText(pItem->iRow, pItem->iColumn, text);
	}
}

// GVN_ENDLABELEDIT
void CDlgTotalSafetyCondition::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE(_T("End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	int r = pItem->iRow;
	int c = pItem->iColumn;
	int res = 0;

	//yuri 20201218
	if (r == 4 || r == 8 || r == 9) {
		CString text = m_Grid.GetItemText(r, c);
		res = stringAllowed(text, 1);
		if (res == TRUE) {
			m_Grid.SetItemText(r, c, "+/- " + text);
		}
		else
		{
			m_Grid.SetItemText(r, c, "");
		}
	}

	// Capacity 계산
	if ((r == 2 || r == 12) && (c == 3 || c == 5)) {
		CString default = m_Grid.GetItemText(2, 3);
		res = stringAllowed(default, 0);
		if (res == FALSE) m_Grid.SetItemText(r, c, "");

		CString set = m_Grid.GetItemText(2, 5);
		res = stringAllowed(set, 0);
		if (res == FALSE) m_Grid.SetItemText(r, c, "");

		CString setsoh = m_Grid.GetItemText(12, 5);
		res = stringAllowed(setsoh, 0);
		if (res == FALSE) m_Grid.SetItemText(r, c, "");

		m_Grid.SetItemTextFmt(2, 7, "%.1f", atof(default) * (atof(set) / 100.f));
		m_Grid.SetItemTextFmt(12, 7, "%.1f", atof(default) * (atof(setsoh) / 100.f));
		
		m_Grid.RedrawCell(2, 7);
		m_Grid.RedrawCell(12, 7);
	}
}


char CDlgTotalSafetyCondition::stringAllowed(CString stri, int option)
{
	/**/
	TCHAR* tchr;
	CString str = _T("");
	int i, result;

	// 현재 Edit Control 박스에 입력된 문자에 대하여

	for (i = 0; i < stri.GetLength(); i++) {
		// Edit Control의 멤버변수인 CString 문자열 m_strTmp의 i번째 문자열 추출
		str = stri.Mid(i, 1);
		// 문자열을 문자로 변환
		tchr = (TCHAR*)(LPCTSTR)str;

		int asc = 0;

		// 문자를 ASCII 값으로 변환
		asc = __toascii(*tchr);

		// 해당 문자가 알파벳이나 숫자가 아닌 경우
		if (!((asc > 47 && asc < 58) || asc == 46))
		{
			AfxMessageBox(_T("Please check the value."));
			return FALSE;
		}
	}

	return TRUE;
}

void CDlgTotalSafetyCondition::OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("Click on row %d, col %d\n", pItem->iRow, pItem->iColumn);

	if (pItem->iColumn == 1) {
		CGridCellCheck* cell = (CGridCellCheck*)m_Grid.GetCell(pItem->iRow, pItem->iColumn);
		COLORREF fgColor;

		if (cell->GetCheck()) {
			fgColor = RGB(0x00, 0x00, 0x00);
			m_Grid.SetItemState(pItem->iRow, 5, m_Grid.GetItemState(2, 1) & ~GVIS_READONLY);
		}
		else {
			fgColor = RGB(0xA0, 0xA0, 0xA0);
			m_Grid.SetItemState(pItem->iRow, 5, m_Grid.GetItemState(2, 1) | GVIS_READONLY);
		}

		for (int i = 1; i < 6; i++) {
			m_Grid.SetItemFgColour(pItem->iRow, pItem->iColumn + i, fgColor);
			m_Grid.RedrawCell(pItem->iRow, pItem->iColumn + i);
		}
	}
}

void CDlgTotalSafetyCondition::OnBnClickedOk()
{
	//CList<CChannel*, CChannel*>& channelList = CMgrChannel::GetMgr()->GetChannelList();
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;
	CList<CChannel*, CChannel*>& channelList = pMgr->GetChannelList();

	auto pos = channelList.GetHeadPosition();
	BOOL bShowAuthDlg = FALSE;
	CArray<INT> arrayChNum;
	TOTAL_SAFETY totalSafety;

	while (pos) {
		CChannel* pChannel = channelList.GetNext(pos);

		if (NULL == pChannel) {
			continue;
		}

		if (!pChannel->GetSelected()) {
			continue;
		}

		int chNumber = pChannel->GetChannelNumber();
		arrayChNum.Add(chNumber);

		totalSafety.fCapacity = -1;
		totalSafety.fVoltage = -1;
		totalSafety.fCurrent = -1;
		totalSafety.nTestEndTime = 0;
		totalSafety.nNoVoltageVariation = -1;
		totalSafety.fVoltageVariation = -1;
		totalSafety.fChargeCapacityVariation = -1;
		totalSafety.fDischargeCapacityVariation = -1;
		totalSafety.fChamberMaxTemp = -1;
		totalSafety.fSampleMaxTemp = -1;
		totalSafety.fSOH = -1;
		totalSafety.fsohPercent = m_Grid.GetItemText(12, 5);	//yuri xml 통합안전조건 값을 xml파일에 추가
		totalSafety.fCapPercent = m_Grid.GetItemText(2, 5);	//yuri xml 통합안전조건 값을 xml파일에 추가
		CString text;
		CGridCellCheck* cell = NULL;
		for (int i = 3; i < 13; i++) {
			cell = (CGridCellCheck*)m_Grid.GetCell(i, 1);
			
			//totalSafety.checkState[i - 3] = cell->GetCheck(); //yuri xml파일에 통합안전조건 check 상태 기록

			if (cell->GetCheck()) {
				if (i == 10 || i == 8) {
					bShowAuthDlg = TRUE;
				}
			}
			else {
				if (i != 10 && i != 8) {
					bShowAuthDlg = TRUE;
				}
			}
		}

		float f = static_cast<float>(atof(m_Grid.GetItemText(2, 5)));
		totalSafety.fInputCapacity = static_cast<float>(atof(m_Grid.GetItemText(2, 3)));
		totalSafety.fCapacity = static_cast<float>(atof(m_Grid.GetItemText(2, 7)));

		if (fabsf(f - 105.f) > FLT_EPSILON) {
			bShowAuthDlg = TRUE;
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(3, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(3, 5);
			text.Replace("+/- ", "");

			f = static_cast<float>(atof(text));
			totalSafety.fVoltage = f;

			if (fabsf(f - 0.05f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(4, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(4, 5);
			text.Replace("+/- ", "");

			f = static_cast<float>(atof(text));
			totalSafety.fCurrent = f;

			if (fabsf(f - 50.f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(5, 1);
		if (cell->GetCheck()) {
			ULONGLONG d = GetTickCount(m_Grid.GetItemText(5, 5));
			totalSafety.nTestEndTime = d;
			totalSafety.strTestEndTime = m_Grid.GetItemText(5, 5); //yuri xml 통합안전조건 값을 xml파일에 추가

			if (d != 5400000) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(6, 1);
		if (cell->GetCheck()) {
			ULONGLONG d = GetTickCount(m_Grid.GetItemText(6, 5));
			totalSafety.nNoVoltageVariation = d;

			if (d != 180000) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(7, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(7, 5);
			text.Replace("+/- ", "");

			f = static_cast<float>(atof(text));
			totalSafety.fVoltageVariation = f;

			if (fabsf(f - 100.f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(8, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(8, 5);
			text.Replace("+/- ", "");

			f = static_cast<float>(atof(text));
			totalSafety.fChargeCapacityVariation = f;

			if (fabsf(f - 5.f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(9, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(9, 5);
			text.Replace("+/- ", "");

			f = static_cast<float>(atof(text));
			totalSafety.fDischargeCapacityVariation = f;

			if (fabsf(f - 5.f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(10, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(10, 5);
			totalSafety.fChamberMaxTemp = static_cast<float>(atof(text));
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(11, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(11, 5);
			totalSafety.fSampleMaxTemp = static_cast<float>(atof(text));
		}

		cell = (CGridCellCheck*)m_Grid.GetCell(12, 1);
		if (cell->GetCheck()) {
			text = m_Grid.GetItemText(12, 5);

			f = static_cast<float>(atof(text));
			totalSafety.fSOH = static_cast<float>(atof(m_Grid.GetItemText(12, 7)));

			if (fabsf(f - 60.f) > FLT_EPSILON) {
				bShowAuthDlg = TRUE;
			}
		}
	}

	if (bShowAuthDlg) {
		CDlgAuth dlg;
		if (dlg.DoModal() == IDOK) {
			for (int i = 0; i < arrayChNum.GetCount(); i++) {
				CMgrConfig::GetMgr()->SetTotalSafety(arrayChNum[i], totalSafety);
			}
			CDialogEx::OnOK();
		}
		else {
			return;
		}
	}
	else {
		for (int i = 0; i < arrayChNum.GetCount(); i++) {
			CMgrConfig::GetMgr()->SetTotalSafety(arrayChNum[i], totalSafety);
		}
		CDialogEx::OnOK();
	}
}

ULONGLONG CDlgTotalSafetyCondition::GetTickCount(CString text)
{
	CString temp;
	std::vector<CString> vecText;
	int nP = 0;
	while (FALSE != AfxExtractSubString(temp, text, nP++, _T(':')))	{
		vecText.push_back(temp);
	}

	ULONGLONG dwTickHour = atoi(vecText[0]) * 60 * 60 * 1000;
	ULONGLONG dwTickMinute = atoi(vecText[1]) * 60 * 1000;

	return dwTickHour + dwTickMinute;
}