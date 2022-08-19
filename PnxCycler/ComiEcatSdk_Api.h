#ifndef _COMIECAT_SDK_API_H_
#define _COMIECAT_SDK_API_H_

#include "ComiEcatSdk_Def.h"
#include "ComiEcatSdk_Err.h"
#include "ComiEcatSdk_SysDef.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef _COMIECAT_SDK_LOADER_C_
 #define EC_EXTERN
#else
 #define EC_EXTERN extern
#endif

//*****************************************************************************************************************************
//*
//*							Functions for General Users
//*
//*****************************************************************************************************************************


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
EC_EXTERN t_success ecDll_Load (void);
EC_EXTERN t_success ecDll_Unload (void);
EC_EXTERN BOOL		ecDll_IsLoaded(void);

//====================== GENERAL FUNCTIONS ==============================================================//
EC_EXTERN t_success (CECAT_API *ecGn_LoadDevices) (t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_UnloadDevices) (t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_SetBootWaitMode) (t_bool IsWaitBootCompt, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecGn_GetBootWaitMode) (t_i32 *ErrCode);
EC_EXTERN BOOL		(CECAT_API *ecGn_IsDevLoaded) (t_i32 DevIdx, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecGn_GetNumDevices) (t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecGn_GetNumNetworks) (t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecGn_GetDevInfo) (t_i32 DevIdx, TEcDevInfo *pDevInfo, t_i32 *ErrCode);

//====================== GENERAL UNTILITY FUNCTIONS ============================================//
EC_EXTERN void		(CECAT_API *ecUtl_ShowError_A) (HWND hParentWnd, char *szFormat, ...);
EC_EXTERN void		(CECAT_API *ecUtl_ShowError_W) (HWND hParentWnd, WCHAR *szFormat, ...);
EC_EXTERN void		(CECAT_API *ecUtl_ShowMessage_A) (HWND hParentWnd, char *szFormat, ...);
EC_EXTERN void		(CECAT_API *ecUtl_ShowMessage_W) (HWND hParentWnd, WCHAR *szFormat, ...);
EC_EXTERN t_ui32	(CECAT_API *ecUtl_GetCntDiff_UI32) (t_ui32 PrvVal, t_ui32 CurVal);
EC_EXTERN t_ui16	(CECAT_API *ecUtl_GetCntDiff_UI16) (t_ui16 PrvVal, t_ui16 CurVal);
EC_EXTERN t_bool	(CECAT_API *ecUtl_IsTimeOut) (t_ui32 dwStartTime_ms, t_ui32 dwTimeOutVal_ms);

//====================== NET INTERFACE FUNCTIONS =========================================================//
EC_EXTERN t_bool	(CECAT_API *ecNet_IsBootCompt) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_GetDevIdx) (t_i32 NetID);
EC_EXTERN t_i32		(CECAT_API *ecNet_GetLocNetIdx) (t_i32 NetID);
EC_EXTERN t_success (CECAT_API *ecNet_GetVerInfo) (t_i32 NetID, TEcFileVerInfo_SDK *pVerInfo_SDK, TEcFileVerInfo_WDM *pVerInfo_WDM, TEcFileVerInfo_FW *pVerInfo_FW, t_i32 *ErrCode);
EC_EXTERN t_ui32	(CECAT_API *ecNet_ScanSlaves) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_ui32	(CECAT_API *ecNet_GetCfgSlaveCount) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_SetCfgSlaveCount) (t_i32 NetID, t_ui32 SlaveCount, t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecNet_SetAlState) (t_i32 NetID, EEcAlState AlState, t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecNet_SetAlState_FF) (t_i32 NetID, EEcAlState AlState, t_i32 *ErrCode);
EC_EXTERN EEcAlState	(CECAT_API *ecNet_GetAlState) (t_i32 NetID, t_i32 *ErrCode);


EC_EXTERN void*		(CECAT_API *ecNet_InPDO_GetBufPtr) (t_i32 NetID, t_i32 OfsPos, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_InPDO_GetData) (t_i32 NetID, t_i32 OfsPos, t_ui16 Size, void *pBuf, t_i32 *ErrCode);
EC_EXTERN t_byte	(CECAT_API *ecNet_InPDO_GetData_B) (t_i32 NetID, t_i32 OfsPos, t_i32 *ErrCode);
EC_EXTERN t_word	(CECAT_API *ecNet_InPDO_GetData_W) (t_i32 NetID, t_i32 OfsPos, t_i32 *ErrCode);
EC_EXTERN t_dword	(CECAT_API *ecNet_InPDO_GetData_D) (t_i32 NetID, t_i32 OfsPos, t_i32 *ErrCode);
EC_EXTERN void*		(CECAT_API *ecNet_OutPDO_GetBufPtr) (t_i32 NetID, t_i32 OfsPos, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_OutPDO_SetData) (t_i32 NetID, t_i32 OfsPos, t_ui16 Size, void *pBuf, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_OutPDO_SetData_B) (t_i32 NetID, t_i32 OfsPos, t_byte Data, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_OutPDO_SetData_W) (t_i32 NetID, t_i32 OfsPos, t_word Data, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_OutPDO_SetData_D) (t_i32 NetID, t_i32 OfsPos, t_dword Data, t_i32 *ErrCode);

//====================== SLAVE I/F FUNCTIONS ============================================================//
EC_EXTERN t_i32			(CECAT_API *ecSlv_SlvIdx2PhysAddr) (t_i32 NetID, t_ui16 SlaveIndex, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_PhysAddr2SlvIdx) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_GetProdInfo_A) (t_i32 NetID, t_i32 SlaveIndex, TEcSlvProdInfo *pProdInfoBuf, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_GetProdInfo) (t_i32 NetID, t_i32 SlvPhysAddr, TEcSlvProdInfo *pProdInfoBuf, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_GetTypeInfo) (t_i32 NetID, t_ui16 SlvPhysAddr, TEcSlvTypeInfo *pTypeInfoBuf, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_GetTypeInfo_A) (t_i32 NetID, t_i32 SlaveIndex, TEcSlvTypeInfo *pTypeInfoBuf, t_i32 *ErrCode);
EC_EXTERN EEcAlState	(CECAT_API *ecSlv_GetAlState) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 *ErrCode);
EC_EXTERN EEcAlState	(CECAT_API *ecSlv_GetAlState_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadReg) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 RegAddr, t_i32 DataSize, _out void *pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadReg_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 RegAddr, t_i32 DataSize, _out void *pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteReg) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 RegAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteReg_FF) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 RegAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteReg_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 RegAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteReg_A_FF) (t_i32 NetID, t_i32 SlaveIdx, t_i32 RegAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadCoeSdo) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadCoeSdo_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteCoeSdo) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteCoeSdo_FF) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteCoeSdo_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteCoeSdo_A_FF) (t_i32 NetID, t_i32 SlaveIdx, t_i32 Index, t_i32 SubIdx, t_i32 IsComptAccess, t_i32 DataSize, void* pData, _out t_i32 *ErrCode);
EC_EXTERN void*			(CECAT_API *ecSlv_InPDO_GetBufPtr) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 InPdoUnitIdx, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_InPDO_GetBufLen) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 InPdoUnitIdx, t_i32 *ErrCode);
EC_EXTERN void*			(CECAT_API *ecSlv_OutPDO_GetBufPtr) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_OutPDO_GetBufLen) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_OutPDO_SetInitValMode) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 OutPDOInitMode, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_OutPDO_GetInitValMode) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_OutPDO_SetInitVal) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 OfsPos, t_i32 DataSize, void *pData, t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_OutPDO_GetInitVal) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui8 OutPdoUnitIdx, t_i32 OfsPos, t_i32 BufSize, void *pBuf, t_i32 *ErrCode);

