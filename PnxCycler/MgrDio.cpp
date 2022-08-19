#include "stdafx.h"
#include "MgrDio.h"

#include "MgrChannel.h"
#include "MgrComm.h"
#include "MgrConfig.h"
#include "CMgrModel.h"
#include "MgrAlarm.h"
#include "MgrStatus.h"
#include "MgrPCan.h"
#include "MgrPlcInterface.h"

#include "ComiSWEcatSdk_Api.h"
#include "SockProc.h"
#include "mxio.h"

//#include "DlgViewChamberAlarm.h"

#define	DioConfigFileName	(_T("DioConfig.ini"))

#ifndef UNICODE
TCHAR const * WCHAR_TO_TCHAR(WCHAR const * in, TCHAR * out)
{
	WideCharToMultiByte(CP_OEMCP, NULL, in, -1, out, MAX_DEVICE_DESC_LEN, NULL, FALSE);
	return out;
}
#else
TCHAR const * WCHAR_TO_TCHAR(WCHAR const * in, TCHAR * out)
{
	wcscpy_s(out, wcslen(in) + 1, in);
	return out;
}
#endif

#ifndef UNICODE
WCHAR const * TCHAR_TO_WCHAR(TCHAR const * in, WCHAR* out)
{
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)in, -1, out, MAX_DEVICE_DESC_LEN);
	return out;
}
#else
WCHAR const * TCHAR_TO_WCHAR(TCHAR const * in, WCHAR* out)
{
	wcscpy_s(out, wcslen(in) + 1, in);
	return out;
}
#endif

// -------------------------------------------------------------------------------------------------


CMgrDio::CMgrDio()
{
	Initialize();

	nTimeOutCnt = 0;
	nTimeOutMax = 30;

	m_ePressRunstepMeasurementCurrent = 0;
	m_AutoCompleteFlag = FALSE;
	m_ManualUnloadStartFlag = FALSE;
	m_ManualStartStop = FALSE;
	m_RepeatRunStart = FALSE;



	SetConnect(FALSE);

	////ZebraBarcod_COMPORT = SYSTEM.ini 에서 받아오자
	//#define BARCODE_CBR_9600					9600		///통신속도
	//#define BARCODE_DATA_BIT					8		  ///데이터 비트
	//#define BARCODE_STOP_BIT					0		  ///데이터 비트
	//#define BARCODE_CHANNEL						0		  ///데이터 비트


	Sleep(100);

	m_bStatusLightLoop = FALSE;

	memset(m_bFanSatus, FALSE, sizeof(m_bFanSatus));
}

CMgrDio::~CMgrDio()
{
	// EtherCat DLL Load  add by KYJ 210901 //
	secDll_Unload();
	//////////////////////////////////////////
}

// -------------------------------------------------------------------------------------------------


bool CMgrDio::Initialize()
{
	InitializeVariables();

	LoadDioIni();

	InitializeThread();

	return TRUE;
}

bool CMgrDio::InitializeVariables()
{
	m_nCardNum = 0;
	m_uOutPut = 0;
	m_uInPut = 0;
	m_bLoop = TRUE;
	//m_pDioThread = NULL;
	m_confParam.deviceNumber = -1;

	m_portCount = 0;

	m_bBuzzer = TRUE;

	m_pStatusLightThread = NULL;
	m_bStatusLightLoop = FALSE;

	// DIO STATUS AE±aE­
	for (int i = 0; i < 64; i++)
	{
		m_nDOStatus[i] = -1;
	}

	//m_instantDoCtrl = NULL;
	//m_instantDoCtrl = InstantDoCtrl::Create();

	//m_instantDiCtrl = NULL;
	//m_instantDiCtrl = InstantDiCtrl::Create();

	// ----------------------------------------------------------------------------

	// PAGE Init
	m_nFirstInputDioNo = -1;
	m_nFirstOutputDioNo = -1;

	// IoName Init
	for (int slotCount = 0; slotCount < IO_SLOT_MAX; slotCount++)
	{
		for (int paraCount = 0; paraCount < IO_PARA_COUNT; paraCount++)
		{
			for (int valueCount = 0; valueCount < IO_VALUE_COUNT; valueCount++)
			{
				m_strIoName[slotCount][paraCount][valueCount] = _T("");
			}
		}
	}
	for (int nBoardNo = 0; nBoardNo < IO_SLOT_COUNT; nBoardNo++)
	{
		for (int nChNo = 0; nChNo < IO_CHANNLE_COUNT; nChNo++)
		{
			m_bIO[nBoardNo][nChNo] = FALSE;
		}
	}

	SetBcrRead(FALSE);

	return TRUE;
}

bool CMgrDio::LoadDioIni()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s.ini"), (LPCSTR)GetExePath(), CMgrConfig::GetMgr()->GetDioIniName());

	//EQ Type Set
	CString strEQName = CMgrConfig::GetMgr()->GetDioIniName();	//임시

	SetEQType(strEQName);
	CString strAppName = _T("DIO");

	CString strManufacturer = INIReadStr(strAppName, _T("Manufacturer"), strFilePath);
	CString strSubEqpUsable = INIReadStr(strAppName, _T("DioSubEqpUsable"), strFilePath);

	if (strSubEqpUsable.CompareNoCase(_T("Y")) == 0)
	{
		int nTotalSubEqpCount = INIReadInt(strAppName, _T("TotalSubEqpCount"), strFilePath);
		int nEqpTotalDioCount = 0;

		for (int subEqpIndexCount = 0; subEqpIndexCount < nTotalSubEqpCount; subEqpIndexCount++)
		{
			CString tempAppName1;
			tempAppName1.Format(_T("DIOSUBEQP%d"), subEqpIndexCount + 1);

			m_SubEqpDioInfo[subEqpIndexCount].m_nSubEqpNo = subEqpIndexCount + 1;
			m_SubEqpDioInfo[subEqpIndexCount].m_strSubEqpId = INIReadStr(tempAppName1, _T("SubEqpId"), strFilePath);
			m_SubEqpDioInfo[subEqpIndexCount].m_nTotalDioCount = INIReadInt(tempAppName1, _T("TotalDioCount"), strFilePath);
			m_SubEqpDioInfo[subEqpIndexCount].m_nInputDioTotalCount = INIReadInt(tempAppName1, _T("InputDioCount"), strFilePath);
			m_SubEqpDioInfo[subEqpIndexCount].m_nOutputDioTotalCount = INIReadInt(tempAppName1, _T("OutputDioCount"), strFilePath);

			int tempTotalDioCount = INIReadInt(tempAppName1, _T("TotalDioCount"), strFilePath);
			nEqpTotalDioCount += tempTotalDioCount;
		}

		// -------------
		CString tempAppName1;
		CString tempAppValue;
		CString Temp;

		tempAppName1.Format(_T("TowerLamp"));
		if (GetEQType() == enumEQType_Formation)
		{
			tempAppValue = INIReadStr(tempAppName1, _T("Idle"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Idle][nTowerLamp] = atoi(Temp);
			}

			tempAppValue = INIReadStr(tempAppName1, _T("Ready"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Ready][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Stop"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Stop][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Run"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Run][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Complete"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Complete][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Alarm"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTowerLamp < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_Hanwha[(int)enumEQStatus_Hanwha::enumEQHanwha_Alarm][nTowerLamp] = atoi(Temp);
			}
		}
		else if (GetEQType() == enumEQType_DFS)
		{
			tempAppValue = INIReadStr(tempAppName1, _T("Idle"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Idle][nTowerLamp] = atoi(Temp);
			}

			tempAppValue = INIReadStr(tempAppName1, _T("Ready"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Ready][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Stop"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Stop][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Run"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Run][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Complete"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Complete][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Alarm"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Alarm][nTowerLamp] = atoi(Temp);
			}
			tempAppValue = INIReadStr(tempAppName1, _T("Manual"), strFilePath);
			for (int nTowerLamp = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTowerLamp < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTowerLamp++)
			{
				AfxExtractSubString(Temp, tempAppValue, nTowerLamp, ',');
				m_iStatusTowerLamp_DFS[(int)enumEQStatus_DFS::enumEQDFS_Manual][nTowerLamp] = atoi(Temp);
			}
		}

		//--------------------------------------------------------------

		int nInputPageNo = 0;
		int nOutputPageNo = 0;

		for (int dioCount = 0; dioCount < nEqpTotalDioCount; dioCount++)
		{
			CString tempAppName2;
			tempAppName2.Format(_T("DIO%d"), dioCount + 1);

			// --------------------------------------------------------------------------

			int nSubEqpNo = INIReadInt(tempAppName2, _T("SubEqpNo"), strFilePath);
			int nEqpStageNo = INIReadInt(tempAppName2, _T("EqpStageNo"), strFilePath);
			CString strDioCardType = INIReadStr(tempAppName2, _T("CardType"), strFilePath);
			CString strIoType = INIReadStr(tempAppName2, _T("IoType"), strFilePath);
			m_nChCount = INIReadInt(tempAppName2, _T("ChannelCount"), strFilePath);

			int nSubEqpIndexNo = nSubEqpNo - 1;
			int nEqpStageIndexNo = nEqpStageNo - 1;

			if (strIoType.Find(_T("INPUT")) != -1)
			{
				if (m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nFirstInputDioNo < 0)
				{
					m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nFirstInputDioNo = dioCount + 1;
				}

				nInputPageNo++;

				m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nLastInputDioNo = dioCount + 1;
				m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nInputPageMap[nInputPageNo] = dioCount;
			}
			if (strIoType.Find(_T("OUTPUT")) != -1)
			{
				if (m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nFirstOutputDioNo < 0)
				{
					m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nFirstOutputDioNo = dioCount + 1;
				}

				nOutputPageNo++;

				m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nLastOutputDioNo = dioCount + 1;
				m_SubEqpDioInfo[nSubEqpIndexNo].m_EqpStageDioInfo[nEqpStageIndexNo].m_nOutputPageMap[nOutputPageNo] = dioCount;
			}

			// --------------------------------------------------------------------------

			for (int channelIndexCount = 0; channelIndexCount < m_nChCount; channelIndexCount++)
			{
				CString strAddressItem;
				strAddressItem.Format(_T("IOADDR%d"), channelIndexCount + 1);
				CString strAddressName = INIReadStr(tempAppName2, _T(strAddressItem), strFilePath);

				CString strIoNameItem;
				strIoNameItem.Format(_T("IONAME%d"), channelIndexCount + 1);
				CString strIoName = INIReadStr(tempAppName2, _T(strIoNameItem), strFilePath);

				m_strIoName[dioCount][IO_ADDRESS][channelIndexCount] = strAddressName;
				m_strIoName[dioCount][IO_NAME][channelIndexCount] = strIoName;
			}
		}
	}
	else
	{
		int totalDioCount = INIReadInt(strAppName, _T("Count"), strFilePath);

		m_nTotalDioCount = totalDioCount;
		m_nInputDioCount = INIReadInt(strAppName, _T("InputDioCount"), strFilePath);
		m_nOutputDioCount = INIReadInt(strAppName, _T("OutputDioCount"), strFilePath);

		for (int dioCount = 0; dioCount < totalDioCount; ++dioCount)
		{
			CString strApp;
			strApp.Format(_T("DIO%d"), dioCount + 1);

			CString strDioCardType = INIReadStr(strApp, _T("CardType"), strFilePath);
			m_nChCount = INIReadInt(strApp, _T("ChannelCount"), strFilePath);
			CString strIoType = INIReadStr(strApp, _T("IoType"), strFilePath);				// INPUT/OUTPUT

			if (strIoType.Find(_T("INPUT")) != -1)
			{
				if (m_nFirstInputDioNo == -1)
					m_nFirstInputDioNo = dioCount + 1;
			}
			if (strIoType.Find(_T("OUTPUT")) != -1)
			{
				if (m_nFirstOutputDioNo == -1)
					m_nFirstOutputDioNo = dioCount + 1;
			}

			for (int channelCount = 0; channelCount < m_nChCount; channelCount++)
			{
				CString strAddressItem;
				strAddressItem.Format(_T("IOADDR%d"), channelCount + 1);
				CString strAddressName = INIReadStr(strApp, _T(strAddressItem), strFilePath);

				CString strIoNameItem;
				strIoNameItem.Format(_T("IONAME%d"), channelCount + 1);
				CString strIoName = INIReadStr(strApp, _T(strIoNameItem), strFilePath);

				m_strIoName[dioCount][IO_ADDRESS][channelCount] = strAddressName;
				m_strIoName[dioCount][IO_NAME][channelCount] = strIoName;
			}
		}
	}



	return TRUE;
}

bool CMgrDio::InitializeThread()
{
	return TRUE;
}

void CMgrDio::Init_OutputIO()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	if (pMgrDio->GetEQType() == enumEQType_Formation)
	{
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
	}
	else if (pMgrDio->GetEQType() == enumEQType_DFS)
	{
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_Green_Lamp, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_Yellow_Lamp, FALSE);
		pMgrDio->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_RED_Lamp, FALSE);
	}
}


// -----------------------------------------------------------------------------

