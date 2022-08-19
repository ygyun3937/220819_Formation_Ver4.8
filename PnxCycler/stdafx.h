
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifdef _DEBUG
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include "turby_define.h"
#include "xShadeButton/xShadeButton.h"
#include "defineIO.h"

#pragma warning(disable:4840)
#pragma warning(disable:4477)

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include <direct.h>
#include <io.h>

// for use gdiplus
#include <gdiplus.h>
#include <afxsock.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include "MgrSystemLog.h"
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
using namespace Gdiplus;
#pragma comment( lib, "gdiplus" )

#pragma comment(lib, "winmm.lib") 

//220113 YGY Log Data Test
#define MAX_LOG_DATA	10

//220111 YGY UI Test
#define EQ_STAGE_INDEX		2			//Stage 개수
#define EQ_CH_INDEX	48			//장비 채널 개수


#define		COLOR_BACK						RGB(128,128,0)
#define		COLOR_RED						RGB(255,0,0)
#define		COLOR_RED_200				RGB(200,0,0)
#define		COLOR_RED_128				RGB(128,0,0)
#define		COLOR_RED_OUTPUT			RGB(255,200,200)		
#define		COLOR_GREEN					RGB(59,232,36)
#define		COLOR_GREEN_200			RGB(0,200,0)
#define		COLOR_GREEN_128			RGB(0, 128, 0)
#define		COLOR_GREEN_INPUT		RGB(200,255,200)
#define		COLOR_BLUE						RGB(0,0,255)
#define		COLOR_BLUE_128				RGB(0, 128, 128)
#define		COLOR_BLUE_C1				RGB(0,128,200)
#define		COLOR_BLUE_C2				RGB(0,100,255)
#define		COLOR_LINE						RGB(217,223,220)
#define		COLOR_SUB_NONE				RGB(38,22,25)
#define		COLOR_CLICK_BUTTON		RGB(27,9,133)/*203,109,57*//*27,9,133*//*200,136,8*//*241,13,176*/
#define		COLOR_SUB_BUTTON			RGB(208,107,40)
#define		COLOR_SUB_TITLE				RGB(78,94,84)
#define		COLOR_MENU_GUIDE			RGB(119,119,119)
#define		COLOR_SUB_CLICK				RGB(35,15,39)
#define		COLOR_COMMAND_CLICK	RGB(225,231,228)
#define		COLOR_CMDCLICK_TEXT		RGB(33,39,36)
#define		COLOR_COMMAND_NOT		RGB(44,44,44)
#define		COLOR_NON_BUTTON		RGB(203,109,57)
#define		COLOR_FUNC_BTN				RGB(132,151,176)
#define		COLOR_WAIT_BTN				RGB(206,214,224)
#define		COLOR_PUSH_BTN				RGB(48,59,74)
#define		COLOR_PRODUCT_TITLE		RGB(54,54,54)
#define		COLOR_LOGIN_BUTTON		RGB(24,30,26)
#define		COLOR_WHITE_PINK			RGB(250,30,156)
#define		COLOR_WHITE_RED			RGB(255,42,37)
#define		COLOR_MENU					RGB(57,69,62)
#define		COLOR_SUB_MENU				RGB(94,114,103)
#define		COLOR_TOPBAR					RGB(121,121,121)
#define		COLOR_CLOUDBLUE			RGB(128, 184, 223)
#define		COLOR_CYAN_RIGHT			RGB(189,255,255)
#define		COLOR_CYAN						RGB(0, 255, 255)
#define		COLOR_CYAN_128				RGB(0, 128, 128)
#define		COLOR_MAGENTA				RGB(255, 0, 255)
#define		COLOR_MAGENTA_128		RGB(128, 0, 128)
#define		COLOR_BASIC					RGB(212, 208, 200)
#define		COLOR_AUTO_MSG_BACK			RGB(0,32,96)
#define		COLOR_TITLE_POINT			RGB(30, 70, 140)
#define		COLOR_TITLE_TEXT			RGB(255,255,255)//RGB(0,0,0)
#define		COLOR_CONTOUR				RGB(255,255,255)
#define		COLOR_BLACK					RGB(0,0,0)
#define		COLOR_TAB_BACK				RGB(84,103,130)