//====================== Digital Input FUNCTIONS =======================================================//
EC_EXTERN t_i32		(CECAT_API *ecdiGetSlaveIndex) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecdiGetSlaveID) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecdiGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecdiGetOne) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_dword	(CECAT_API *ecdiGetMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, t_i32 *ErrCode);

//====================== Digital Output FUNCTIONS =======================================================//
EC_EXTERN t_i32		(CECAT_API *ecdoGetSlaveIndex) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecdoGetSlaveID) (t_i32 NetID, t_ui32 DiChannel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecdoGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecdoPutOne) (t_i32 NetID, t_ui32 DoChannel, t_bool OutState, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecdoPutMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, t_dword dwOutStates, _out t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecdoGetOne) (t_i32 NetID, t_ui32 DoChannel, _out t_i32 *ErrCode);
EC_EXTERN t_dword	(CECAT_API *ecdoGetMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, _out t_i32 *ErrCode);

EC_EXTERN t_success (CECAT_API *ecdoL_PutMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, t_dword dwOutStates, _out t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecdoL_GetOne) (t_i32 NetID, t_ui32 DoChannel, _out t_i32 *ErrCode);
EC_EXTERN t_dword	(CECAT_API *ecdoL_GetMulti) (t_i32 NetID, t_ui32 IniChannel, t_ui8 NumChannels, _out t_i32 *ErrCode);

//====================== Analog Input FUNCTIONS =======================================================//
EC_EXTERN t_i32		(CECAT_API *ecaiGetSlaveIndex) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaiGetSlaveID) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaiGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaiGetChanVal_I) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaiGetChanVal_F) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaiGetChanVal_FS) (t_i32 NetID, t_ui32 Channel, t_f32 ScaleMin, t_f32 ScaleMax, _out t_i32 *ErrCode);


EC_EXTERN t_i32		(CECAT_API *ecaiL_GetChanVal_I) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaiL_GetChanVal_F) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaiL_GetChanVal_FS) (t_i32 NetID, t_ui32 Channel, t_f32 ScaleMin, t_f32 ScaleMax, _out t_i32 *ErrCode);

//====================== Analog Output FUNCTIONS =======================================================//
EC_EXTERN t_i32		(CECAT_API *ecaoGetSlaveIndex) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaoGetSlaveID) (t_i32 NetID, t_ui32 Channel, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaoGetNumChannels) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecaoSetChanVal_I) (t_i32 NetID, t_ui32 Channel, t_i32 OutData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecaoSetChanVal_F) (t_i32 NetID, t_ui32 Channel, t_f64 OutData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecaoSetChanVal_FS) (t_i32 NetID, t_ui32 Channel, t_f64 OutData, t_f64 ScaleMin, t_f64 ScaleMax, _out t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecaoGetOutValue_I) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaoGetOutValue_F) (t_i32 NetID, t_ui32 Channel, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecaoGetOutValue_FS) (t_i32 NetID, t_ui32 Channel, t_f64 ScaleMin, t_f64 ScaleMax, _out t_i32 *ErrCode);

//====================== MOTION - GENEAL FUNCTIONS =======================================================//
EC_EXTERN t_i32		(CECAT_API *ecmGn_GetAxisList) (t_i32 NetID, t_ui8 AxisListBuf[], t_ui8 AxisListBufSize, t_i32 *ErrCode);
EC_EXTERN TEcmInPDO_Header* (CECAT_API *ecmGn_GetInPDOHeader) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmGn_AxisToSlaveIndex) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmGn_AxisToSlaveID) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmGn_SlaveIndexToAxis) (t_i32 NetID, t_i32 SlaveIndex, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmGn_SlaveIDToAxis) (t_i32 NetID, t_i32 SlaveID, t_i32 *ErrCode);

