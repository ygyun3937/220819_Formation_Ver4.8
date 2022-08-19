#include "stdafx.h"
#include "CPeakNet.h"

PCANBasicClass* CPeakNet::m_pPeakBasic = nullptr;

ePeakType PeakTypeStrToEnum(const TCHAR* szText)
{
	for (int i = 0; i < ePT_MAX; ++i)
	{
		if (0 == _tcscmp(szText, g_PeakTypeText[i]))
			return (ePeakType)i;
	}

	return ePT_NONEBUS;
}

CPeakNet::CPeakNet()
{
	for (int i = 0; i < ePeakCH_MAX; ++i)
	{
		m_PCanHandle[i] = PCAN_NONEBUS;
		m_PCanIndex[i] = -1;		// Master, Slave ½ÇÁ¦ ÀÎµ¦½º
	}
	m_PCanHwType = 0;
	m_ePeakType = ePT_NONEBUS;

}


CPeakNet::~CPeakNet()
{
}

void CPeakNet::CreatePCanDLL()
{
	if (nullptr == m_pPeakBasic)
		m_pPeakBasic = new PCANBasicClass;
}
void CPeakNet::DestroyPCanDLL()
{
	if (m_pPeakBasic)
	{
		delete m_pPeakBasic;
		m_pPeakBasic = nullptr;
	}
}

TPCANHandle CPeakNet::GetPCanandle(ePeakType eType, int nPortIndex)
{
	int nPortCount = nPortIndex + 1;


	if (ePT_ISABUS == eType)
	{
		if (nPortCount >= ePMC_ISA)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_ISABUS_DEFAULT + nPortIndex);
	}

	if (ePT_DNG == eType)
	{
		if (nPortCount >= ePMC_DNG)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_DNGBUS_DEFAULT + nPortIndex);
	}

	if (ePT_PCI == eType)
	{
		if (nPortCount >= ePMC_PCI)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_PCIBUS_DEFAULT + nPortIndex);
	}

	if (ePT_USB == eType)
	{
		if (nPortCount >= ePMC_USB)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_USBBUS_DEFAULT + nPortIndex);
	}

	if (ePT_PCC == eType)
	{
		if (nPortCount >= ePMC_PCC)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_PCCBUS_DEFAULT + nPortIndex);
	}

	if (ePT_LAN == eType)
	{
		if (nPortCount >= ePMC_LAN)
			return PCAN_NONEBUS;

		return TPCANHandle(PCAN_LANBUS_DEFAULT + nPortIndex);
	}

	return PCAN_NONEBUS;
}

CString CPeakNet::GetErrorEnum(DWORD dwFlag)
{
	CString str;
	if (dwFlag & PCAN_ERROR_XMTFULL)
		str += _T("PCAN_ERROR_XMTFULL > Transmit buffer in CAN controller is full\n");

	if (dwFlag & PCAN_ERROR_OVERRUN)
		str += _T("PCAN_ERROR_OVERRUN > CAN controller was read too late\n");

	if (dwFlag & PCAN_ERROR_BUSLIGHT)
		str += _T("PCAN_ERROR_BUSLIGHT > Bus error: an error counter reached the light limit\n");

	if (dwFlag & PCAN_ERROR_BUSHEAVY)
		str += _T("PCAN_ERROR_BUSHEAVY > Bus error: an error counter reached the heavy limit\n");

	if (dwFlag & PCAN_ERROR_BUSPASSIVE)
		str += _T("PCAN_ERROR_BUSPASSIVE > Bus error: the CAN controller is error passive\n");

	if (dwFlag & PCAN_ERROR_BUSOFF)
		str += _T("PCAN_ERROR_BUSOFF > Bus error: the CAN controller is in bus-off state\n");

	if (dwFlag & PCAN_ERROR_QRCVEMPTY)
		str += _T("PCAN_ERROR_QRCVEMPTY > Receive queue is empty\n");

	if (dwFlag & PCAN_ERROR_QOVERRUN)
		str += _T("PCAN_ERROR_QOVERRUN > Receive queue was read too late\n");

	if (dwFlag & PCAN_ERROR_QXMTFULL)
		str += _T("PCAN_ERROR_QXMTFULL > Transmit queue is full\n");

	if (dwFlag & PCAN_ERROR_REGTEST)
		str += _T("PCAN_ERROR_REGTEST > Test of the CAN controller hardware registers failed (no hardware found)\n");

	if (dwFlag & PCAN_ERROR_NODRIVER)
		str += _T("PCAN_ERROR_NODRIVER > Driver not loaded\n");

	if (dwFlag & PCAN_ERROR_HWINUSE)
		str += _T("PCAN_ERROR_HWINUSE > Hardware already in use by a Net\n");

	if (dwFlag & PCAN_ERROR_NETINUSE)
		str += _T("PCAN_ERROR_HWINUSE > A Client is already connected to the Net\n");

	if (dwFlag & PCAN_ERROR_ILLHW)
		str += _T("PCAN_ERROR_ILLHW > Hardware handle is invalid\n");

	if (dwFlag & PCAN_ERROR_ILLNET)
		str += _T("PCAN_ERROR_ILLNET > Net handle is invalid\n");

	if (dwFlag & PCAN_ERROR_ILLCLIENT)
		str += _T("PCAN_ERROR_ILLCLIENT > Client handle is invalid\n");

	if (dwFlag & PCAN_ERROR_RESOURCE)
		str += _T("PCAN_ERROR_RESOURCE > Resource (FIFO, Client, timeout) cannot be created\n");

	if (dwFlag & PCAN_ERROR_ILLPARAMTYPE)
		str += _T("PCAN_ERROR_ILLPARAMTYPE > Invalid parameter\n");

	if (dwFlag & PCAN_ERROR_ILLPARAMVAL)
		str += _T("PCAN_ERROR_ILLPARAMVAL > Invalid parameter value\n");

	if (dwFlag & PCAN_ERROR_UNKNOWN)
		str += _T("PCAN_ERROR_UNKNOWN > Unknown error\n");

	if (dwFlag & PCAN_ERROR_ILLDATA)
		str += _T("PCAN_ERROR_ILLDATA > Invalid data, function, or action\n");

	if (dwFlag & PCAN_ERROR_CAUTION)
		str += _T("PCAN_ERROR_CAUTION > An operation was successfully carried out, however, irregularities were registered\n");

	if (dwFlag & PCAN_ERROR_INITIALIZE)
		str += _T("PCAN_ERROR_INITIALIZE > Channel is not initialized [Value was changed from 0x40000 to 0x4000000]\n");

	if (dwFlag & PCAN_ERROR_ILLOPERATION)
		str += _T("PCAN_ERROR_ILLOPERATION > Invalid operation [Value was changed from 0x80000 to 0x8000000]\n");
	
	return str;
}

CString CPeakNet::GetErrorExtEnum(DWORD dwFlag)
{
	CString str;
	if (dwFlag & PCAN_ERROR_BUSWARNING)
		str += _T("PCAN_ERROR_BUSWARNING > Bus error: an error counter reached the warning limit\n");

	if (dwFlag & PCAN_ERROR_ANYBUSERR)
		str += _T("PCAN_ERROR_ANYBUSERR > Mask for all bus errors\n");

	if (dwFlag & PCAN_ERROR_ILLHANDLE)
		str += _T("PCAN_ERROR_ILLHANDLE > Mask for all handle errors\n");

	return str;
}
