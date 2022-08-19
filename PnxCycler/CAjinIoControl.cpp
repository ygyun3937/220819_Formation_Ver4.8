#include "stdafx.h"
#include "CAjinIoControl.h"
#include "MgrDio.h"

#include "AXL.h"
#include "AXD.h"
#include "AXA.h"
#include "defineIO.h"		//Ajin I/O

/**
 * .
 *
 * METHOD_NAME : CAjinIoControl
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
CAjinIoControl::CAjinIoControl()
{



}

/**
 * .
 *
 * METHOD_NAME : ~CAjinIoControl
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
CAjinIoControl::~CAjinIoControl()
{
}

/**
 * .
 *
 * METHOD_NAME : Initialize
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
void CAjinIoControl::Initialize()
{
	InitializeMemberVariables();

	if (InitializeLibrary() == TRUE)
	{
		m_bInitDIO = TRUE;
	}

	InitializeIo();
}

/**
 * .
 * 
 * METHOD_NAME : InitializeMemberVariables
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
BOOL CAjinIoControl::InitializeMemberVariables()
{
	m_bInitDIO = FALSE;

	m_iMaxOutput = 0;
	m_iMaxInput = 0;

	m_pInfo_DIO = NULL;
	m_pInfo_Input = NULL;
	m_pInfo_Output = NULL;

	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{	
		// INPUT
		for (int inputCount = 0; inputCount < INPUT_Hanwha_TOTAL; inputCount++)
		{
			m_bInputIo_Hanwha[inputCount] = FALSE;
		}

		// OUTPUT
		for (int outputCount = 0; outputCount < OUTPUT_Hanwha_TOTAL; outputCount++)
		{
			m_bOutputIo_Hanwha[outputCount] = FALSE;
		}
	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)	
	{
		// INPUT
		for (int inputCount = 0; inputCount < INPUT_DFS_TOTAL; inputCount++)
		{
			m_bInputIo_DFS[inputCount] = FALSE;
		}

		// OUTPUT
		for (int outputCount = 0; outputCount < OUTPUT_DFS_TOTAL; outputCount++)
		{
			m_bOutputIo_DFS[outputCount] = FALSE;
		}
	}
	return TRUE;
}

/**
 * .
 * 
 * METHOD_NAME : InitializeIo
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
BOOL CAjinIoControl::InitializeIo()
{
	// OUTPUT_LIST
	// OUTPUT_PROBE_PIN_UP = 0
	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{
		for (int outputCount = OUTPUT_Hanwha_STAGE1_PUSHER_UNIT_DOWN; outputCount < OUTPUT_Hanwha_TOTAL; outputCount++)
		{
			if (!WriteOutputIoBit(outputCount, false))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)
	{
		for (int outputCount = OUTPUT_DFS_STAGE1_SPARE1; outputCount < OUTPUT_DFS_TOTAL; outputCount++)
		{

			if (!WriteOutputIoBit(outputCount, false))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/**
 * .
 * 
 * METHOD_NAME : InitializeLibrary
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
BOOL CAjinIoControl::InitializeLibrary()
{
	DWORD dRet = 0;

	if (!AxlIsOpened())
	{
		dRet = AxlOpen(7);
		if (dRet != AXT_RT_SUCCESS)
		{
			SYSLOG(Dbg, "CAjinIOControl - [AJIN] BOARD CONNECTION FAIL.");

			//AfxMessageBox(_T("[AJIN] BOARD CONNECTION FAIL."));
			return FALSE;
		}
	}

	Sleep(200);

	return TRUE;
}

/**
 * .
 *
 * METHOD_NAME : ScanDIoModule
 * METHOD_TYPE : -
 * DESCRIPTION : IO MODULE SCAN
 */
void CAjinIoControl::ScanDioModule()
{

}

