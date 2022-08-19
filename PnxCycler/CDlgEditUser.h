#pragma once


// CDlgEditUser 대화 상자
#include "ColorStaticST.h"
#include "Singleton.h"

class CDlgEditUser : public CDialogEx, public CSingleton< CDlgEditUser >
{
	DECLARE_DYNAMIC(CDlgEditUser)

public:
	CDlgEditUser(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditUser();

	enum { IDD = IDD_DLG_USER_ADD };
private:
	enum eUserAddObject
	{
		eUserAddObjectID = 0,
		eUserAddObjectPW,
		eUserAddObjectLogFilePath,
		eUserAddObjectMaxCount
	};

	CColorStaticST m_ctrlText[eUserAddObjectMaxCount];

	
	enum eUserLevel
	{
		eUserLevelAdmin =0,
		eUserLevelEngineer,
		eUserLevelOperator,
		eUserLevelMax
	};

	CString m_strUserID;
	CString m_strUserPW;
	CString m_strUserLevel;
	CString m_strUserFilePath;
private:
	void	InitStatic();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnAddPath();
	afx_msg void OnBnClickedBtnUserAddAdd();
	afx_msg void OnBnClickedBtnUserAddClose();
	afx_msg void OnCbnSelchangeComboUserLevel();
	CComboBox m_cbUserLevel;
};