#define COLOR_INPUT_IO_OFF_SIGNAL_BACK		RGB(49,89,158)
#define COLOR_INPUT_IO_OFF_SIGNAL_TEXT		RGB(255,255,255)
#define COLOR_INPUT_IO_ON_SIGNAL_BACK		RGB(25,118,12)
#define COLOR_INPUT_IO_ON_SIGNAL_TEXT		RGB(0,0,0)
#define COLOR_NONCLICK_CHECK_TEXT			RGB(255,255,255)
#define COLOR_NONCLICK_CHECK_BACK			RGB(49,89,158)
//#define COLOR_TITLE_BACK					RGB(38,45,56)
#define COLOR_SUB_SUB_MENU_TEXT				RGB(0,0,0)
#define COLOR_PROGRAM_BACKGROUND			RGB(245,245,245)

#define ColorRed		RGB( 228, 64, 42 )
#define	ColorWhite		RGB( 255, 255, 255 )
#define ColorBlack		RGB( 0, 0, 0 )
#define ColorYellow		RGB( 232, 174, 34 )
#define ColorGreen		RGB( 141, 196, 63 )
#define ColorBlue		RGB( 34, 154, 230 )
#define ColorGray		RGB( 202, 202, 202 )
#define ColorLightRed	RGB( 199, 92, 92 )
#define ColorLightGreen	RGB( 118, 194, 175 )
#define ColorLightBlue	RGB( 119, 179, 212 )
#define ColorLightBlack	RGB( 79, 93, 115 )
#define ColorLightGray	RGB(126, 136, 145)
#define ColorPurple		RGB(102, 0, 102)
#define ColorSlateblue  RGB(106, 90, 205)
#define ColorWhiteYellow  RGB(255, 255, 115)

//#define	_LANG_KOR
#define _LANG_ENG

#define TOTAL_SAFETY_ENABLE		0

#define SKINBTN( ctrl, nID, Wnd, StdPath, HovPath )	\
{	\
	ctrl.SubclassDlgItem(nID, Wnd);	\
	ctrl.SetTextAlign(DT_CENTER); \
	ctrl.SetBtnType(CSkinButton::E_TYPE_NORMAL); \
	ctrl.SetBtnEffect(FALSE); \
	ctrl.SetBtnImagePath(CSkinButton::E_HALF_1, StdPath, StdPath, HovPath, StdPath);	\
	ctrl.SetWindowText(_T("")); \
}

#define CST_DEFAULT( ctrl, nID, Wnd, Height, Width, Bold, BkColor, TxtColor, Str ) \
{	\
	ctrl.SubclassDlgItem(nID, Wnd); \
	ctrl.SetFontSize(Height, Width, TRUE);	\
	ctrl.SetBkColor(BkColor);	\
	ctrl.SetTextColor(TxtColor);	\
	ctrl.ModifyStyleEx(0, WS_EX_TRANSPARENT);	\
	ctrl.SetWindowText(Str);	\
}

#define DELETE_POINTER( p ) { if( NULL != p ) { delete ( p ); ( p ) = NULL; } }

// By JSK - 주의.. GUI 관련 메시지는 전부 PostMessage 로 처리할것!
#define WM_PROGRAM_EXIT						( WM_USER + 200 )
#define WM_UPDATE_VIEW_CHANNEL				( WM_USER + 201 )
#define WM_UPDATE_VIEW_ALARM				( WM_USER + 202 )
#define WM_DAQ_SAFETY_WARNING				( WM_USER + 203 )
#define WM_DAQ_COMPLETE_WARNING				( WM_USER + 204 )
#define WM_UPDATE_VIEW_ALARM_STATUS			( WM_USER + 206 )
#define WM_HIDE_SCHEDULE_GUI				( WM_USER + 207 )
#define WM_NEXT_PAUSE_RESERVE				( WM_USER + 208 )
#define WM_NEXT_COMPLETE_RESERVE			( WM_USER + 209 )
#define WM_NEXT_STEP_RESERVE				( WM_USER + 210 )
#define WM_MULTI_SCHEDULE_START				( WM_USER + 212 )
#define WM_DAQ_UPDATE						( WM_USER + 213 )
#define WM_UPDATE_DAQ_VIEW					( WM_USER + 214 )
#define WM_DSP_PORT_STATUS					( WM_USER + 215 )

