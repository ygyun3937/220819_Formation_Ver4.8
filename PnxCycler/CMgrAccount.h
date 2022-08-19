#pragma once

#include <vector>

enum eCustomViewSortType
{
	eCustomViewSortTypeUp = 0,
	eCustomViewSortTypeDown
};

struct sAccount
{
	TCHAR	szUserID[_MAX_PATH] = { 0, };
	TCHAR	szUserPW[_MAX_PATH] = { 0, };
	TCHAR	szUserLevel[_MAX_PATH] = { 0, };
	TCHAR	szSaveFilePath[_MAX_PATH] = { 0, };

	bool	bUseDAQView;
};

struct sChannelView
{
	TCHAR szName[_MAX_PATH] = { 0, };	// 해당 값은 항상 고정일것
	TCHAR szHeader[_MAX_PATH] = { 0, };

	int		nWidth;
	int		nSerialNo;

	sChannelView()
	{
		
	}

	struct sCompareGreater
	{
		bool operator()(const sChannelView* lhs, const sChannelView* rhs)
		{
			return (lhs->nSerialNo < rhs->nSerialNo);
		}
	};
};

struct sCustomView
{
	TCHAR	szUserID[_MAX_PATH];

	std::vector<sChannelView*> vecViewItem;

	sCustomView()
	{
		ZeroMemory(szUserID, _MAX_PATH);
	}
};

#include "Singleton.h"
#include <map>

class CMgrAccount : public CSingleton< CMgrAccount >
{
public:
	CMgrAccount();
	virtual ~CMgrAccount();

	void	SortCustomView(int nCurSel, eCustomViewSortType eType);
	void	SetModified(bool bModify) { m_bModified = bModify; }
	void	AddNewAccount(sAccount* pNewElement);
	void	SaveAccount();

	bool	IsLogin();
	void	Logout();
	bool	IsExistUserID(CString strID);
	bool	IsModified() { return m_bModified; }
	
	int		GetCustomViewSize();

	CString	GetLoginID() { return m_strLoginID; }
	CString GetLoginPW() { return m_strLoginPW; }


	sAccount*	LogInAccount(CString strID, CString strPW);
	sAccount*	GetLoginedAccount();

	sCustomView*	GetCustomView();

private:
	void	LoadAccount();
	void	LoadCustomChannelView();
	void	AddAccount(CString strID, bool bDefault);
	void	SaveCustomChannelView();
	void	SaveCustomChannelView(CString strID);

	void	SetDefaultCustomView(sCustomView* lpCustomView);

private:
	std::vector<sAccount*> m_vecAccount;

	std::map<CString, sCustomView* > m_mapCustom;

	//
	CString m_strLoginID;
	CString m_strLoginPW;

	bool	m_bModified;
	sAccount*	m_loginAccount;	//로그인된 Account
};

