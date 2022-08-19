#pragma once


// CDlgViewChannelPack 대화 상자입니다.
#include "GroupListCtrl.h"
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "GridCtrl_src/GridCtrl.h"

enum ListCtrlPackType
{
	ListCtrlPackTypeChannelNumber = 0,
	ListCtrlPackTypeStatus,
	ListCtrlPackTypeMode,
	ListCtrlPackTypeOperationCode,
	ListCtrlPackTypeProcess,
	ListCtrlPackTypeVoltage,
	ListCtrlPackTypeAmpere,
	ListCtrlPackTypeWatt,
	ListCtrlPackTypeAmpereHour,
	ListCtrlPackTypeWattHour,
	ListCtrlPackTypeStepTime,
	ListCtrlPackTypeTotalTime,
	ListCtrlPackTypeDAQTemperature,
	ListCtrlPackTypeDAQVoltage,
	ListCtrlPackTypeCycleCount,
	ListCtrlPackTypeCycleNumber,
	ListCtrlPackTypeStepNumber,
	ListCtrlPackTypeFileName,
	ListCtrlPackTypeSchedule,
	ListCtrlPackTypeEndCondition,	
	ListCtrlPackTypeEQMode,
	ListCtrlPackTypeCount,
	ListCtrlPackTypeTemperaturePV,	// 현재
	ListCtrlPackTypeTemperatureSP,	// 설정
};

class CDlgViewChannelPack : public CDialogEx, public CSingleton< CDlgViewChannelPack >
{
	DECLARE_DYNAMIC(CDlgViewChannelPack)

public:
	CDlgViewChannelPack(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewChannelPack();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MAIN_VIEW_CHANNEL_PACK_TYPE };

public:
	void SetChannelDisconnect(int idx, bool bDisconnect);		// 연결 끊긴 채널 빨간색 처리

private:
	CSkinButton		m_btnChannel;

	CColorStaticST	m_ctrlChannelDesc;
	CColorStaticST	m_ctrlChannelName;	

	enum MenuType
	{
		MenuTypeCommand = 0,
		MenuTypeSetting,
		MenuTypeView
	};

	enum
	{
		BitmapControlStart = 0,
		BitmapControlRestart,
		BitmapControlNext,
		BitmapControlClear,
		BitmapControlSetDevice,
		BitmapControlAutoCal,
		BitmapControlMaxCount
	};

	enum
	{
		BitmapPauseOp1 = 0,
		BitmapPauseOp2,
		BitmapPauseOp3,
		BitmapPauseMaxCount
	};

	enum
	{
		BitmapCompOp1 = 0,
		BitmapCompOp2,
		BitmapCompOp3,
		BitmapCompMaxCount
	};

	enum
	{
		BitmapRelayInputOff = 0,
		BitmapRelayInputOn,
		BitmapRelayOutputOff,
		BitmapRelayOutputOn,
		BitmapRelayAllOff,
		BitmapRelayMaxCount
	};

	CBitmap	m_bitmap[BitmapControlMaxCount];
	CBitmap	m_bitmapPause[BitmapPauseMaxCount];
	CBitmap	m_bitmapComp[BitmapCompMaxCount];
	CBitmap m_bitmapRelay[BitmapRelayMaxCount];

	CCriticalSection	m_csLock;

private:
	void	InitCtrl();
	void	InitListCtrlChannel();

	void	LoadChannel();
	void	DrawChannel();
	void	LoadDrawINI();
	
	COLORREF	Status2Color(CString strStatus);
	COLORREF	OpCode2Color(CString strOpCode);
	COLORREF	RunMode2Color(CString strStatus);

	CStringArray m_strArrTitle;
	CArray<int, int> m_ArrTitleSize;

	int m_nGridRowSize = 0;
	int m_nSelectJig = 0;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//pnx_hr 20210310 
	CGridCtrl m_Grid;

	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();

	//pnx_hr 2021.02.16
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridBeginDrag(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	LRESULT CompleteInitDialog(WPARAM w, LPARAM l);
};
