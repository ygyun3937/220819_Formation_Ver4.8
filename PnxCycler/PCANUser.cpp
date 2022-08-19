#include "StdAfx.h"
#include "PCANUser.h"

#pragma warning(disable:4244)


PCANUser::PCANUser(void)
{
	m_TestStartDate = 0;			//실험시작일시
	m_TestTotalTime = 0;			//실험 총시간 1ms 을 1로함
	m_CycleTotalTime = 0;			//Cycle 총 실험시간 1ms를 1로 함
	m_CycleStartDate = 0;			//Cycle 시작일시
	m_CycleTime = 0;				//Cycle 실험시간 1ms를 1로 함
	m_StepTime = 0;					//Step 실험시간 1ms를 1로 함

	m_dTestVersion = 0;				//실험버전
	m_dVoltage = 0;					//전압
	m_dCurrent = 0;					//전류
	m_dCap = 0;						//용량					//단위 (Ah) : 전류 / 3600 + 이전전류 / 3600
	m_dTotalCap = 0;					//누적용량				//단위 (Ah) : (충방전)전류 / 3600 + (충방전)이전전류 / 3600
	m_dChargeCap = 0;					//충전용량				//단위 (Ah) : 충전전류 / 3600 + 이전충전전류 / 3600
	m_dDischargeCap = 0;				//방전용량				//단위 (Ah) : 방전전류 / 3600 + 이전방전전류 / 3600
	m_dWatt = 0;						//전력					//단위 (W) :  전압 x 전류
	m_dWattHour = 0;					//전력량				//단위 (Wh) : 전압 x 전류 / 3600 + 이전전압 x 이전전류 / 3600
	m_dTotalWattHour = 0;				//누적전력량			//단위 (Wh) : 전압 x 전류 + 이전전압 x 이전전류
	m_dChargeWattHour = 0;				//충방전전력량			//단위 (Wh) : 충전전력 / 3600 + 이전충전전력 / 3600
	m_dCycleCap = 0;					//사이클누적용량		//단위 (Ah) : 사이클(충방전)전류 / 3600 + 사이클(충방전)이전전류 / 3600
	m_dCap_Ah_Base = 0;					//공칭용량

	m_hEvent = NULL;
}


PCANUser::~PCANUser(void)
{
}
CString PCANUser::IntToStr(int iValue)
{
	char chToReceive[20];

	_itoa_s(iValue,chToReceive,10);
	return chToReceive;
}

CString PCANUser::IntToHex(int iValue, short iDigits)
{	
	CString strTemp, strtest;

	strTemp.Format("%0" + IntToStr(iDigits) + "X",iValue);

	return strTemp;
}
DWORD PCANUser::HexTextToInt(CString ToConvert)
{
	DWORD iToReturn = 0;
	int iExp = 0;
	char chByte;

	// The string to convert is empty
	//
	if(ToConvert == "")
		return 0;
	// The string have more than 8 character (the equivalent value
	// exeeds the DWORD capacyty
	//
	if(ToConvert.GetLength() > 8)
		return 0;
	// We convert any character to its Upper case
	//
	ToConvert = ToConvert.MakeUpper();

	try
	{
		// We calculate the number using the Hex To Decimal formula
		//
		for(int i= ToConvert.GetLength()-1; i >= 0; i--){
			chByte = ToConvert[i];
			switch(int(chByte)){
			case 65:
				iToReturn += (DWORD)(10*pow(16.0f,iExp));
				break;
			case 66:
				iToReturn += (DWORD)(11*pow(16.0f,iExp));
				break;
			case 67:
				iToReturn += (DWORD)(12*pow(16.0f,iExp));
				break;
			case 68:
				iToReturn += (DWORD)(13*pow(16.0f,iExp));
				break;
			case 69:
				iToReturn += (DWORD)(14*pow(16.0f,iExp));
				break;
			case 70:
				iToReturn += (DWORD)(15*pow(16.0f,iExp));
				break;
			default:
				if((int(chByte) <48)||(int(chByte)>57))
					return (DWORD)(-1);
				iToReturn += (DWORD)(atoi(&chByte)*pow(16.0f,iExp));
				break;

			}
			iExp++;
		}
	}
	catch(...)
	{
		// Error, return 0
		//
		return 0;
	}

	return iToReturn;
}