#define WM_SOCKET_EVENT_CH1					( WM_USER + 400 )
#define WM_SOCKET_EVENT_CH2					( WM_USER + 401 )
#define WM_SOCKET_EVENT_CH3					( WM_USER + 402 )
#define WM_SOCKET_EVENT_CH4					( WM_USER + 403 )
#define WM_SOCKET_EVENT_CH5					( WM_USER + 404 )
#define WM_SOCKET_EVENT_CH6					( WM_USER + 405 )
#define WM_SOCKET_EVENT_PLC_INTERFACE			( WM_USER + 406 )

#define WM_VIEW_PATTERN						( WM_USER + 501 )


#define WM_TCP_SOCKET_MSG (WM_USER+1)

#define ROUNDING(x, digit) (floor((x)*pow(float(10), digit) + 0.5f) / pow(float(10), digit))
//LOG 구조 변경
#define SYSLOG					CMgrSystemLog::GetMgr()->LOG
//


enum eCyclerChannelState
{
	eCyclerChannelStateUnknown = 0,
	eCyclerChannelStateIdle,
	eCyclerChannelStateReady,
	eCyclerChannelStateStart,
	eCyclerChannelStatePause,					// 작업자에 의한 Pause
	eCyclerChannelStatePauseFromDwell,			// Dwell Time
	eCyclerChannelStatePauseFromWaitChamber,	// 챔버 승온 대기
	eCyclerChannelStatePauseFromTempSync,		// 온도/챔버 동기화
	eCyclerChannelStatePauseFromStepSync,		// 스텝 동기화
	eCyclerChannelStateComplete,
	eCyclerChannelStateError,
	eCyclerChannelStateCommuicationError,
	eCyclerChannelStateMaxCount
};

enum eInverterStatus
{
	eInverterStatusDisconnect = 0,
	eInverterStatusInit,
	eInverterStatusInputReadyOn,
	eInverterStatusVoltCalcStart,
	eInverterStatusConnectComplete,
	eInverterStatusError,
	eInverterStatusMaxCount
};


enum eInterlockType
{
	eInterlockTypeCyclerChannel = 0,
	eInterlockTypePause,
	eInterlockTypeComplete,
	eInterlockTypeJump,
	eInterlockTypeMaxCount
};

enum eIFBoardCyclerCode
{
	eIFBoardCyclerCodeEmpty = 0,
	eIFBoardCyclerCodeTimeCut,
	eIFBoardCyclerCodeCurrentCut,
	eIFBoardCyclerCodeCapacityCut,
	eIFBoardCyclerCodeWattCut,
	eIFBoardCyclerCodeVoltageCut,
	eIFBoardCyclerCodeCycleLoopComplete,	// 사이클 루프 만족하여 다음으로 이동할 떄
	eIFBoardCyclerCodeCycleLoopResume,		// 사이클 루프 비 만족하여 초기 진입점으로 이동할 때
	eIFBoardCyclerCodeCycleVoltageCut,
	eIFBoardCyclerCodeCycleTimeCut,
	eIFBoardCyclerCodeCycleCapacityCut,
	eIFBoardCyclerCodeCycleWattCut,
	eIFBoardCyclerCodeCycleLowVoltageCut,
	eIFBoardCyclerCodeJump,
	eIFBoardCyclerCodeComplete,
	eIFBoardCyclerCodeAlarm,
	eIFBoardCyclerCodePause,
	eIFBoardCyclerCodeDwellSingle,
	eIFBoardCyclerCodeDwellMulti,
	eIFBoardCyclerCodeRisingChamberSingle,  // 18번 더이상 사용안함으로, 올라오면 완료처리
	eIFBoardCyclerCodeRisingChamberMulti,
	eIFBoardCyclerCodeStepSync,
	eIFBoardCyclerCodeMaxCount
};


