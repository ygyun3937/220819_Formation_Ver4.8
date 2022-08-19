#pragma once

#include "Singleton.h"

using namespace std;

#include <vector>

struct SSchedule
{
	CString strSerialNumber;
	CString strScheduleName;
	CString	strScheduleFilePath;
	CString strScheduleDescription;
	CString strWriteTime;
	CString strModifyTime;
	CTime sModifyTime;

	void	Init()
	{
		strSerialNumber.Empty();
		strScheduleName.Empty();
		strScheduleFilePath.Empty();
		strScheduleDescription.Empty();
		strWriteTime.Empty();
		strModifyTime.Empty();
	}
};

struct SModel
{
	CString strSerialNumber;
	CString strModelName;
	CString strWritter;
	CString strModelDescription;
	CString strWriteTime;
	CString strFilePath;

	vector<SSchedule*> vecSchedule;

	void	Init()
	{
		strSerialNumber.Empty();
		strModelName.Empty();
		strWritter.Empty();
		strModelDescription.Empty();
		strWriteTime.Empty();
		strFilePath.Empty();
	}

	void	Clear()
	{
		auto iter = vecSchedule.begin();
		auto iterE = vecSchedule.end();

		while (iter != iterE)
		{
			SSchedule* lpSchedule = (*iter);

			if (lpSchedule)
			{
				delete lpSchedule;
				lpSchedule = nullptr;
			}

			++iter;
		}

		vecSchedule.clear();
	}

	void	push_back(SSchedule* lpSchedule)
	{
		vecSchedule.push_back(lpSchedule);
	}

	int		GetSize()
	{
		return (int)vecSchedule.size();
	}

	SSchedule*	GetSchedule(int nIndex)
	{
		if (nIndex < 0 || (int)vecSchedule.size() <= nIndex)
			return nullptr;

		return vecSchedule[nIndex];
	}

	vector<SSchedule*>&	GetScheduleVector()
	{
		return vecSchedule;
	}
};



class CMgrModel : public CSingleton< CMgrModel >
{
public:
	CMgrModel();
	virtual ~CMgrModel();

	bool	LoadModel();

	SModel*	GetModel(int nIndex);

	std::vector<SModel*>&	GetModelVector();

	void	SetModelIndex(int nIndex) { m_nModelIndex = nIndex; }
	void	SetScheduleIndex(int nIndex) { m_nScheduleIndex = nIndex; }

	//220526 YGY Formation Type Added function for selecting by stage
	void	SetModelIndex(int nStageNo,int nIndex) { m_nArrayModelIndex[nStageNo] = nIndex; }
	void	SetScheduleIndex(int nStageNo,int nIndex) { m_nArrayScheduleIndex[nStageNo] = nIndex; }


	int		GetModelIndex() { 
		return m_nModelIndex; }
	int		GetScheduleIndex() { 
		return m_nScheduleIndex; }

	//220526 YGY Formation Type Added function for selecting by stage
	int		GetModelIndex(int nStageNo) {
		return m_nArrayModelIndex[nStageNo];
	}
	int		GetScheduleIndex(int nStageNo) {
		return m_nArrayScheduleIndex[nStageNo];
	}
	BOOL		GetStageScheduleSelect(int nStageNo) { return m_bScheduleSelect[nStageNo]; }
	void		SetStageScheduleSelect(int nStageNo, BOOL bSelected) { m_bScheduleSelect[nStageNo] = bSelected; }
	void	ClearModel();
private:
	int		m_nModelIndex;
	int		m_nScheduleIndex;
	int		m_nSyncMode;

	CString	m_strFilePath;
	//220526 YGY Formation Type Added function for selecting by stage
	int			m_nArrayModelIndex[2];
	int			m_nArrayScheduleIndex[2];

	BOOL		m_bScheduleSelect[2];



	std::vector<SModel*> m_vecModel;
private:
	void	SearchModel();
	void	SearchSchedule(SModel* lpModel, CString strFilePath);
};