void PCANUser::RevCanBitSet(int nType)	//nType 0 = 모토로라 / 1 = 인텔
{
	if(nType == REV_DATA_MOTOROLA)
	{
		m_nCanBit[0]  = UnPack_dATA.bit.bit64;	
		m_nCanBit[1]  = UnPack_dATA.bit.bit63;
		m_nCanBit[2]  = UnPack_dATA.bit.bit62;
		m_nCanBit[3]  = UnPack_dATA.bit.bit61;
		m_nCanBit[4]  = UnPack_dATA.bit.bit60;
		m_nCanBit[5]  = UnPack_dATA.bit.bit59;
		m_nCanBit[6]  = UnPack_dATA.bit.bit58;
		m_nCanBit[7]  = UnPack_dATA.bit.bit57;
		m_nCanBit[8]  = UnPack_dATA.bit.bit56;
		m_nCanBit[9]  = UnPack_dATA.bit.bit55;
		m_nCanBit[10] = UnPack_dATA.bit.bit54;
		m_nCanBit[11] = UnPack_dATA.bit.bit53;
		m_nCanBit[12] = UnPack_dATA.bit.bit52;
		m_nCanBit[13] = UnPack_dATA.bit.bit51;
		m_nCanBit[14] = UnPack_dATA.bit.bit50;
		m_nCanBit[15] = UnPack_dATA.bit.bit49;
		m_nCanBit[16] = UnPack_dATA.bit.bit48;
		m_nCanBit[17] = UnPack_dATA.bit.bit47;
		m_nCanBit[18] = UnPack_dATA.bit.bit46;
		m_nCanBit[19] = UnPack_dATA.bit.bit45;
		m_nCanBit[20] = UnPack_dATA.bit.bit44;
		m_nCanBit[21] = UnPack_dATA.bit.bit43;
		m_nCanBit[22] = UnPack_dATA.bit.bit42;
		m_nCanBit[23] = UnPack_dATA.bit.bit41;
		m_nCanBit[24] = UnPack_dATA.bit.bit40;
		m_nCanBit[25] = UnPack_dATA.bit.bit39;
		m_nCanBit[26] = UnPack_dATA.bit.bit38;
		m_nCanBit[27] = UnPack_dATA.bit.bit37;
		m_nCanBit[28] = UnPack_dATA.bit.bit36;
		m_nCanBit[29] = UnPack_dATA.bit.bit35;
		m_nCanBit[30] = UnPack_dATA.bit.bit34;
		m_nCanBit[31] = UnPack_dATA.bit.bit33;
		m_nCanBit[32] = UnPack_dATA.bit.bit32;
		m_nCanBit[33] = UnPack_dATA.bit.bit31;
		m_nCanBit[34] = UnPack_dATA.bit.bit30;
		m_nCanBit[35] = UnPack_dATA.bit.bit29;
		m_nCanBit[36] = UnPack_dATA.bit.bit28;
		m_nCanBit[37] = UnPack_dATA.bit.bit27;
		m_nCanBit[38] = UnPack_dATA.bit.bit26;
		m_nCanBit[39] = UnPack_dATA.bit.bit25;
		m_nCanBit[40] = UnPack_dATA.bit.bit24;
		m_nCanBit[41] = UnPack_dATA.bit.bit23;
		m_nCanBit[42] = UnPack_dATA.bit.bit22;
		m_nCanBit[43] = UnPack_dATA.bit.bit21;
		m_nCanBit[44] = UnPack_dATA.bit.bit20;
		m_nCanBit[45] = UnPack_dATA.bit.bit19;
		m_nCanBit[46] = UnPack_dATA.bit.bit18;
		m_nCanBit[47] = UnPack_dATA.bit.bit17;
		m_nCanBit[48] = UnPack_dATA.bit.bit16;
		m_nCanBit[49] = UnPack_dATA.bit.bit15;
		m_nCanBit[50] = UnPack_dATA.bit.bit14;
		m_nCanBit[51] = UnPack_dATA.bit.bit13;
		m_nCanBit[52] = UnPack_dATA.bit.bit12;
		m_nCanBit[53] = UnPack_dATA.bit.bit11;
		m_nCanBit[54] = UnPack_dATA.bit.bit10;
		m_nCanBit[55] = UnPack_dATA.bit.bit9;
		m_nCanBit[56] = UnPack_dATA.bit.bit8;
		m_nCanBit[57] = UnPack_dATA.bit.bit7;
		m_nCanBit[58] = UnPack_dATA.bit.bit6;
		m_nCanBit[59] = UnPack_dATA.bit.bit5;
		m_nCanBit[60] = UnPack_dATA.bit.bit4;
		m_nCanBit[61] = UnPack_dATA.bit.bit3;
		m_nCanBit[62] = UnPack_dATA.bit.bit2;
		m_nCanBit[63] = UnPack_dATA.bit.bit1;
	}									
	else if(nType == REV_DATA_INTEL)	//인텔타입
	{
		m_nCanBit[0]  = UnPack_dATA.bit.bit57;
		m_nCanBit[1]  = UnPack_dATA.bit.bit58;
		m_nCanBit[2]  = UnPack_dATA.bit.bit59;
		m_nCanBit[3]  = UnPack_dATA.bit.bit60;
		m_nCanBit[4]  = UnPack_dATA.bit.bit61;
		m_nCanBit[5]  = UnPack_dATA.bit.bit62;
		m_nCanBit[6]  = UnPack_dATA.bit.bit63;
		m_nCanBit[7]  = UnPack_dATA.bit.bit64;
		m_nCanBit[8]  = UnPack_dATA.bit.bit49;
		m_nCanBit[9]  = UnPack_dATA.bit.bit50;
		m_nCanBit[10] = UnPack_dATA.bit.bit51;
		m_nCanBit[11] = UnPack_dATA.bit.bit52;
		m_nCanBit[12] = UnPack_dATA.bit.bit53;
		m_nCanBit[13] = UnPack_dATA.bit.bit54;
		m_nCanBit[14] = UnPack_dATA.bit.bit55;
		m_nCanBit[15] = UnPack_dATA.bit.bit56;
		m_nCanBit[16] = UnPack_dATA.bit.bit41;
		m_nCanBit[17] = UnPack_dATA.bit.bit42;
		m_nCanBit[18] = UnPack_dATA.bit.bit43;
		m_nCanBit[19] = UnPack_dATA.bit.bit44;
		m_nCanBit[20] = UnPack_dATA.bit.bit45;
		m_nCanBit[21] = UnPack_dATA.bit.bit46;
		m_nCanBit[22] = UnPack_dATA.bit.bit47;
		m_nCanBit[23] = UnPack_dATA.bit.bit48;
		m_nCanBit[24] = UnPack_dATA.bit.bit33;
		m_nCanBit[25] = UnPack_dATA.bit.bit34;
		m_nCanBit[26] = UnPack_dATA.bit.bit35;
		m_nCanBit[27] = UnPack_dATA.bit.bit36;
		m_nCanBit[28] = UnPack_dATA.bit.bit37;
		m_nCanBit[29] = UnPack_dATA.bit.bit38;
		m_nCanBit[30] = UnPack_dATA.bit.bit39;
		m_nCanBit[31] = UnPack_dATA.bit.bit40;
		m_nCanBit[32] = UnPack_dATA.bit.bit25;
		m_nCanBit[33] = UnPack_dATA.bit.bit26;
		m_nCanBit[34] = UnPack_dATA.bit.bit27;
		m_nCanBit[35] = UnPack_dATA.bit.bit28;
		m_nCanBit[36] = UnPack_dATA.bit.bit29;
		m_nCanBit[37] = UnPack_dATA.bit.bit30;
		m_nCanBit[38] = UnPack_dATA.bit.bit31;
		m_nCanBit[39] = UnPack_dATA.bit.bit32;
		m_nCanBit[40] = UnPack_dATA.bit.bit17;
		m_nCanBit[41] = UnPack_dATA.bit.bit18;
		m_nCanBit[42] = UnPack_dATA.bit.bit19;
		m_nCanBit[43] = UnPack_dATA.bit.bit20;
		m_nCanBit[44] = UnPack_dATA.bit.bit21;
		m_nCanBit[45] = UnPack_dATA.bit.bit22;
		m_nCanBit[46] = UnPack_dATA.bit.bit23;
		m_nCanBit[47] = UnPack_dATA.bit.bit24;
		m_nCanBit[48] = UnPack_dATA.bit.bit9;
		m_nCanBit[49] = UnPack_dATA.bit.bit10;
		m_nCanBit[50] = UnPack_dATA.bit.bit11;
		m_nCanBit[51] = UnPack_dATA.bit.bit12;
		m_nCanBit[52] = UnPack_dATA.bit.bit13;
		m_nCanBit[53] = UnPack_dATA.bit.bit14;
		m_nCanBit[54] = UnPack_dATA.bit.bit15;
		m_nCanBit[55] = UnPack_dATA.bit.bit16;
		m_nCanBit[56] = UnPack_dATA.bit.bit1;
		m_nCanBit[57] = UnPack_dATA.bit.bit2;
		m_nCanBit[58] = UnPack_dATA.bit.bit3;
		m_nCanBit[59] = UnPack_dATA.bit.bit4;
		m_nCanBit[60] = UnPack_dATA.bit.bit5;
		m_nCanBit[61] = UnPack_dATA.bit.bit6;
		m_nCanBit[62] = UnPack_dATA.bit.bit7;
		m_nCanBit[63] = UnPack_dATA.bit.bit8;

	}
}

