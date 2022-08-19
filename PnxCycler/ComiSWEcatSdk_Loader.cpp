/*******************************************************************************
* [ ComiEcatSdk_Loader.cpp ]
* Source(C/C++) file for loading COMIZOA Soft-ECAT SDK library
* - Provider: COMIZOA Co., Ltd.
* - Phone: +82-42-936-6500~6
* - Fax  : +82-42-936-6507
* - URL  : http://www.comizoa.co.kr,  http://www.comizoa.com
********************************************************************************/

#include "stdafx.h" // VC++ 이 아닌 다른곳에서 컴파일할 때는 이 구문을 삭제하십시오.

#if defined(__BCPLUSPLUS__)  // Borland C++ Builder specific
 #include <vcl.h>
 #pragma hdrstop
#endif

#include <windows.h>
#include <stdio.h>

#define _COMISWECAT_SDK_LOADER_C_

#include "ComiSWEcatSdk_Api.h"
#define __DLL_FILE_FULL_PATH__ _T("ComiSWEcatSdk.DLL")

typedef struct{
    char *name;
    FARPROC* ptr;
}LIB_FUNCT;

static HINSTANCE g_hCmmLibrary=NULL;

static LIB_FUNCT g_secSdkFunc[] = {
	{"ecGn_LoadDevices",			(FARPROC*)&secGn_LoadDevices},
	{"ecGn_UnloadDevices",			(FARPROC*)&secGn_UnloadDevices},
	{"ecNet_ScanSlaves",			(FARPROC*)&secNet_ScanSlaves},
	{"ecNet_ScanSlaves_ff",			(FARPROC*)&secNet_ScanSlaves_ff},	
	{"ecSlv_GetProdInfo_A",			(FARPROC*)&secSlv_GetProdInfo_A},
	{"ecSlv_GetState_A",			(FARPROC*)&secSlv_GetState_A},
	{"ecSlv_ReadCoeSdo_A",			(FARPROC*)&secSlv_ReadCoeSdo_A},
	{"ecSlv_WriteCoeSdo_A",			(FARPROC*)&secSlv_WriteCoeSdo_A},
	{"ecSlv_InPDO_GetBufPtr_A",		(FARPROC*)&secSlv_InPDO_GetBufPtr_A},
	{"ecSlv_InPDO_GetBufLen_A",		(FARPROC*)&secSlv_InPDO_GetBufLen_A},
	{"ecSlv_OutPDO_GetBufPtr_A",	(FARPROC*)&secSlv_OutPDO_GetBufPtr_A},
	{"ecSlv_OutPDO_GetBufLen_A",	(FARPROC*)&secSlv_OutPDO_GetBufLen_A},
	{"ecdiGetSlaveIndex",			(FARPROC*)&secdiGetSlaveIndex},
	{"ecdiGetSlaveID",				(FARPROC*)&secdiGetSlaveID},
	{"ecdiGetNumChannels",			(FARPROC*)&secdiGetNumChannels},
	{"ecdiGetOne",					(FARPROC*)&secdiGetOne},
	{"ecdiGetMulti",				(FARPROC*)&secdiGetMulti},
	{"ecdoGetSlaveIndex",			(FARPROC*)&secdoGetSlaveIndex},
	{"ecdoGetSlaveID",				(FARPROC*)&secdoGetSlaveID},
	{"ecdoGetNumChannels",			(FARPROC*)&secdoGetNumChannels},
	{"ecdoPutOne",					(FARPROC*)&secdoPutOne},
	{"ecdoPutMulti",				(FARPROC*)&secdoPutMulti},
	{"ecdoGetOne",					(FARPROC*)&secdoGetOne},
	{"ecdoGetMulti",				(FARPROC*)&secdoGetMulti},
	{"ecaiGetSlaveIndex",			(FARPROC*)&secaiGetSlaveIndex},
	{"ecaiGetSlaveID",				(FARPROC*)&secaiGetSlaveID},
	{"ecaiGetNumChannels",			(FARPROC*)&secaiGetNumChannels},
	{"ecaiGetChanVal_I",			(FARPROC*)&secaiGetChanVal_I},
	{"ecaiGetChanVal_F",			(FARPROC*)&secaiGetChanVal_F},
	{"ecaiGetChanVal_FS",			(FARPROC*)&secaiGetChanVal_FS},
	{"ecaoGetSlaveIndex",			(FARPROC*)&secaoGetSlaveIndex},
	{"ecaoGetSlaveID",				(FARPROC*)&secaoGetSlaveID},
	{"ecaoGetNumChannels",			(FARPROC*)&secaoGetNumChannels},
	{"ecaoSetChanVal_I",			(FARPROC*)&secaoSetChanVal_I},
	{"ecaoSetChanVal_F",			(FARPROC*)&secaoSetChanVal_F},
	{"ecaoSetChanVal_FS",			(FARPROC*)&secaoSetChanVal_FS},
	{"ecaoGetOutValue_I",			(FARPROC*)&secaoGetOutValue_I},
	{"ecaoGetOutValue_F",			(FARPROC*)&secaoGetOutValue_F},
	{"ecaoGetOutValue_FS",			(FARPROC*)&secaoGetOutValue_FS},
	{NULL, NULL}
};


t_success secDll_Load(void)
{
	LIB_FUNCT *f;
	
	if((g_hCmmLibrary = LoadLibrary (__DLL_FILE_FULL_PATH__)) == NULL)
    	return FALSE;
	
	f = &g_secSdkFunc[0];

	while(f->ptr != NULL)
	{
		*(f->ptr) = GetProcAddress ((HMODULE)g_hCmmLibrary, f->name);
		if(*(f->ptr) == NULL){
			//printf("Failed to load \"%s\" Function\n", f->name);
		}
		f++;
	}
	
	return TRUE;
}

t_success secDll_Unload(void)
{
	if(g_hCmmLibrary){
		FreeLibrary(g_hCmmLibrary);
		g_hCmmLibrary = NULL;
	}
	return true;
}

BOOL secDll_IsLoaded(void)
{
	return !(g_hCmmLibrary == NULL);
}

