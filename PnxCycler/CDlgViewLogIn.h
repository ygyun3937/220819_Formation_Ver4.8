#pragma once


// CDlgViewLogIn 대화 상자
#include "ColorStaticST.h"
#include "Singleton.h"

class CDlgViewLogIn : public CDialogEx, public CSingleton< CDlgViewLogIn >
{
	DECLARE_DYNAMIC(CDlgViewLogIn)

public:
	CDlgViewLogIn(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewLogIn();

	enum { IDD = IDD_DLG_LOGIN };
private:
	enum eLogInObject
	{
		eLogInObjectTitle = 0,
		eLogInObjectID,
		eLogInObjectPW,
		eLogInObjectMaxCount
	};

	CColorStaticST m_ctrlText[eLogInObjectMaxCount];

	CString m_strLoginID;
	CString m_strLoginPW;
private:
	void	InitStatic();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnLoginAdd();
	afx_msg void OnBnClickedBtnLoginLogin();
	afx_msg void OnBnClickedBtnLoginClose();

	void SetUserList();
	CFont m_ListBoxFont;

	CListCtrl m_List_User;
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	CxShadeButton m_BtnLogin;
	CxShadeButton m_BtnLogOut;
	CxShadeButton m_BtnUserAdd;
};