/**
 * .
 *
 * METHOD_NAME : ReadInputIoBit
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
BOOL CAjinIoControl::ReadInputIoBit(int addr)
{
	if (!IsInitializeLibrary())
	{
		return FALSE;
	}

	bool bVal = true;
	DWORD bOnOff;
	int ModuleNum = MODULE_INPORT_START + (addr / MODULE_COUNT);
	int Offset = addr % MODULE_COUNT;

	AxdiReadInportBit(ModuleNum, Offset, &bOnOff);
	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{
		m_bInputIo_Hanwha[(ModuleNum * MODULE_COUNT) + Offset] = bOnOff;
	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)
	{
		m_bInputIo_DFS[(ModuleNum * MODULE_COUNT) + Offset] = bOnOff;
	}
	return bOnOff;
}

/**
 * .
 *
 * METHOD_NAME : ReadInputIoDword
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
BOOL CAjinIoControl::ReadInputIoDword()
{
	if (!IsInitializeLibrary())
	{
		return FALSE;
	}

	DWORD dw;
	DWORD dwResult;

	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{
		int InModuleCount = (InputList_Hanwha_AJIN::INPUT_Hanwha_TOTAL) / MODULE_COUNT;

		for (int i = 0; i < InModuleCount + 1; i++)
		{
			if (AXT_RT_SUCCESS != AxdiReadInportDword(i + MODULE_INPORT_START, 0, &dw))
			{
				continue;
			}
			
			for (int j = 0; j < MODULE_COUNT; j++)
			{
				DWORD dwResult = 0x01 & (dw >> j);				/// 
				int bRet = dwResult == 0x01 ? TRUE : FALSE;
				m_bInputIo_Hanwha[(i * MODULE_COUNT) + j] = bRet;		/// = 0x01 & (dw >> j); 이렇게 해도 될거 같은데 나중에 테스트
			}
		}
	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)
	{

		int InModuleCount = (InputList_DFS_AJIN::INPUT_DFS_TOTAL) / MODULE_COUNT;

		for (int i = 0; i < InModuleCount + 1; i++)
		{
			if (AXT_RT_SUCCESS != AxdiReadInportDword(i + MODULE_INPORT_START, 0, &dw))
			{
				continue;
			}

			for (int j = 0; j < MODULE_COUNT; j++)
			{
				DWORD dwResult = 0x01 & (dw >> j);				/// 
				int bRet = dwResult == 0x01 ? TRUE : FALSE;
				m_bInputIo_DFS[(i * MODULE_COUNT) + j] = bRet;		/// = 0x01 & (dw >> j); 이렇게 해도 될거 같은데 나중에 테스트
			}
		}		
	}

	

	Sleep(10);

	return TRUE;
}

/**
 * .
 *
 * METHOD_NAME : ReadOutputIoBit
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
BOOL CAjinIoControl::ReadOutputIoBit(int addr)
{
	if (!IsInitializeLibrary())
	{
		return FALSE;
	}
	DWORD bOnOff;

	bool bVal = TRUE;
	int ModuleNum = MODULE_OUTPORT_START + (addr / MODULE_COUNT);
	int Offset = addr % MODULE_COUNT;

	AxdoReadOutportBit(ModuleNum, Offset, &bOnOff);

	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{
		m_bOutputIo_Hanwha[((ModuleNum - MODULE_OUTPORT_START) * MODULE_COUNT) + Offset] = bOnOff;

	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)
	{
		m_bOutputIo_DFS[((ModuleNum - MODULE_OUTPORT_START) * MODULE_COUNT) + Offset] = bOnOff;
	}
	return TRUE;
}

/**
 * .
 *
 * METHOD_NAME : ReadOutputIoDword
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
BOOL CAjinIoControl::ReadOutputIoDword()
{
	if (!IsInitializeLibrary())
	{
		return FALSE;
	}

	DWORD dw;

	if (CMgrDio::GetMgr()->GetEQType() == enumEQType_Formation)
	{
		int OutModuleCount = (OutputList_Hanwha_AJIN::OUTPUT_Hanwha_TOTAL) / MODULE_COUNT;
		for (int i = 0; i < OutModuleCount + 1; i++)
		{

			if (AXT_RT_SUCCESS != AxdoReadOutportDword(i + MODULE_OUTPORT_START, 0, &dw))
				continue;

			for (int j = 0; j < MODULE_COUNT; j++)
			{
				DWORD dwResult = 0x01 & (dw >> j);
				int bRet = dwResult == 0x01 ? TRUE : FALSE;
				m_bOutputIo_Hanwha[(i * MODULE_COUNT) + j] = bRet;
			}
		}
	}
	else if (CMgrDio::GetMgr()->GetEQType() == enumEQType_DFS)
	{
		int OutModuleCount = (OutputList_DFS_AJIN::OUTPUT_DFS_TOTAL) / MODULE_COUNT;
		for (int i = 0; i < OutModuleCount + 1; i++)
		{

			if (AXT_RT_SUCCESS != AxdoReadOutportDword(i + MODULE_OUTPORT_START, 0, &dw))
				continue;

			for (int j = 0; j < MODULE_COUNT; j++)
			{
				DWORD dwResult = 0x01 & (dw >> j);
				int bRet = dwResult == 0x01 ? TRUE : FALSE;
				m_bOutputIo_DFS[(i * MODULE_COUNT) + j] = bRet;
			}
		}
	}


	
	return TRUE;
}

BOOL CAjinIoControl::WriteOutputIoBit(int addr, BOOL OnOff)
{
	if (!IsInitializeLibrary())
	{
		return FALSE;
	}

	bool bVal = true;
	DWORD OutPort;
	int ModuleNum = MODULE_OUTPORT_START + (addr / MODULE_COUNT);
	int Offset = addr % MODULE_COUNT;

	DWORD dw = AxdoWriteOutportBit(ModuleNum, Offset, OnOff);
	if (AXT_RT_SUCCESS != dw)
	{
		return FALSE;
	}
	AxdoReadOutportBit(ModuleNum, Offset, &OutPort);

	if (OnOff == (BOOL)OutPort)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
