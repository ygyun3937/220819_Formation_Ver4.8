
// PnxCyclerDlg.h : header file
//

#pragma once

// CPnxCyclerDlg dialog
class CPnxCyclerDlg : public CDialogEx
{
public:
	CPnxCyclerDlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_PNXCYCLER_DIALOG };
private:
	enum CommandType
	{
		CommandTypeStart = 0,
		CommandTypePauseOption1,
		CommandTypePauseOption2,
		CommandTypePauseOption3,
		CommandTypePauseOption4,
		CommandTypeContinue,
		CommandTypeNextOption1,
		CommandTypeNextOption2,
		CommandTypeCompleteOption1,
		CommandTypeCompleteOption2,
		CommandTypeCompleteOption3,
		CommandTypeCompleteOption4,
		CommandTypeAlarmClear,
		CommandTypeSafetyCondition,
		CommandTypeViewAlarm,
		CommandTypeViewPattern,
		CommandTypeViewLog,
		CommandTypeAnalyze_Simple,
		CommandTypeAnalyze_Detail,
		CommandTypeViewRelayCount,
		CommandTypeViewReserve,
		CommandTypeViewReserveSchedule,
		CommandTypeViewScheduleAppend,
		CommandTypeModeChangeSingle,
		CommandTypeModeChangeParallel,
		CommandTypeExcuteDAQEditor,
		CommandTypeExcuteDAQLoading,
		CommandTypeReconnect,
		CommandTypeChannelRestart,
		CommandTypeCancelSync,
		CommandTypeMaxCount
	};

	enum
	{
		BitmapFileNew = 0,
		BitmapFileOpen,
		BitmapFileSave,
		BitmapFileSaveAs,
		BitmapFileRecent,
		BitmapFileExit,
		BitmapFileMaxCount
	};

	enum
	{
		BitmapEditUndo = 0,
		BitmapEditCut,
		BitmapEditCopy,
		BitmapEditPaste,
		BitmapEditMaxCount
	};

	enum
	{
		BitmapPauseOp1 = 0,
		BitmapPauseOp2,
		BitmapPauseOp3,
		BitmapPauseOp4,
		BitmapPauseMaxCount
	};

	enum
	{
		BitmapNextOp1 = 0,
		BitmapNextOp2,
		BitmapNextMaxCount
	};

	enum
	{
		BitmapCompOp1 = 0,
		BitmapCompOp2,
		BitmapCompOp3,
		BitmapCompOp4,
		BitmapCompMaxCount
	};

	enum
	{
		BitmapControlStart = 0,
		BitmapControlRestart,
		BitmapAralmclear,
		BitmapPatternView,
		BitmapcontrolAnalzeToolSimple,
		BitmapcontrolAnalzeToolReal,
		BitmapViewReserve,
		BitmapViewReserveSchedule,
		BitmapViewScheduleAppend,
		BitmapControlMaxCount
	};

	enum
	{
		BitmapSetSafety = 0,
		BitmapSetDAQ,
		BitmapLoadDAQ,
		BitmapSetMaxCount
	};

	enum
	{
		BitmapViewLog = 0,
		BitmapViewAlarm,
		BitmapViewMaterial,
		BitmapViewMaxCount
	};


	int		m_nDialogWidth;

	CMenu	m_Menu;

	CBitmap	m_bitmapFile[BitmapFileMaxCount];
	CBitmap	m_bitmapEdit[BitmapEditMaxCount];
	CBitmap	m_bitmapPause[BitmapPauseMaxCount];
	CBitmap	m_bitmapComp[BitmapCompMaxCount];
	CBitmap	m_bitmapControl[BitmapControlMaxCount];
	CBitmap	m_bitmapSetting[BitmapSetMaxCount];
	CBitmap	m_bitmapView[BitmapViewMaxCount];
	CBitmap	m_bitmapNext[BitmapNextMaxCount];

private:
	void	StartSchedule();
	void	PauseOption1();	// Áï½Ã ¸ØÃã
	void	PauseOption2();	// Step ¿Ï·á ÈÄ ¸ØÃã
	void	PauseOption3();	// Cycle ¿Ï·á ÈÄ ¸ØÃã
	void	PauseOption4();
	void	RestartSchedule();
	void	NextOption1();
	void	NextOption2();
	void	CompleteOption1();	// Áï½Ã ¿Ï·á
	void	CompleteOption2();	// Step ¿Ï·á ÈÄ ¿Ï·á
	void	CompleteOption3();	// Cycle ¿Ï·á ÈÄ ¿Ï·á
	void	CompleteOption4();
	void	AlarmClear();
	void	ViewSystemSafetyCondition();
	void	ViewAlarm();
	void	ViewPattern();
	void	ViewLogOpen();
	void	ViewAnalyzeSimple();
	void	ViewAnalyzeDetail();
	void	ViewMaterial();
	void	ViewReserve();
	void	ViewReserveSchedule();
	void	ViewScheduleAppend();
	void	OpenDeleteMainLog();
	void	SetMonitorResolution();
	void	CreateUserClass();
	void	DestroyUserClass();
	void	CreateUserDirectory();
	void	InitUI();
	void	InitMenu();
	void	CloseMenu();
	void	OnClickCommand(UINT nID);
	void	OnReconnect(UINT nID);
	void	ExecuteDAQEditor();
	void	SetModeChangeSingle();
	void	SetModeChangeParallel();
	void    ViewDAQ();
	void    m_fnSelectMasterSlave();
	void    m_fnInitEqMode();

	bool	IsSystemError();
	bool    isSafetyCheck();

	LRESULT	OnReceiveSelectChannel(WPARAM wParam, LPARAM lParam);
	LRESULT	OnReceiveProgramExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveAlarm(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveAlarmUpdate(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveDAQSafety(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveDAQComplete(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveReservePause(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveReserveComplete(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveReserveStep(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveHideScheduleGui(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveMultiScheduleStart(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveDaQView(WPARAM WParam, LPARAM lParam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
	
	afx_msg void OnAcceleratorF2();
	afx_msg void OnCommReconnect();
	afx_msg void OnErrorChannelRestart();
	afx_msg void OnCancelSync();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CString m_strDioType;
};
