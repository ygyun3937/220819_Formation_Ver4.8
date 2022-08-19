#pragma once


// CTabCtrlEx

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)
public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();

	void	SetBkColor(int nCurSel, BOOL bChange);
	void	SetTabCtrlEnd();
private:
	CList< BOOL, BOOL > m_listChange;

	CFont m_Font;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