bool CMgrDio::m_fnDioOpen(CString strType)
{
	if (strType == "ADLINK")
	{
		bool bret = FALSE; // fail
		int nCardNum = 0;
		if (m_iPciCard[nCardNum] >= 0)
			UD_Release_Card((U16)nCardNum);

		int iRegister_Card_Value = 0;
		//m_iDioCardNumber[iCardSelect] = 0; //select card number
		//iRegister_Card_Value = Register_Card(PCI_7230, m_iDioCardNumber[iCardSelect]); //PCI_7230, PCI_7432

		//pnx_hr 2021.05.10 
		CString strTmp;
		strTmp = CMgrConfig::GetMgr()->GetDioName();
		int iType = 0;
		m_nCardType = GetCardType(strTmp);

		iRegister_Card_Value = UD_Register_Card(m_nCardType, (U16)nCardNum); //PCI_7230, PCI_7432

		m_iPciCard[nCardNum] = (I16)iRegister_Card_Value;

		//if ((card=Register_Card(PCI_7432, card_number)) < 0)
		if (iRegister_Card_Value < 0)
		{
			//////////////////////////////////////////////////////////////////////////
			//		colEditCol=RGB(255,50,0);
			// 		CWnd *pEWnd=GetDlgItem(IDC_EDIT_PORT);
			// 		if (pEWnd)
			// 		{
			// 			pEWnd->Invalidate(TRUE);
			// 		}
			//////////////////////////////////////////////////////////////////////////
			CString strTemp;
			//wsprintf(s,"카드가 등록되지 않았습니다.DIO 카드를 확인해 주세요!\n");
			strTemp.Format("[%d] 카드가 등록되지 않았습니다.DIO 카드를 확인해 주세요!", nCardNum);
			//		MessageBox(strTemp, "Error", MB_OK);
			bret = FALSE; // fail
			m_bDioOpen = FALSE;
			//pnx_hr 2021.05.11 수정 m_bAdrink;
			//SetAdlinkType(FALSE);
		}
		else
		{
			bret = TRUE; // sucess
			m_bDioOpen = TRUE;
			SetAdLinkType(TRUE);
			//ConfDevice(1);
			//StartThread();
		}

		return bret;
	}
	else if (strType == "ADVANTECH")
	{
		InstantDoCtrl * instantDoCtrl = InstantDoCtrl::Create();
		Array<DeviceTreeNode>* sptedDevices = instantDoCtrl->getSupportedDevices();

		if (sptedDevices->getCount() == 0)
		{
			return FALSE;
		}
		else {
			for (int i = 0; i < sptedDevices->getCount(); ++i)
			{
				DeviceTreeNode const & node = sptedDevices->getItem(i);
				DeviceInformation devInfo(node.DeviceNumber);

				CString strTmp(node.Description);

				if (strTmp.Find(CMgrConfig::GetMgr()->GetDioName()) == 0)
				{
					m_confParam.deviceNumber = i;
					TCHAR_TO_WCHAR(_T(""), m_confParam.profilePath);
				}
			}
			sptedDevices->Dispose();
		}
		instantDoCtrl->Dispose();

		ConfDevice(0);

		InstantDiCtrl * instantDiCtrl = InstantDiCtrl::Create();
		sptedDevices = instantDiCtrl->getSupportedDevices();

		if (sptedDevices->getCount() == 0)
		{
			return FALSE;
		}
		else {
			//		int deviceCount = 0;
			for (int i = 0; i < sptedDevices->getCount(); ++i)
			{
				DeviceTreeNode const & node = sptedDevices->getItem(i);
				DeviceInformation devInfo(node.DeviceNumber);
				//			ErrorCode errorCode = instantDoCtrl->setSelectedDevice(devInfo);

				CString strTmp(node.Description);

				if (strTmp.Find(CMgrConfig::GetMgr()->GetDioName()) == 0)
				{
					m_confParam.deviceNumber = i;
					TCHAR_TO_WCHAR(_T(""), m_confParam.profilePath);
				}
			}
			sptedDevices->Dispose();
		}
		instantDiCtrl->Dispose();

		ConfDevice(1);

		//StartThread();

		SetAdLinkType(FALSE);

		//IF START...ALL CHANEEL SET TRUE (SERIAL MODE)
		int* pData = GetDO();

		for (int i = 0; i < 12; ++i)
		{
			if (pData[i] == TRUE)
				SetDIO(i, TRUE);

			Sleep(100);
		}

		GetDI();

		return TRUE;
	}
	else if (strType == "EtherCat")
	{
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		int netID = 0;
		int errCode;
		t_i32 ErrCode;
		TEcSlvProdInfoEx pEcSlvtype;

		BOOL bIsLoaded = secDll_IsLoaded();
		if (!secDll_Load())  //  DLL Load
		{
			m_bDioOpen = FALSE;
			AfxMessageBox(_T("EtherCat DLL Load Fail"));
		}

		if (!secGn_LoadDevices(&ErrCode)) // Device Load
		{
			m_bDioOpen = FALSE;
			AfxMessageBox(_T("EtherCat Device Load Fail"));
		}

		int Di1_NumCh, Di2_NumCh, Do1_NumCh, Do2_NumCh, Di1_StCh, Di2_StCh, Do1_StCh, Do2_StCh;

		t_i32 NumNetSlave = secNet_ScanSlaves_ff(0, &ErrCode); //Current Port Count Get Function
		t_i32 NumNetChannel = secdoGetNumChannels(0, &ErrCode); //Current Port Get Function

		//int NumNet = ecGn_GetNumNetworks(&ErrCode); //Currnet Port Get 7Ch
		//ecSlv_GetTypeInfo_A(netID, 0, &pEcSlvtype, &ErrCode);

		//Di1_NumCh = pEcSlvtype.DI.NumChannels;
		//Di1_StCh = pEcSlvtype.DI.StaGlobChannel;

		//ecSlv_GetTypeInfo_A(netID, 1, &pEcSlvtype, &ErrCode);

		//Do1_NumCh = pEcSlvtype.DO.NumChannels;
		//Do1_StCh = pEcSlvtype.DO.StaGlobChannel;

		//DioInit


		StartThread();

		return TRUE;
	}
	//네트워크 타입 펄스 타입 구분도 필요
	else if (strType == "AJIN")
	{
		m_AjinIoControl = new CAjinIoControl();
		m_AjinIoControl->Initialize();

		if (m_AjinIoControl->m_bInitDIO == TRUE)
		{
			if (GetEQType() == enumEQType_Formation)
			{
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, TRUE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, TRUE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
			}
			else if (GetEQType() == enumEQType_DFS)
			{
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, TRUE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, TRUE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, TRUE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_Green_Lamp, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_Yellow_Lamp, FALSE);
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_RED_Lamp, FALSE);

				//Auto Ouput Signal Alway On
				CMgrDio::GetMgr()->SetAjinOutputData(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Auto_Mode_Start, TRUE);
			}
			SetConnect(TRUE);
			StartThread();
		}
		else
		{
			//ALARM_ITEM item;
			//CString strTemp = "";
			//strTemp.Format(_T("IO Board DisConnect "));
			//CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_IO_BOARD_DISCONNECT, strTemp, ALARM_HEAVY, &item);
			SetConnect(FALSE);
		}
	}
	else if (strType == "MOXA")
	{
	int nConnectCount = 0;
	//Init  
	CString strMsg = _T("");
	int iRet = MXEIO_Init();
	CheckErr(iRet, "MXEIO_Init");
	if (iRet == MXIO_OK)
	{
		//printf("MXEIO_Init Initiate the socket succeed.\n");
		//SYSLOG(Dbg, _T("MXEIO_Init Initiate the socket succeed."));
	}

	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	//Connect
	for (int i = 0; i < CMgrConfig::GetMgr()->m_nDioCount; i++) {

		DWORD nPort = 502;

		char IPAddress[16] = { '\0' };
		dwTimeOut = 1000;
		bytInputValue[32] = { '\0' };


		strcpy(IPAddress, vecValue[i]->strIP);

		Password[8] = { '\0' };


		iRet = MXEIO_E1K_Connect(IPAddress,		//IP address
			nPort,					//TCP port number
			dwTimeOut,			//timeout
			&vecValue[i]->nHandle,				//connection handle
			Password);			//ligin password
		CheckErr(iRet, "MXEIO_E1K_Connect");
		if (iRet == MXIO_OK)
		{

			vecValue[i]->bConnect = TRUE;
			CMgrConfig::GetMgr()->SetMoxaIoInfor(i, vecValue);

			nConnectCount++;
			//SYSLOG(Dbg, _T("MXEIO_E1K_Connect Creates Adapter IP=%s connection succeed. TimeOut=%d, Password=%s"), IPAddress, dwTimeOut, Password);
		}
		//Sleep(300);
	}
	if (nConnectCount == CMgrConfig::GetMgr()->m_nDioCount)
	{

		CMgrDio::GetMgr()->SetMoxa_PowerOnLamp(TRUE);
		SetConnect(TRUE);
		StartThread();
		return TRUE;
	}
	else
	{
		//ALARM_ITEM item;
		//CString strTemp = "";
		//strTemp.Format(_T("IO Board DisConnect "));
		//CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_IO_BOARD_DISCONNECT, strTemp, ALARM_HEAVY, &item);
		SetConnect(FALSE);
	}


		//if (GetEQType() == enumEQType_DCIR)
		//{
		//	//Test 
		//	MoxaIOBoard[IO_SLOT_INPUT1].strIP = "192.168.127.2";
		//	MoxaIOBoard[IO_SLOT_INPUT1].nChCount = 16;
		//	nModuleCount++;
		//	MoxaIOBoard[IO_SLOT_INPUT2].strIP = "192.168.127.3";
		//	MoxaIOBoard[IO_SLOT_INPUT2].nChCount = 16;
		//	nModuleCount++;
		//	MoxaIOBoard[IO_SLOT_INOUT1].strIP = "192.168.127.4";
		//	MoxaIOBoard[IO_SLOT_INOUT1].nChCount = 8;
		//	nModuleCount++;
		//	MoxaIOBoard[IO_SLOT_INOUT2].strIP = "192.168.127.5";
		//	MoxaIOBoard[IO_SLOT_INOUT2].nChCount = 8;
		//	nModuleCount++;
		//	MoxaIOBoard[IO_SLOT_INOUT3].strIP = "192.168.127.6";
		//	MoxaIOBoard[IO_SLOT_INOUT3].nChCount = 8;
		//	nModuleCount++;
		//}
		//else if (GetEQType() == enumEQType_Precharger)
		//{
		//	MoxaIOBoard[0].strIP = "192.168.127.1";
		//	MoxaIOBoard[0].nChCount = 16;
		//	nModuleCount++;
		//	MoxaIOBoard[1].strIP = "192.168.127.2";
		//	MoxaIOBoard[1].nChCount = 8;
		//	nModuleCount++;
		//	MoxaIOBoard[2].strIP = "192.168.127.3";
		//	MoxaIOBoard[2].nChCount = 8;
		//	nModuleCount++;
		//}
		////Init  
		//CString strMsg = _T("");
		//int iRet = MXEIO_Init();
		//CheckErr(iRet, "MXEIO_Init");
		//if (iRet == MXIO_OK)
		//{
		//	//printf("MXEIO_Init Initiate the socket succeed.\n");
		//	//SYSLOG(Dbg, _T("MXEIO_Init Initiate the socket succeed."));
		//}
		////Connect
		//for (int i = 0; i < nModuleCount; i++) {
		//	DWORD nPort = 502;
		//	char IPAddress[16] = { '\0' };
		//	dwTimeOut = 1000;
		//	bytInputValue[32] = { '\0' };
		//	strcpy(IPAddress, MoxaIOBoard[i].strIP);
		//	Password[8] = { '\0' };
		//	iRet = MXEIO_E1K_Connect(IPAddress,		//IP address
		//		nPort,					//TCP port number
		//		dwTimeOut,			//timeout
		//		&MoxaIOBoard[i].nHandle,				//connection handle
		//		Password);			//ligin password
		//	CheckErr(iRet, "MXEIO_E1K_Connect");
		//	if (iRet == MXIO_OK)
		//	{
		//		nConnectCount++;
		//		//SYSLOG(Dbg, _T("MXEIO_E1K_Connect Creates Adapter IP=%s connection succeed. TimeOut=%d, Password=%s"), IPAddress, dwTimeOut, Password);
		//	}
		//	//Sleep(300);
		//}
		//if (nConnectCount == nModuleCount)
		//{
		//	for (int nMoxaNo = 0; nMoxaNo < nModuleCount; nMoxaNo++)
		//	{
		//		MoxaIOBoard[nMoxaNo].bConnect = TRUE;
		//	}				
		//	CMgrDio::GetMgr()->SetMoxa_PowerOnLamp(TRUE);
		//	SetConnect(TRUE);
		//	StartThread();
		//	return TRUE;
		//}
		//else
		//{
		//	//ALARM_ITEM item;
		//	//CString strTemp = "";
		//	//strTemp.Format(_T("IO Board DisConnect "));
		//	//CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_IO_BOARD_DISCONNECT, strTemp, ALARM_HEAVY, &item);
		//	SetConnect(FALSE);
		//}
		////MoxaIOConnect();
	}

	return FALSE;
}

