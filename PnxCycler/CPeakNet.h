#pragma once

enum ePeakType
{
	ePT_NONEBUS,	// 1
	ePT_ISABUS,	// 8
	ePT_DNG,		// 1
	ePT_PCI,		// 16
	ePT_USB,		// 16
	ePT_PCC,		// 2
	ePT_LAN,		// 16
	ePT_MAX,
};

enum ePeakMaxCount
{
	ePMC_ISA = 8,
	ePMC_DNG = 1,
	ePMC_PCI = 16,
	ePMC_USB = 16,
	ePMC_PCC = 2,
	ePMC_LAN = 16,
};

static TCHAR* g_PeakTypeText[ePT_MAX] = {
	_T("NONEBUS"),
	_T("ISABUS"),
	_T("DNG"),
	_T("PCI"),
	_T("USB"),
	_T("PCC"),
	_T("LAN"),
};

enum ePeakCanCH
{
	ePeakCH01 = 0,
	ePeakCH02 = 1,
	ePeakCH_MAX,
};

ePeakType PeakTypeStrToEnum(const TCHAR* szText);

class PCANBasicClass;
class CPeakNet
{
public:
	static void CreatePCanDLL();
	static void DestroyPCanDLL();
	
public:
	CPeakNet();
	virtual ~CPeakNet();
	virtual eNetworkItemType GetNetType(){ return eBNT_NONE; }

public:
	virtual bool InitPeakCan(void* pSetting) = 0;
	virtual void Release() = 0;
	virtual void Recv(int nIdx = 0) = 0;
	virtual void Send(void* p) = 0;
	virtual void GetCanData(SBMSPacket& data) = 0;
	virtual int GetPeakCanPortCount() { return 0; }

protected:
	static PCANBasicClass*	m_pPeakBasic;
	TPCANHandle		m_PCanHandle[ePeakCH_MAX];
	int				m_PCanIndex[ePeakCH_MAX];		// Master, Slave Ω«¡¶ ¿Œµ¶Ω∫
	TPCANType		m_PCanHwType;
	ePeakType		m_ePeakType;


	TPCANHandle		GetPCanandle(ePeakType eType, int nPortIndex);

	CString GetErrorEnum(DWORD dwFlag);
	CString GetErrorExtEnum(DWORD dwFlag);
};

