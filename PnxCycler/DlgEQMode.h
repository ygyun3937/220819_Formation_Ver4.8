#pragma once


// CDlgEQMode 대화 상자입니다.
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "afxwin.h"

enum EQModeStatic
{
	EQModeStaticSave = 0,
	EQModeStaticExit,
	EQModeStaticMaxCount
};

enum EQModeButton
{
	EQModeButtonSerial = 0,
	EQModeButtonParallel,
	EQModeButtonCV,
	EQModeButtonSave,
	EQModeButtonExit,
	EQModeButtonMaxCount
};

class CDlgEQMode : public CDialogEx, public CSingleton< CDlgEQMode >
{
	DECLARE_DYNAMIC(CDlgEQMode)

public:
	CDlgEQMode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEQMode();

	void	SetCVModeCheckButton(int nCh, BOOL bOn);
	void	SetPowerMapCheckButton(int nCh, BOOL bOn);
private:
	CSkinButton	m_btnEQMode[EQModeButtonMaxCount];

	CColorStaticST	m_ctrlEQMode[EQModeStaticMaxCount];

	int m_nEQMode;
	BOOL	m_bCVMode;

	BOOL	m_bPowerMap;


private:
	void	InitStatic();
	void	InitButton();

	void	LoadEQMode();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_EQUIPMENT_MODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnEqModeSave();
	afx_msg void OnBnClickedBtnEqModeExit();
	afx_msg void OnBnClickedRdoEqModeSerial();
	afx_msg void OnBnClickedRdoEqModeParallel();
	afx_msg void OnBnClickedCVMode();

	afx_msg void OnPaint();
	CButton m_check_CVMode;			//kw--- check 박스 멤버변수 추가
	CButton m_check_CVMode2;
	afx_msg void OnBnClickedCheckCvmode2();
};