enum eMonitroingCyclerCode
{
	eMonitroingCyclerCodeSystemSafetyVoltageMax = 100,
	eMonitroingCyclerCodeSystemSafetyVoltageMin,
	eMonitroingCyclerCodeSystemSafetyCurrentMax,
	eMonitroingCyclerCodeSystemSafetyCurrentMin,
	eMonitroingCyclerCodeSystemSafetyTemperatureMax,
	eMonitroingCyclerCodeSystemSafetyTemperatureMin,
	eMonitroingCyclerCodeSystemSafetyCapacityMax,
	eMonitroingCyclerCodeSystemSafetyCapacityMin,
	eMonitroingCyclerCodeSystemSafetyDeltaV,
	eMonitroingCyclerCodeSystemSafetyDeltaI,
	eMonitroingCyclerCodeCycleSafetyVoltageMax,
	eMonitroingCyclerCodeCycleSafetyVoltageMin,
	eMonitroingCyclerCodeCycleSafetyWattMax,
	eMonitroingCyclerCodeCycleSafetyCapacityMax,
	eMonitroingCyclerCodeCycleSafetyCurrentMax,
	eMonitroingCyclerCodeCycleSafetyWatthourMax,
	eMonitroingCyclerCodeStepSafetyVoltageMax,
	eMonitroingCyclerCodeStepSafetyVoltageMin,
	eMonitroingCyclerCodeStepSafetyCurrentMax,
	eMonitroingCyclerCodeStepSafetyCurrentMin,
	eMonitroingCyclerCodeStepSafetyCapacityMax,
	eMonitroingCyclerCodeStepSafetyTemperatureMax,
	eMonitroingCyclerCodeStepComplete,
	eMonitroingCyclerCodecycleComplete,
	eMonitroingCyclerCodeStepPause,
	eMonitroingCyclerCodeCyclePause,
	eMonitroingCyclerCodeSoC,
	eMonitroingCyclerCodeSoE,
	eMonitroingCyclerCodeDaQVoltage,
	eMonitroingCyclerCodeDaQTemperature,
	eMonitroingCyclerCodeBms,
	eMonitroingCyclerCodeChamber,
	eMonitroingCyclerCodeCycleChargeCapacity,
	eMonitroingCyclerCodeSoH,
	eMonitroingCyclerCodeScheduleSafetyVoltageMax,
	eMonitroingCyclerCodeScheduleSafetyVoltageMin,
	eMonitroingCyclerCodeSvdfRestCellDevDetect,
	eMonitroingCyclerCodeSvdfUpperCellDevDetect,
	eMonitroingCyclerCodeSvdfLowerCellDevDetect,
	eMonitroingCyclerCodeSystemSafetyCvDeltaV,
	eMonitroingCyclerCodeSystemSafetyCvDeltaI,
	eMonitroingCyclerCodeSystemSafetyContactReg,
	eMonitoringCyclerCodeStage1FireSensor,
	eMonitoringCyclerCodeStage2FireSensor,
	eMonitoringCyclerCodeStage1SmokeSensor1,
	eMonitoringCyclerCodeStage2SmokeSensor1,
	eMonitoringCyclerCodeStage1SmokeSensor2,
	eMonitoringCyclerCodeStage2SmokeSensor2,
	eMonitoringCyclerCodeStage1FanAlarm,
	eMonitoringCyclerCodeStage2FanAlarm,

	eMonitroingCyclerCodeMaxCount
};

enum eCyclerCommandOption
{
	eCyclerCommandOptionPauseImmediately = 0,
	eCyclerCommandOptionPauseAfterStepComplete,
	eCyclerCommandOptionPauseAfterCycleComplete,
	eCyclerCommandOptionPauseReserved,
	eCyclerCommandOptionJumpNext,
	eCyclerCommandOptionJumpReserved,
	eCyclerCommandOptionCompleteImmediately,
	eCyclerCommandOptionCompleteAfterStepComplete,
	eCyclerCommandOptionCompleteAfterCycleComplete,
	eCyclerCommandOptionCompleteReserved,
	eCyclerCommandOptionCalibration_Volt,
	eCyclerCommandOptionCalibration_MasterCalibration,
	eCyclerCommandOptionCalibration_SlaveCalibration,

};