//====================== MOTION - SINGLE AXIS FUNCTIONS ==================================================//
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_SetUnitDist) (t_i32 NetID, t_i32 Axis, t_f64 UnitDist, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxCfg_GetUnitDist) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_SetUnitSpeed) (t_i32 NetID, t_i32 Axis, t_f64 UnitSpeed, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxCfg_GetUnitSpeed) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetMioProp) (t_i32 NetID, t_i32 Axis, EEcmMioPropId PropId, t_i32 PropVal, _out t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_GetMioProp) (t_i32 NetID, t_i32 Axis, EEcmMioPropId PropId, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedPatt) (t_i32 NetID, t_i32 Axis, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 Acc, t_f64 Dec, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedPatt_FF) (t_i32 NetID, t_i32 Axis, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 Acc, t_f64 Dec, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_GetSpeedPatt) (t_i32 NetID, t_i32 Axis, t_i32 *SpeedMode, t_f64 *VIni, t_f64 *VEnd, t_f64 *VWork, t_f64 *Acc, t_f64 *Dec, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedPatt_Time) (t_i32 NetID, t_i32 Axis, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 AccTime, t_f64 DecTime, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedPatt_Time_FF) (t_i32 NetID, t_i32 Axis, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 AccTime, t_f64 DecTime, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_GetSpeedPatt_Time) (t_i32 NetID, t_i32 Axis, t_i32 *SpeedMode, t_f64 *VIni, t_f64 *VEnd, t_f64 *VWork, t_f64 *AccTime, t_f64 *DecTime, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedRatio) (t_i32 NetID, t_i32 Axis, t_f64 SpeedRatio, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSpeedRatio_FF) (t_i32 NetID, t_i32 Axis, t_f64 SpeedRatio, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxCfg_GetSpeedRatio) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetJerkRatio) (t_i32 NetID, t_i32 Axis, t_f64 JerkTimeRatio, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetJerkRatio_FF) (t_i32 NetID, t_i32 Axis, t_f64 JerkTimeRatio, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxCfg_GetJerkRatio) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_SetMinAccDecTime) (t_i32 NetID, t_i32 Axis, t_f64 MinAccTime, t_f64 MinDecTime, t_f64 LowLimitVel, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_GetMinAccDecTime) (t_i32 NetID, t_i32 Axis, t_f64 *MinAccTime, t_f64 *MinDecTime, t_f64 *LowLimitVel, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetTouchProbeFunc) (t_i32 NetID, t_i32 Axis, t_ui8 TouchProbeIndex, t_byte TouchProbeFuncVal, _out t_i32 *ErrCode);
EC_EXTERN t_byte	(CECAT_API *ecmSxCfg_GetTouchProbeFunc) (t_i32 NetID, t_i32 Axis, t_ui8 TouchProbeIndex, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSyncOtherEnv) (t_i32 NetID, t_i32 Axis, t_i32 SyncAxis, t_i32 SyncType, t_i32 PosSyncMethod, t_f64 SyncPosition, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_GetSyncOtherEnv) (t_i32 NetID, t_i32 Axis, t_i32 *SyncAxis, t_i32 *SyncType, t_i32 *PosSyncMethod, t_f64 *SyncPosition, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCfg_SetSyncOtherEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_bool IsOneShot, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_GetSyncOtherEnable) (t_i32 NetID, t_i32 Axis, t_bool* IsEnable, t_bool* IsOneShot, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_PosCorr_SetTableSize) (t_i32 NetID, t_i32 Axis, t_i32 TableSize, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_PosCorr_GetTableSize) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_PosCorr_SetTableData) (t_i32 NetID, t_i32 Axis, t_i32 TableIndex, t_f64 RefCmdPos, t_f64 ActMotorPos, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_PosCorr_GetTableData) (t_i32 NetID, t_i32 Axis, t_i32 TableIndex, t_f64* RefCmdPos, t_f64* ActMotorPos, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_PosCorr_SetTableFromFile) (t_i32 NetID, t_i32 Axis, char *szFilePath, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_PosCorr_SetEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxCfg_PosCorr_GetEnable) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_PosCorr_ClearTable) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_SetValMode) (t_i32 NetID, t_i32 Axis, t_i32 ValMode, t_i32 NumMultiVals, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_AutoTorq_GetValMode) (t_i32 NetID, t_i32 Axis, t_i32 *NumMultiVals, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_SetMultiVal) (t_i32 NetID, t_i32 Axis, t_i32 ValIndex, t_i32 TorqVal, t_i32 Duration, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_GetMultiVal) (t_i32 NetID, t_i32 Axis, t_i32 ValIndex, t_i32* TorqVal, t_i32* Duration, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_SetValue) (t_i32 NetID, t_i32 Axis, t_i32 OutTorqVal, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_AutoTorq_GetValue) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_SetLimit) (t_i32 NetID, t_i32 Axis, t_ui32 LimitMask, t_f64 HighSpeedLimit, t_f64 LowSpeedLimit, t_i32 TimeLimit, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_GetLimit) (t_i32 NetID, t_i32 Axis, t_ui32 *LimitMask, t_f64 *HighSpeedLimit, t_f64 *LowSpeedLimit, t_i32 *TimeLimit, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxCfg_AutoTorq_SetEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_bool IsOneShotEnable, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxCfg_AutoTorq_GetEnable) (t_i32 NetID, t_i32 Axis, t_bool *IsOneShotEnable, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxCfg_Ring_GetEnable) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_Ring_SetEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxCfg_Ring_GetPosRange) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_Ring_SetPosRange) (t_i32 NetID, t_i32 Axis, t_f64 RingPosRange, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCfg_Ring_GetDirMode) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCfg_Ring_SetDirMode) (t_i32 NetID, t_i32 Axis, t_i32 RingDirMode, t_i32 *ErrCode);

EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_SetSvon) (t_i32 NetID, t_i32 Axis, t_i32 SvonVal, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_SetSvon_FF) (t_i32 NetID, t_i32 Axis, t_i32 SvonVal, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_SetAlmRst) (t_i32 NetID, t_i32 Axis, t_i32 IsSetAlmRst, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_SetAlmRst_FF) (t_i32 NetID, t_i32 Axis, t_i32 IsSetAlmRst, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_ResetAlm) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxCtl_ResetAlm_FF) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);

EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_VMoveStart) (t_i32 NetID, t_i32 Axis, t_i32 Dir, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_VMoveStart_FF) (t_i32 NetID, t_i32 Axis, t_i32 Dir, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_MoveStart) (t_i32 NetID, t_i32 Axis, t_f64 Distance, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_MoveStart_FF) (t_i32 NetID, t_i32 Axis, t_f64 Distance, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxMot_Move) (t_i32 NetID, t_i32 Axis, t_f64 Distance, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxMot_Move_NB) (t_i32 NetID, t_i32 Axis, t_f64 Distance, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_MoveToStart) (t_i32 NetID, t_i32 Axis, t_f64 Position, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_MoveToStart_FF) (t_i32 NetID, t_i32 Axis, t_f64 Position, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxMot_MoveTo) (t_i32 NetID, t_i32 Axis, t_f64 Position, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxMot_MoveTo_NB) (t_i32 NetID, t_i32 Axis, t_f64 Position, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_OverrideSpeed) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_OverrideMove) (t_i32 NetID, t_i32 Axis, t_f64 NewDist, _out t_bool *IsIgnored, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_OverrideMoveTo) (t_i32 NetID, t_i32 Axis, t_f64 NewPos, _out t_bool *IsIgnored, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_Stop) (t_i32 NetID, t_i32 Axis, t_i32 IsDecStop, t_i32 IsWaitCompt, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxMot_Stop_FF) (t_i32 NetID, t_i32 Axis, t_i32 IsDecStop, _out t_i32 *ErrCode);

