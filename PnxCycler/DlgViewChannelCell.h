#pragma once


// CDlgViewChannelCell ��ȭ �����Դϴ�.

#include "Singleton.h"
#include "GroupListCtrl.h"
#include "SkinButton.h"
#include "ColorStaticST.h"

class CDlgViewChannelCell : public CDialogEx, public CSingleton< CDlgViewChannelCell >
{
	DECLARE_DYNAMIC(CDlgViewChannelCell)

public:
	CDlgViewChannelCell(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgViewChannelCell();

// ��ȭ ���� �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
