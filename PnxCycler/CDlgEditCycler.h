#pragma once


// CDlgEditCycler 대화 상자
//#include "Singleton.h"

class CDlgEditCycler : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditCycler)

public:
	CDlgEditCycler(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditCycler();

	enum { IDD = IDD_DLG_EDIT_CYCLER };
private:
	void	LoadFilePath();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnEditCyclerDefault();
	afx_msg void OnBnClickedBtnEditCyclerSave();
	afx_msg void OnBnClickedBtnEditCycler1();
	afx_msg void OnBnClickedBtnEditCycler2();
};
