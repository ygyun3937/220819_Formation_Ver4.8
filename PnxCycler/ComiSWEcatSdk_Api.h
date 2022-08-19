#ifndef _COMISWECATMASTER_SDK_H_
#define _COMISWECATMASTER_SDK_H_

#include "ComiEcatSdk_Def.h"
#include "ComiEcatSdk_Err.h"
#include "ComiEcatSdk_SysDef.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _COMISWECAT_SDK_LOADER_C_
#define EC_EXTERN
#else
#define EC_EXTERN extern
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SetBit() : 지정한 변수의 특정 비트의 값을 변경한다. 매크로이므로 함수명과 괄호가 떨어지면 않됨에 주의!
// v:대상 변수, n:bit번호, s:변경값(state)
#define ecSetBit(v,n,s)	if(s) (v)|=(1<<(n)); else (v)&=~(1<<(n))
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetBit() : 지정한 변수의 특정 비트의 값을 반환한다. 매크로이므로 함수명과 괄호가 떨어지면 않됨에 주의!
// v:대상 변수, n:bit번호
#define ecGetBit(v,n)		(((v)>>(n))&0x1)

#if 1 // DLL이 'stdcall' 호출방식을 사용했을 때...
#define CECAT_API WINAPI
#else // DLL이 'cdecl' 호출방식을 사용했을 때...
#define CECAT_API
#endif

//====================== DLL LOAD/UNLOAD FUNCTIONS ======================================================//
EC_EXTERN t_success		secDll_Load (void);
EC_EXTERN t_success		secDll_Unload (void);
EC_EXTERN BOOL			secDll_IsLoaded(void);

//====================== NET INTERFACE FUNCTIONS =========================================================//
EC_EXTERN t_ui32		(CECAT_API *secNet_ScanSlaves)(t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_ui32		(CECAT_API *secNet_ScanSlaves_ff)(t_i32 NetID, t_i32 *ErrCode);

//====================== GENERAL FUNCTIONS ==============================================================//
EC_EXTERN t_success		(CECAT_API *secGn_LoadDevices) (t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secGn_UnloadDevices) (t_i32 *ErrCode);

//====================== SLAVE I/F FUNCTIONS ============================================================//
EC_EXTERN t_success		(CECAT_API *secSlv_GetProdInfo_A) (t_i32 NetID, t_i32 SlaveIdx, TEcSlvProdInfoEx *pProdInfoBuf, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secSlv_GetState_A) (t_i32 NetID, t_i32 SlaveIdx, TEcSlvState *pSlaveStateBuf, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secSlv_ReadCoeSdo_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secSlv_WriteCoeSdo_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pData, _out t_i32 *ErrCode);
EC_EXTERN void*			(CECAT_API *secSlv_InPDO_GetBufPtr_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secSlv_InPDO_GetBufLen_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 *ErrCode);
EC_EXTERN void*			(CECAT_API *secSlv_OutPDO_GetBufPtr_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secSlv_OutPDO_GetBufLen_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 *ErrCode);

//====================== Digital Input FUNCTIONS =======================================================//
EC_EXTERN t_i32			(CECAT_API *secdiGetSlaveIndex) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secdiGetSlaveID) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secdiGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_bool		(CECAT_API *secdiGetOne) (t_i32 NetID, t_ui32 DiChannel, _out t_i32 *ErrCode);
EC_EXTERN t_dword		(CECAT_API *secdiGetMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, _out t_i32 *ErrCode);

//====================== Digital Output FUNCTIONS =======================================================//
EC_EXTERN t_i32			(CECAT_API *secdoGetSlaveIndex) (t_i32 NetID, t_ui32 DoChannel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secdoGetSlaveID) (t_i32 NetID, t_ui32 DoChannel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secdoGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secdoPutOne) (t_i32 NetID, t_ui32 DoChannel, t_bool OutState, _out t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secdoPutMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, t_dword dwOutStates, _out t_i32 *ErrCode);
EC_EXTERN t_bool		(CECAT_API *secdoGetOne) (t_i32 NetID, t_ui32 DoChannel, _out t_i32 *ErrCode);
EC_EXTERN t_dword		(CECAT_API *secdoGetMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, _out t_i32 *ErrCode);

//====================== Analog Input FUNCTIONS =======================================================//
EC_EXTERN t_i32			(CECAT_API *secaiGetSlaveIndex) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaiGetSlaveID) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaiGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaiGetChanVal_I) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64			(CECAT_API *secaiGetChanVal_F) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64			(CECAT_API *secaiGetChanVal_FS) (t_i32 NetID, t_ui32 Channel, t_f32 ScaleMin, t_f32 ScaleMax, _out t_i32 *ErrCode);

//====================== Analog Output FUNCTIONS =======================================================//
EC_EXTERN t_i32			(CECAT_API *secaoGetSlaveIndex) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaoGetSlaveID) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaoGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secaoSetChanVal_I) (t_i32 NetID, t_ui32 Channel, t_i32 OutData, _out t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secaoSetChanVal_F) (t_i32 NetID, t_ui32 Channel, t_f64 OutData, _out t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *secaoSetChanVal_FS) (t_i32 NetID, t_ui32 Channel, t_f64 OutData, t_f64 ScaleMin, t_f64 ScaleMax, _out t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *secaoGetOutValue_I) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64			(CECAT_API *secaoGetOutValue_F) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64			(CECAT_API *secaoGetOutValue_FS) (t_i32 NetID, t_ui32 Channel, t_f64 ScaleMin, t_f64 ScaleMax, _out t_i32 *ErrCode);

#ifdef __cplusplus
}
#endif

#endif