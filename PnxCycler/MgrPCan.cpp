#include "stdafx.h"
#include "MgrPCan.h"

#include <list>
#include "MgrComm.h"
#include "MgrChannel.h"

CMgrPCan::CMgrPCan()
{
	Initialize();
}

CMgrPCan::~CMgrPCan()
{
	Dispose();
}

void CMgrPCan::Initialize()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypePeakCan, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CPCanCom* pPcan = new CPCanCom;

			//m_listCom.AddTail(pPcan);
			int nChannelNumber = lpDevice->nChannelNumber;

			pPcan->SetChannelNo(nChannelNumber);
			pPcan->SetPrimaryKey(lpDevice->nPrimaryKey);

			pPcan->SetInitBaudrate(lpDevice->nBaudrate);
			pPcan->SetInitUSBChannel(lpDevice->nUsbChannel);
			pPcan->SetResvCanIdArray(lpDevice->arrCanId);
			pPcan->SetCheckID(lpDevice->nCheckCanID);

			if (pPcan->OpenCan())
			{
				pPcan->StartThread();
				//pPcan->SetSendCanCommand(DSP_SEND_CMD_INITIATE);
				//pPcan->SendDisplayCanMessage();

				if (pPcan->IsFirstRecvOK() == false)
				{
					pPcan->CloseCan();
					pPcan->StopAllThread();
				}
			}
			
			m_listCom.AddTail(pPcan);
		}

		++iter;
	}

	StartThread();
}

void CMgrPCan::Dispose()
{
	//ClosePcan();


	RemoveAll();
}

// -------------------------------------------------------------------------------------------------

void CMgrPCan::StartThread()
{
	auto Pos = m_listCom.GetHeadPosition();
	CPCanCom* pPcan = NULL;
	while (Pos)
	{
		pPcan = m_listCom.GetNext(Pos);

		if (NULL == pPcan)
			continue;

		if (pPcan->OpenCan())
		{
			pPcan->StartThread();

			pPcan->SendDisplayCanMegErrorClear();
		}

		Sleep(1);
	}
}

void CMgrPCan::StopThread()
{	
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CPCanCom* pComm = m_listCom.GetNext(Pos);
		if (NULL == pComm)
			continue;

		if (pComm->GetCloseMode() != true)
		{
			pComm->SetCloseStep(CloseStep_DisConnect);
			pComm->SetCloseMode(true);
		}
		if (pComm->GetProgramClose() == true)
		{
			pComm->StopAllThread();
			continue;
		}

		if (pComm->IsOpen() == true && pComm->GetProgramClose() != true && pComm->GetInverInit() == true)
		{
			Pos = m_listCom.GetHeadPosition();
			continue;
		}

		if (pComm->CloseCan() != true)
		{
			continue;
		}

		pComm->StopAllThread();
	}
}

// -------------------------------------------------------------------------------------------------

CPCanCom*	CMgrPCan::GetAt(int nIndex)
{
	auto Pos = m_listCom.FindIndex(nIndex);

	if (Pos)
	{
		return m_listCom.GetAt(Pos);
	}

	return FALSE;
}

CPCanCom * CMgrPCan::GetStation(int nStation)
{
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CPCanCom* pComm = m_listCom.GetNext(Pos);

		if (NULL == pComm)
			continue;

		if (pComm->GetChannelNo() == nStation)
			return pComm;
	}

	return NULL;
}

CPCanCom*	CMgrPCan::GetAtComm(int nPrimaryKey)
{
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CPCanCom* pComm = m_listCom.GetNext(Pos);

		if (NULL == pComm)
			continue;

		if (pComm->GetPrimaryKey() == nPrimaryKey)
			return pComm;
	}

	return NULL;
}


CList<CPCanCom*, CPCanCom*>& CMgrPCan::GetComList()
{
	return m_listCom;
}

void	CMgrPCan::RemoveAll()
{
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CPCanCom* pComm = m_listCom.GetNext(Pos);
		if (NULL == pComm)
			continue;
	}

	while (!m_listCom.IsEmpty())
	{
		CPCanCom* pPcan = m_listCom.RemoveHead();
		delete pPcan;
	}
}

void	CMgrPCan::RemoveElement(int nPrimaryKey)
{
	auto Pos = m_listCom.GetHeadPosition();

	while (Pos)
	{
		CPCanCom* pComm = m_listCom.GetAt(Pos);

		if (pComm == NULL)
		{
			m_listCom.GetNext(Pos);
			continue;
		}

		if (pComm->GetPrimaryKey() == nPrimaryKey)
		{
			delete pComm;

			pComm = NULL;

			m_listCom.RemoveAt(Pos);

			break;
		}

		m_listCom.GetNext(Pos);
	}
}
