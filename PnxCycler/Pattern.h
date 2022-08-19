#pragma once

// Pattern 별 Data 관리
struct TPattern
{
	int		nIndex;
	int		nTime;
	int		nTimeGap;
	float	fCurr;;
	float   fVol;
	float	fWatt;
	float	fTemp;

	TPattern()
	{
		nIndex = 0;
		nTime = 0;
		fCurr = 0.0;
		fVol = 0.0;
		fWatt = 0.0;
		fTemp = 0.0;
	}
};

#define	CSVPatternOptionCC	(_T("0"))
#define CSVPatternOptionCP	(_T("1"))
#define CSVPatternOptionCPCV (_T("2"))

class CPattern
{
public:
	CPattern();
	virtual ~CPattern();

	void	LoadPatternCsv( LPCTSTR lpszFilePath );

	void	SetPatternMode(int nPTNMode);
	int		GetPatternMode();
	int		GetPatternIndex();
	int		GetPatternTime();
	int		GetPatternTemp();
	int		GetSize();	

	TPattern*	GetAt();
	TPattern*	GetAt(int nIndex);

	void	SetPatternIndex();
	void	SetPatternIndex(int nIndex);
	

	CList< TPattern*, TPattern* >&	GetPatternList();
private:
	CList< TPattern*, TPattern* > m_listPattern;

	int		m_nPatternMode;
	int		m_nPatternIndex;	// 현재 진행중 순번
	int		m_nPatternTime;
	int		m_nPatternTemp;

	enum PatternMode
	{
		PatternModeUnknown = -1,
		PatternModeCC = 1,
		PatternModeCP = 3,
		PatternModeCPCV = 4
	};

	enum CsvFlag
	{
		CsvFlagUnknown = -1,
		CsvFlagOption,
		CsvFlagData
	};
private:
	void	RemoveAll();

	void	ParsePatternOption( LPCTSTR lpszString );
	void	ParsePatternData(LPCTSTR lpszString );
	void	ParsePatternTemp(LPCTSTR lpszString);
	void	CalcPatternData();
};