long PCANUser::BitDataVal_Motorola(int nStart,int nEnd,int nSignedType)
{
	long BitData = 0;
	long lSigndBitData = 0;
//	int nSum = 0;
	CString strTemp = "";
	CString strData = "";
	CString strTotalData = "";
	CString strHead1 = "";
	CString strHead2 = "";
	CString strHead = "";
	int i = 0;
// 	nStart = 56;
// 	nEnd = 8;
	for (i = 0; i < 64; i++)			//시작비트부터 시작하여 종료비트까지 카운터
	{
		if(i%8 == 0)
			strTotalData += _T(" ");

		strTemp.Format("%d",m_nCanBit[i]);
		strTotalData += strTemp;								//시작비트부터 종료비트까지의 값이 저장	//사용이유는 TRACE로 어떤값들이 들어오는지 확인하기 위해.
	}
	for (i = nStart; i < (nStart+nEnd); i++)			//시작비트부터 시작하여 종료비트까지 카운터
	{
		strTemp.Format("%d",m_nCanBit[i]);
		strData += strTemp;								//시작비트부터 종료비트까지의 값이 저장	//사용이유는 TRACE로 어떤값들이 들어오는지 확인하기 위해.
	}
	int nShift = nEnd;
//	int nSigned = 1; //1=(-), 0=(+);
	int n = 0;
	for (int i = (nStart+nEnd-1); i > (nStart-1); i--)	
	//              16         16 - 12=4
	//for (int i = (nStart); i > nStart - nEnd - 1; i--)	
	{
// 		if(REV_DATA_UNSIGNED == nSignedType)
// 		{
			nShift--;
			BitData = BitData | (m_nCanBit[i] << nShift);
			lSigndBitData = lSigndBitData | (1 << n);
			n++;
			// 			if(i == (nStart+nEnd-1))
			// 				BitData += m_nCanBit[i];
			// 			else
			// 			{
			// 				BitData += (m_nCanBit[i]*(2<<nSum));
			// 				nSum++;
			// 			}
// 		}
// 		else if(REV_DATA_SIGNED == nSignedType)
// 		{
// 			nShift--;
// 			if(i == (nStart+nEnd-1))
// 			{
// 				if(m_nCanBit[i] == 1)
// 					lSigndBitData = m_nCanBit[i] << nEnd;
// 					//nSigned = -1;
// 			}
// 			else{
// 				BitData = BitData | (m_nCanBit[i] << nShift);
// 			}
// 			// 			else if(nStart == i)
// 			// 			{
// 			// 				long nSined = (m_nCanBit[i]*(2<<nSum));
// 			// 
// 			// 				BitData = BitData - nSined;
// 			// 			}
// 			// 			else
// 			// 			{
// 			// 				BitData += (m_nCanBit[i]*(2<<nSum));
// 			// 				nSum++;
// 			// 			}
// 		}
	}

	//if(REV_DATA_SIGNED == nSignedType)
	//	BitData = BitData - lSigndBitData;
	if(REV_DATA_SIGNED == nSignedType) {
		if((BitData >> (nEnd-1)) == 1)
			BitData = (lSigndBitData - BitData + 1) * -1.0;
	}

//	BitData = BitData * nSigned;

// 	strHead1 = _T("[    Head1]= 00000000 00111111 11112222 22222233 33333333 44444444 44555555 55556666\n");
// 	strHead2 = _T("[    Head2]= 01234567 89012345 67890123 45678901 23456789 01234567 89012345 67890123");
// 	strHead = strHead1 + strHead2;
// 	TRACE("%s\n", strHead);
// 	TRACE("TotalBinary=%s, ID=0x%x, S(-):1/US(+):0 [%d], StartBit = %d, CountBit = %d, BitBinary = %s  , Dec = %ld \n", strTotalData, nId, nSignedType,nStart, nEnd, strData, BitData);

	return BitData;
}