enum eCyclerChannelItemName
{
	eCyclerChannelItemNameChannelNumber = 0,
	eCyclerChannelItemNameStatus,
	eCyclerChannelItemNameMode,
	eCyclerChannelItemNameUserID,
	eCyclerChannelItemNameOperationCode,
	eCyclerChannelItemNameEndReason,
	eCyclerChannelItemNameVoltage,
	eCyclerChannelItemNameCurrent,
	eCyclerChannelItemNameWatt,
	eCyclerChannelItemNameWattHour,
	eCyclerChannelItemNameAh,
	eCyclerChannelItemNameStepTime,
	eCyclerChannelItemNameTotalTime,
	eCyclerChannelItemNameChamberValue,
	eCyclerChannelItemNameChamberSetting,
	eCyclerChannelItemNameCycleCount,
	eCyclerChannelItemNameCycleNumber,
	eCyclerChannelItemNameStepNumber,
	eCyclerChannelItemNameScheduleFilePath,
	eCyclerChannelItemNameScheduleName,
	eCyclerChannelItemNameRating,
	eCyclerChannelItemNameSoc,
	eCyclerChannelItemNameSoh,
	eCyclerChannelItemNameDAQTemperature,
	eCyclerChannelItemNameSampleName,
	eCyclerChannelItemNameDescription,
	eCyclerChannelItemNameMaxCount
};

enum eIFBoardCommand
{
	eIFBoardCommandRest = 0x81,
	eIFBoardCommandCharge = 0x82,
	eIFBoardCommandDischarge = 0x83,
	eIFBoardCommandAlarmClear = 0x84,
	eIFBoardCommandAlarmRequest = 0x90,
	eIFBoardCommandSerialMode = 0x92,
	eIFBoardCommandPogoPinTestMode = 0x93

};

enum eIFBoardPauseCommand
{
	eIFBoardPauseCommandStop = 0,
	eIFBoardPauseCommandPause,
	eIFBoardPauseCommandComplete,
	eIFBoardPauseCommandRestart,
	eIFBoardPauseCommandVoltCalibrationEnd = 0x10,
	eIFBoardPauseCommandMasterCurrentCalibrationEnd = 0x11,
	eIFBoardPauseCommandSlaveCurrentCalibrationEnd = 0x12,

};

enum eIFBoardStepType
{
	eIFBoardStepTypeUnknown = -1,
	eIFBoardStepTypeRest,
	eIFBoardStepTypeCharge,
	eIFBoardStepTypeDischarge,
	eIFBoardStepTypeExternalPattern,
	eIFBoardStepTypeOcv,
	eIFBoardStepTypeImpedance,
	eIFBoardStepTypePogoPinTest

};

enum eSequenceCylinderStep
{
	eSequenceCylinderStep_Wait = -1,
	eSequenceCylinderStep_Side_Forward,
	eSequenceCylinderStep_FrontBack_Forward,
	eSequenceCylinderStep_Top_Down,
	eSequenceCylinderStep_Bottom_Up,
	eSequenceCylinderStep_Bottom_Down,
	eSequenceCylinderStep_Top_Up,
	eSequenceCylinderStep_FrontBack_Backward,
	eSequenceCylinderStep_Side_Backward,
	eSequenceCylinderStep_Max,

};

enum eIFBoardStepMode
{
#if UPDATE_CP_MODE_TURBY
	eIFBoardStepModeUnknown = -1,
#else
	eIFBoardStepModeUnknown = -1,
#endif
	eIFBoardStepModeDefault,
	eIFBoardStepModeCC,
	eIFBoardStepModeCCCV,
	eIFBoardStepModeCP,
	eIFBoardStepModeCPCV
	, elFBoradStepModeAutocalVoltage = 10		// AUTO_CALIBRATION
	, elFBoradStepModeMasterAutocalCurrent = 11		// AUTO_CALIBRATION
	, elFBoradStepModeSlaveAutocalCurrent = 12		// AUTO_CALIBRATION
};

enum eIFBoardOpCode
{
	eIFBoardOpCodeUnknown = 0,
	eIFBoardOpCodeInputRelayOn,
	eIFBoardOpCodeOutputReplyOn,
	eIFBoardOpCodeChargeCC,
	eIFBoardOpCodeChargeCV,
	eIFBoardOpCodeDischargeCC,
	eIFBoardOpCodeDischargeCV,
	eIFBoardOpCodeChargeCP,
	eIFBoardOpCodeDischargeCP
};


