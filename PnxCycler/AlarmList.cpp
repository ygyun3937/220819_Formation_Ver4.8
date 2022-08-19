#include "stdafx.h"
#include "AlarmList.h"
#include <locale.h>

CAlarmList::CAlarmList()
{
	m_nAlarmType = eAlarmTypeUnknown;
}


CAlarmList::~CAlarmList()
{
	RemoveAll();
}

void	CAlarmList::LoadAlarm(CString strType, CString strAlarmCount, CString strFileName)
{
	RemoveAll();

	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\AlarmList\\%s"), (LPCSTR)GetExePath(), strFileName.GetString());

	setlocale(LC_ALL, "Korean");

	CStdioFile fp;

	if (TRUE == fp.Open(strFilePath, CFile::modeRead))
	{
		ULONGLONG nReadByte = fp.GetLength();

		if (nReadByte >= 0)
		{
			CString strContent = _T("");
			CString strAlarm = _T("");
			CString strComment = _T("");

			while (fp.ReadString(strContent))
			{
				CAlarm* pAlarm = new CAlarm;

				//, 가 있는 경우 Alarm에 대한 코멘트 추가
				if (strContent.Find(",") != -1)
				{
					AfxExtractSubString(strAlarm, strContent, 0, ',');
					AfxExtractSubString(strComment, strContent, 1, ',');
					pAlarm->SetAlarmText(strAlarm);
					pAlarm->SetAlarmComment(strComment);
				}
				else
				{
					pAlarm->SetAlarmText(strContent);
				}
				pAlarm->SetAlarmType(strType);

				m_listAlarm.AddTail(pAlarm);
			}
		}

		fp.Close();
	}

	int nAlarmCount = atoi(strAlarmCount);

	// Alarm 개수 Check
	if (nAlarmCount > m_listAlarm.GetSize())
	{
		for (int i = m_listAlarm.GetSize(); i < nAlarmCount; ++i)
		{
			CAlarm* pAlarm = new CAlarm;

			pAlarm->SetAlarmText(_T(""));
			pAlarm->SetAlarmType(strType);

			m_listAlarm.AddTail(pAlarm);
		}
	}
}

CAlarm*	CAlarmList::GetAlarm(int nIndex)
{
	auto Pos = m_listAlarm.FindIndex(nIndex);

	if (Pos)
	{
		return m_listAlarm.GetAt(Pos);
	}

	return NULL;
}

void	CAlarmList::RemoveAll()
{
	while (!m_listAlarm.IsEmpty())
		delete m_listAlarm.RemoveHead();
}

BOOL	CAlarmList::ExistAlarm()
{
	auto Pos = m_listAlarm.GetHeadPosition();

	while (Pos)
	{
		CAlarm* pAlarm = m_listAlarm.GetNext(Pos);

		if (NULL == pAlarm)
			continue;

		if (TRUE == pAlarm->GetAlarmSet())
			return TRUE;
	}

	return FALSE;
}

void	CAlarmList::SetAllAlarm(BOOL bAlarm)
{
	auto Pos = m_listAlarm.GetHeadPosition();

	while (Pos)
	{
		CAlarm* pAlarm = m_listAlarm.GetNext(Pos);

		if (NULL == pAlarm)
			continue;

		pAlarm->SetAlarmSet(bAlarm);
	}
}