long PCANUser::BitDataVal_Intel(int nStart,int nEnd,int nSignedType)
{
	long BitData = 0;
	long lSigndBitData = 0;
//	int nSum = 0;
	CString strTemp = "";
	CString strData = "";
	CString strTotalData = "";
	CString strHead1 = "";
	CString strHead2 = "";
	CString strHead = "";
	int i = 0;
	// 	nStart = 56;
	// 	nEnd = 8;
	for (i = 0; i < 64; i++)			//시작비트부터 시작하여 종료비트까지 카운터
	{
		if(i%8 == 0)
			strTotalData += _T(" ");

		strTemp.Format("%d",m_nCanBit[i]);
		strTotalData += strTemp;								//시작비트부터 종료비트까지의 값이 저장	//사용이유는 TRACE로 어떤값들이 들어오는지 확인하기 위해.
	}
	for (i = nStart; i < (nStart+nEnd); i++)			//시작비트부터 시작하여 종료비트까지 카운터
	{
		strTemp.Format("%d",m_nCanBit[i]);
		strData += strTemp;								//시작비트부터 종료비트까지의 값이 저장	//사용이유는 TRACE로 어떤값들이 들어오는지 확인하기 위해.
	}
	int nShift = nEnd;
//	int nSigned = 1; //1=(-), 0=(+);
	int n = 0;
	//for (int i = (nStart+nEnd-1); i > (nStart-1); i--)	
	for (int i = 64-(nStart+nEnd); i < 64-nStart; i++)	
		//              16         16 - 12=4
		//for (int i = (nStart); i > nStart - nEnd - 1; i--)	
	{
		//if(REV_DATA_UNSIGNED == nSignedType)
		//{
			nShift--;
			BitData = BitData | (m_nCanBit[i] << nShift);
			lSigndBitData = lSigndBitData + (1 << n);
			n++;
			// 			if(i == (nStart+nEnd-1))
			// 				BitData += m_nCanBit[i];
			// 			else
			// 			{
			// 				BitData += (m_nCanBit[i]*(2<<nSum));
			// 				nSum++;
			// 			}
		//}
// 		else if(REV_DATA_SIGNED == nSignedType)
// 		{
// 			nShift--;
// 			if(i == (nStart+nEnd-1))
// 			{
// 				if(m_nCanBit[i] == 1)
// 					lSigndBitData = m_nCanBit[i] << nEnd;
// 					//nSigned = -1;
// 			}
// 			else{
// 				BitData = BitData | (m_nCanBit[i] << nShift);
// 			}
// 			// 			else if(nStart == i)
// 			// 			{
// 			// 				long nSined = (m_nCanBit[i]*(2<<nSum));
// 			// 
// 			// 				BitData = BitData - nSined;
// 			// 			}
// 			// 			else
// 			// 			{
// 			// 				BitData += (m_nCanBit[i]*(2<<nSum));
// 			// 				nSum++;
// 			// 			}
// 		}
 	}

	//if(REV_DATA_SIGNED == nSignedType)
	//	BitData = BitData - lSigndBitData;
	if(REV_DATA_SIGNED == nSignedType) {
		if((BitData >> (nEnd-1)) == 1)
			BitData = (lSigndBitData - BitData + 1) * -1.0;
	}

//	BitData = BitData * nSigned;

	// 	strHead1 = _T("[    Head1]= 00000000 00111111 11112222 22222233 33333333 44444444 44555555 55556666\n");
	// 	strHead2 = _T("[    Head2]= 01234567 89012345 67890123 45678901 23456789 01234567 89012345 67890123");
	// 	strHead = strHead1 + strHead2;
	// 	TRACE("%s\n", strHead);
	//TRACE("TotalBinary=%s, ID=0x%x, S(-):1/US(+):0 [%d], StartBit = %d, CountBit = %d, BitBinary = %s  , Dec = %ld \n", strTotalData, nId, nSignedType,nStart, nEnd, strData, BitData);

	return BitData;
}
//////////////////////////////////////////////////////////////////////////////////////////////CH1
// CriticalSection class
//
#pragma region Critical Section Class
clsCritical::clsCritical(CRITICAL_SECTION *cs, bool createUnlocked, bool lockRecursively)
{
	ASSERT(cs != NULL);	

	m_objpCS_CH1 = cs;
	m_dwLocked = -1;
	m_bDoRecursive = lockRecursively;
	m_dwOwnerThread = GetCurrentThreadId();

	if(!createUnlocked)
		Enter();
}	

clsCritical::~clsCritical()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked >= 0)
		if(Leave() == iFail) 
			break;
}