bool CMgrDio::m_fnDioOpenAdLink(int nCardNum)
{
	bool bret = FALSE; // fail
	if (m_iPciCard[nCardNum] >= 0)
		UD_Release_Card((U16)nCardNum);

	int iRegister_Card_Value = 0;
	//m_iDioCardNumber[iCardSelect] = 0; //select card number
	//iRegister_Card_Value = Register_Card(PCI_7230, m_iDioCardNumber[iCardSelect]); //PCI_7230, PCI_7432

	//pnx_hr 2021.05.10 
	CString strTmp;
	strTmp = CMgrConfig::GetMgr()->GetDioName();
	int iType = 0;
	m_nCardType = GetCardType(strTmp);

	iRegister_Card_Value = UD_Register_Card(m_nCardType, (U16)nCardNum); //PCI_7230, PCI_7432

	m_iPciCard[nCardNum] = (I16)iRegister_Card_Value;

	//if ((card=Register_Card(PCI_7432, card_number)) < 0)
	if (iRegister_Card_Value < 0)
	{
		//////////////////////////////////////////////////////////////////////////
		//		colEditCol=RGB(255,50,0);
		// 		CWnd *pEWnd=GetDlgItem(IDC_EDIT_PORT);
		// 		if (pEWnd)
		// 		{
		// 			pEWnd->Invalidate(TRUE);
		// 		}
		//////////////////////////////////////////////////////////////////////////
		CString strTemp;
		//wsprintf(s,"카드가 등록되지 않았습니다.DIO 카드를 확인해 주세요!\n");
		strTemp.Format("[%d] 카드가 등록되지 않았습니다.DIO 카드를 확인해 주세요!", nCardNum);
		//		MessageBox(strTemp, "Error", MB_OK);
		bret = FALSE; // fail
		m_bDioOpen = FALSE;
		//pnx_hr 2021.05.11 수정 m_bAdrink;
		//SetAdlinkType(FALSE);
	}
	else
	{
		bret = TRUE; // sucess
		m_bDioOpen = TRUE;
		SetAdLinkType(TRUE);
		//ConfDevice(1);
		//StartThread();
	}

	return bret;
}

bool CMgrDio::m_fnDioOpen4750()
{
	InstantDoCtrl * instantDoCtrl = InstantDoCtrl::Create();
	Array<DeviceTreeNode>* sptedDevices = instantDoCtrl->getSupportedDevices();

	if (sptedDevices->getCount() == 0)
	{
		return FALSE;
	}
	else {
		for (int i = 0; i < sptedDevices->getCount(); ++i)
		{
			DeviceTreeNode const & node = sptedDevices->getItem(i);
			DeviceInformation devInfo(node.DeviceNumber);

			CString strTmp(node.Description);

			if (strTmp.Find(CMgrConfig::GetMgr()->GetDioName()) == 0)
			{
				m_confParam.deviceNumber = i;
				TCHAR_TO_WCHAR(_T(""), m_confParam.profilePath);
			}
		}
		sptedDevices->Dispose();
	}
	instantDoCtrl->Dispose();

	ConfDevice(0);

	InstantDiCtrl * instantDiCtrl = InstantDiCtrl::Create();
	sptedDevices = instantDiCtrl->getSupportedDevices();

	if (sptedDevices->getCount() == 0)
	{
		return FALSE;
	}
	else {
		//		int deviceCount = 0;
		for (int i = 0; i < sptedDevices->getCount(); ++i)
		{
			DeviceTreeNode const & node = sptedDevices->getItem(i);
			DeviceInformation devInfo(node.DeviceNumber);
			//			ErrorCode errorCode = instantDoCtrl->setSelectedDevice(devInfo);

			CString strTmp(node.Description);

			if (strTmp.Find(CMgrConfig::GetMgr()->GetDioName()) == 0)
			{
				m_confParam.deviceNumber = i;
				TCHAR_TO_WCHAR(_T(""), m_confParam.profilePath);
			}
		}
		sptedDevices->Dispose();
	}
	instantDiCtrl->Dispose();

	ConfDevice(1);

	//StartThread();

	SetAdLinkType(FALSE);

	//IF START...ALL CHANEEL SET TRUE (SERIAL MODE)
	int* pData = GetDO();

	for (int i = 0; i < 12; ++i)
	{
		if (pData[i] == TRUE)
			SetDIO(i, TRUE);

		Sleep(100);
	}

	GetDI();

	return TRUE;
}

/**
 * .
 *
 * METHOD_NAME : m_fnEtherCatDioOpen
 * METHOD_TYPE : -
 * DESCRIPTION : 2021-09-01 EtherCat DLL Load  add by KYJ
 * \return
 */
bool CMgrDio::m_fnDioOpenEtherCat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int netID = 0;
	int errCode;
	t_i32 ErrCode;
	TEcSlvTypeInfo pEcSlvtype;

	if (!secDll_Load())  //  DLL 로드
	{
		m_bDioOpen = FALSE;
		AfxMessageBox(_T("EtherCat DLL Load Fail"));
	}

	if (!secGn_LoadDevices(&ErrCode)) // 디바이스 로드
	{
		m_bDioOpen = FALSE;
		AfxMessageBox(_T("EtherCat Device Load Fail"));
	}

	//int NumNet = ecGn_GetNumNetworks(&ErrCode); //현재 포트 개수 얻어오는 함수

	//int Di1_NumCh, Di2_NumCh, Do1_NumCh, Do2_NumCh, Di1_StCh, Di2_StCh, Do1_StCh, Do2_StCh;

	//ecSlv_GetTypeInfo_A(netID, 0, &pEcSlvtype, &ErrCode);

	//Di1_NumCh = pEcSlvtype.DI.NumChannels;
	//Di1_StCh = pEcSlvtype.DI.StaGlobChannel;

	//ecSlv_GetTypeInfo_A(netID, 1, &pEcSlvtype, &ErrCode);

	//Do1_NumCh = pEcSlvtype.DO.NumChannels;
	//Do1_StCh = pEcSlvtype.DO.StaGlobChannel;

	return TRUE;
}

/**
 * .
 *
 * METHOD_NAME : m_fnDioOpenAjin
 * METHOD_TYPE : -
 * DESCRIPTION : gis
 * \return
 */
BOOL CMgrDio::m_fnDioOpenAjin()
{
	return TRUE;
}

// -------------------------------------------------------------------------------------------------

/**
 * .
 *
 * METHOD_NAME : ConfDevice
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
void CMgrDio::ConfDevice(int nType)
{
	ConfigureDevice(nType);
}

void CMgrDio::ConfigureDevice(int nType)
{
	ErrorCode	errorCode;
	DeviceInformation devInfo(m_confParam.deviceNumber);
	if (nType == 0)
	{
		errorCode = m_instantDoCtrl->setSelectedDevice(devInfo);
		CheckError(errorCode);
		// 	errorCode = m_instantDoCtrl->LoadProfile(_T(m_confParam.profilePath));
		// 	CheckError(errorCode);

		m_instantDoCtrl->getSelectedDevice(devInfo);

		m_portCount = m_instantDoCtrl->getFeatures()->getPortCount();
		//m_portData = new byte[m_portCount];

		BYTE bportData[2];

		errorCode = m_instantDoCtrl->Read(0, m_portCount, bportData/*m_portData*/);
		CheckError(errorCode);

		int portCount = 0;
		if (m_portCount < 9)
		{
			portCount = m_portCount;
		}
		else
		{
			if (m_portCount >= 9)
			{
				portCount = 8;
			}
		}

		CBitmap   bmp;
		byte portData;
		for (int i = 0; i < portCount; i++)
		{
			portData = 0;
			for (int j = 0; j < 12; j++)
			{
				portData = drawButton(i, j, errorCode, i, bportData/*m_portData*/);
			}
		}
		//delete[] m_portData;
	}
	else
	{
		errorCode = m_instantDiCtrl->setSelectedDevice(devInfo);
		CheckError(errorCode);
		// 	errorCode = m_instantDoCtrl->LoadProfile(_T(m_confParam.profilePath));
		// 	CheckError(errorCode);

		m_instantDiCtrl->getSelectedDevice(devInfo);

		m_portCount = m_instantDiCtrl->getFeatures()->getPortCount();
		//m_portData = new byte[m_portCount];
		BYTE bportData[2];


		errorCode = m_instantDiCtrl->Read(0, m_portCount, bportData/*m_portData*/);
		CheckError(errorCode);

		int portCount = 0;
		if (m_portCount < 9)
		{
			portCount = m_portCount;
		}
		else
		{
			if (m_portCount >= 9)
			{
				portCount = 8;
			}
		}

		CBitmap   bmp;
		byte portData;
		for (int i = 0; i < portCount; i++)
		{
			portData = 0;
			for (int j = 0; j < 12; j++)
			{
				portData = drawButtonIn(i, j, errorCode, i, bportData/*m_portData*/);
			}
		}
		//delete[] m_portData;
	}
}

int CMgrDio::GetCardType(CString strType)
{//pnx_hr 2021.05.11 DIO card Type return 
	int nDeviceType = 0;
	if (strType == "PCI-7200")
		nDeviceType = PCI_7200;
	else if (strType == "PCI-7230")
		nDeviceType = PCI_7230;
	else if (strType == "PCI-7250")
		nDeviceType = PCI_7250;
	else if (strType == "PCI-7432")
		nDeviceType = PCI_7432;
	else if (strType == "PCI-7433")
		nDeviceType = PCI_7433;
	else if (strType == "PCI-7434")
		nDeviceType = PCI_7434;
	else if (strType == "PCI-7296")
		nDeviceType = PCI_7296;
	else if (strType == "USB-1902")
		nDeviceType = USB_1902;
	else if (strType == "USB-1903")
		nDeviceType = USB_1903;
	else if (strType == "USB-1901")
		nDeviceType = USB_1901;
	else if (strType == "USB-2401")
		nDeviceType = USB_2401;
	else if (strType == "USB-7250")
		nDeviceType = USB_7250;
	else if (strType == "USB-7230")
		nDeviceType = USB_7230;
	else if (strType == "USB-2405")
		nDeviceType = USB_2405;
	else if (strType == "USB-2401")
		nDeviceType = USB_2401;

	return nDeviceType;
}

int CMgrDio::GetCardTypeint()
{
	return m_nCardType;
}

bool CMgrDio::m_fnDioADLINKControl(int nCardNum, int nCh, bool bOn)
{
	if (m_bDioOpen == FALSE)
	{
		//		LeaveCriticalSection(&m_csLockDio);  //20121019ksw, Critical Section Lock
		return FALSE;
	}

	bool bret = FALSE; // fail
	CString strTemp;
	U32 iRelayWirteCh = 0, iRelayReadCh = 0;
	//	iRelayWirteCh = (((U32)1L) << ichannel);
	iRelayWirteCh = 0x1 << nCh;
	if (m_iPciCard[nCardNum] >= 0)
	{
		if (bOn == TRUE) // relay ON //5-DMM-shield ALL CHECK
		{
			m_uOutPut = m_uOutPut | iRelayWirteCh;


		}
		else  // relay OFF
		{
			if (((iRelayWirteCh >> nCh & 0x1)) && ((m_uOutPut >> nCh) & 0x1))
			{
				m_uOutPut = m_uOutPut - iRelayWirteCh;

			}
		}

		UD_DO_WritePort((U16)nCardNum, 0, m_uOutPut);

		bret = TRUE; // sucess
	}
	else
	{
		strTemp.Format("[%d] Card not registered. Please check your DIO card!", nCardNum);
		//MessageBox(strTemp, "Error", MB_OK);
		bret = FALSE; // fail
		return bret;
	}

	Sleep(200);

	UD_DI_ReadPort((U16)nCardNum, 0, &m_uInPut);

	iRelayReadCh = (m_uInPut >> nCh) & 0x1;

	if (bOn == TRUE) // relay ON
	{
		if (iRelayReadCh == 0x1)
			bret = TRUE; // OK
		else
			bret = FALSE; //fail
		strTemp.Format("[DIO_ON ] CH=%d READ=%d (0x%04X)", nCh, iRelayReadCh, m_uInPut);
	}
	else  // relay OFF
	{
		if (iRelayReadCh == 0x0)
			bret = TRUE; // OK
		else
			bret = FALSE; //fail
		strTemp.Format("[DIO_OFF] CH=%d READ=%d (0x%04X)", nCh, iRelayReadCh, m_uInPut);
	}

	//	LeaveCriticalSection(&m_csLockDio); //20121019ksw, Critical Section Lock


	//	GetDlg_Dio_Control_Pointer()->DioView_LED(ichannel,bret);	//A≪μa¹øE￡ 0 , 26, A¤³I, ,0  RELAY ON
	strTemp.Format("[relay] ch=%02d, [0N(1)/OFF(0)]=%d ", nCh, bOn);


	return bret;
}

bool CMgrDio::m_fnDioStatus(int nCardNum, BYTE byStatus[50])
{

	bool bret = TRUE;
	int ich;
	if (m_bDioOpen == TRUE) {

		UD_DI_ReadPort((U16)nCardNum, 0, &m_uInPut);
		for (ich = 0; ich < 50; ich++)
		{
			byStatus[ich] = (m_uInPut >> ich) & 0x1;
		}
	}
	return bret;
}

// -----------------------------------------------------------------------------

//void	CMgrDio::StartThread()
//{
//	if (NULL == m_pDioThread)
//		m_pDioThread = AfxBeginThread(DioThread, this);
//}

//void	CMgrDio::StopThread()
//{
//	m_bLoop = FALSE;
//
//	if (m_pDioThread)
//	{
//		::WaitForSingleObject(m_pDioThread->m_hThread, INFINITE);
//
//		m_pDioThread = NULL;
//	}
//}