enum eIFBoardComplete
{
	eIFBoardCompleteVoltage = 0,
	eIFBoardCompleteTime,
	eIFBoardCompleteCurrent,
	eIFBoardCompleteCapacity,
	eIFBoardCompleteWatt,
	eIFBoardCompleteMaxCount
};

enum eIFBoardOperation
{
	eIFBoardOperationNone = 0,
	eIFBoardOperationChargeCC,
	eIFBoardOperationChargeCCCV,
	eIFBoardOperationChargeCP,
	eIFBoardOperationChargeCPCV,
	eIFBoardOperationDischargeCC,
	eIFBoardOperationDischargeCCCV,
	eIFBoardOperationDischargeCP,
	eIFBoardOperationDischargeCPCV,

};

enum eScheduleValidate
{
	eScheduleValidateNoError = 0,
	eScheduleValidateStepSize,
	eScheduleValidateRestBasic,
	eScheduleValidateCCBasic,
	eScheduleValidateCCCVBasic,
	eScheduleValidateCPBasic,
	eScheduleValidateCPCVBasic
};

enum eScheduleSafetyCheck
{
	eScheduleSafetyCheckNoError = 0,
	eScheduleSafetyCheckConfig,		//시스템Config 없음
	eScheduleSafetyCheckVoltMin,	//최소 전압 
	eScheduleSafetyCheckVoltMax,	//최대 전압 
	eScheduleSafetyCheckCurrMax,	//최대 전류
	eScheduleSafetyCheckWHMax	//전력량
};

enum eScheduleSyncMode
{
	eScheduleSyncModeNotUse = 0,
	eScheduleSyncModeStep,
	eScheduleSyncModeTemperature
};

enum eDaQOperator
{
	eDaQOperatorNone = 0,
	eDaQOperatorLessThan,
	eDaQOperatorLessEqual,
	eDaQOperatorEqual,
	eDaQOperatorGreaterThan,
	eDaQOperatorGraterEqual,
	eDaQOperatorDifferent
};


enum eScheduleLoadingType
{
	eScheduleLoadingTypeAdd = 0,
	eScheduleLoadingTypeAppend
};

enum eChamberDataType
{
	eChamberDataTypeValue = 0,
	eChamberDataTypeSetting
};

enum eDeviceType
{
	eDeviceTypeIFBoard = 0,
	eDeviceTypeDaQ,
	eDeviceTypeChamber,
	eDeviceTypeChiller,
	eDeviceTypeBalanceBoard,
	eDeviceTypeVectorCan,
	eDeviceTypeDmmAnalyzer,
	eDeviceTypeDmmPower,
	eDeviceTypeBmsPower,
	eDeviceTypeAnalyzeTool,
	eDeviceTypePeakCan,
	eDeviceTypeRemote,
	eDeviceTypeCalibrator,
	eDeviceTypePid,
	eDeviceTypePlcInterface,
	eDeivceTypeBarcode,
	eDeviceTypeMaxCount
};

enum StepNextCondition
{		//   start End
	eStepNextEachEach = 0,		//00 00		Chamber battery 동시 시작, 타 채널 기다리지 않는다.  "짝수" 채널 기준으로 챔버 동작한다.
	eStepNextEachWith,			//00 01		Chamber battery 동시 시작, 타 채널 기다린다.
	eStepNextWithEach,			//01 00		Chamber 시작, 온도 만족 후 battery 시작, 타 채널 기다리지 않는다.
	eStepNextWithWith			//01 01		Chamber 시작, 온도 만족 후 battery 시작, 타 채널 기다린다.
};


static const TCHAR* g_lpszInverterStatusString[eInverterStatusMaxCount] =
{
	//"Unknown",									//0
	//"Init",	
	//"Start",								//1
	//"Error"

	"DisConnect",									//0
	"Init",	
	"Input Ready On",								//1
	"Volt Calc Start",
	"Connect Complete",
	"Error",

};

