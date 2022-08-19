#pragma once


// PnxFormationDlg 대화 상자

class CPnxFormationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPnxFormationDlg)

public:
	CPnxFormationDlg(CWnd* pParent = nullptr);			// 표준 생성자입니다.
	virtual ~CPnxFormationDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PNXFORMATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

public:
	// Generated message map functions
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

	afx_msg void OnCommReconnect();
	afx_msg void OnErrorChannelRestart();
	afx_msg void OnCancelSync();

public:
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

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
		CommandTypeSelectChannelGroup0,
		CommandTypeSelectChannelGroup1,
		CommandTypeSelectChannelGroup2,
		CommandTypeSelectChannelGroup3,
		CommandTypeSelectChannelGroup4,
		CommandTypeSelectChannelGroup5,
		CommandTypeSelectChannelGroup6,
		CommandTypeSelectChannelGroup7,
		CommandTypeSelectChannelGroup8,
		CommandTypeSelectChannelGroup9,
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

private:
	LRESULT	OnReceiveProgramExit(WPARAM wParam, LPARAM lParam);
	LRESULT	OnReceiveSelectChannel(WPARAM wParam, LPARAM lParam);

	LRESULT OnReceiveAlarm(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveAlarmUpdate(WPARAM wParam, LPARAM lParam);

	LRESULT OnReceiveDAQSafety(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveDAQComplete(WPARAM wParam, LPARAM lParam);

	LRESULT OnReceiveReservePause(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveReserveComplete(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveReserveStep(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveHideScheduleGui(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveMultiScheduleStart(WPARAM wParam, LPARAM lParam);
	LRESULT OnReceiveDaqView(WPARAM WParam, LPARAM lParam);

private:
	void	OpenDeleteMainLog();
	void	SetMonitorResolution();		// 모니터 해상도
	void	CreateUserClass();
	void	DestroyUserClass();
	void	CreateUserDirectory();
	void	InitUI();
	void	InitMenu();
	void	CloseMenu();

	void	OnClickCommand(UINT nID);

	void	StartSchedule();
	void	PauseOption1();		// 즉시 멈춤
	void	PauseOption2();		// Step 완료 후 멈춤
	void	PauseOption3();		// Cycle 완료 후 멈춤
	void	PauseOption4();
	void	RestartSchedule();
	void	NextOption1();
	void	NextOption2();
	void	CompleteOption1();	// 즉시 완료
	void	CompleteOption2();	// Step 완료 후 완료
	void	CompleteOption3();	// Cycle 완료 후 완료
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

	void	OnReconnect(UINT nID);
	void	ExecuteDAQEditor();

	void	SetModeChangeSingle();
	void	SetModeChangeParallel();
	void    ViewDAQ();

	void	SetSelectChannelGroup(CString strGroupName);
	void	SetSelectChannelGroup1();
	void	SetSelectChannelGroup2();

	void    fnSelectMasterSlave();
	void    fnInitEqMode();

	bool	IsSystemError();
	bool    IsSafetyCheck();

private:
	int		m_nDialogWidth;

	CMenu	m_Menu;
	CString m_strDioType;

	CBitmap	m_bitmapFile[BitmapFileMaxCount];
	CBitmap	m_bitmapEdit[BitmapEditMaxCount];
	CBitmap	m_bitmapPause[BitmapPauseMaxCount];
	CBitmap	m_bitmapComp[BitmapCompMaxCount];
	CBitmap	m_bitmapControl[BitmapControlMaxCount];
	CBitmap	m_bitmapSetting[BitmapSetMaxCount];
	CBitmap	m_bitmapView[BitmapViewMaxCount];
	CBitmap	m_bitmapNext[BitmapNextMaxCount];

	CString	m_strRunMode;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