EC_EXTERN t_bool	(CECAT_API *ecmSxSt_IsConnected)(t_i32 NetID, t_i32 Axis, EEcmConnStsDetail *ConnStsDetail, t_i32 *ErrCode);
EC_EXTERN void*		(CECAT_API *ecmSxSt_GetInPDOPtr)(t_i32 NetID, t_i32 Axis, t_ui8 *PDODataType, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxSt_IsBusy) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxSt_WaitCompt) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmSxSt_WaitCompt_NB) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxSt_SetCount) (t_i32 NetID, t_i32 Axis, t_i32 NewPosCount, _out t_i32 *ErrCode);
EC_EXTERN t_i32 	(CECAT_API *ecmSxSt_GetCount) (t_i32 NetID, t_i32 Axis, t_i32 TargCntr, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmSxSt_SetPosition) (t_i32 NetID, t_i32 Axis, t_f64 NewPosition, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxSt_GetPosition) (t_i32 NetID, t_i32 Axis, t_i32 TargCntr, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxSt_GetCurSpeed) (t_i32 NetID, t_i32 Axis, t_i32 TargCntr, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxSt_GetCurTorque) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxSt_GetMotState) (t_i32 NetID, t_i32 Axis, _out t_i32 *SubErrData);
EC_EXTERN t_word	(CECAT_API *ecmSxSt_GetFlags) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_word	(CECAT_API *ecmSxSt_GetDI) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_byte	(CECAT_API *ecmSxSt_GetTouchProbeSts) (t_i32 NetID, t_i32 Axis, t_i32 TouchProbeIndex, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmSxSt_GetTouchProbePos) (t_i32 NetID, t_i32 Axis, t_i32 TouchProbeIndex, _out t_i32 *ErrCode);

//====================== MOTION - POSITION COMPARE OUTPUT FUNCTIONS =================================================//
EC_EXTERN t_success	(CECAT_API *ecmSxCmpOne_SetEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_i32 GdoChanIdx, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxCmpOne_GetEnable) (t_i32 NetID, t_i32 Axis, t_i32* GdoChanIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpOne_SetEnv) (t_i32 NetID, t_i32 Axis, t_i32 CmpCntrType, t_i32 CmpMethod, t_f64 CmpRefPos, t_i32 OutSigLogic, t_i32 OutSigOnTime, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpOne_GetEnv) (t_i32 NetID, t_i32 Axis, t_i32* CmpCntrType, t_i32* CmpMethod, t_f64* CmpRefPos, t_i32* OutSigLogic, t_i32* OutSigOnTime, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpOne_GetOutSts) (t_i32 NetID, t_i32 Axis, t_i32 *OutCount, t_f64 *LastOutPos, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpOne_ResetOutSts) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_SetEnable) (t_i32 NetID, t_i32 Axis, t_bool IsEnable, t_i32 GdoChanIdx, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmSxCmpCont_GetEnable) (t_i32 NetID, t_i32 Axis, t_i32* GdoChanIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_SetTableSize) (t_i32 NetID, t_i32 Axis, t_i32 TableSize, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCmpCont_GetTableSize) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_SetTableData) (t_i32 NetID, t_i32 Axis, t_i32 TableIndex, t_i32 CmpCntrType, t_i32 CmpMethod, t_f64 CmpRefPos, t_i32 OutSigLogic, t_i32 OutSigOnTime, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_GetTableData) (t_i32 NetID, t_i32 Axis, t_i32 TableIndex, t_i32* CmpCntrType, t_i32* CmpMethod, t_f64* CmpRefPos, t_i32* OutSigLogic, t_i32* OutSigOnTime, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmSxCmpCont_GetActTblIdx) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_SetActTblIdx) (t_i32 NetID, t_i32 Axis, t_i32 TableIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_GetOutSts) (t_i32 NetID, t_i32 Axis, t_i32 *OutCount, t_f64 *LastOutPos, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmSxCmpCont_ResetOutSts) (t_i32 NetID, t_i32 Axis, t_i32 *ErrCode);

//====================== MOTION - MULTIPLE AXES FUNCTIONS =================================================//
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_VMoveStart) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 DirList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_VMoveStart_FF) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 DirList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_MoveStart) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_MoveStart_FF) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_MoveToStart) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_MoveToStart_FF) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_Stop) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 IsDecStop, t_i32 IsWaitCompt, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmMxMot_Stop_FF) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 IsDecStop, _out t_i32 *ErrCode);

EC_EXTERN t_bool	(CECAT_API *ecmMxSt_IsBusy) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmMxSt_WaitCompt) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmMxSt_WaitCompt_NB) (t_i32 NetID, t_i32 NumAxes, t_i32 AxisList[], t_i32 *ErrCode);

//====================== MOTION - INTERPOLATION FUNCTIONS ================================================//
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_MapAxes) (t_i32 NetID, t_i32 MapIndex, t_i32 NumAxes, t_i32 AxisList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_MapAxes_FF) (t_i32 NetID, t_i32 MapIndex, t_i32 NumAxes, t_i32 AxisList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_UnmapAxes) (t_i32 NetID, t_i32 MapIndex, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_UnmapAxes_FF) (t_i32 NetID, t_i32 MapIndex, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetSpeedPatt) (t_i32 NetID, t_i32 MapIndex, t_i32 IxSpdPattType, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 Acc, t_f64 Dec, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetSpeedPatt_FF) (t_i32 NetID, t_i32 MapIndex, t_i32 IxSpdPattType, t_i32 SpeedMode, t_f64 VIni, t_f64 VEnd, t_f64 VWork, t_f64 Acc, t_f64 Dec, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxCfg_GetSpeedPatt) (t_i32 NetID, t_i32 MapIndex, t_i32 *IxSpdPattType, t_i32 *SpeedMode, t_f64 *VIni, t_f64 *VEnd, t_f64 *VWork, t_f64 *Acc, t_f64 *Dec, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetJerkRatio) (t_i32 NetID, t_i32 MapIndex, t_f64 JerkTimeRatio, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetJerkRatio_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 JerkTimeRatio, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmIxCfg_GetJerkRatio) (t_i32 NetID, t_i32 MapIndex, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetSpeedPatt_MR) (t_i32 NetID, t_i32 MapIndex, t_i32 SpeedMode, t_f64 MasterRatio, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetSpeedPatt_MR_FF) (t_i32 NetID, t_i32 MapIndex, t_i32 SpeedMode, t_f64 MasterRatio, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_GetSpeedPatt_MR) (t_i32 NetID, t_i32 MapIndex, t_i32 *SpeedMode, t_f64 *MasterRatio, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxCfg_SetMastWeight) (t_i32 NetID, t_i32 MapIndex, t_f64 *MastWeightList, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_GetMastWeight) (t_i32 NetID, t_i32 MapIndex, t_f64 *MastWeightList, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_ClearPool) (t_i32 NetID, t_i32 MapIndex, t_i32 *ErrCode);
EC_EXTERN t_handle32 (CECAT_API *ecmIxCfg_Spline_AddNewObj) (t_i32 NetID, t_i32 MapIndex,  t_ui32 NumRefPoints, t_bool IsAbsPosMode, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxCfg_Spline_SetRefPoint) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 PointIndex, t_f64 Point[], t_i32 NumPointData, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxCfg_Spline_BuildObj) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_SetTrgOutEnv) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_bool IsEnableTrgOut, t_i16 TrgOfsTime_ms, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_GetTrgOutEnv) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_bool* IsEnableTrgOut, t_i16* TrgOfsTime_ms, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmIxCfg_Spline_GetTrgOutPos) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 Axis, t_i32 PointIndex, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_SetJsType) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 JerkSmoothType, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmIxCfg_Spline_GetJsType) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_SetJsProp_F) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 PropId, t_f64 PropVal, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmIxCfg_Spline_GetJsProp_F) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 PropId, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxCfg_Spline_SetJerkThresh) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 Axis, t_f64 JerkThreshold, t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmIxCfg_Spline_GetJerkThresh) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_i32 Axis, t_i32 *ErrCode);