static const TCHAR* g_lpszCyclerStateString[eCyclerChannelStateMaxCount] =
{
	"None",				//0
	"Sample not connected",					//1
	"Possible to Proceed",				//2
	"Proceeding",					//3
	"Pause(worker)",				//4
	"Pause(Dwell)",				//5
	"Pause(Heating)",					//6
	"Pause(Temp Sync)",				//7
	"Pause(Step Sync)",				//8
	"End",				//9
	"Error",				//10
	"Communication error",				//11

};

static const TCHAR* g_lpszIFBoardCode[eIFBoardCyclerCodeMaxCount] =
{

	"NONE",
	"Time",
	"Current",
	"Capacity",
	"Power",
	"Voltage",
	"Repeat Complete",
	"Repeat Entry",
	"Cycle Voltage",
	"Cycle Time",
	"Cycle Capacity",
	"Cycle Power",
	"Cycle Voltage Min",
	"User Move",
	"User End",
	"Alarm",
	"User Stop",
	"Chamber Temp Stabilization(single)",
	"Chamber Temp Stabilization(multi)",
	"Waiting chamber heating (single)",
	"Waiting chamber heating (multi)",
	"Waiting Step Sync"
};

static const TCHAR* g_lpszMonitoringCode[eMonitroingCyclerCodeMaxCount - eMonitroingCyclerCodeSystemSafetyVoltageMax] =
{
	"System Safety Voltage Max",
	"System Safety Voltage Min",
	"System Safety Current Max",
	"System Safety Current Min",
	"System Safety Temp Max",
	"System Safety Temp Min",
	"System Safety Capacity Max",
	"System Safety Capacity Min",
	"System Safety Voltage Change",
	"System Safety Current Change",
	"Cycle Safety Voltage Max",
	"Cycle Safety Voltage Min",
	"Cycle Safety Power Max",
	"Cycle Safety Capacity Max",
	"Cycle Safety Current Max",
	"Cycle Safety Watt Max",
	"Step Safety Voltage Max",
	"Step Safety Voltage Min",
	"Step Safety Currnet Max",
	"Step Safety Current Min",
	"Step Safety Capacity Max",
	"Step Safety Temp Max",
	"Step End",
	"Cycle End",
	"Step Stop",
	"Cycle Stop",
	"SOC/DOD/TTA",
	"SOE/DOE/TTE",
	"DAQ Voltage",
	"DAQ Temp",
	"BMS",
	"Chamber",
	"Cycle Charge Capacity",
	"SOH",
	"Schedule Safety Voltage Max",
	"Schedule Safety Voltage Min",
	"Resting voltage rise",
	"Lower voltage drop",
	"Upper voltage drop",

};

static const TCHAR* g_lpszCyclerChannelItemName[eCyclerChannelItemNameMaxCount] =
{
	"Channel",
	"Status",
	"Mode",
	"User",
	"OperationCode",
	"EndReason",
	"Voltage",
	"Current",
	"Watt",
	"WattHour",
	"Ah",
	"StepTime",
	"TotalTime",
	"ChamberValue",
	"ChamberSetting",
	"CycleCount",
	"CycleNumber",
	"StepNumber",
	"ScheduleFilePath",
	"ScheduleName",
	"Rating",
	"SOC",
	"SOH",
	"DAQTemp",
	"SampleName",
	"Description"
};

