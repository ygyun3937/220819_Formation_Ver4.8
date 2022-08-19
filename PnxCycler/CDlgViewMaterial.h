#pragma once


// CDlgViewMaterial 대화 상자
#include "Singleton.h"
#include "ColorStaticST.h"

class CDlgViewMaterial : public CDialogEx, public CSingleton< CDlgViewMaterial >
{
	DECLARE_DYNAMIC(CDlgViewMaterial)

public:
	CDlgViewMaterial(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewMaterial();

	enum { IDD = IDD_DLG_VIEW_MATERIAL };
private:
	enum eViewMaterialObject
	{
		eViewMaterialObjectTitle = 0,
		eViewMaterialObjectValue,
		eViewMaterialObjectMaxCount
	};

	CListCtrl m_listRelay;

	CColorStaticST m_ctrlText[eViewMaterialObjectMaxCount];
private:
	void	InitStatic();
	void	InitListCtrl();
	void	LoadRelayCount();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnMaterialClose();
};