//UINT	CMgrDio::DioThread(LPVOID pParam)
//{
//	CMgrDio* pDioCom = (CMgrDio*)pParam;
//
//	while (pDioCom->m_bLoop)
//	{
//		if (CMgrConfig::GetMgr()->GetDioName().Compare(_T("USB-4750")) == 0)
//		{
//			// 			BYTE	byData[50];
//			// 			memset(byData, 0x00, sizeof(byData));
//			// 			
//			// 			if (pDioCom->m_nDOStatus[3] == 1)
//			// 			{
//			// 				CString str;
//			// 				str = "";
//			// 			}
//		}
//		else
//		{
//			BYTE	byData[50];
//			memset(byData, 0x00, sizeof(byData));
//
//			pDioCom->m_fnDioStatus(0, byData);
//
//			if (byData[Chamber_1] == TRUE)
//			{
//				CList< TComm*, TComm* >* listComm = CMgrComm::GetMgr()->GetCommList(CommObjChamber);
//
//				if (NULL != listComm)
//				{
//					TComm* pComm = listComm->GetHead();
//
//					if (pComm)
//					{
//						for (int i = 0; i < pComm->arrChannel.GetSize(); ++i)
//						{
//							int nChannelNumber = atoi(pComm->arrChannel.GetAt(i));
//
//							CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
//
//							if (NULL == pChannel)
//								continue;
//
//							pChannel->SendPause(PauseTypeComplete);
//						}
//					}
//				}
//
//				CDlgViewChamberAlarm::GetMgr()->ShowWindow(SW_SHOW);
//			}
//
//			if (byData[Chamber_2] == TRUE)
//			{
//				CList< TComm*, TComm* >* listComm = CMgrComm::GetMgr()->GetCommList(CommObjChamber);
//
//				if (NULL != listComm)
//				{
//					TComm* pComm = listComm->GetTail();
//
//					if (pComm)
//					{
//						for (int i = 0; i < pComm->arrChannel.GetSize(); ++i)
//						{
//							int nChannelNumber = atoi(pComm->arrChannel.GetAt(i));
//
//							CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
//
//							if (NULL == pChannel)
//								continue;
//
//							pChannel->SendPause(PauseTypeComplete);
//						}
//					}
//				}
//
//				CDlgViewChamberAlarm::GetMgr()->ShowWindow(SW_SHOW);
//			}
//
//			if (FALSE == byData[Chamber_1] && FALSE == byData[Chamber_2])
//			{
//				CDlgViewChamberAlarm*	pDlg = CDlgViewChamberAlarm::GetMgr();
//				if (pDlg && pDlg->GetSafeHwnd() && pDlg->IsWindowVisible())
//				{
//					pDlg->ShowWindow(SW_HIDE);
//				}
//				// 			if (CDlgViewChamberAlarm::GetMgr()->IsWindowVisible())
//				// 			{
//				// 				CDlgViewChamberAlarm::GetMgr()->ShowWindow(SW_HIDE);
//				// 			}
//			}
//
//		}
//
//		Sleep(500);
//	}
//
//	return 0;
//}

// -----------------------------------------------------------------------------

void CMgrDio::DIOAllOff(int nChannel)
{
	//DIO ALL OFF
	if (nChannel < 2)
	{
		for (int i = 0; i < 3; i++)
			SetDIO(i, FALSE);
	}
	else
	{
		for (int i = 3; i < 6; i++)
			SetDIO(i, FALSE);
	}
}

void CMgrDio::SetStatus(int nStatus, int nChannel)
{
	CString strStatus;
	strStatus.Empty();

	DIOAllOff(nChannel); //..°e¼O ²?´U°¡ AO´U°¡ CI´A ≫oE²...≫y°￠CØº¸¸e AIAu¿¡μμ AO¾uA½...³O³ª..?

	// 경광등하나로 두개의 충방전을 제어 --- 수정필요

	if (CMgrDio::GetMgr() == NULL)
		return;

	int nDivide = 0;
	if (nChannel >= 2)
		nDivide = 1;

	switch (nStatus)
	{
	case 0:
		m_bBuzzer = TRUE;
		SetDIO(3 * nDivide, TRUE);
		break;
	case 1:
		m_bBuzzer = TRUE;
		SetDIO(3 * nDivide + 1, TRUE);
		break;
	case 2:
		if (m_bBuzzer)
			SetDIO(3 * nDivide + 2, TRUE);
		else
			SetDIO(3 * nDivide + 2, FALSE);
		break;
	}
}

void CMgrDio::SetDIO(int nCh, BOOL bOn)
{
	if (CMgrDio::GetMgr() == NULL)
		return;

	CString strDioName = CMgrConfig::GetMgr()->GetDioName();

	if (strDioName.CompareNoCase("ADLINK") == 0)
	{
		m_fnDioADLINKControl(0, nCh, bOn);
	}
	else if (strDioName.CompareNoCase("ADVANTECH") == 0)
	{
		m_fnDioADVANTECHControl(nCh, bOn);
	}
	else if (strDioName.CompareNoCase("EtherCat") == 0)
	{
		m_fnDioEtherCatControl(nCh, bOn);
	}
}

bool CMgrDio::m_fnDioADVANTECHControl(int nCh, bool bOn)
{
	if (m_bDioOpen == FALSE)
	{
		//		LeaveCriticalSection(&m_csLockDio);  //20121019ksw, Critical Section Lock
		return FALSE;
	}
	else
	{
		int nPort = nCh / 8;

		m_nDOStatus[nCh] = bOn;

		byte portData = 0;
		for (int i = 0; i < 8; i++)
		{
			if (1 == m_nDOStatus[8 * nPort + i])
			{
				portData = (byte)(portData | (1 << i));
			}
		}

		CString portVal;
		portVal.Format(_T("%X"), portData);
		ErrorCode errorCode = m_instantDoCtrl->Write(nPort, 1, &portData);
		CheckError(errorCode);
		return TRUE;
	}
}

bool CMgrDio::m_fnDioEtherCatControl(int nCh, BOOL bOn)
{
	t_i32 ErrCode;

	if (m_bDioOpen == FALSE)
	{
		//		LeaveCriticalSection(&m_csLockDio);  //20121019ksw, Critical Section Lock
		return FALSE;
	}
	else
	{
		secdoPutOne(0, nCh, bOn, &ErrCode);
		return TRUE;
	}
}

bool CMgrDio::m_fnGetDioOutput(int nCh, bool* bOn)
{
	t_i32 ErrCode;

	if (m_bDioOpen == FALSE)
	{
		return FALSE;
	}
	else
	{
		*bOn = secdoGetOne(0, nCh, &ErrCode);
	}

	return TRUE;
}

int* CMgrDio::GetDO()
{
	ErrorCode	errorCode;
	m_portCount = m_instantDoCtrl->getFeatures()->getPortCount();
	//m_portData = new byte[m_portCount];
	BYTE bportData[2];

	errorCode = m_instantDoCtrl->Read(0, m_portCount, bportData/*m_portData*/);
	CheckError(errorCode);

	int portCount = 0;
	if (m_portCount < 9)
	{
		portCount = m_portCount;
	}
	else
	{
		if (m_portCount >= 9)
		{
			portCount = 8;
		}
	}

	CBitmap   bmp;
	byte portData;
	for (int i = 0; i < portCount; i++)
	{
		portData = 0;
		for (int j = 0; j < 12; j++)
		{
			portData = drawButton(i, j, errorCode, i, bportData/*m_portData*/);
		}
	}

	//delete [] m_portData;
	return m_nDOStatus;
}

void CMgrDio::GetDI()
{
	// 	m_portData = new byte[m_portCount];
	// 	memset(&m_portData, 0, sizeof(&m_portData));
	BYTE bportData[2];

	ErrorCode errorCode = m_instantDiCtrl->Read(0, m_portCount, bportData/*m_portData*/);

	CheckError(errorCode);

	CString portVal;

	int newBitValue = 0;
	int portCount = 0;
	if (m_portCount < 9)
	{
		portCount = m_portCount;
	}
	else
	{
		if (m_portCount > 9)
		{
			portCount = 8;
		}
	}
	for (int i = 0; i < portCount; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			if (j > 3)
			{
				//newBitValue = (m_portData!=NULL)?(m_portData[i] >> (j - 4)) & 0x1:1;
				newBitValue = (bportData[i] >> (j - 4)) & 0x1;
				if (newBitValue == 0)
				{
					m_nDIStatus[i * 8 + j - 4] = 0;
				}
				else
				{
					m_nDIStatus[i * 8 + j - 4] = 1;
				}
			}
		}
	}
	// 	if (m_portData != NULL)
	// 	{
	// 		delete[] m_portData;
	// 		m_portData = NULL;
	// 	}
}

void CMgrDio::CheckError(ErrorCode errorCode)
{
	if (errorCode != Success)
	{
		CString str;
		str.Format(_T("Some errors happened, the error code is:0x%x"), errorCode);
		SYSLOG(Dbg, str);
	}
}

byte CMgrDio::drawButton(int i, int j, ErrorCode ret, int  portNumber, byte * portValue)
{
	int dicBitValue;
	int dataBitValue;
	byte portData = 0;
	if (j > 3)
	{
		Array<uint8>* doDataMask = m_instantDoCtrl->getFeatures()->getDoDataMask();
		uint8 dataMask = doDataMask->getItem(portNumber);

		dicBitValue = (dataMask >> (j - 4)) & 0x1;
		dataBitValue = (portValue[portNumber] >> (j - 4)) & 0x1;

		if (Success == ret)
		{
			if (0 == dicBitValue)
			{
				m_nDOStatus[i * 8 + j - 4] = 2;
			}
			else
			{
				if (0 == dataBitValue)
				{
					m_nDOStatus[i * 8 + j - 4] = 0;
				}
				else
				{
					if (1 == dataBitValue)
					{
						m_nDOStatus[i * 8 + j - 4] = 1;
					}
				}
				if (1 == dicBitValue && 1 == dataBitValue)
				{
					portData = (byte)(portData | (1 << (j - 4)));
				}
			}
		}
		else
		{
			if (0 == dataBitValue)
			{
				m_nDOStatus[i * 8 + j - 4] = 0;
			}
			else
			{
				if (1 == dataBitValue)
				{
					m_nDOStatus[i * 8 + j - 4] = 1;
				}
			}
		}
		//		m_instantDoCtrl->Dispose();
	}
	return portData;
}

byte CMgrDio::drawButtonIn(int i, int j, ErrorCode ret, int  portNumber, byte * portValue)
{
	int dicBitValue;
	int dataBitValue;
	byte portData = 0;
	if (j > 3)
	{
		Array<uint8>* doDataMask = m_instantDiCtrl->getFeatures()->getDoDataMask();
		uint8 dataMask = doDataMask->getItem(portNumber);

		dicBitValue = (dataMask >> (j - 4)) & 0x1;
		dataBitValue = (portValue[portNumber] >> (j - 4)) & 0x1;

		if (Success == ret)
		{
			if (0 == dicBitValue)
			{
				m_nDIStatus[i * 8 + j - 4] = 2;
			}
			else
			{
				if (0 == dataBitValue)
				{
					m_nDIStatus[i * 8 + j - 4] = 0;
				}
				else
				{
					if (1 == dataBitValue)
					{
						m_nDIStatus[i * 8 + j - 4] = 1;
					}
				}
				if (1 == dicBitValue && 1 == dataBitValue)
				{
					portData = (byte)(portData | (1 << (j - 4)));
				}
			}
		}
		else
		{
			if (0 == dataBitValue)
			{
				m_nDIStatus[i * 8 + j - 4] = 0;
			}
			else
			{
				if (1 == dataBitValue)
				{
					m_nDIStatus[i * 8 + j - 4] = 1;
				}
			}
		}
		//		m_instantDoCtrl->Dispose();
	}
	return portData;
}

void CMgrDio::SetEQType(CString strType)
{
	//PreCharger
	if (strType == "DioConfig_PreCharge")
	{
		m_nEQType = enumEQType_Precharger;
	}
	//DCIR
	if (strType == "DioConfig_DCIR")
	{
		m_nEQType = enumEQType_DCIR;
	}
	//Formation
	if (strType == "DioConfig_2Floor_Hanwha")
	{
		m_nEQType = enumEQType_Formation;
	}
	//DFS Formation (PreCharger)
	if (strType == "DioConfig_DFS")
	{
		m_nEQType = enumEQType_DFS;
	}
}

void	CMgrDio::StartThread()
{
	if (NULL == m_pStatusLightThread)
		m_pStatusLightThread = AfxBeginThread(m_fnStatusLightThread, this);
}

void	CMgrDio::StopThread()
{
	m_bStatusLightLoop = FALSE;

	if (m_pStatusLightThread)
	{
		::WaitForSingleObject(m_pStatusLightThread->m_hThread, INFINITE);
		m_pStatusLightThread = NULL;
	}
}

void CMgrDio::SetBcrRead(BOOL nFlag)
{
	m_bSetBcrReadFlag = nFlag;
}

bool CMgrDio::GetBcrRead()
{
	return m_bSetBcrReadFlag;
}

void CMgrDio::SetMeasurementStep(int estepPress)
{
	m_ePressRunstepMeasurementCurrent = estepPress;
}

void CMgrDio::SetCenteringCylinder(int nStageNo, bool bForBack)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	if (GetEQType() == enumEQType_DCIR)
	{
		SetMoxa_DCIR_Tray_Centering_ForBack(bForBack);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		if (nStageNo == 0)
		{
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_CENTERING_FORWARD, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_CENTERING_BACKWARD, !bForBack);
		}
		else
		{

			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_CENTERING_FORWARD, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_CENTERING_BACKWARD, !bForBack);
		}
	}
}