static const TCHAR* g_lpszChamberAlarmString[65] =
{
	"NONE",
	"CM1 compressor overcurrent trip."	,
	"CM1 compressor overpressure protection tripping."	,
	"CM1 compressor oil pressure anomaly."	,
	"CM2 compressor overcurrent trip."	,
	"CM2 compressor overpressure protection tripping."	,
	"CM2 compressor oil pressure anomaly."	,
	"CM3 compressor overcurrent trip."	,
	"CM3 compressor overpressure protection tripping."	,
	"storage tank water. （Delayed alarm）"	,
	"storage tank water. （Alarm immediately for water shortage）"	,
	"CM3 compressor oil pressure anomaly."	,
	"CM4 compressor overcurrent trip."	,
	"CM4 compressor overpressure protection tripping."	,
	"CM4 compressor oil pressure anomaly."	,
	"empty burning heating tube protection."	,
	"empty tube humidification burn protection."	,
	"power due to phase or reverse phase."	,
	"the temperature inside the over-temperature protection."	,
	"Alarm B."	,
	"Alarm C."	,
	"box fan operation exception."	,
	"abnormal cooling towers."	,
	"dry-bulb SCR failure."	,
	"wet bulb SCR failure."	,
	"temperature sensor error."	,
	"Humidity sensor error."	,
	"inside the temperature gauge anomaly."	,
	"blower flow。"	,
	"condenser fan flow。"	,
	"filter differential pressure。"	,
	"supplying pressure difference。"	,
	"first compressor low pressure switch protection。"	,
	"second compressor low pressure switch protection。"	,
	"Dehumidifier fault protection。"	,
	"Equipment have not closed the door alarm"	,
	"The box body the emergency stop switch alarm"	,
	"The emergency stop switch alarm"	,
	"Circulating fan flow alarm"	,
	"One exhaust fan alarm"	,
	"Two exhaust fan alarm"	,
	"CM1 compressor heat。"	,
	"CM2 compressor heat。"	,
	"VVVF ERR"	,
	"CIRCULATION FAN ERR"	,
	"AIR DUCT ERR"	,
	"ALM1 TEMP OR HUMIDITY OUT."	,
	"ALM2 TEMP OR HUMIDITY OUT。"	,
	"ALM3 TEMP OR HUMIDITY OUT。"	,
	"Alarm of low pressure"	,
	"Alarm of cool cycle no water"	,
	"INFO1（Custom alarm）"	,
	"INFO2（Custom alarm）"	,
	"INFO3（Custom alarm）"	,
	"INFO4（Custom alarm）"	,
	"INFO5（Custom alarm）"	,
	"INF200（Extension module custom alarm）"	,
	"INF201（Extension module custom alarm）"	,
	"INF202（Extension module custom alarm）"	,
	"INF203（Extension module custom alarm）"	,
	"INF204（Extension module custom alarm）"	,
	"INF205（Extension module custom alarm）"	,
	"INF206（Extension module custom alarm）"	,
	"INF207（Extension module custom alarm）"	,
	"Smoke alarm",
};





extern CCriticalSection g_criticalExe;
extern CCriticalSection g_criticalLog;

extern bool g_bPatternStep[32];
extern int g_nPatternStepCount;
extern int g_nPatternStartStep[32];

extern int g_nScheduleCycleLoop[1000];
extern int g_nCycleLoopStartSet[1000];

extern double g_dTotalCapLog[32];
extern double g_dStepCapLog[32][3];
extern int g_nTotalCapCalIndex[32];

extern int g_nAlarmStatus[32];

extern int g_nRatioSpec[32];

//void	LOG(int nType, const char* fmt, ...);

CString	GetExePath();

CString INIReadStr(CString strAppName, CString strKeyName, CString strFilePath);
CString	INIReadStrW(CString strAppName, CString strKeyName, CString strFilePath);
void	INIWriteStr(CString strAppName, CString strKeyName, CString strValue, CString strFilePath);

int		INIReadInt(CString strAppName, CString strKeyName, CString strFilePath);
void	INIWriteInt(CString strAppName, CString strKeyName, int nValue, CString strFilePath);
void	INIWriteFloat(CString strAppName, CString strKeyName, float fValue, CString strFilePath);

float	INIReadFloat(CString strAppName, CString strKeyName, CString strFilePath);

float	AbsFunc(float f);

int		GetChamberType();
int		GetChamberIndex(int channel);

static int CALLBACK BrowseCalbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED: SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData); break;
	}

	return 0;
}


static void ProcessMessage()
{
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
static void delayMs(ULONGLONG waitTime)
{
	ULONGLONG stick, etick;
	stick = ::GetTickCount64();
	while (1)
	{
		etick = ::GetTickCount64();
		if ((etick - stick) > waitTime)
			break;
		ProcessMessage();
	}
}

//pnx_hr 2021.05.12 추가
bool m_bAdLink;
void  SetAdLinkType(bool bAdLink);
BOOL  GetAdLinkType();
//#define IBPP_WINDOWS

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


