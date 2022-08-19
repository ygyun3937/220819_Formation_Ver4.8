#pragma once


//#include "GridCtrl.h"
#include "./GridCtrl_src/GridCtrl.h"

class CDlgTotalSafetyCondition : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTotalSafetyCondition)

public:
	CDlgTotalSafetyCondition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTotalSafetyCondition();

// Dialog Data
	enum { IDD = IDD_DLGTOTALSAFETYCONTITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	void OnGridClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	char stringAllowed(CString stri, int option);
	DECLARE_MESSAGE_MAP()

private:
	void InitializeGrid();
	ULONGLONG GetTickCount(CString text);

public:
	CGridCtrl m_Grid;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
