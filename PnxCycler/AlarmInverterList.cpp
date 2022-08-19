#include "stdafx.h"
#include "AlarmInverterList.h"
#include <locale.h>

CAlarmInverterList::CAlarmInverterList()
{
	m_nAlarmType = eAlarmInverterTypeUnknown;

}

CAlarmInverterList::~CAlarmInverterList()
{
	RemoveAll();

}


void	CAlarmInverterList::LoadAlarmInverter(CString strType, CString strAlarmCount, CString strFileName)
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
				CAlarmInverter* pAlarm = new CAlarmInverter;

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
			CAlarmInverter* pAlarm = new CAlarmInverter;

			pAlarm->SetAlarmText(_T(""));
			pAlarm->SetAlarmType(strType);

			m_listAlarm.AddTail(pAlarm);
		}
	}
}

CAlarmInverter*	CAlarmInverterList::GetAlarm(int nIndex)
{
	auto Pos = m_listAlarm.FindIndex(nIndex);

	if (Pos)
	{
		return m_listAlarm.GetAt(Pos);
	}

	return NULL;
}

void	CAlarmInverterList::RemoveAll()
{
	while (!m_listAlarm.IsEmpty())
		delete m_listAlarm.RemoveHead();
}

BOOL	CAlarmInverterList::ExistAlarm()
{
	auto Pos = m_listAlarm.GetHeadPosition();

	while (Pos)
	{
		CAlarmInverter* pAlarm = m_listAlarm.GetNext(Pos);

		if (NULL == pAlarm)
			continue;

		if (TRUE == pAlarm->GetAlarmSet())
			return TRUE;
	}

	return FALSE;
}

void	CAlarmInverterList::SetAllAlarmInverter(BOOL bAlarm)
{
	auto Pos = m_listAlarm.GetHeadPosition();

	while (Pos)
	{
		CAlarmInverter* pAlarm = m_listAlarm.GetNext(Pos);

		if (NULL == pAlarm)
			continue;

		pAlarm->SetAlarmSet(bAlarm);
	}
}
