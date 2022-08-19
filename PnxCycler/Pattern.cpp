#include "stdafx.h"
#include "Pattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CPattern::CPattern()
{
	m_nPatternMode	= PatternModeUnknown;
	m_nPatternIndex = 0;
	m_nPatternTime	= 0;
	m_nPatternTemp  = 0;
}


CPattern::~CPattern()
{
	RemoveAll();
}

void	CPattern::LoadPatternCsv(LPCTSTR lpszFilePath)
{
	m_nPatternMode	= PatternModeUnknown;
	m_nPatternTime	= 0;
	m_nPatternTemp  = 0;

	RemoveAll();

	FILE* fp = nullptr;

	if(fopen_s(&fp, lpszFilePath, "r"))
	{
		AfxMessageBox(_T("Excel file is open. Please close and start again."));
		return;
	}

	if (fp == NULL) {
		return;
	}

//	TRACE(_T("CSV File Read FilePath(%s)\n"), lpszFilePath);

	char szContent[512] = {0,};

	memset(szContent, NULL, 512);

	int nCsvFlag = CsvFlagUnknown;

	while (fgets(szContent, 512, fp))
	{
		CString strContent;

		strContent.Format(_T("%s"), szContent);

		strContent.Remove('\r');
		strContent.Remove('\n');

		memset(szContent, NULL, 512);

		// 20201103 turby CSV option Data 를 찾을 수가 없어서 Time 으로 바꿨더니 파싱 시작.
		if (strContent.Find(_T("OPTION,")) >= 0)
		{
			nCsvFlag = CsvFlagOption;

			continue;
		}
		else if (strContent.Find(_T("Time,")) >= 0)
		{
			nCsvFlag = CsvFlagData;

			continue;
		}
		if (CsvFlagUnknown == nCsvFlag)
		{
			continue;
		}
		else if (CsvFlagOption == nCsvFlag)
		{
			ParsePatternOption(strContent);

			continue;
		}
		else if (CsvFlagData == nCsvFlag)
		{
			if (strContent.IsEmpty())
				break;

			if (PatternModeUnknown == m_nPatternMode)
				break;

			ParsePatternData(strContent);

			int nPos = 0;
			g_nPatternStepCount = (int)_ttoi(strContent.Tokenize(_T(","), nPos));

// 			if (m_nPatternTemp == 0)
// 				break;
//  			else
//  				ParsePatternTemp(strContent); //taek 처음에는 따로 구분 해서 만들려고 했는데 따로 구분 안 하고 하려고
		}
	}

	fclose(fp);
	
	if (m_listPattern.GetSize() > 0)
		CalcPatternData();
}

void	CPattern::SetPatternMode(int nPTNMode)
{
	m_nPatternMode = nPTNMode;
}

int		CPattern::GetPatternMode()
{
	return m_nPatternMode;
}

int		CPattern::GetPatternIndex()
{
	return m_nPatternIndex;
}

int		CPattern::GetPatternTime()
{
	return m_nPatternTime;
}

int		CPattern::GetPatternTemp()
{
	return m_nPatternTemp;
}

int		CPattern::GetSize()
{
	return m_listPattern.GetSize();
}

void	CPattern::SetPatternIndex()
{
	m_nPatternIndex++;
}

void	CPattern::SetPatternIndex(int nIndex)
{
	m_nPatternIndex = nIndex;
}

TPattern*	CPattern::GetAt()
{
	auto Pos = m_listPattern.FindIndex(m_nPatternIndex);

	if (Pos)
	{
		return m_listPattern.GetAt(Pos);
	}

	return NULL;
}

TPattern*	CPattern::GetAt(int nIndex)
{
	auto Pos = m_listPattern.FindIndex(nIndex);

	if (Pos)
	{
		return m_listPattern.GetAt(Pos);
	}

	return NULL;
}

CList< TPattern*, TPattern* >&	CPattern::GetPatternList()
{
	return m_listPattern;
}

void	CPattern::RemoveAll()
{
	while (!m_listPattern.IsEmpty())
		delete m_listPattern.RemoveHead();
}

void	CPattern::ParsePatternOption( LPCTSTR lpszString )
{
	CString rString;
	int		nSubString = 0;

	while (AfxExtractSubString(rString, lpszString, nSubString++, ','))
	{
		switch (nSubString)
		{
			case 1: break;
			case 2:
			{
					 if (rString.CompareNoCase(CSVPatternOptionCC) == 0)
						 m_nPatternMode = PatternModeCC;
				else if (rString.CompareNoCase(CSVPatternOptionCP) == 0)	
						 m_nPatternMode = PatternModeCP;				
				else if (rString.CompareNoCase(CSVPatternOptionCPCV) == 0)	
						 m_nPatternMode = PatternModeCPCV;

				break;
			}
			case 3: break;
			case 4:
			{
					 if (rString.CompareNoCase("0") == 0) m_nPatternTemp = 0;
				else if (rString.CompareNoCase("1") == 0) m_nPatternTemp = 1;
				break; //여기에서 온도 관련 메시지 확인
			}
		}
	}
}

void	CPattern::ParsePatternData( LPCTSTR lpszString )
{
	CString rString;
	int		nSubString = 0;

	TPattern* pPattern = new TPattern;

	pPattern->nIndex = m_listPattern.GetSize();

	while (AfxExtractSubString(rString, lpszString, nSubString++, ','))
	{
		switch (nSubString)
		{
		case 1: pPattern->nTime = (int)(atof(rString) * 10.0); break;
		case 2: pPattern->fCurr = (float)atof(rString);			break;
		case 3: pPattern->fVol = (float)atof(rString);			break;
		case 4: pPattern->fWatt = (float)atof(rString);			break;
		case 5: pPattern->fTemp = (float)atof(rString);			break; //온도 관련 데이터 획득
		}
	}

	m_listPattern.AddTail(pPattern);
}

void	CPattern::CalcPatternData()
{
	TPattern* pHead = m_listPattern.GetHead();
	TPattern* pTail = m_listPattern.GetTail();

	if (NULL == pHead || NULL == pTail)
		return;

	if (pHead->nIndex == pTail->nIndex)
	{
		pHead->nTimeGap = pHead->nTime;
	}
	else
	{
		for (int i = m_listPattern.GetSize() - 1; i >= 0; --i)
		{
			TPattern* pSrc = GetAt(i);
			TPattern* pDst = GetAt(i - 1);

			if (NULL == pSrc)
				continue;

			if (pSrc && !pDst)
			{
				pSrc->nTimeGap = pHead->nTime;
			}
			else if (pSrc && pDst)
			{
				pSrc->nTimeGap = pSrc->nTime - pDst->nTime;
			}

			m_nPatternTime += pSrc->nTimeGap;
		}
	}
}