void CMgrDio::SetClampCylinder(int nStageNo, bool bForBack)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	if (GetEQType() == enumEQType_DCIR)
	{
		SetMoxa_DCIR_Tray_Clamp_ForBack(bForBack);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		if (nStageNo == 0)
		{
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_CLAMP_CLAMP, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_CLAMP_UNCLAMP, !bForBack);
		}
		else
		{

			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_CLAMP_CLAMP, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_CLAMP_UNCLAMP, !bForBack);
		}
	}
}

//TRUE : UP FALSE: DOWN
void CMgrDio::SetProbeCylinder(int nStageNo, bool bUpDown)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	if (GetEQType() == enumEQType_DCIR)
	{

		SetMoxa_DCIR_Probe_UpDown(bUpDown);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		if (nStageNo == 0)
		{

			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_PROBE_UNIT_DOWN, !bUpDown);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_PROBE_UNIT_UP, bUpDown);
		}
		else
		{

			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_PROBE_UNIT_DOWN, !bUpDown);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_PROBE_UNIT_UP, bUpDown);
		}
	}
}

void CMgrDio::SetPusherCylinder(int nStageNo, bool bUpDown)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	if (GetEQType() == enumEQType_DCIR)
	{
		SetMoxa_DCIR_Pusher_UpDown(bUpDown);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		if (nStageNo == 0)
		{

			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_PUSHER_UNIT_DOWN, !bUpDown);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_PUSHER_UNIT_UP, bUpDown);
		}
		else
		{
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_PUSHER_UNIT_DOWN, !bUpDown);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_PUSHER_UNIT_UP, bUpDown);
		}

	}
}

void CMgrDio::SetJobChangeCylinder(int nStageNo, bool bForBack)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	if (GetEQType() == enumEQType_DCIR)
	{
		pMgrDio->SetOutPutIO(2, 12, !bForBack);
		pMgrDio->SetOutPutIO(2, 13, bForBack);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		if (nStageNo == 0)
		{
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_JOB_CHANGER_APPLY, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_JOB_CHANGER_NOT_APPLY, !bForBack);
		}
		else
		{
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_JOB_CHANGER_APPLY, bForBack);
			pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_JOB_CHANGER_NOT_APPLY, !bForBack);
		}
	}
}

bool CMgrDio::GetFireCheckSensor(int nStageNo)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bRet = FALSE;

	if (GetEQType() == enumEQType_DCIR)
	{
		bRet = pMgrDio->GetIOStatus(2, 4);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		bRet = pMgrDio->GetAjinInputData(nStageNo * 64 + INPUT_Hanwha_STAGE1_FIRE_CHECK_SENSOR);
	}
	return bRet;
}

bool CMgrDio::GetSmokeCheckSensor1(int nStageNo)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bRet = FALSE;

	if (GetEQType() == enumEQType_DCIR)
	{
		bRet = pMgrDio->GetIOStatus(2, 2);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		bRet = pMgrDio->GetAjinInputData(nStageNo * 64 + INPUT_Hanwha_STAGE1_SMOKE_CHECK_SENSOR_1);
	}
	return bRet;
}

bool CMgrDio::GetSmokeCheckSensor2(int nStageNo)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bRet = FALSE;

	if (GetEQType() == enumEQType_DCIR)
	{
		bRet = pMgrDio->GetIOStatus(2, 3);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		bRet = pMgrDio->GetAjinInputData(nStageNo * 64 + INPUT_Hanwha_STAGE1_SMOKE_CHECK_SENSOR_2);
	}
	return bRet;
}

bool CMgrDio::GetEMOCheckSensor(int nStageNo)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bRet = FALSE;

	if (GetEQType() == enumEQType_DCIR || GetEQType() == enumEQType_Precharger)
	{
		bRet = pMgrDio->GetIOStatus(0, 0);
	}
	if (GetEQType() == enumEQType_Formation)
	{

		if (nStageNo == 1)
		{
			bRet = pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_EMO_SW);
		}
		if (nStageNo == 0)
		{
			bRet = pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_EMO_SW);

		}
	}
	else if (GetEQType() == enumEQType_DFS)
	{

		if (nStageNo == 1)
		{
			bRet = pMgrDio->GetAjinInputData(INPUT_DFS_STAGE1_EMO_SW);
		}
		if (nStageNo == 0)
		{
			bRet = pMgrDio->GetAjinInputData(INPUT_DFS_STAGE2_EMO_SW);

		}
	}

	return bRet;
}

void CMgrDio::SetMoxa_PowerOnLamp(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 11, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 8, bLampOn);
	}

}

void CMgrDio::SetMoxa_PowerOffLamp(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 9, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 9, bLampOn);
	}
}

void CMgrDio::SetMoxa_PowerResetLamp(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 10, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 10, bLampOn);
	}
}

bool CMgrDio::GetMoxa_PowerOnSignal()
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 1);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 1);
	}	
}

bool CMgrDio::GetMoxa_PowerOffSignal()
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 2);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 2);
	}
}

bool CMgrDio::GetMoxa_PowerResetSignal()
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 3);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		return pMgrDio->GetIOStatus(0, 3);
	}
}

void CMgrDio::SetMoxa_TowerLamp_Red(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 12, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 12, bLampOn);
	}
}

void CMgrDio::SetMoxa_TowerLamp_YELLOW(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 13, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 13, bLampOn);
	}
}

void CMgrDio::SetMoxa_TowerLamp_GREEN(bool bLampOn)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 14, bLampOn);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 14, bLampOn);
	}
}

void CMgrDio::SetMoxa_TowerLamp_BUZZER(bool bBuzzer)
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(2, 15, bBuzzer);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio* pMgrDio = CMgrDio::GetMgr();
		pMgrDio->SetOutPutIO(4, 15, bBuzzer);
	}
}


//
void CMgrDio::SetMoxa_DCIR_Pusher_UpDown(bool bUpDown)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetOutPutIO(2, 8, !bUpDown);
	pMgrDio->SetOutPutIO(2, 9, bUpDown);
}
//
void CMgrDio::SetMoxa_DCIR_Probe_UpDown(bool bUpDown)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetOutPutIO(2, 10, !bUpDown);
	pMgrDio->SetOutPutIO(2, 11, bUpDown);
}
//
void CMgrDio::SetMoxa_DCIR_Tray_Centering_ForBack(bool bForBack)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetOutPutIO(2, 14, !bForBack);
	pMgrDio->SetOutPutIO(2, 15, bForBack);
}

void CMgrDio::SetMoxa_DCIR_Tray_Clamp_ForBack(bool bClampUnclamp)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetOutPutIO(3, 8, !bClampUnclamp);
	pMgrDio->SetOutPutIO(3, 9, bClampUnclamp);
}



static bool bBlink_Sw1 = FALSE;
static bool bBlink_Sw2 = FALSE;
void CMgrDio::Set_SwitchLamp()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	if (bSubStage1Use == TRUE)
	{
		int nMgrStatus1 = CMgrStatus::GetMgr()->GetEqpRunStatus(0);

		if (pMgrDio->GetConnect() == TRUE)
		{
			if (GetEQType() == enumEQType_DCIR || GetEQType() == enumEQType_Precharger)
			{
				//Error
				if (nMgrStatus1 == eCyclerChannelStateError || nMgrStatus1 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE)
				{
					SetMoxa_PowerOffLamp(FALSE);
					SetMoxa_PowerOnLamp(FALSE);
					//Reset
					if (GetMoxa_PowerResetSignal() == TRUE)
					{
						SetMoxa_PowerResetLamp(FALSE);
						SetMoxa_PowerOnLamp(FALSE);
						SetMoxa_PowerOffLamp(FALSE);
					}
					else
					{
						if (bBlink_Sw1)
						{
							SetMoxa_PowerResetLamp(TRUE);
							bBlink_Sw1 = FALSE;
						}
						else
						{
							SetMoxa_PowerResetLamp(FALSE);
							bBlink_Sw1 = TRUE;
						}
					}
				}
				else
				{
					//Case : Power On
					if (GetMoxa_PowerOnSignal() == TRUE)
					{
						SetMoxa_PowerOnLamp(TRUE);
						SetMoxa_PowerOffLamp(FALSE);
						SetMoxa_PowerResetLamp(FALSE);
					}
					//Case : Power Off
					if (GetMoxa_PowerOffSignal() == TRUE || GetEMOCheckSensor(0))
					{
						SetMoxa_PowerOffLamp(TRUE);
						SetMoxa_PowerOnLamp(FALSE);
						SetMoxa_PowerResetLamp(FALSE);
					}
					//Case : Power Reset
					if (GetMoxa_PowerResetSignal() == TRUE)
					{
						SetMoxa_PowerResetLamp(TRUE);
						SetMoxa_PowerOnLamp(FALSE);
						SetMoxa_PowerOffLamp(FALSE);
					}
				}
			}
			else if (GetEQType() == enumEQType_Formation)
			{
				//Error
				if (nMgrStatus1 == eCyclerChannelStateError || nMgrStatus1 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE)
				{

					pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, FALSE);
					pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
					//Reset
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_RESET_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
					}
					else
					{
						if (bBlink_Sw1)
						{
							pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, TRUE);
							bBlink_Sw1 = FALSE;
						}
						else
						{
							pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
							bBlink_Sw1 = TRUE;
						}
					}

				}
				else
				{
					//Case : Power On
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_POWER_ON_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, TRUE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
					}
					//Case : Power Off
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_POWER_OFF_SW) == TRUE || pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_EMO_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, TRUE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, FALSE);
					}
					//Case : Power Reset
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_RESET_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP, TRUE);
					}
				}
			}
			else if (GetEQType() == enumEQType_DFS)
			{
				m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Auto_Mode_Start, TRUE);

				//Main
				if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_MAIN_POWER_ON_SW))
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, TRUE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, FALSE);
				}
				if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_MAIN_POWER_OFF_SW))
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, TRUE);
				}
				//Stage1
				//Error
				if (nMgrStatus1 == eCyclerChannelStateError || nMgrStatus1 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE)
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
					//Reset
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_RESET_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
					}
					else
					{
						if (bBlink_Sw1)
						{
							m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, TRUE);
							bBlink_Sw1 = FALSE;
						}
						else
						{
							m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
							bBlink_Sw1 = TRUE;
						}
					}

				}
				else
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, TRUE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, TRUE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);

					//Power On
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_POWER_ON_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, TRUE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
					}
					//Power Off
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_POWER_OFF_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, TRUE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, FALSE);
					}
					//Reset
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE1_RESET_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_Reset_SW_LAMP, TRUE);
					}
				}
			}
		}
	}
	if (bSubStage2Use == TRUE)
	{
		int nMgrStatus2 = CMgrStatus::GetMgr()->GetEqpRunStatus(1);
		if (pMgrDio->GetConnect() == TRUE)
		{
			if (GetEQType() == enumEQType_Formation)
			{
				//Error
				if (nMgrStatus2 == eCyclerChannelStateError || nMgrStatus2 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == TRUE)
				{

					pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, FALSE);
					pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
					//Reset
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_RESET_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
					}
					else
					{
						if (bBlink_Sw2)
						{
							pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, TRUE);
							bBlink_Sw2 = FALSE;
						}
						else
						{
							pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
							bBlink_Sw2 = TRUE;
						}
					}

				}
				else
				{
					//Case : Power On
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_POWER_ON_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, TRUE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
					}
					//Case : Power Off
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_POWER_OFF_SW) == TRUE || pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_EMO_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, TRUE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, FALSE);
					}
					//Case : Power Reset
					if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE1_RESET_SW) == TRUE)
					{
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP, FALSE);
						pMgrDio->SetAjinOutputData(OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP, TRUE);
					}
				}
			}
			else if (GetEQType() == enumEQType_DFS)
			{
				//Stage2
				//Error
				if (nMgrStatus2 == eCyclerChannelStateError || nMgrStatus2 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == TRUE)
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);

					//Rest Blink
					if (bBlink_Sw2)
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, TRUE);
						bBlink_Sw2 = FALSE;
					}
					else
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);
						bBlink_Sw2 = TRUE;
					}
				}
				else
				{
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, TRUE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp, TRUE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp, FALSE);
					m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);

					//Power On
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE2_POWER_ON_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, TRUE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);
					}
					//Power Off
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE2_POWER_OFF_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, TRUE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, FALSE);
					}
					//Reset
					if (m_AjinIoControl->ReadInputIoBit(InputList_DFS_AJIN::INPUT_DFS_STAGE2_RESET_SW))
					{
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP, FALSE);
						m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE2_Reset_SW_LAMP, TRUE);
					}
				}
			}
		}
	}
}