EC_EXTERN t_success	(CECAT_API *ecmIxCfg_MPRLin2X_ClearPool) (t_i32 NetID, t_i32 MapIndex, t_i32 *ErrCode);
EC_EXTERN t_handle32 (CECAT_API *ecmIxCfg_MPRLin2X_AddNewObj) (t_i32 NetID, t_i32 MapIndex,  t_ui32 NumRefPoints, t_bool IsAbsPosMode, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxCfg_MPRLin2X_SetRefPoint) (t_i32 NetID, t_i32 MapIndex, t_handle32 hMPRLin2XObj, t_i32 PointIndex, t_f64 Point[], t_f64 RoundData, t_i32 RoundDataType, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxCfg_MPRLin2X_BuildObj) (t_i32 NetID, t_i32 MapIndex, t_handle32 hMPRLin2XObj, _out t_f64 LastPointBuf[], _out t_i32 *ErrCode);

EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_LineStart) (t_i32 NetID, t_i32 MapIndex, t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_LineStart_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_Line) (t_i32 NetID, t_i32 MapIndex, t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_Line_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 DistList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_LineToStart) (t_i32 NetID, t_i32 MapIndex, t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_LineToStart_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_LineTo) (t_i32 NetID, t_i32 MapIndex, t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_LineTo_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 PosList[], _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_ArcAng_R_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_ArcAng_R_Start_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_ArcAng_R) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxMot_ArcAng_R_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_ArcAng_A_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_ArcAng_A_Start_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcAng_A) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcAng_A_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_ArcPos_R_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 XEndOffset, t_f64 YEndOffset, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_ArcPos_R_Start_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 XEndOffset, t_f64 YEndOffset, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcPos_R) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 XEndOffset, t_f64 YEndOffset, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcPos_R_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentOffset, t_f64 YCentOffset, t_f64 XEndOffset, t_f64 YEndOffset, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_ArcPos_A_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 XEndPos, t_f64 YEndPos, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_ArcPos_A_Start_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 XEndPos, t_f64 YEndPos, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcPos_A) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 XEndPos, t_f64 YEndPos, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_ArcPos_A_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 XCentPos, t_f64 YCentPos, t_f64 XEndPos, t_f64 YEndPos, t_i32 Direction, t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_Arc3P_R_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XOffset, t_f64 P2_YOffset, t_f64 P3_XOffset, t_f64 P3_YOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_Arc3P_R_Start_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XOffset, t_f64 P2_YOffset, t_f64 P3_XOffset, t_f64 P3_YOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_Arc3P_R) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XOffset, t_f64 P2_YOffset, t_f64 P3_XOffset, t_f64 P3_YOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_Arc3P_R_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XOffset, t_f64 P2_YOffset, t_f64 P3_XOffset, t_f64 P3_YOffset, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_Arc3P_A_Start) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XPos, t_f64 P2_YPos, t_f64 P3_XPos, t_f64 P3_YPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx 	(CECAT_API *ecmIxMot_Arc3P_A_FF) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XPos, t_f64 P2_YPos, t_f64 P3_XPos, t_f64 P3_YPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_Arc3P_A) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XPos, t_f64 P2_YPos, t_f64 P3_XPos, t_f64 P3_YPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmIxMot_Arc3P_A_NB) (t_i32 NetID, t_i32 MapIndex, t_f64 P2_XPos, t_f64 P2_YPos, t_f64 P3_XPos, t_f64 P3_YPos, t_f64 RotAngle_Deg, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_Spline_Start) (t_i32 NetID, t_i32 MapIndex, t_handle32 hSplineObj, t_bool IsReverseDir, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_MPRLin2X_Start) (t_i32 NetID, t_i32 MapIndex, t_handle32 hMPRLin2XObj, t_bool IsReverseDir, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_MoveVia2X_Start) (t_i32 NetID, t_ui16 MapIndex, t_f64 P2[], t_f64 P3[], t_bool IsAbsPosMode, t_i32 RoundPosType, t_f64 NormRadius, t_f64 MinRadius, t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_Stop) (t_i32 NetID, t_i32 MapIndex, t_i32 IsDecStop, t_i32 IsWaitCompt, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmIxMot_Stop_FF) (t_i32 NetID, t_i32 MapIndex, t_i32 IsDecStop, _out t_i32 *ErrCode);

EC_EXTERN t_bool	(CECAT_API *ecmIxSt_IsBusy) (t_i32 NetID, t_i32 MapIndex, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxSt_WaitCompt) (t_i32 NetID, t_i32 MapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmIxSt_WaitCompt_NB) (t_i32 NetID, t_i32 MapIndex, t_i32 *ErrCode);

//====================== MOTION - HOMING OPERATION FUNCTIONS ============================================//
EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeCfg_SetMode) (t_i32 NetID, t_i32 Axis, t_i32 HomeOpMode, _out t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmHomeCfg_GetMode) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeCfg_SetOffset) (t_i32 NetID, t_i32 Axis, t_f64 Offset, _out t_i32 *ErrCode);
EC_EXTERN t_f64		(CECAT_API *ecmHomeCfg_GetOffset) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeCfg_SetSpeedPatt) (t_i32 NetID, t_i32 Axis, t_i32 SpeedMode, t_f64 Vel, t_f64 Acc, t_f64 Dec, t_f64 HomeSpecVel, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmHomeCfg_GetSpeedPatt) (t_i32 NetID, t_i32 Axis, t_i32* SpeedMode, t_f64* Vel, t_f64* Acc, t_f64* Dec, t_f64* HomeSpecVel, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeCfg_SetOption) (t_i32 NetID, t_i32 Axis, EEcmHomeOptID OptionID, t_i32 OptionVal, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmHomeCfg_GetOption) (t_i32 NetID, t_i32 Axis, EEcmHomeOptID OptionID, t_i32 *ErrCode);

EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeMot_MoveStart) (t_i32 NetID, t_i32 Axis, t_i32 Direction, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx	(CECAT_API *ecmHomeMot_MoveStart_FF) (t_i32 NetID, t_i32 Axis, t_i32 Direction, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmHomeMot_Move) (t_i32 NetID, t_i32 Axis, t_i32 Direction, _out t_i32 *ErrCode);

EC_EXTERN t_bool	(CECAT_API *ecmHomeSt_IsBusy) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmHomeSt_WaitCompt) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_word	(CECAT_API *ecmHomeSt_GetFlags) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);

//====================== MOTION - Master/Slave FUNCTIONS ============================================//
EC_EXTERN t_success (CECAT_API *ecmMsCfg_SetSlvEnv) (t_i32 NetID, t_i32 Axis, t_i32 MasterAxis, t_f64 PosRatio, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmMsCfg_GetSlvEnv) (t_i32 NetID, t_i32 Axis, t_i32* MasterAxis, t_f64* PosRatio, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmMsCtl_StartSlv) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmMsCtl_StopSlv) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecmMsSt_IsSlvStarted) (t_i32 NetID, t_i32 Axis, _out t_i32 *ErrCode);

//====================== MOTION - ListMotion FUNCTIONS ============================================//
EC_EXTERN t_success	(CECAT_API *ecmLmCtl_Begin) (t_i32 NetID, t_i32 LmMapIndex, t_ui32 AxisMask1, t_ui32 AxisMask2, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCtl_End) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCtl_Run) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCtl_Stop) (t_i32 NetID, t_i32 LmMapIndex, t_bool IsComptCurStep, t_i32 DecelTime_ms, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCtl_ClearQue) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCfg_SetQueFullMode) (t_i32 NetID, t_i32 LmMapIndex, t_i32 QueFullMode, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmLmCfg_GetQueFullMode) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCfg_SetQueDepth) (t_i32 NetID, t_i32 LmMapIndex, t_i32 QueDepth, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmLmCfg_GetQueDepth) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCfg_SetStepId) (t_i32 NetID, t_i32 LmMapIndex, t_i32 StepId, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmLmCfg_GetStepId) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCfg_SetStepParam) (t_i32 NetID, t_i32 LmMapIndex, t_i32 StepParam1, t_i32 StepParam2, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCfg_GetStepParam) (t_i32 NetID, t_i32 LmMapIndex, t_dword *StepParam1, t_dword *StepParam2, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmLmSt_GetRunSts) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecmLmSt_GetRemStepCount) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmSt_GetRunStepInfo) (t_i32 NetID, t_i32 LmMapIndex, t_i32 *RunStepCount, t_i32 *RunStepId, t_i32 *RunStepSts, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmSt_GetRunStepParam) (t_i32 NetID, t_i32 LmMapIndex, t_dword *StepParam1, t_dword *StepParam2, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmLmCmd_Delay) (t_i32 NetID, t_i32 LmMapIndex, t_i32 DelayTime_ms, t_i32 *ErrCode);

//====================== MOTION - PT-Motion FUNCTIONS ============================================//
EC_EXTERN t_success	(CECAT_API *ecmPtmCtl_Begin) (t_i32 NetID, t_i32 PtmMapIndex, t_ui32 AxisMask1, t_ui32 AxisMask2, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmPtmCtl_End) (t_i32 NetID, t_i32 PtmMapIndex, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmPtmCtl_SetHold) (t_i32 NetID, t_i32 PtmMapIndex, t_bool IsHold, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecmPtmCmd_AddItem_PT) (t_i32 NetID, t_i32 PtmMapIndex, t_f64 DurTime, t_f64 PosDataList[], t_i32 NumPosData, t_bool IsAbsPos, t_i32 *ErrCode);
EC_EXTERN EEcmPtmSts (CECAT_API *ecmPtmSt_GetRunSts) (t_i32 NetID, t_i32 PtmMapIndex, t_i32 *RunStepCount, t_i32 *RemStepCount, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecmPtmSt_GetRunStepInfo) (t_i32 NetID, t_i32 PtmMapIndex, t_i32 *RunStepCount, t_i32 *RunStepSts, t_i32 *ErrCode);


//*****************************************************************************************************************************
//*
//*							Functions for Advanced Users
//*
//*****************************************************************************************************************************

//====================== DEBUG LOGGING FUNCTIONS =========================================================//
EC_EXTERN t_success (CECAT_API *ecDlog_SetFilePath) (char* szLogFilePath, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecDlog_GetFilePath) (char *pszLogFilePath, t_i32 BufLen, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecDlog_SetLogType) (int DebugLogType, t_i32 *ErrCode);
EC_EXTERN int		(CECAT_API *ecDlog_GetLogType) (void);
EC_EXTERN t_success (CECAT_API *ecDlog_SetLogLevel) (int DebugLogLevel, t_i32 *ErrCode);
EC_EXTERN int		(CECAT_API *ecDlog_GetLogLevel) (void);