int clsCritical::Enter()
{
	if(m_dwOwnerThread != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive || (m_dwLocked == -1))
		{
			EnterCriticalSection(m_objpCS_CH1);
			InterlockedIncrement(&m_dwLocked);
		}
		return m_dwLocked;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical::Leave()
{
	if (m_dwOwnerThread != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked >= 0)
		{
			LeaveCriticalSection(m_objpCS_CH1);
			InterlockedDecrement(&m_dwLocked);
			return m_dwLocked;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical::IsLocked()
{
	return (m_dwLocked > -1);
}

int clsCritical::GetRecursionCount()
{
	return m_dwLocked;
}
////////////////////////////////////////////////////////////////////////// CH2
clsCritical_CH2::clsCritical_CH2(CRITICAL_SECTION *cs_CH2, bool createUnlocked_CH2, bool lockRecursively_CH2)
{
	ASSERT(cs_CH2 != NULL);	

	m_objpCS_CH2 = cs_CH2;
	m_dwLocked_CH2 = -1;
	m_bDoRecursive_CH2 = lockRecursively_CH2;
	m_dwOwnerThread_CH2 = GetCurrentThreadId();

	if(!createUnlocked_CH2)
		Enter_CH2();
}	

clsCritical_CH2::~clsCritical_CH2()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_CH2 >= 0)
		if(Leave_CH2() == iFail) 
			break;
}

int clsCritical_CH2::Enter_CH2()
{
	if (m_dwOwnerThread_CH2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_CH2 || (m_dwLocked_CH2 == -1))
		{
			EnterCriticalSection(m_objpCS_CH2);
			InterlockedIncrement(&m_dwLocked_CH2);
		}
		return m_dwLocked_CH2;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_CH2::Leave_CH2()
{
	if (m_dwOwnerThread_CH2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_CH2 >= 0)
		{
			LeaveCriticalSection(m_objpCS_CH2);
			InterlockedDecrement(&m_dwLocked_CH2);
			return m_dwLocked_CH2;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_CH2::IsLocked_CH2()
{
	return (m_dwLocked_CH2 > -1);
}

int clsCritical_CH2::GetRecursionCount_CH2()
{
	return m_dwLocked_CH2;
}

clsCritical_CH3::clsCritical_CH3(CRITICAL_SECTION *cs_CH3, bool createUnlocked_CH3, bool lockRecursively_CH3)
{
	ASSERT(cs_CH3 != NULL);	

	m_objpCS_CH3 = cs_CH3;
	m_dwLocked_CH3 = -1;
	m_bDoRecursive_CH3 = lockRecursively_CH3;
	m_dwOwnerThread_CH3 = GetCurrentThreadId();

	if(!createUnlocked_CH3)
		Enter_CH3();
}	

clsCritical_CH3::~clsCritical_CH3()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_CH3 >= 0)
		if(Leave_CH3() == iFail) 
			break;
}

int clsCritical_CH3::Enter_CH3()
{
	if (m_dwOwnerThread_CH3 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_CH3 || (m_dwLocked_CH3 == -1))
		{
			EnterCriticalSection(m_objpCS_CH3);
			InterlockedIncrement(&m_dwLocked_CH3);
		}
		return m_dwLocked_CH3;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_CH3::Leave_CH3()
{
	if (m_dwOwnerThread_CH3 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_CH3 >= 0)
		{
			LeaveCriticalSection(m_objpCS_CH3);
			InterlockedDecrement(&m_dwLocked_CH3);
			return m_dwLocked_CH3;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_CH3::IsLocked_CH3()
{
	return (m_dwLocked_CH3 > -1);
}

int clsCritical_CH3::GetRecursionCount_CH3()
{
	return m_dwLocked_CH3;
}


//////////////////////////////////////////////////////////////////////////
clsCritical_Send_CH1::clsCritical_Send_CH1(CRITICAL_SECTION *cs_SEND_CH1, bool createUnlocked_SEND_CH1, bool lockRecursively_SEND_CH1)
{
	ASSERT(cs_SEND_CH1 != NULL);	

	m_objpCS_SEND_CH1 = cs_SEND_CH1;
	m_dwLocked_SEND_CH1 = -1;
	m_bDoRecursive_SEND_CH1 = lockRecursively_SEND_CH1;
	m_dwOwnerThread_SEND_CH1 = GetCurrentThreadId();

	if(!createUnlocked_SEND_CH1)
		Enter_SEND_CH1();
}	

clsCritical_Send_CH1::~clsCritical_Send_CH1()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_SEND_CH1 >= 0)
		if(Leave_SEND_CH1() == iFail) 
			break;
}

int clsCritical_Send_CH1::Enter_SEND_CH1()
{
	if (m_dwOwnerThread_SEND_CH1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_SEND_CH1 || (m_dwLocked_SEND_CH1 == -1))
		{
			EnterCriticalSection(m_objpCS_SEND_CH1);
			InterlockedIncrement(&m_dwLocked_SEND_CH1);
		}
		return m_dwLocked_SEND_CH1;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_Send_CH1::Leave_SEND_CH1()
{
	if (m_dwOwnerThread_SEND_CH1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_SEND_CH1 >= 0)
		{
			LeaveCriticalSection(m_objpCS_SEND_CH1);
			InterlockedDecrement(&m_dwLocked_SEND_CH1);
			return m_dwLocked_SEND_CH1;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_Send_CH1::IsLocked_SEND_CH1()
{
	return (m_dwLocked_SEND_CH1 > -1);
}

int clsCritical_Send_CH1::GetRecursionCount_SEND_CH1()
{
	return m_dwLocked_SEND_CH1;
}

//////////////////////////////////////////////////////////////////////////
clsCritical_Send_CH2::clsCritical_Send_CH2(CRITICAL_SECTION *cs_SEND_CH2, bool createUnlocked_SEND_CH2, bool lockRecursively_SEND_CH2)
{
	ASSERT(cs_SEND_CH2 != NULL);	

	m_objpCS_SEND_CH2 = cs_SEND_CH2;
	m_dwLocked_SEND_CH2 = -1;
	m_bDoRecursive_SEND_CH2 = lockRecursively_SEND_CH2;
	m_dwOwnerThread_SEND_CH2 = GetCurrentThreadId();

	if(!createUnlocked_SEND_CH2)
		Enter_SEND_CH2();
}	

clsCritical_Send_CH2::~clsCritical_Send_CH2()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_SEND_CH2 >= 0)
		if(Leave_SEND_CH2() == iFail) 
			break;
}

int clsCritical_Send_CH2::Enter_SEND_CH2()
{
	if (m_dwOwnerThread_SEND_CH2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_SEND_CH2 || (m_dwLocked_SEND_CH2 == -1))
		{
			EnterCriticalSection(m_objpCS_SEND_CH2);
			InterlockedIncrement(&m_dwLocked_SEND_CH2);
		}
		return m_dwLocked_SEND_CH2;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_Send_CH2::Leave_SEND_CH2()
{
	if (m_dwOwnerThread_SEND_CH2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_SEND_CH2 >= 0)
		{
			LeaveCriticalSection(m_objpCS_SEND_CH2);
			InterlockedDecrement(&m_dwLocked_SEND_CH2);
			return m_dwLocked_SEND_CH2;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_Send_CH2::IsLocked_SEND_CH2()
{
	return (m_dwLocked_SEND_CH2 > -1);
}

int clsCritical_Send_CH2::GetRecursionCount_SEND_CH2()
{
	return m_dwLocked_SEND_CH2;
}

clsCritical_Send_CH3::clsCritical_Send_CH3(CRITICAL_SECTION *cs_SEND_CH3, bool createUnlocked_SEND_CH3, bool lockRecursively_SEND_CH3)
{
	ASSERT(cs_SEND_CH3 != NULL);	

	m_objpCS_SEND_CH3 = cs_SEND_CH3;
	m_dwLocked_SEND_CH3 = -1;
	m_bDoRecursive_SEND_CH3 = lockRecursively_SEND_CH3;
	m_dwOwnerThread_SEND_CH3 = GetCurrentThreadId();

	if(!createUnlocked_SEND_CH3)
		Enter_SEND_CH3();
}	

clsCritical_Send_CH3::~clsCritical_Send_CH3()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_SEND_CH3 >= 0)
		if(Leave_SEND_CH3() == iFail) 
			break;
}

int clsCritical_Send_CH3::Enter_SEND_CH3()
{
	if (m_dwOwnerThread_SEND_CH3 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_SEND_CH3 || (m_dwLocked_SEND_CH3 == -1))
		{
			EnterCriticalSection(m_objpCS_SEND_CH3);
			InterlockedIncrement(&m_dwLocked_SEND_CH3);
		}
		return m_dwLocked_SEND_CH3;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_Send_CH3::Leave_SEND_CH3()
{
	if (m_dwOwnerThread_SEND_CH3 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_SEND_CH3 >= 0)
		{
			LeaveCriticalSection(m_objpCS_SEND_CH3);
			InterlockedDecrement(&m_dwLocked_SEND_CH3);
			return m_dwLocked_SEND_CH3;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_Send_CH3::IsLocked_SEND_CH3()
{
	return (m_dwLocked_SEND_CH3 > -1);
}

int clsCritical_Send_CH3::GetRecursionCount_SEND_CH3()
{
	return m_dwLocked_SEND_CH3;
}
//////////////////////////////////////////////////////////////////////////DSP SEND REV CH1/CH2
clsCritical_DSP1::clsCritical_DSP1(CRITICAL_SECTION *cs_DSP1, bool createUnlocked_DSP1, bool lockRecursively_DSP1)
{
	ASSERT(cs_DSP1 != NULL);	

	m_objpCS_DSP1 = cs_DSP1;
	m_dwLocked_DSP1 = -1;
	m_bDoRecursive_DSP1 = lockRecursively_DSP1;
	m_dwOwnerThread_DSP1 = GetCurrentThreadId();

	if(!createUnlocked_DSP1)
		Enter_DSP1();
}	

clsCritical_DSP1::~clsCritical_DSP1()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_DSP1 >= 0)
		if(Leave_DSP1() == iFail) 
			break;
}

int clsCritical_DSP1::Enter_DSP1()
{
	if (m_dwOwnerThread_DSP1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_DSP1 || (m_dwLocked_DSP1 == -1))
		{
			EnterCriticalSection(m_objpCS_DSP1);
			InterlockedIncrement(&m_dwLocked_DSP1);
		}
		return m_dwLocked_DSP1;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_DSP1::Leave_DSP1()
{
	if (m_dwOwnerThread_DSP1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_DSP1 >= 0)
		{
			LeaveCriticalSection(m_objpCS_DSP1);
			InterlockedDecrement(&m_dwLocked_DSP1);
			return m_dwLocked_DSP1;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_DSP1::IsLocked_DSP1()
{
	return (m_dwLocked_DSP1 > -1);
}

int clsCritical_DSP1::GetRecursionCount_DSP1()
{
	return m_dwLocked_DSP1;
}
////////////////////////////////////////////////////////////////////////// CH2
clsCritical_DSP2::clsCritical_DSP2(CRITICAL_SECTION *cs_DSP2, bool createUnlocked_DSP2, bool lockRecursively_DSP2)
{
	ASSERT(cs_DSP2 != NULL);	

	m_objpCS_DSP2 = cs_DSP2;
	m_dwLocked_DSP2 = -1;
	m_bDoRecursive_DSP2 = lockRecursively_DSP2;
	m_dwOwnerThread_DSP2 = GetCurrentThreadId();

	if(!createUnlocked_DSP2)
		Enter_DSP2();
}	

clsCritical_DSP2::~clsCritical_DSP2()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_DSP2 >= 0)
		if(Leave_DSP2() == iFail) 
			break;
}

int clsCritical_DSP2::Enter_DSP2()
{
	if (m_dwOwnerThread_DSP2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_DSP2 || (m_dwLocked_DSP2 == -1))
		{
			EnterCriticalSection(m_objpCS_DSP2);
			InterlockedIncrement(&m_dwLocked_DSP2);
		}
		return m_dwLocked_DSP2;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_DSP2::Leave_DSP2()
{
	if (m_dwOwnerThread_DSP2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_DSP2 >= 0)
		{
			LeaveCriticalSection(m_objpCS_DSP2);
			InterlockedDecrement(&m_dwLocked_DSP2);
			return m_dwLocked_DSP2;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_DSP2::IsLocked_DSP2()
{
	return (m_dwLocked_DSP2 > -1);
}

int clsCritical_DSP2::GetRecursionCount_DSP2()
{
	return m_dwLocked_DSP2;
}


//////////////////////////////////////////////////////////////////////////
clsCritical_Send_DSP1::clsCritical_Send_DSP1(CRITICAL_SECTION *cs_SEND_DSP1, bool createUnlocked_SEND_DSP1, bool lockRecursively_SEND_DSP1)
{
	ASSERT(cs_SEND_DSP1 != NULL);	

	m_objpCS_SEND_DSP1 = cs_SEND_DSP1;
	m_dwLocked_SEND_DSP1 = -1;
	m_bDoRecursive_SEND_DSP1 = lockRecursively_SEND_DSP1;
	m_dwOwnerThread_SEND_DSP1 = GetCurrentThreadId();

	if(!createUnlocked_SEND_DSP1)
		Enter_SEND_DSP1();
}	

clsCritical_Send_DSP1::~clsCritical_Send_DSP1()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_SEND_DSP1 >= 0)
		if(Leave_SEND_DSP1() == iFail) 
			break;
}

int clsCritical_Send_DSP1::Enter_SEND_DSP1()
{
	if (m_dwOwnerThread_SEND_DSP1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_SEND_DSP1 || (m_dwLocked_SEND_DSP1 == -1))
		{
			EnterCriticalSection(m_objpCS_SEND_DSP1);
			InterlockedIncrement(&m_dwLocked_SEND_DSP1);
		}
		return m_dwLocked_SEND_DSP1;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_Send_DSP1::Leave_SEND_DSP1()
{
	if(m_dwOwnerThread_SEND_DSP1 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_SEND_DSP1 >= 0)
		{
			LeaveCriticalSection(m_objpCS_SEND_DSP1);
			InterlockedDecrement(&m_dwLocked_SEND_DSP1);
			return m_dwLocked_SEND_DSP1;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_Send_DSP1::IsLocked_SEND_DSP1()
{
	return (m_dwLocked_SEND_DSP1 > -1);
}

int clsCritical_Send_DSP1::GetRecursionCount_SEND_DSP1()
{
	return m_dwLocked_SEND_DSP1;
}

//////////////////////////////////////////////////////////////////////////
clsCritical_Send_DSP2::clsCritical_Send_DSP2(CRITICAL_SECTION *cs_SEND_DSP2, bool createUnlocked_SEND_DSP2, bool lockRecursively_SEND_DSP2)
{
	ASSERT(cs_SEND_DSP2 != NULL);	

	m_objpCS_SEND_DSP2 = cs_SEND_DSP2;
	m_dwLocked_SEND_DSP2 = -1;
	m_bDoRecursive_SEND_DSP2 = lockRecursively_SEND_DSP2;
	m_dwOwnerThread_SEND_DSP2 = GetCurrentThreadId();

	if(!createUnlocked_SEND_DSP2)
		Enter_SEND_DSP2();
}	

clsCritical_Send_DSP2::~clsCritical_Send_DSP2()
{
	int iFail = (int)0x80000000;

	while(m_dwLocked_SEND_DSP2 >= 0)
		if(Leave_SEND_DSP2() == iFail) 
			break;
}

int clsCritical_Send_DSP2::Enter_SEND_DSP2()
{
	if (m_dwOwnerThread_SEND_DSP2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_bDoRecursive_SEND_DSP2 || (m_dwLocked_SEND_DSP2 == -1))
		{
			EnterCriticalSection(m_objpCS_SEND_DSP2);
			InterlockedIncrement(&m_dwLocked_SEND_DSP2);
		}
		return m_dwLocked_SEND_DSP2;
	}
	catch(...)
	{	
		return 0x80000000;
	}
}

int clsCritical_Send_DSP2::Leave_SEND_DSP2()
{
	if (m_dwOwnerThread_SEND_DSP2 != (LONG)GetCurrentThreadId())
		throw "class clsCritical: Thread cross-over error. ";

	try
	{
		if(m_dwLocked_SEND_DSP2 >= 0)
		{
			LeaveCriticalSection(m_objpCS_SEND_DSP2);
			InterlockedDecrement(&m_dwLocked_SEND_DSP2);
			return m_dwLocked_SEND_DSP2;
		}
		return -1;
	}
	catch(...)
	{
		return 0x80000000;
	}
}

bool clsCritical_Send_DSP2::IsLocked_SEND_DSP2()
{
	return (m_dwLocked_SEND_DSP2 > -1);
}

int clsCritical_Send_DSP2::GetRecursionCount_SEND_DSP2()
{
	return m_dwLocked_SEND_DSP2;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////////
// MessageStatus class
//
#pragma region Message Status class
MessageStatus::MessageStatus(TPCANMsg canMsg, TPCANTimestamp canTimestamp, int listIndex)
{
	m_Msg = canMsg;
	m_TimeStamp = canTimestamp;
	m_oldTimeStamp = canTimestamp;
	m_iIndex = listIndex;
	m_Count = 1;
	m_bShowPeriod = false;
	m_bWasChanged = false;
}

void MessageStatus::Update(TPCANMsg canMsg, TPCANTimestamp canTimestamp)
{
	m_Msg = canMsg;
	m_oldTimeStamp = m_TimeStamp;
	m_TimeStamp = canTimestamp;
	m_bWasChanged = true;
	m_Count += 1;
}

TPCANMsg MessageStatus::GetCANMsg()
{
	return m_Msg;
}

TPCANTimestamp MessageStatus::GetTimestamp()
{
	return m_TimeStamp;
}

int MessageStatus::GetPosition()
{
	return m_iIndex;
}

CString MessageStatus::GetTypeString()
{
	CString strTemp;

	// Add the new ListView Item with the type of the message
	//
	if((m_Msg.MSGTYPE & PCAN_MESSAGE_EXTENDED) != 0)
		strTemp = "EXTENDED";
	else
		strTemp = "STANDARD";

	if((m_Msg.MSGTYPE & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		strTemp = (strTemp + "/RTR");

	return strTemp;
}

CString MessageStatus::GetIdString()
{
	CString strTemp;

	// We format the ID of the message and show it
	//
	if((m_Msg.MSGTYPE & PCAN_MESSAGE_EXTENDED) != 0)
		strTemp.Format("%08Xh",m_Msg.ID);
	else
		strTemp.Format("%03Xh",m_Msg.ID);

	return strTemp;
}

CString MessageStatus::GetDataString()
{
	CString strTemp, strTemp2;

	strTemp = "";
	strTemp2 = "";

	if((m_Msg.MSGTYPE & PCAN_MESSAGE_RTR) == PCAN_MESSAGE_RTR)
		return "Remote Request";
	else
		for(int i=0; i< m_Msg.LEN; i++)
		{
			strTemp.Format("%s %02X", strTemp2, m_Msg.DATA[i]);
			strTemp2 = strTemp;
		}

		return strTemp2;
}

CString MessageStatus::GetTimeString()
{
	double fTime;
	CString str;

	fTime = m_TimeStamp.millis + (m_TimeStamp.micros / 1000.0);
//	if (m_bShowPeriod)	//
		fTime -= (m_oldTimeStamp.millis + (m_oldTimeStamp.micros / 1000.0));
	str.Format("%.1f", fTime);

	return str;
}

int MessageStatus::GetCount()
{
	return m_Count;
}

bool MessageStatus::GetShowingPeriod()
{
	return m_bShowPeriod;
}

bool MessageStatus::GetMarkedAsUpdated()
{
	return m_bWasChanged;
}

void MessageStatus::SetShowingPeriod(bool value)
{
	if (m_bShowPeriod ^ value)
	{
		m_bShowPeriod = value;
		m_bWasChanged = true;
	}
}

void MessageStatus::SetMarkedAsUpdated(bool value)
{
	m_bWasChanged = value;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////////////

void PCANUser::Data_Err_Set(BYTE szdata[12])	//
{
	m_szErr_Warn2	=	stErr.szErr_W_2; szdata[0] = m_szErr_Warn2;
	m_szErr_Warn1	=	stErr.szErr_W_1; szdata[1] = m_szErr_Warn1;
	m_szErr_Warn0	=	stErr.szErr_W_0; szdata[2] = m_szErr_Warn0;
	m_szErr_Grid2	=	stErr.szErr_G_2; szdata[3] = m_szErr_Grid2;
	m_szErr_Grid1	=	stErr.szErr_G_1; szdata[4] = m_szErr_Grid1;
	m_szErr_Grid0	=	stErr.szErr_G_0; szdata[5] = m_szErr_Grid0;
	m_szErr_Dc2		=	stErr.szErr_D_2; szdata[6] = m_szErr_Dc2;		
	m_szErr_Dc1		=	stErr.szErr_D_1; szdata[7] = m_szErr_Dc1;		
	m_szErr_Dc0		=	stErr.szErr_D_0; szdata[8] = m_szErr_Dc0;		
	m_szErr_Fault2	=	stErr.szErr_F_2; szdata[9] = m_szErr_Fault2;
	m_szErr_Fault1	=	stErr.szErr_F_1; szdata[10] = m_szErr_Fault1;
	m_szErr_Fault0	=	stErr.szErr_F_0; szdata[11] = m_szErr_Fault0;

}
void PCANUser::Data_Err_Init()	//Data 초기화
{
	m_szErr_Warn2	=	0x0;
	m_szErr_Warn1	=	0x0;
	m_szErr_Warn0	=	0x0;
	m_szErr_Grid2	=	0x0;
	m_szErr_Grid1	=	0x0;
	m_szErr_Grid0	=	0x0;
	m_szErr_Dc2		=	0x0;
	m_szErr_Dc1		=	0x0;
	m_szErr_Dc0		=	0x0;
	m_szErr_Fault2	=	0x0;
	m_szErr_Fault1	=	0x0;
	m_szErr_Fault0	=	0x0;

}

BYTE *PCANUser::GetByteToStr(int nNum,CString strData)	//ch1 Data 초기화
{

	CString strTemp[10];
	BYTE *byData = new BYTE[8];	//동적으로 표시해죠야함.
	for(int i = 0; i<8; i++ )
	{
		strTemp[i] = "";
		strTemp[i] = strData.Mid(i*2,2);	//문자열을 2자리씩 자른다. 
	}
	for (int i = 0; i<8; i++)
	{
		byData[i] = (BYTE)HexTextToInt(strTemp[i]);
	}

	memcpy( stCanSendData[nNum].DATA,byData,sizeof(stCanSendData[nNum].DATA));
	delete[] byData;
	return byData;
}

DWORD PCANUser::GetDwordToStr(CString strID)	//ch1 Data 초기화
{
	DWORD dwID;
	dwID = HexTextToInt(strID);
	return dwID;
}

void PCANUser::DataInit()
{
	//	m_nSumCount = 0;				//데이터 1초 전송하기 위한 카운트
	// 	m_Voltage = 0;					//전압
	// 	m_Current = 0;					//전류
	m_dCap = 0;						//용량	- step	Ah	
	m_dTotalCap = 0;					//누적용량		
	m_dChargeCap = 0;				//충전용량		
	m_dDischargeCap = 0;				//방전용량		
	m_dWatt = 0;						//전력			
	m_dWattHour = 0;					//전력량		Wh
	m_dTotalWattHour = 0;			//누적전력량	
	m_dChargeWattHour = 0;			//충전전력량	
	m_dCycleCap = 0;					//사이클누적용량
	m_dDcir = 0;
	m_dCap_Ah_Base = 0;
}

void PCANUser::ReqData(double fltVolt,double fltCurr, long lTimeMs)
{
	m_dVoltage = fltVolt;
	m_dCurrent = fltCurr;
	double fCalc = (3600*1000L); //ms 단위로 변환
	double dCapDebug = 0;
	dCapDebug = fltCurr/3600L;
	dCapDebug = fltCurr/fCalc*lTimeMs;
	//m_Cap += fltCurr/(3600*1000L)*lTimeMs;							//용량[Ah]
	m_dCap += fltCurr/fCalc*lTimeMs;							//용량[Ah]
	m_dCycleCap += fltCurr/fCalc*lTimeMs;					//사이클용량[Ah]
	//	TRACE("DEBUG_CAP 1sec = %f, cur=%f, long_time(sec)=%Id, CAP=%f\n", dCapDebug, fltCurr, lTimeMs, m_dCap);
	//m_Cap += fltCurr/3600L*lTimeMs;							//용량[Ah]

	m_dWatt =  fltVolt * fltCurr;							//전력[W]
	m_dWattHour += m_dWatt/fCalc*lTimeMs;						//전력량[Wh]

	if(fltCurr > 0)											//충전 용량,전력 계산
	{
		m_dTotalCap += fltCurr/fCalc*lTimeMs;				//누적용량
		m_dChargeCap += fltCurr/fCalc*lTimeMs;				//충전용량
		m_dTotalWattHour += m_dWatt/fCalc*lTimeMs;			//누적전력량
		m_dChargeWattHour += m_dWatt/fCalc*lTimeMs;			//충방전전력량
	}
	else													//방전 용량,전력 계산
	{
		m_dTotalCap += (fltCurr * (-1))/fCalc*lTimeMs;		//누적용량
		m_dDischargeCap += fltCurr/fCalc*lTimeMs;			//방전용량
		m_dTotalWattHour += (m_dWatt* (-1))/fCalc*lTimeMs;	//누적전력량
		m_dChargeWattHour += (m_dWatt* (-1))/fCalc*lTimeMs;			//충방전전력량
	}


	//	TRACE("전압[%f] 전류[%f] 전력[%f] 전력량[%f] 용량[%f] 기록시간[%I64d]\n",fltVolt,fltCurr,m_Watt,m_WattHour,m_Cap, lTimeMs);
	//	TRACE("충전용량[%f] 방전용량[%f] 충전전력량[%f] 누적용량[%f] 누적전력량[%f]\n",m_ChargeCap,m_DischargeCap,m_ChargeWattHour,m_TotalCap,m_TotalWattHour);


}