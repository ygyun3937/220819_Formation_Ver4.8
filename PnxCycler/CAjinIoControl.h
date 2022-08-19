
#include "defineIO.h"

#pragma once

typedef struct
{
	UINT16	nMaxOut;
	UINT16	nMaxInp;
	UINT16	nOutStr;
	UINT16	nInpStr;
}*PDIO_INFO, DIO_INFO;

typedef struct
{
	BOOL	bOldOn;
	BOOL	bOldOff;
	UINT	nOldOn;
	UINT	nOldOff;
}*PBIT_INFO, BIT_INFO;


class CAjinIoControl
{
public:
	CAjinIoControl();
	~CAjinIoControl();

public:
	void	Initialize();
	BOOL	InitializeMemberVariables();
	BOOL	InitializeIo();

	// IO 실제 동작 함수 
	BOOL	InitializeLibrary();
	BOOL	IsInitializeLibrary() { return m_bInitDIO; };

public:
	BOOL	m_bInitDIO = FALSE;
	int		m_iMaxOutput = 0;
	int		m_iMaxInput = 0;
	int		m_iModuleCount = 0;

	// IO 실제 동작 관련
	DIO_INFO *m_pInfo_DIO;
	BIT_INFO *m_pInfo_Input;
	BIT_INFO *m_pInfo_Output;

	BOOL	m_bInputIo_Hanwha[INPUT_Hanwha_TOTAL];
	BOOL	m_bOutputIo_Hanwha[OUTPUT_Hanwha_TOTAL];

	BOOL	m_bInputIo_DFS[INPUT_DFS_TOTAL];
	BOOL	m_bOutputIo_DFS[OUTPUT_DFS_TOTAL];
public:
	// IO 실제 동작 함수 
	void	ScanDioModule();

	BOOL	ReadInputIoBit(int addr);
	BOOL	ReadInputIoDword();
	BOOL	ReadOutputIoBit(int addr);
	BOOL	ReadOutputIoDword();

	BOOL	WriteOutputIoBit(int addr, BOOL OnOff);

















};

