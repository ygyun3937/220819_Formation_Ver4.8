#pragma once


// CDlgEditChannelView 대화 상자

#include "Singleton.h"
#include "ColorStaticST.h"

class CDlgEditChannelView : public CDialogEx, public CSingleton<CDlgEditChannelView>
{
	DECLARE_DYNAMIC(CDlgEditChannelView)

public:
	CDlgEditChannelView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditChannelView();

	enum { IDD = IDD_DLG_EDIT_HEADER };
private:
	CListCtrl m_listChannel;

	CString m_strSerialNo;
	CString m_strName;
	CString m_strWidth;

	int		m_nCurSel;

	enum eEditChannelObject
	{
		eEditChannelObjectSerial = 0,
		eEditChannelObjectName,
		eEditChannelObjectWidth,
		eEditChannelObjectMaxCount
	};

	CColorStaticST	m_ctrlText[eEditChannelObjectMaxCount];

private:
	void	InitStatic();
	void	InitListCtrl();
	void	InitCheckBox();
	void	LoadListCtrl();
	void	LoadCheckBox();
	void	ListCtrl2EditBox();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnEditHeaderFunc1();
	afx_msg void OnBnClickedBtnEditHeaderFunc2();
	afx_msg void OnBnClickedBtnEditHeaderFunc3();
	afx_msg void OnBnClickedChkEditDaqView();
	afx_msg void OnClickListEditHeader(NMHDR *pNMHDR, LRESULT *pResult);
};