static bool bBlink = FALSE;
void CMgrDio::Set_TowerLamp(int nStatus)
{
	if (GetEQType() == enumEQType_Formation)
	{
		for (int nTower = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTower < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTower++)
		{
			if (m_iStatusTowerLamp_Hanwha[nStatus][(int)nTower] != 2)
			{
				m_AjinIoControl->WriteOutputIoBit(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_TOWER_RED_LAMP + nTower, m_iStatusTowerLamp_Hanwha[nStatus][nTower]);
			}
			else
			{
				m_AjinIoControl->WriteOutputIoBit(OutputList_Hanwha_AJIN::OUTPUT_Hanwha_STAGE1_TOWER_RED_LAMP + nTower, bBlink);
			}
		}
	}
	else if (GetEQType() == enumEQType_DCIR)
	{
		for (int nTower = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTower < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTower++)
		{
			if (m_iStatusTowerLamp_Hanwha[nStatus][(int)nTower] != 2)
			{
				SetOutPutIO(4, 12 + nTower, m_iStatusTowerLamp_Hanwha[nStatus][(int)nTower]);
			}
			else
			{
				SetOutPutIO(4, 12 + nTower, bBlink);
			}
		}
	}
	else if (GetEQType() == enumEQType_Precharger)
	{
		for (int nTower = (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_Red; nTower < (int)enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count; nTower++)
		{
			if (m_iStatusTowerLamp_Hanwha[nStatus][(int)nTower] != 2)
			{
				SetOutPutIO(2, 12 + nTower, m_iStatusTowerLamp_Hanwha[nStatus][(int)nTower]);
			}
			else
			{
				SetOutPutIO(2, 12 + nTower, bBlink);
			}
		}
	}
	else if (GetEQType() == enumEQType_DFS)
	{
		for (int nTower = (int)enumTowerLampUnit_DFS::enumTowerDFS_Red; nTower < (int)enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count; nTower++)
		{
			if (m_iStatusTowerLamp_DFS[nStatus][(int)nTower] != 2)
			{
				m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_RED_Lamp + nTower, m_iStatusTowerLamp_DFS[nStatus][nTower]);
			}
			else
			{
				m_AjinIoControl->WriteOutputIoBit(OutputList_DFS_AJIN::OUTPUT_DFS_STAGE1_TowerLamp_RED_Lamp + nTower, bBlink);
			}
		}
	}
	if (bBlink)
	{
		bBlink = FALSE;

	}
	else
	{
		bBlink = TRUE;

	}
}

bool CMgrDio::GetMoxa_DCIR_TrayCheckSensor()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	return pMgrDio->GetIOStatus(1, 0);
}

bool CMgrDio::GetMoxa_DCIR_TrayInterlockCheckSensor()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	return pMgrDio->GetIOStatus(0, 7);
}




BOOL CMgrDio::Fan_Status_Check(int nFanIndex, int nCheckTime)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	ULONGLONG ulTime = 0;
	CString strTemp;
	bool bFanStatus = pMgrDio->GetFanStatus(nFanIndex);

	if (bFanStatus)
	{
		m_ullastFanStatusOnLastTime[nFanIndex] = ulTime;
	}

	if (ulTime - m_ullastFanStatusOnLastTime[nFanIndex] >= nCheckTime)
	{
		ALARM_ITEM item;
		strTemp.Format(_T("Please check the fan status(Stage %d , Fan %d)"), (nFanIndex / 8 + 1), (nFanIndex % 8 + 1));
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_STAGE1_FAN_STATUS_ALARM + (nFanIndex / 8), strTemp, ALARM_LIGHT, strTemp, &item);
		return FALSE;
	}
	return TRUE;
}
void CMgrDio::Set_FanStatus(int nFanIndex, bool bFanSatus)
{
	m_bFanSatus[nFanIndex / 8][nFanIndex % 8] = bFanSatus;
}
BOOL CMgrDio::Get_FanStatus(int nStageNo, int nFanIndex)
{
	return m_bFanSatus[nStageNo][nFanIndex];
}
BOOL CMgrDio::Get_Alarm_FanStatus(int nStageNo, int nAlarmCount)
{
	int nFanAlarm = 0;

	for (int nFanIndex = 0; nFanIndex < 8; nFanIndex++)
	{
		if (Get_FanStatus(nStageNo, nFanIndex) == FALSE)
		{
			nFanAlarm++;
			if (nFanAlarm == nAlarmCount)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
void CMgrDio::Update_TowerLamp()
{	

	int nMgrStatus1 = CMgrStatus::GetMgr()->GetEqpRunStatus(0);
	int nMgrStatus2 = CMgrStatus::GetMgr()->GetEqpRunStatus(1);
	CMgrPCan* pMgr = CMgrPCan::GetMgr();

	BOOL bMgrStage1Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(0);
	BOOL bMgrStage2Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(1);

	if (GetEQType() == enumEQType_Formation|| GetEQType() == enumEQType_DCIR|| GetEQType() == enumEQType_Precharger)
	{
		//Alarm
		if (nMgrStatus1 == eCyclerChannelStateError || nMgrStatus1 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE || bMgrStage1Alarm == TRUE
			|| nMgrStatus2 == eCyclerChannelStateError || nMgrStatus2 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == TRUE || bMgrStage2Alarm == TRUE)
		{
			Set_TowerLamp((int)enumEQHanwha_Alarm);
		}
		//Stop
		else if (nMgrStatus1 == eCyclerChannelStatePause || nMgrStatus2 == eCyclerChannelStatePause)
		{

			Set_TowerLamp((int)enumEQHanwha_Stop);
		}
		//Complete
		else if (nMgrStatus1 == eCyclerChannelStateComplete && nMgrStatus2 == eCyclerChannelStateComplete)
		{
			Set_TowerLamp((int)enumEQHanwha_Complete);
		}
		//Run
		else if (nMgrStatus1 == eCyclerChannelStateStart && nMgrStatus2 == eCyclerChannelStateComplete)
		{
			Set_TowerLamp((int)enumEQHanwha_Run);
		}
		//Ready
		else if (nMgrStatus1 == eCyclerChannelStateReady && nMgrStatus2 == eCyclerChannelStateReady)
		{
			Set_TowerLamp((int)enumEQHanwha_Ready);
		}
		//Idle
		else
		{
			Set_TowerLamp((int)enumEQHanwha_Idle);
		}
	}
	else if (GetEQType() == enumEQType_DFS)
	{
		//Alarm
		if (nMgrStatus1 == eCyclerChannelStateError || nMgrStatus1 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE || bMgrStage1Alarm == TRUE
			|| nMgrStatus2 == eCyclerChannelStateError || nMgrStatus2 == eCyclerChannelStateCommuicationError || CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == TRUE || bMgrStage2Alarm == TRUE)
		{
			Set_TowerLamp((int)enumEQDFS_Alarm);
		}
		//Stop
		else if (nMgrStatus1 == eCyclerChannelStatePause || nMgrStatus2 == eCyclerChannelStatePause)
		{

			((int)enumEQHanwha_Stop);
		}
		//Complete
		else if (nMgrStatus1 == eCyclerChannelStateComplete && nMgrStatus2 == eCyclerChannelStateComplete)
		{
			Set_TowerLamp((int)enumEQDFS_Complete);
		}
		//Run
		else if (nMgrStatus1 == eCyclerChannelStateStart && nMgrStatus2 == eCyclerChannelStateComplete)
		{
			Set_TowerLamp((int)enumEQDFS_Run);
		}
		//Ready
		else if (nMgrStatus1 == eCyclerChannelStateReady && nMgrStatus2 == eCyclerChannelStateReady)
		{
			Set_TowerLamp((int)enumEQDFS_Ready);
		}
		//Idle
		else
		{
			Set_TowerLamp((int)enumEQDFS_Idle);
		}
	}
}
//A¼³I ≫oAA¿¡ μu¸￥ °æ±¤μi A|¾i
UINT	CMgrDio::m_fnStatusLightThread(LPVOID pParam)
{
	CMgrDio* pMgrDio = (CMgrDio*)pParam;
	CMgrModel* pMgrModel = CMgrModel::GetMgr();
	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	bool bHearBeatIO = FALSE;
	pMgrDio->m_bStatusLightLoop = TRUE;

	while (pMgrDio->m_bStatusLightLoop)
	{
		//Precharger 인 경우
		if (pMgrDio->GetEQType() == enumEQType_Precharger)
		{
			//Connect Status
			if (pMgrDio->GetConnect() != TRUE)
			{
				//Communication Status :: HearBeat Off, Ready Off
				//Heart Bit
				pMgrDio->SetMoxa_PC_HeartBit(FALSE);
				//Ready Bit
				pMgrDio->SetMoxa_PC_Ready(FALSE);

				return 0;
			}

			for (int i = 0; i < IO_SLOT_COUNT; i++)
			{
				pMgrDio->MoxaIoRead(i);
			}
			//Heart Bit
			//Pc -> PLC HeartBeat IO 0.5 Toggle Signal
			bHearBeatIO = pMgrDio->GetIOStatus(1, 8);
			pMgrDio->SetMoxa_PC_HeartBit(!bHearBeatIO);

			//PLC Heart Bit Check
			pMgrDio->PLC_Heart_Bit_Check(::GetTickCount64() / 1000);

			//Alarm Status :: Ready Off
			int nMgrStatus1 = CMgrStatus::GetMgr()->GetEqpRunStatus(0);
			int nMgrStage1Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(0);
			//Heavy Alarm Case				
			//case 1 : Inverter Alarm
			//case 2 : All Channel Error
			if (CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == TRUE || nMgrStatus1 == eCyclerChannelStateError|| nMgrStage1Alarm == ALARM_HEAVY)
			{
				//Ready Bit off
				pMgrDio->SetMoxa_PC_Ready(FALSE);
				pMgrDio->SetMoxa_PC_Busy(FALSE);
			}
			else
			{
				//Ready Bit on
				pMgrDio->SetMoxa_PC_Ready(TRUE);

				//Start 인 경우 PC -> PLC Busy Bit On
				if (nMgrStatus1 == eCyclerChannelStateStart)
				{
					pMgrDio->m_AutoCompleteFlag = FALSE;
					pMgrDio->SetMoxa_PC_Busy(TRUE);
					CMgrPlcInterface* pMgrPlcInterface = CMgrPlcInterface::GetMgr();

					if (pMgrDio->GetMoxa_ChamberDoorOpen() == TRUE && pMgrPlcInterface->Get_AutoMode() == TRUE)
					{
						ALARM_ITEM item;

						CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_CHAMBER_DOOR_OPEN_ERROR, "Chamber Dool Status Check Please", ALARM_LIGHT, "Chamber Dool Status Check Please", &item);
					}
				}
				//Pause 인 경우 PC -> PLC Busy Bit Off
				else if (nMgrStatus1 == eCyclerChannelStatePause ||
					nMgrStatus1 == eCyclerChannelStatePauseFromDwell ||
					nMgrStatus1 == eCyclerChannelStatePauseFromWaitChamber ||
					nMgrStatus1 == eCyclerChannelStatePauseFromTempSync ||
					nMgrStatus1 == eCyclerChannelStatePauseFromStepSync)
				{
					pMgrDio->SetMoxa_PC_Busy(FALSE);
				}
				//Complete인 경우 PC-> PLC Work Complete On (0.5초) -> Work Complete Off, Busy Off
				else if (nMgrStatus1 == eCyclerChannelStateComplete)
				{
					if (pMgrDio->m_AutoCompleteFlag != TRUE)
					{
						pMgrDio->m_AutoCompleteFlag = TRUE;
						pMgrDio->SetMoxa_PC_WorkComplete(TRUE);
						Sleep(100);
						pMgrDio->SetMoxa_PC_WorkComplete(FALSE);
						pMgrDio->SetMoxa_PC_Busy(FALSE);
					}

				}
			}
			//팬 상태 확인
			for (int nFanIndex = 0; nFanIndex < 8; nFanIndex++)
			{
				pMgrDio->Set_FanStatus(nFanIndex, pMgrDio->Fan_Status_Check(nFanIndex, 300));
			}

		}
		//DCIR 인 경우
		else if (pMgrDio->GetEQType() == enumEQType_DCIR)
		{
			//Connect Status
			if (pMgrDio->GetConnect() != TRUE)
			{
				return 0;
			}

			for (int i = 0; i < IO_SLOT_COUNT; i++)
			{
				pMgrDio->MoxaIoRead(i);
			}
			//팬 상태 확인
			for (int nFanIndex = 0; nFanIndex < 8; nFanIndex++)
			{
				pMgrDio->Set_FanStatus(nFanIndex, pMgrDio->Fan_Status_Check(nFanIndex, 300));
			}
		}
		//Formation인 경우
		else if (pMgrDio->GetEQType() == enumEQType_Formation)
		{
			//Connect Status
			if (pMgrDio->GetConnect() != TRUE)
			{
				return 0;
			}
			//팬 상태 확인
			for (int nFanIndex = 0; nFanIndex < 16; nFanIndex++)
			{
				pMgrDio->Set_FanStatus(nFanIndex, pMgrDio->Fan_Status_Check(nFanIndex, 300));
			}
		}

		//스위치, 타워램프 제어
		pMgrDio->Set_SwitchLamp();
		pMgrDio->Update_TowerLamp();
		Sleep(100);
	}
	return 0;
}

void CMgrDio::PLC_Heart_Bit_Check(ULONGLONG ulTime)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	bool bHearBeatIO = pMgrDio->GetMoxa_PLC_HeartBeat();

	if (bHearBeatIO)
	{
		m_ullastPLCHeartBitReceiveTime = ulTime;
	}

	if (ulTime - m_ullastPLCHeartBitReceiveTime >= 20)
	{
		ALARM_ITEM item;
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_PLC_HEARTBIT_RECEIVE_ERROR, "PLC Heart Bit Check Please", ALARM_LIGHT, "PLC Heart Bit Check Please", &item);
	}
}



// -------------------------------------------------------------------------------------------------