//====================== NET INTERFACE FUNCTIONS =========================================================//
EC_EXTERN t_success (CECAT_API *ecNet_SetFastFuncType) (t_i32 NetID, t_ui8 FastCmdType, t_i32 *ErrCode);
EC_EXTERN t_ui8		(CECAT_API *ecNet_GetFastFuncType) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_InPDO_GetSectPos) (t_i32 NetID, t_ui16 SectionID, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_OutPDO_GetSectPos) (t_i32 NetID, t_ui16 SectionID, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_InQue_ClearChanList) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN HANDLE 	(CECAT_API *ecNet_InQue_AddChannel) (t_i32 NetID, TEcInQueDataDesc *pDataDescList, int NumDataDescLisItems, t_i32 QueBufDepth, t_bool IsCircular, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_DelChannel) (t_i32 NetID, HANDLE QueChanHandle, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecNet_InQue_GetNumDataDescItems) (t_i32 NetID, HANDLE QueChanHandle, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_InQue_GetDataDesc) (t_i32 NetID, HANDLE QueChanHandle, t_i32 ItemIndex, TEcInQueDataDesc *pDataDesc, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_InQue_GetQueSizeInfo)	(t_i32 NetID, HANDLE QueChanHandle, t_i32 *QueItemSize, t_i32 *QueBufDepth, t_bool *IsCircular, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_SetStaTrgEnv) (t_i32 NetID, HANDLE QueChanHandle, TEcInQueTrgCfg *pTrgCfgData, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_GetStaTrgEnv) (t_i32 NetID, HANDLE QueChanHandle, TEcInQueTrgCfg *pTrgCfgBuf, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_SetStopTrgEnv) (t_i32 NetID, HANDLE QueChanHandle, TEcInQueTrgCfg *pTrgCfgData, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_GetStopTrgEnv) (t_i32 NetID, HANDLE QueChanHandle, TEcInQueTrgCfg *pTrgCfgBuf, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_StartChannel) (t_i32 NetID, HANDLE QueChanHandle, t_i32 SamplingInterval, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_StopChannel) (t_i32 NetID, HANDLE QueChanHandle, t_i32 *ErrCode);
EC_EXTERN t_i32 	(CECAT_API *ecNet_InQue_GetQueuedCount) (t_i32 NetID, HANDLE QueChanHandle, t_i32 *ErrCode);
EC_EXTERN t_i32 	(CECAT_API *ecNet_InQue_Deque) (t_i32 NetID, HANDLE QueChanHandle, void *pBuf, int nBufSize, t_i32 ReqDequeCnt, t_i32 *ErrCode);
EC_EXTERN t_i32 	(CECAT_API *ecNet_InQue_PeekData) (t_i32 NetID, HANDLE QueChanHandle, void *pBuf, int nBufSize, int nStartIndex, t_i32 ReqPeekCnt, t_i32 *ErrCode);
EC_EXTERN HANDLE	(CECAT_API *ecNet_InQue_AddCommonTrg) (t_i32 NetID, TEcInQueCommonTrg *pCommonTrgObj, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_DelCommonTrg) (t_i32 NetID, HANDLE CommTrgHandle, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_EnableCommonTrg) (t_i32 NetID, HANDLE CommTrgHandle, t_bool IsEanble, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_InQue_SetCommonTrgState) (t_i32 NetID, HANDLE CommTrgHandle, t_bool State, t_i32 *ErrCode);
EC_EXTERN t_pdoidx	(CECAT_API *ecNet_GetLastPDOIdx) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecNet_IsPDOIdxCompt) (t_i32 NetID, t_pdoidx PDOIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_WaitPDOIdxCompt) (t_i32 NetID, t_pdoidx PDOIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_WaitPDOIdxCompt_NB) (t_i32 NetID, t_pdoidx PDOIdx, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecNet_IsCmdIdxCompt) (t_i32 NetID, t_cmdidx CmdIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_WaitCmdIdxCompt) (t_i32 NetID, t_cmdidx CmdIdx, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecNet_WaitCmdIdxCompt_NB) (t_i32 NetID, t_cmdidx CmdIdx, t_i32 *ErrCode);
EC_EXTERN t_ui32	(CECAT_API *ecNet_GetCycleProcessTime) (t_i32 NetID, t_ui8 ProcTimeType, t_i32 *ErrCode);

//====================== ECAT CONFIG FUNCTIONS ==========================================================//
EC_EXTERN t_success (CECAT_API *ecCfgFile_Download) (char *szFilePath, t_i32 DevIdx, t_i32 LocNetIdx, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfgFile_Verify) (char *szFilePath, t_i32 DevIdx, t_i32 LocNetIdx, t_i32 *ErrCode);


//*****************************************************************************************************************************
//*
//*							Functions for Internal System Users
//*
//*****************************************************************************************************************************

//====================== GENERAL FUNCTIONS ==============================================================//
EC_EXTERN ULONG		(CECAT_API *ecGn_GetDebugData) (t_i32 DevIdx, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpB) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_byte Data, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpsB) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_byte* pData, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpW) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_word Data, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpsW) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_word* pData, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpD) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_dword Data, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_OutpsD) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_dword* pData, t_i32 *ErrCode);
EC_EXTERN t_byte	(CECAT_API *ecGn_InpB) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_InpsB) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_byte* pBuffer, t_i32 *ErrCode);
EC_EXTERN t_word	(CECAT_API *ecGn_InpW) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_InpsW) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_word* pBuffer, t_i32 *ErrCode);
EC_EXTERN t_dword	(CECAT_API *ecGn_InpD) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_InpsD) (t_i32 DevIdx, t_i32 CS, t_i32 ByteOffset, t_i32 Count, t_dword* pBuffer, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_DbgTempCmd) (t_i32 DevIdx, t_i32 Data1, t_i32 Data2, t_i32 Data3, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecGn_ReadInPDO) (t_i32 NetID, t_ui32 StartAddr, t_ui32 Size, void* pBuf, t_i32 *ErrCode);

//====================== NET INTERFACE FUNCTIONS =========================================================//
EC_EXTERN t_cmdidx	(CECAT_API *ecNet_SendRcvEcatCmd) (t_i32 NetID, EEcEcatCmd EcatCmd, t_ui16 ADP, t_ui16 ADO, t_ui16 DataSize, t_byte *pDataBuf, t_ui32 Timeout, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWDnld_Process) (t_i32 NetID, t_char szFilePath[], t_bool IsVerify, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWDnld_Cancel) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWDnld_GetSts) (t_i32 NetID, TEcFwuStatus *pStsBuf, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWDnld_ResetSts) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWUpld_GetFileInfo) (t_i32 NetID, TEcFwuFileInfo *pFileInfo, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWUpld_Process) (t_i32 NetID, t_char szFilePath[], t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWUpld_Cancel) (t_i32 NetID, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWUpld_GetSts) (t_i32 NetID, TEcFwuStatus *pStsBuf, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecNet_FWUpld_ResetSts) (t_i32 NetID, t_i32 *ErrCode);

