#include "stdafx.h"
#include "ChamberProc.h"
#include "MgrSerial.h"
#include "MgrComm.h"

CChamberProc::CChamberProc()
{
	m_nPortNo =0;
	m_nBaurdrate =0;
	m_bLoop =TRUE;
	m_fnInit();
}


CChamberProc::~CChamberProc()
{
	m_fnDeInit();
}

int	CChamberProc::m_fnInit()
{

	CMgrComm* pMgr = CMgrComm::GetMgr();

	//Chamber 관련 사용유무 만들어 주세요~  
	if (CommTypeRs422 != pMgr->GetCommType(CommObjIF) && CommTypeRs485 != pMgr->GetCommType(CommObjIF))
		return 0;
	//

	CList< TComm*, TComm* >* pListComm = CMgrComm::GetMgr()->GetCommList(CommObjIF);

	if (NULL == pListComm)
		return 0;

	auto Pos = pListComm->GetHeadPosition();

	while (Pos)
	{
		TComm* pComm = pListComm->GetNext(Pos);

		if (NULL == pComm)
			continue;

		CSerialCom* pSerial = new CSerialCom;

		pSerial->m_fnSetPort(pComm->nCommPort);
		pSerial->m_fnSetBaurd(pComm->nBaudrate);
		pSerial->m_fnSetPrimaryKey(pComm->nChannelNumber);

		ST_SerialPara	stPara;

		stPara.bPortNum = (BYTE)pSerial->m_fnGetPort();
		stPara.dwBaud = pSerial->m_fnGetBaurd();
		stPara.bByteSize = PORT_DATA_BIT;
		stPara.bStopBits = PORT_STOP_BIT;
		stPara.bParity = PORT_PARITY;
		stPara.bDelimiter = PORT_TERMINATE_CYCLER; //PORT_TERMINATE;
		stPara.bBufClear = PORT_CLEAR_BUF;

		pSerial->OpenPort(&stPara);
	}
	m_fnStartThread();
	return 1;
}
int CChamberProc::m_fnDeInit()
{
	m_fnStopThread();
	while (!m_listCom.IsEmpty())
		delete m_listCom.RemoveHead();
	return 1;
}

void CChamberProc::m_fnStartThread()
{

	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CSerialCom* pCom = m_listCom.GetNext(Pos);
		if (NULL == pCom)
			continue;
		pCom->m_fnThreadChamberRun();
	}
}
void CChamberProc::m_fnStopThread()
{
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CSerialCom* pCom = m_listCom.GetNext(Pos);
		if (NULL == pCom)
			continue;
		pCom->m_fnThreadChamberStop();
	}
}