SUBEQP_DIO_INFO CMgrDio::GetSubEqpDioInfo(CString subEqpId)
{
	for (int nIndexCount = 0; nIndexCount < EQP_SUBEQP_MAX_COUNT; nIndexCount++)
	{
		if (subEqpId.Compare(m_SubEqpDioInfo[nIndexCount].m_strSubEqpId) == 0)
		{
			return m_SubEqpDioInfo[nIndexCount];
		}
	}
	return SUBEQP_DIO_INFO();
}

/**
 * .
 *
 * METHOD_NAME : GetAjinInputData
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
bool CMgrDio::GetAjinInputData(int adress)
{
	m_AjinIoControl->ReadInputIoBit(adress);
	if (GetEQType() == enumEQType_Formation)
	{
		return m_AjinIoControl->m_bInputIo_Hanwha[adress];
	}
	else if (GetEQType() == enumEQType_DFS)
	{
		return m_AjinIoControl->m_bInputIo_DFS[adress];
	}
}

/**
 * .
 *
 * METHOD_NAME : GetAjinOutputData
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
bool CMgrDio::GetAjinOutputData(int adress)
{
	m_AjinIoControl->ReadOutputIoBit(adress);

	if (GetEQType() == enumEQType_Formation)
	{
		return m_AjinIoControl->m_bOutputIo_Hanwha[adress];
	}
	else if (GetEQType() == enumEQType_DFS)
	{
		return m_AjinIoControl->m_bOutputIo_DFS[adress];
	}


}

/**
 * .
 *
 * METHOD_NAME : SetAjinOutputData
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
bool CMgrDio::SetAjinOutputData(int address, bool value)
{
	if (value == TRUE)
	{
		if (GetAjinOutputData(address) == FALSE)
		{
			m_AjinIoControl->WriteOutputIoBit(address, value);
		}
	}
	else
	{
		if (GetAjinOutputData(address) == TRUE)
		{
			m_AjinIoControl->WriteOutputIoBit(address, value);
		}
	}

	return FALSE;
}

/**
 * .
 *
 * METHOD_NAME : IsGetAjinOutputData
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
bool CMgrDio::IsGetAjinOutputData(int address, bool value)
{
	bool returnFlag = FALSE;

	if (value == GetAjinOutputData(address))
	{
		returnFlag = TRUE;
	}

	return returnFlag;
}

// -------------------------------------------------------------------------------------------------

bool CMgrDio::GetFanStatus(int nIndex)
{
	int nStartIndex = 0;

	if (GetEQType() == enumEQType_DCIR || GetEQType() == enumEQType_Precharger)
	{
		nStartIndex = 8;

		CMgrDio* pMgrDio = CMgrDio::GetMgr();

		nIndex = nStartIndex + nIndex;

		return pMgrDio->GetIOStatus(0, nIndex);
	}
	if (GetEQType() == enumEQType_Formation)
	{
		nStartIndex = INPUT_Hanwha_STAGE1_FAN1_STATUS;

		nIndex = nStartIndex + ((nIndex / 8) * 64);

		m_AjinIoControl->ReadOutputIoBit(nIndex);
		return m_AjinIoControl->m_bInputIo_Hanwha[nIndex];
	}
}

void CMgrDio::SetConnect(bool bConnect)
{
	m_bConnect = bConnect;
}

bool CMgrDio::GetConnect()
{
	CString strTemp = "";
	if (m_bConnect != TRUE)
	{
		ALARM_ITEM item;
		strTemp.Format(_T("IO Board DisConnect "));		
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_IO_BOARD_DISCONNECT, strTemp, ALARM_HEAVY,"Please check the connection status of the IO Board", &item);
		return FALSE;
	}
	return TRUE;

}

void CMgrDio::MoxaIOConnect()
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig == NULL)
		return;
	//int nModuleCount = 0;
	int nConnectCount = 0;


	//if (GetEQType() == enumEQType_DCIR)
	//{
	//	//Test 
	//	MoxaIOBoard[IO_SLOT_INPUT1].strIP = "192.168.127.1";
	//	MoxaIOBoard[IO_SLOT_INPUT1].nChCount = 16;
	//	nModuleCount++;
	//	MoxaIOBoard[IO_SLOT_INPUT2].strIP = "192.168.127.2";
	//	MoxaIOBoard[IO_SLOT_INPUT2].nChCount = 16;
	//	nModuleCount++;
	//	MoxaIOBoard[IO_SLOT_INOUT1].strIP = "192.168.127.3";
	//	MoxaIOBoard[IO_SLOT_INOUT1].nChCount = 8;
	//	nModuleCount++;
	//	MoxaIOBoard[IO_SLOT_INOUT2].strIP = "192.168.127.4";
	//	MoxaIOBoard[IO_SLOT_INOUT2].nChCount = 8;
	//	nModuleCount++;
	//	MoxaIOBoard[IO_SLOT_INOUT3].strIP = "192.168.127.5";
	//	MoxaIOBoard[IO_SLOT_INOUT3].nChCount = 8;
	//	nModuleCount++;
	//}
	//else if (GetEQType() == enumEQType_Precharger)
	//{
	//	MoxaIOBoard[0].strIP = "192.168.127.1";
	//	MoxaIOBoard[0].nChCount = 16;
	//	nModuleCount++;
	//	MoxaIOBoard[1].strIP = "192.168.127.2";
	//	MoxaIOBoard[1].nChCount = 8;
	//	nModuleCount++;
	//	MoxaIOBoard[2].strIP = "192.168.127.3";
	//	MoxaIOBoard[2].nChCount = 8;
	//	nModuleCount++;

	//}
	//Init  
	CString strMsg = _T("");
	int iRet = MXEIO_Init();
	CheckErr(iRet, "MXEIO_Init");
	if (iRet == MXIO_OK)
	{
		//printf("MXEIO_Init Initiate the socket succeed.\n");
		SYSLOG(Dbg, _T("MXEIO_Init Initiate the socket succeed."));
	}
	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	//Connect
	for (int i = 0; i < pMgrConfig->m_nDioCount; i++) {

		DWORD nPort = 502;

		char IPAddress[16] = { '\0' };
		dwTimeOut = 1000;
		bytInputValue[32] = { '\0' };

		strcpy(IPAddress, vecValue[i]->strIP);

	/*	MoxaIOBoard[i].strIP = vecValue[i]->strIP;
		MoxaIOBoard[i].nChCount = vecValue[i]->nChCount;*/

		Password[8] = { '\0' };


		iRet = MXEIO_E1K_Connect(IPAddress,		//IP address
			nPort,					//TCP port number
			dwTimeOut,			//timeout
			&vecValue[i]->nHandle,				//connection handle
			Password);			//ligin password
		CheckErr(iRet, "MXEIO_E1K_Connect");
		if (iRet == MXIO_OK)
		{
			vecValue[i]->bConnect = TRUE;
			CMgrConfig::GetMgr()->SetMoxaIoInfor(i, vecValue);
			nConnectCount++;
			SYSLOG(Dbg, _T("MXEIO_E1K_Connect Creates Adapter IP=%s connection succeed. TimeOut=%d, Password=%s"), IPAddress, dwTimeOut, Password);
		}
		//Sleep(300);
	}
	if (nConnectCount == pMgrConfig->m_nDioCount)
	{
		CMgrDio::GetMgr()->SetMoxa_PowerOnLamp(TRUE);
		SetConnect(TRUE);
		StartThread();
		return;

	}
}

void CMgrDio::MoxaIOCheckConnection(int nIONumber)
{
	CString strMsg = _T("");
	iHandle = 0;
	dwTimeOut = 5000;
	bytStatus = 0;

	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);

	//This function establishes a connection to the port of the Ethernet I/O server.
	iRet = MXEIO_CheckConnection(vecValue[nIONumber]->nHandle,			//Connection handle
		dwTimeOut,		//check connection timeout(ms)
		&bytStatus);		//Connection Status
	CheckErr(iRet, "MXEIO_CheckConnection");
	switch (bytStatus)
	{
	case CHECK_CONNECTION_OK:
	{
		strMsg.Format(_T("Check connection ok !"));
		//		printf("Check connection ok ! \n");
		break;
	}
	case CHECK_CONNECTION_FAIL:
		strMsg.Format(_T("Check connection ok !"));
		//printf("Check connection ok !\n");
		break;
	case CHECK_CONNECTION_TIME_OUT:
		strMsg.Format(_T("Check connection time out !"));
		//printf("Check connection time out !\n");
	}
	SYSLOG(Dbg, strMsg);
}

void CMgrDio::MoxaIOClose()
{
	int nModuleCount = 0;

	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);

	if (GetEQType() == enumEQType_DCIR)
	{
		nModuleCount = 5;
	}
	else if (GetEQType() == enumEQType_Precharger)
	{
		nModuleCount = 3;
	}
	CString strMsg = _T("");
	for (int i = 0; i < nModuleCount; i++) {

		int iRet = MXEIO_Disconnect(vecValue[i]->nHandle);
		CheckErr(iRet, "MXEIO_Disconnect");
		if (iRet == MXIO_OK)
		{
			//MoxaIOBoard[IO_SLOT_INPUT1].bConnect = FALSE;

			SYSLOG(Dbg, _T("Disconnect module %d connection succeeded."));
		}
	}

	SetConnect(FALSE);

}

void CMgrDio::SetMoxa_Init()
{
	if (GetEQType() == enumEQType_Precharger)
	{
		CMgrDio::GetMgr()->SetMoxa_PC_HeartBit(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PC_Ready(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PC_Busy(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PC_WorkComplete(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PowerOnLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PowerOffLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PowerResetLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_Red(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_YELLOW(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_GREEN(FALSE);
	}
	if (GetEQType() == enumEQType_DCIR)
	{
		CMgrDio::GetMgr()->SetMoxa_PowerOnLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PowerOffLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_PowerResetLamp(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_Red(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_YELLOW(FALSE);
		CMgrDio::GetMgr()->SetMoxa_TowerLamp_GREEN(FALSE);
	}
}

bool CMgrDio::GetMoxa_PLC_HeartBeat()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	return pMgrDio->GetIOStatus(1, 1);
}
bool CMgrDio::GetMoxa_PLC_Ready()
{

	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	if (pMgrDio->GetConnect() == TRUE)
	{
		return pMgrDio->GetIOStatus(1, 2);
	}
}

bool CMgrDio::GetMoxa_PLC_Start()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	return pMgrDio->GetIOStatus(1, 3);
}

bool CMgrDio::GetMoxa_ChamberDoorOpen()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	return pMgrDio->GetIOStatus(1, 0);
}


void CMgrDio::SetMoxa_PC_HeartBit(bool bHeartBit)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetIO(1, 8, bHeartBit);
	pMgrDio->MoxaIoWrite(1);
}

void CMgrDio::SetMoxa_PC_Ready(bool bReadyBit)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	pMgrDio->SetIO(1, 9, bReadyBit);
	pMgrDio->MoxaIoWrite(1);
}

void CMgrDio::SetMoxa_PC_Busy(bool bBusyBit)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	pMgrDio->SetIO(1, 10, bBusyBit);
	pMgrDio->MoxaIoWrite(1);
}

void CMgrDio::SetMoxa_PC_WorkComplete(bool bCompleteBit)
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	pMgrDio->SetIO(1, 11, bCompleteBit);
	pMgrDio->MoxaIoWrite(1);

}

bool CMgrDio::GetMoxa_PC_Busy()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	bool bPCBusy = FALSE;

	bPCBusy = pMgrDio->GetOutIoStatus(1, 10);
	return bPCBusy;
}


BOOL CMgrDio::MoxaIoRead(int nIONumber)
{
	CString strMsg = _T("");
	bytStartChannel = 0;
	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	for (int k = 0; k < vecValue[nIONumber]->nChCount; k++)
	{
		wValue[k] = 0;
	}
	iRet = E1K_DI_SetModes(vecValue[nIONumber]->nHandle, bytStartChannel, vecValue[nIONumber]->nChCount, wValue);
	CheckErr(iRet, "E1K_DI_SetModes");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DI_SetModes succeed.\n");
		for (int i = bytStartChannel; i < vecValue[nIONumber]->nChCount; i++)
		{
			//SYSLOG(Dbg, _T("ch[%d]=%s\n"), i - bytStartChannel, wValue[i - bytStartChannel] ? "Count Mode" : "DI mode");
		}
	}
	////==========================

	iRet = E1K_DI_Reads(vecValue[nIONumber]->nHandle, bytStartChannel, vecValue[nIONumber]->nChCount, dwValue);
	CheckErr(iRet, "E1K_DI_Reads");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DI_Reads succeed.\n");
		for (int i = vecValue[nIONumber]->nChCount - 1; i >= bytStartChannel; i--)
		{
			//SYSLOG(Dbg, _T("ch[%d] = %d\n"), i - bytStartChannel, (dwValue[bytStartChannel] & (1 << (i - bytStartChannel))) >> i);
			m_bIO[nIONumber][i] = (dwValue[bytStartChannel] & (1 << (i - bytStartChannel))) >> i;
		}
	}
	return TRUE;

}

