#include <vector>
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"

#pragma once


// CDlgViewProgramStatus 대화 상자

class CDlgViewProgramStatus : public CDialogEx, public CSingleton< CDlgViewProgramStatus >
{
	DECLARE_DYNAMIC(CDlgViewProgramStatus)

public:
	CDlgViewProgramStatus(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewProgramStatus();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGRAM_STATUS };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();

	void Initialize();
	void InitializeControls();
	void InitializeTimers();


	void Set_Progress_Pos(int nPos);


private:
	CProgressCtrl m_Progress_Program;
public:
	afx_msg void OnDestroy();
};
