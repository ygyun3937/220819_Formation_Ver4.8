#pragma once


// CDlgViewChamber 대화 상자
#include "Singleton.h"
#include "GroupListCtrl.h"

class CDlgViewChamber : public CDialogEx, public CSingleton< CDlgViewChamber >
{
	DECLARE_DYNAMIC(CDlgViewChamber)

public:
	CDlgViewChamber(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewChamber();

	enum { IDD = IDD_DLG_VIEW_CHAMBER };
private:
	enum eViewChamberObject
	{
		eViewChamberObjectSerial = 0,
		eViewChamberObjectCyclerChannel,
		eViewChamberObjectStatus,
		eViewChamberObjectCurrentTemp,
		eViewChamberObjectSettingAutoTemp,
		eViewChamberObjectSettingManualTemp,
		eViewChamberObjectOnOff,
		eViewChamberObjectMaxCount
	};

	CGroupListCtrl	m_listChamber;

	CRect	m_rect;
private:
	void	InitListCtrl();
	void	LoadChamber();

	int		GetListCtrlColumn(int nIndex);

	LRESULT OnReceiveEditCtrlChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveListSelect(WPARAM wParam, LPARAM lParam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
