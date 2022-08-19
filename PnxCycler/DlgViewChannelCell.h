#pragma once


// CDlgViewChannelCell 대화 상자입니다.

#include "Singleton.h"
#include "GroupListCtrl.h"
#include "SkinButton.h"
#include "ColorStaticST.h"

class CDlgViewChannelCell : public CDialogEx, public CSingleton< CDlgViewChannelCell >
{
	DECLARE_DYNAMIC(CDlgViewChannelCell)

public:
	CDlgViewChannelCell(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewChannelCell();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MAIN_VIEW_CHANNEL_CELL_TYPE };
private:
	enum eMunuType
	{
		eMenuTypeFile = 0,
		eMenuTypeEditor,
		eMenuTypeCommand,
		eMenuTypeSetting,
		eMenuTypeView
	};
	CGroupListCtrl	m_listCtrlChannel;

	CSkinButton		m_btnChannel;

	CColorStaticST	m_ctrlChannelDesc;
	CColorStaticST	m_ctrlChannelName;

	CCriticalSection	m_csLock;

	int		m_nWindowWidth;

private:
	void	InitCtrl();
	void	InitListCtrlChannel();

	void	LoadChannel();
	void	DrawChannel();

	COLORREF	Status2Color(CString strStatus);
	COLORREF	OpCode2Color(CString strOpCode);
	COLORREF	RunMode2Color(CString strRunMode);

	LRESULT	OnReceiveListCtrlSelect(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveListCtrlRClick(WPARAM wParam, LPARAM lParam);

	int		GetColumnMaxSize();
	int		GetItemIndex(TCHAR* lpszName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();

	LRESULT CompleteInitDialog(WPARAM w, LPARAM l);
};
