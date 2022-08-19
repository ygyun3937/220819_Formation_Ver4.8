#ifndef _COLORSTATICST_H
#define _COLORSTATICST_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ColorStaticST.h : header file
//

class CColorStaticST : public CStatic
{
// Construction
public:
	CColorStaticST();
    enum {	ST_FLS_SLOW,
			ST_FLS_NORMAL,
			ST_FLS_FAST};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStaticST)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CFont* m_pFont;
	virtual ~CColorStaticST();

	void		SetFontSize(int nHeight, int nWidth, BOOL bold = FALSE);
	void		SetTextColor(COLORREF crTextColor = 0xffffffff);
	void		SetBlinkTextColors(COLORREF crBlinkTextColor1, COLORREF crBlinkTextColor2);
	void		StartTextBlink(BOOL bStart = TRUE, UINT nElapse = ST_FLS_NORMAL);
	void		SetBlinkBkColors(COLORREF crBlinkBkColor1, COLORREF crBlinkBkColor2);
	void		StartBkBlink(BOOL bStart = TRUE, UINT nElapse = ST_FLS_NORMAL);
	void		EnableNotify(CWnd* pParent = NULL, UINT nMsg = WM_USER);
	void		SetBkColor(COLORREF crBkColor = 0xffffffff);

	COLORREF	GetTextColor();
	COLORREF	GetBkColor();

	static const short GetVersionI();
	static const char* GetVersionC();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStaticST)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	UINT			m_nTimerId;
	COLORREF		m_crTextColor;
	COLORREF		m_crBlinkTextColors[2];
	COLORREF		m_crBkColor;
	COLORREF		m_crBlinkBkColors[2];
	BOOL			m_bTextBlink;
	int				m_nTextBlinkStep;
	BOOL			m_bBkBlink;
	int				m_nBkBlinkStep;
	CBrush			m_brBkgnd;
	CBrush			m_brBlinkBkgnd[2];
	CWnd*			m_pParent;
	UINT			m_nMsg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
