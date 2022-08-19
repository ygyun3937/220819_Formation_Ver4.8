#pragma once


// CDlgReserveCancel 대화 상자

#include "Singleton.h"

class CDlgReserveCancel : public CDialogEx, public CSingleton< CDlgReserveCancel >
{
	DECLARE_DYNAMIC(CDlgReserveCancel)

public:
	CDlgReserveCancel(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgReserveCancel();

	enum { IDD = IDD_DLG_RESERVE_CANCLE };
private:
	enum eReserveCancleObject
	{
		eReserveCancleObjectPause = 0,
		eReserveCancleObjectPauseAll,
		eReserveCancleObjectComplete,
		eReserveCancleObjectCompleteAll,
		eReserveCancleObjectClose,
		eReserveCancleObjectMaxCount
	};

	CListCtrl	m_listReserve;

	int		m_nPrimaryKey;
	int		m_nCurSel;
private:
	void	InitListCtrl();
	void	LoadListCtrl();
	void	OnClickReserveCancel(UINT nID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnItemChangedListReserveCancel(NMHDR *pNMHDR, LRESULT *pResult);
};