void CMgrDio::MoxaIoWrite(int nIONumber)
{
	Sleep(1);
	CString strMsg = _T("");
	bytStartChannel = 0;
	bytCount = 16;
	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	for (int k = 0; k < bytCount; k++)
		wValue[k] = 0;
	//printf("***** Write DO Direction to DO Mode (0:DO Mode) ***** : ");
	iRet = E1K_DO_SetModes(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_SetModes");
	if (iRet == MXIO_OK)
	{
		//SYSLOG(Dbg, _T("E1K_DO_SetModes success,All DO %d Channel: %s\r\n"), bytCount, (wValue[0] > 0) ? "Pulse Mode" : "DO Mode");
	}
	//==========================
	//==========================
	iRet = E1K_DO_GetModes(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_GetModes");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DO_GetModes success,Channel Value:\r\n");
		for (int k = 0; k < bytCount; k++)
		{
			if ((k != 0) && ((k % 4) == 0))
			{
				//SYSLOG(Dbg, _T("Ch[%02d]: %s, "), k, (wValue[k] > 0) ? "Pulse Mode" : "DO Mode");

			}
		}
		printf("\r\n");
	}
	//==========================
	DWORD dwDoGetValue = 0;
	//printf("Set DO PowerOn Value (0=OFF or 1=ON):");
	//gets(bytInputValue);



	if (atoi(bytInputValue) == 1)
		dwDoGetValue = 0xFFFF;
	else
		dwDoGetValue = 0;



	iRet = E1K_DO_SetPowerOnValues(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, dwDoGetValue);
	CheckErr(iRet, "E1K_DO_SetPowerOnValues");
	if (iRet == MXIO_OK)
	{
		//SYSLOG(Dbg, _T("E1K_DO_SetPowerOnValues success,Set all DO Power On Value = %s\r\n"), (dwDoGetValue == 0) ? "OFF" : "ON");
	}
	//==========================
	printf("Get DO PowerOn Value (0=OFF or 1=ON):");
	iRet = E1K_DO_GetPowerOnValues(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, &dwDoGetValue);
	CheckErr(iRet, "E1K_DO_GetPowerOnValues");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DO_GetPowerOnValues success,\r\n");
		for (int k = 0; k < bytCount; k++)
		{
			//SYSLOG(Dbg, _T("Get Power On DO[%d]= %s\r\n"), k, ((dwDoGetValue & (0x01 << k)) > 0) ? "ON" : "OFF");
		}
	}
	//==========================
	//printf("Set DO Safe Value (0=OFF, 1=ON or 2: Hold Last):");
	//gets(bytInputValue);
	if (atoi(bytInputValue) <= 2)
	{
		for (int k = 0; k < bytCount; k++)
			wValue[k] = atoi(bytInputValue);
	}
	else
	{
		for (int k = 0; k < bytCount; k++)
			wValue[k] = 0;
	}
	iRet = E1K_DO_SetSafeValues_W(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_SetSafeValues_W");
	if (iRet == MXIO_OK)
	{
		if (wValue[0] < 2)
		{
			//SYSLOG(Dbg, _T("E1K_DO_SetSafeValues_W success,Set all DO Safe Value = %s\r\n"), (wValue[0] == 0) ? "OFF" : "ON");
		}
		else
		{
			//SYSLOG(Dbg, _T("E1K_DO_SetSafeValues_W success,Set all DO Safe Value = %s\r\n", "Hold Last"));
		}
	}
	//==========================
	//printf("Get DO Safe Value (0=OFF, 1=ON or 2: Hold Last):");
	iRet = E1K_DO_GetSafeValues_W(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_GetSafeValues_W");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DO_GetSafeValues_W success,\r\n");
		for (int k = 0; k < bytCount; k++)
		{
			if (wValue[k] < 2)
			{
				//SYSLOG(Dbg, _T("Get Safe DO[%d]= %s\r\n"), k, (wValue[k] == 0) ? "OFF" : "ON");
			}
			else if (wValue[k] == 2)
			{
				//SYSLOG(Dbg, _T("Get Safe DO[%d]= %s\r\n"), k, "Hold Last");
			}
			else
			{
				//SYSLOG(Dbg, _T("Get Safe DO[%d]= %s (%d)\r\n"), k, "Unknown Value", wValue[k]);
			}
		}
	}
	//==========================
	//printf("***** Enter power on sequence delay time (Second, 0~300)***** : ");
	//gets(bytInputValue);
	if (bytInputValue[0] != 0)
	{
		if (atoi(bytInputValue) < 300)
			for (int k = 0; k < bytCount; k++)
				wValue[k] = atoi(bytInputValue);
		else
			for (int k = 0; k < bytCount; k++)
				wValue[k] = 5;
	}
	iRet = E1K_DO_SetPowerOnSeqDelaytimes(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_SetPowerOnSeqDelaytimes");
	if (iRet == MXIO_OK)
	{
		//SYSLOG(Dbg, _T("E1K_DO_SetPowerOnSeqDelaytimes success,All DO %d Channel: %s\r\n"), bytCount, (wValue[0] > 0) ? "Pulse Mode" : "DO Mode");
	}
	//==========================
	for (int k = 0; k < bytCount; k++)
		wValue[k] = 0;
	iRet = E1K_DO_GetPowerOnSeqDelaytimes(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, wValue);
	CheckErr(iRet, "E1K_DO_GetPowerOnSeqDelaytimes");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DO_GetPowerOnSeqDelaytimes success,Channel Value:\r\n");
		for (int k = 0; k < bytCount; k++)
		{
			if ((k != 0) && ((k % 4) == 0))
			{
				//SYSLOG(Dbg, _T("Ch[%02d]: %d, "), k, wValue[k]);
			}
		}
	}
	//==========================
	//printf("***** Write DO channel to (0:OFF, 1:ON) ***** : ");
	//gets(bytInputValue);
	if (bytInputValue[0] != 0)
	{
		if (atoi(bytInputValue) == 1)
			dwValue[0] = 0xFFFFFFFF;
		else
			dwValue[0] = 0;
	}
	bytStartChannel = 0;
	bytCount = 8;
	dwValue[0] = 0x00;


	int bit = 128;	//4 Length hex code 
	int hex = 0;

	for (int i = 15; i >= 8; i--)
	{
		if (m_bIO[nIONumber][i] == TRUE)
		{

			dwValue[0] += bit;
		}
		bit = bit >> 1;
	}

	iRet = E1K_DO_Writes(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, dwValue[0]);
	CheckErr(iRet, "E1K_DO_Writes");
	if (iRet == MXIO_OK)
	{
		//SYSLOG(Dbg, _T("E1K_DO_Writes success,DO %d Channels: %s\r\n"), bytCount, ((dwValue[0] & 0x01) >= 1) ? "ON" : "OFF");
	}

	//==========================
	iRet = E1K_DO_Reads(vecValue[nIONumber]->nHandle, bytStartChannel, bytCount, &dwValue[0]);
	CheckErr(iRet, "E1K_DO_Reads");
	if (iRet == MXIO_OK)
	{
		//printf("E1K_DO_Reads success,DO %d Channels:\r\n", bytCount);
		for (int k = 0; k < bytCount; k++)
		{
			if ((k != 0) && ((k % 4) == 0))
			{
				//SYSLOG(Dbg, _T("Ch[%02d]: %s, "), k, ((dwValue[0] & (0x01 << k)) > 0) ? "ON" : "OFF");
				m_bIO[nIONumber][k + 8] = dwValue[0] & (0x01 << k) > 0;
			}
		}
	}






}

void CMgrDio::CheckErr(int iRet, char * szFunctionName)
{
	char * szErrMsg = NULL;
	CString strMsg = _T("");
	if (iRet != MXIO_OK)
	{

		switch (iRet)
		{
		case ILLEGAL_FUNCTION:
			szErrMsg = "ILLEGAL_FUNCTION";
			break;
		case ILLEGAL_DATA_ADDRESS:
			szErrMsg = "ILLEGAL_DATA_ADDRESS";
			break;
		case ILLEGAL_DATA_VALUE:
			szErrMsg = "ILLEGAL_DATA_VALUE";
			break;
		case SLAVE_DEVICE_FAILURE:
			szErrMsg = "SLAVE_DEVICE_FAILURE";
			break;
		case SLAVE_DEVICE_BUSY:
			szErrMsg = "SLAVE_DEVICE_BUSY";
			break;
		case EIO_TIME_OUT:
			szErrMsg = "EIO_TIME_OUT";
			break;
		case EIO_INIT_SOCKETS_FAIL:
			szErrMsg = "EIO_INIT_SOCKETS_FAIL";
			break;
		case EIO_CREATING_SOCKET_ERROR:
			szErrMsg = "EIO_CREATING_SOCKET_ERROR";
			break;
		case EIO_RESPONSE_BAD:
			szErrMsg = "EIO_RESPONSE_BAD";
			break;
		case EIO_SOCKET_DISCONNECT:
			szErrMsg = "EIO_SOCKET_DISCONNECT";
			break;
		case PROTOCOL_TYPE_ERROR:
			szErrMsg = "PROTOCOL_TYPE_ERROR";
			break;
		case SIO_OPEN_FAIL:
			szErrMsg = "SIO_OPEN_FAIL";
			break;
		case SIO_TIME_OUT:
			szErrMsg = "SIO_TIME_OUT";
			break;
		case SIO_CLOSE_FAIL:
			szErrMsg = "SIO_CLOSE_FAIL";
			break;
		case SIO_PURGE_COMM_FAIL:
			szErrMsg = "SIO_PURGE_COMM_FAIL";
			break;
		case SIO_FLUSH_FILE_BUFFERS_FAIL:
			szErrMsg = "SIO_FLUSH_FILE_BUFFERS_FAIL";
			break;
		case SIO_GET_COMM_STATE_FAIL:
			szErrMsg = "SIO_GET_COMM_STATE_FAIL";
			break;
		case SIO_SET_COMM_STATE_FAIL:
			szErrMsg = "SIO_SET_COMM_STATE_FAIL";
			break;
		case SIO_SETUP_COMM_FAIL:
			szErrMsg = "SIO_SETUP_COMM_FAIL";
			break;
		case SIO_SET_COMM_TIME_OUT_FAIL:
			szErrMsg = "SIO_SET_COMM_TIME_OUT_FAIL";
			break;
		case SIO_CLEAR_COMM_FAIL:
			szErrMsg = "SIO_CLEAR_COMM_FAIL";
			break;
		case SIO_RESPONSE_BAD:
			szErrMsg = "SIO_RESPONSE_BAD";
			break;
		case SIO_TRANSMISSION_MODE_ERROR:
			szErrMsg = "SIO_TRANSMISSION_MODE_ERROR";
			break;
		case PRODUCT_NOT_SUPPORT:
			szErrMsg = "PRODUCT_NOT_SUPPORT";
			break;
		case HANDLE_ERROR:
			szErrMsg = "HANDLE_ERROR";
			break;
		case SLOT_OUT_OF_RANGE:
			szErrMsg = "SLOT_OUT_OF_RANGE";
			break;
		case CHANNEL_OUT_OF_RANGE:
			szErrMsg = "CHANNEL_OUT_OF_RANGE";
			break;
		case COIL_TYPE_ERROR:
			szErrMsg = "COIL_TYPE_ERROR";
			break;
		case REGISTER_TYPE_ERROR:
			szErrMsg = "REGISTER_TYPE_ERROR";
			break;
		case FUNCTION_NOT_SUPPORT:
			szErrMsg = "FUNCTION_NOT_SUPPORT";
			break;
		case OUTPUT_VALUE_OUT_OF_RANGE:
			szErrMsg = "OUTPUT_VALUE_OUT_OF_RANGE";
			break;
		case INPUT_VALUE_OUT_OF_RANGE:
			szErrMsg = "INPUT_VALUE_OUT_OF_RANGE";
			break;
		case EIO_PASSWORD_INCORRECT:
			szErrMsg = "EIO_PASSWORD_INCORRECT";
			break;
		}
		//printf("Function \"%s\" execution Fail. Error Message : %s\n", szFunctionName, szErrMsg);
		//SYSLOG(Dbg, _T("Function %s execution Fail. Error Message : %s\n"), szFunctionName, szErrMsg);
	}
}

void CMgrDio::SetIO(int nBoardNumber, int nChNumber, bool bOnOff)
{
	m_bIO[nBoardNumber][nChNumber] = bOnOff;
	//MoxaIoWrite(nBoardNumber);
}

void CMgrDio::SetOutPutIO(int nBoardNumber, int nChNumber, bool bOnOff)
{
	if (GetIOStatus(nBoardNumber, nChNumber) == bOnOff)
	{
		return;
	}
	SetIO(nBoardNumber, nChNumber, bOnOff);//Down
	MoxaIoWrite(nBoardNumber);
}

bool CMgrDio::GetOutIoStatus(int nBoardNumber, int nChNumber)
{
	bytStartChannel = 0;
	bytCount = 8;


	std::vector<MoxaIOInfo*> vecValue;
	CMgrConfig::GetMgr()->GetMoxaIoInfo(vecValue);
	iRet = E1K_DO_Reads(vecValue[nBoardNumber]->nHandle, bytStartChannel, bytCount, &dwValue[0]);
	CheckErr(iRet, "E1K_DO_Reads");
	if (iRet == MXIO_OK)
	{
		if ((nChNumber != 0) && ((nChNumber % 4) == 0))
		{
			//SYSLOG(Dbg, _T("Ch[%02d]: %s, "), nChNumber, ((dwValue[0] & (0x01 << nChNumber)) > 0) ? "ON" : "OFF");
			return dwValue[0] & (0x01 << nChNumber) > 0;
		}
	}
	return FALSE;
}