//====================== ECAT CONFIG FUNCTIONS ==========================================================//
EC_EXTERN t_success (CECAT_API *ecCfg_Start) (t_i32 NetID, BOOL IsApplyTimeout, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_End) (t_i32 NetID, BOOL IsSave, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecCfg_SetNetConfig) (t_i32 NetID, _in TEcNetConfig *pNetCfgHeader, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecCfg_GetNetConfig) (t_i32 NetID, _out TEcNetConfig *pNetCfgHeader, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_SetOutPDOLogicSect) (t_i32 NetID, t_i32 SectIdx, _in TEcLogMemSectCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_GetOutPDOLogicSect) (t_i32 NetID, t_i32 SectIdx, _out TEcLogMemSectCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_SetInPDOLogicSect) (t_i32 NetID, t_i32 SectIdx, _in TEcLogMemSectCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_GetInPDOLogicSect) (t_i32 NetID, t_i32 SectIdx, _out TEcLogMemSectCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecCfg_SetSlaveConfig) (t_i32 NetID, t_i32 SlaveIdx, _in TEcSlaveCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecCfg_GetSlaveConfig) (t_i32 NetID, t_i32 SlaveIdx, _out TEcSlaveCfg *pCfgData, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_SetChanMap) (t_i32 NetID, t_ui8 MapType, t_i32 ChanMapIdx, _in void* pChanMapData, t_i32 ChanMapDataSize, _out t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecCfg_GetChanMap) (t_i32 NetID, t_ui8 MapType, t_i32 ChanMapIdx, _in void* pChanMapBuf, t_i32 ChanMapBufSize, _out t_i32 *ErrCode);

//====================== XML FILE READING FUNCTIONS ==========================================================//
EC_EXTERN HANDLE	(CECAT_API *ecXml_LoadByProdInfo) (char *szXmlDirectory, TEcSlvProdInfo *pProdInfo, _out char *szFileNameBuf, t_i32 FileNameBufLen, t_i32 *ErrCode);
EC_EXTERN HANDLE	(CECAT_API *ecXml_LoadByFileName) (char *szXmlDirectory, char *szFileName, t_i32 *ErrCode);
EC_EXTERN void		(CECAT_API *ecXml_Unload) (HANDLE XmlHandle, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_GetTagName) (HANDLE XmlHandle, char *szTagNameBuf, t_i32 TagNameBufLen, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecXml_GetNumElem) (HANDLE XmlHandle, t_bool IsResetPos, const char *szElemPath, t_i32 *ErrCode);
EC_EXTERN t_success	(CECAT_API *ecXml_FindElem) (HANDLE XmlHandle, t_bool IsResetPos, const char *szElemPath, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_IntoElem) (HANDLE XmlHandle, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_OutOfElem) (HANDLE XmlHandle, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_ResetPos) (HANDLE XmlHandle, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_SavePos) (HANDLE XmlHandle, char *szPosName, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_RestorePos) (HANDLE XmlHandle, char *szPosName, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_ReadElemData) (HANDLE XmlHandle, char *szDataBuf, t_i32 DataBufLen, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_ReadElemAttr) (HANDLE XmlHandle, const char *szAttrName, char *szDataBuf, t_i32 DataBufLen, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_FindReadElemData) (HANDLE XmlHandle, t_bool IsResetPos, const char *szElemPath, char *szDataBuf, t_i32 DataBufLen, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_FindReadElemAttr) (HANDLE XmlHandle, t_bool IsResetPos, const char *szElemPath, const char *szElemAttrName, char *szDataBuf, t_i32 DataBufLen, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_FindDeviceByProdInfo) (HANDLE XmlHandle, TEcSlvProdInfo *pProdInfo, t_i32 *ErrCode);
EC_EXTERN t_bool	(CECAT_API *ecXml_Str2Bool) (char *szString, t_i32 *ErrCode);
EC_EXTERN t_i32		(CECAT_API *ecXml_Str2HexDec) (char *szString, t_i32 *ErrCode);
EC_EXTERN t_success (CECAT_API *ecXml_GetProdDesc) (HANDLE XmlHandle, char *szLcId, TEcSlvProdInfo *pProdInfo, TEcSlvProdDesc *pProdDesc, t_i32 *ErrCode);

//====================== SLAVE I/F FUNCTIONS ============================================================//
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadEEPR) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 EeeprAddr, t_i32 DataSize, _out void  *pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_i32			(CECAT_API *ecSlv_ReadEEPR_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 EeeprAddr, t_i32 DataSize, _out void  *pBuf, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteEEPR) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 EeeprAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteEEPR_FF) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 EeeprAddr, t_i32 DataSize, void *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteEEPR_A) (t_i32 NetID, t_i32 SlaveIdx, t_i32 EeeprAddr, t_i32 DataSize, void  *pData, _out t_i32 *ErrCode);
EC_EXTERN t_cmdidx		(CECAT_API *ecSlv_WriteEEPR_A_FF) (t_i32 NetID, t_i32 SlaveIdx, t_i32 EeeprAddr, t_i32 DataSize, void  *pData, _out t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_RenewCrcOfEEPR_A) (t_i32 NetID, t_ui16 SlaveIndex, t_i32 *ErrCode);
EC_EXTERN t_success		(CECAT_API *ecSlv_RenewCrcOfEEPR) (t_i32 NetID, t_ui16 SlvPhysAddr, t_i32 *ErrCode);
EC_EXTERN t_ui16		(CECAT_API *ecSlv_ReadReg134ID) (t_i32 NetID, t_i32 SlaveIdx, _out t_i32 *ErrCode);
EC_EXTERN t_ui16		(CECAT_API *ecSlv_ReadReg134ID_A) (t_i32 NetID, t_ui16 SlvPhysAddr, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiODList*	(CECAT_API *ecSlv_SdoInfo_GetODList) (t_i32 NetID, t_ui16 SlvPhysAddr, EEcSdoiODListType ODListType, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiODList*	(CECAT_API *ecSlv_SdoInfo_GetODList_A) (t_i32 NetID, t_ui16 SlaveIdx, EEcSdoiODListType ODListType, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiObjDesc*	(CECAT_API *ecSlv_SdoInfo_GetObjDesc) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui16 ObjIndex, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiObjDesc*	(CECAT_API *ecSlv_SdoInfo_GetObjDesc_A) (t_i32 NetID, t_ui16 SlaveIdx, t_ui16 ObjIndex, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiEntryDesc* (CECAT_API *ecSlv_SdoInfo_GetEntryDesc) (t_i32 NetID, t_ui16 SlvPhysAddr, t_ui16 ObjIndex, t_ui8 SubIndex, t_ui8 ValueInfo, _out t_i32 *ErrCode);
EC_EXTERN TEcSdoiEntryDesc* (CECAT_API *ecSlv_SdoInfo_GetEntryDesc_A) (t_i32 NetID, t_ui16 SlaveIdx, t_ui16 ObjIndex, t_ui8 SubIndex, t_ui8 ValueInfo, _out t_i32 *ErrCode);
EC_EXTERN t_success			(CECAT_API *ecSlv_SdoInfo_ReleaseData) (t_i32 NetID, t_ui16 SlaveIdx, void *pSdoiStruct, _out t_i32 *ErrCode);

#ifdef __cplusplus
}
#endif

#endif