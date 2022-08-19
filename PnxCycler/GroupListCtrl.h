#pragma once

#include "GroupListDefine.h"
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define WM_GROUP_SELECT				( WM_USER + 100 )
#define WM_GROUP_RCLICK				( WM_USER + 101 )
#define WM_GROUP_COLUMN_OVER		( WM_USER + 102 )
#define WM_GROUP_COLUMN_LEAVE		( WM_USER + 103 )
#define WM_GROUP_COLUMN_LCLICK		( WM_USER + 104 )
#define WM_GROUP_DBCLICK			( WM_USER + 105 )
#define WM_GROUP_CLEAR				( WM_USER + 106 )
#define WM_GROUP_EDIT_CHANGE		( WM_USER + 107)


class CGroupListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGroupListCtrl)

public:
	CGroupListCtrl();
	virtual ~CGroupListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL 		OnEraseBkgnd(CDC* pDC);
	afx_msg void 		OnPaint();
	afx_msg void 		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void 		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int  		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void 		OnSize(UINT nType, int cx, int cy);
	afx_msg void 		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void 		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void		OnMouseLeave();
	virtual BOOL 		PreTranslateMessage(MSG* pMsg);	
public:
	//column ����
	void				SetColumnGradation(BOOL bGradation);
	int					GetColumnHeight();
	void				SetColumnHeight(UINT nHeight);
	void				SetMinColumnWidth(int nMinColumnWidth);
	void				InsertColumn(sColumnInfo info);
	void				InsertColumn(CString strText, int nWidth, eTextFormat eFormat);
	void				EditColumn(CString strText, int nWidth, eTextFormat eFormat, int dIndex);
	void				SetColumnGradationColor(COLORREF colorColumnGradaition);
	void				SetColumnFontColor(COLORREF color){ m_colorGroupFont = color; }
	//list ����
	int					GetGroupInfoSize();
	int					GetItemCount(int nGroupIndex);
	CString				GetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex);

	void				SetBkLineColor(COLORREF colorBKLine);
	void				SetSelectColor(Color color){ m_colorSelectItem = color; }
	void				SetItemFontColor(COLORREF color){ m_colorItemFont = color; }
	void				SetRowGradationColor(COLORREF color){ m_colorItemRow = color; }

	int					InsertGroup(sGroupInfo *pInfo);
	int					InsertGroup(CString strText);

	int					InsertItem(int nGroupIndex, sItemInfo itemInfo);
	int					InsertItem(int nGroupIndex, CString strText);

	//void				RemoveItem(int nGroupIndex, int nItemIndex);
	void				RemoveAllItems();
	void				RemoveAllItems_WithScrollReset();

	void				SetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex, CString strText);
	void				SetSubItemColor(int nGroupIndex, int nItemIndex, int nSubItemIndex, COLORREF subItemColor, BOOL bUseColor, int nBattery = 0);
	void				SetItemColor(int nGroupIndex, int nItemIndex, COLORREF itemColor);
	void				SetItemSelected(int nGroupIndex, int nItemIndex, BOOL bFlag);
	void				SetRowGradation(BOOL bGradation){ m_bRowGradation = bGradation;	}

	BOOL				CreateScrollBar(UINT nVerticalScrollID = 0, UINT nHorizontalScrollID = 0);
	BOOL				CreateEditCtrl(UINT nEditID);

	int					GetAllListHeight();
	void				SetMouseWheel(short zDelta);
	void				SetVScrollPos(int nGroupIndex, int nItemIndex);
	void				SetGroupHeight(int nGroupHeight);

	void				SetFontName( CString strFontName );
	void				SetFontSizeEx( int nFontSize );
	void				SetFontSizeTitle(int nFontSize);

	void				SetItemHeight(int nItemHeight);

	void				SetMultiLine(BOOL bMultiLine);

	void				SetUNQID(const int nID);
	int					GetUNQID();

	void				SetUseClickEvent(BOOL bFlag);
	void				SetBatteryStatusIndex(int nIndex);
	void				SetItemFlag(int nGroupIndex, int nItemIndex, bool bReserveCommand, bool bMultiSchedule);

	void				SetEditEnableColumn(int nColumn) { m_nEnableEditColumn = nColumn; }
	void				SetExplorerColumn(bool bExplorer) { m_bExplorerColumn = bExplorer; }

	// ��޸��������� ��� �۾��� ���� By JSK
	void	SetUserInterlock(bool bInterlock) { m_bUseInterlock = bInterlock; }
	void	SetInterlockCyclerChannel(bool bInterlock) { m_bIternlockCyclerChannel = bInterlock; }
	void	SetInterlockReservePause(bool bInterlock) { m_bInterlockReservePause = bInterlock; }
	void	SetInterlockReserveComplete(bool bInterlock) { m_bInterlockReserveComplete = bInterlock; }
	void	SetInterlockReserveJump(bool bInterlock) { m_bInterlockReserveJump = bInterlock; }

private:
	//column ����
	void				DrawColumn(CDC* pDC);
	void				RemoveAllColumn();
	UINT				GetTextFormat(eTextFormat eFormat);
	void				GetColumnRect(int nColumnIndex, CRect &rect);
	void				CheckSelectColumn(CPoint point);
	int					GetAllColumnWidth();


	//list ����
	void				DrawGradation(CDC * pDC, COLORREF rgbBegin, COLORREF rgbEnd, CRect rect, int nPos);
	void				DrawBkLine(CDC *pDC);
	void				DrawItems(CDC *pDC);
	void				DrawItems(int nGroupIndex, CDC *pDC, sGroupInfo* lpGroupInfo, CRect rcGroup, bool bDrawText = true, bool bDrawBattery = false);
	void				DrawExpand(CDC *pDC, CRect rect, BOOL bExpand);

	BOOL				IsPtInResizeRect(CPoint point, int *pTrackingIndex = NULL);

	void				SetItemLine(BOOL bItemLine);
	void				SetSubItemLine(BOOL bSubItemLine);		
	
	void				GetGroupRect(int nGroupIndex, CRect &rect);
	void				GetItemRect(int nGroupIndex, int nItemIndex, CRect &rect);

	void				GetSubItemRect(int nGroupIndex, int nItemIndex, int nSubItemRect, CRect &rect);
	
	
	//��ũ�Ѱ���
	void				SettingScrollBar();
	void				RemoveScroll();
	void				InvalidateNoScroll();
	void				RedrawNoScroll();
	int					GetScrollPos(CScrollBar* pScrollBar);
	int					GetScrollTrackPos(CScrollBar* pScrollBar);

	//edit ����
	void				InitEditCtrl();
	void				RemoveEditCtrl();

	//
	int		GetSelectedItemCount();
	

private:
	ULONG_PTR			m_gpToken;

	//column ����
	COLORREF			m_colorColumn;			//�÷� ����
	COLORREF			m_colorColumnSelect;	//�÷��� ���õɶ��� ����
	int					m_nSelectColumn;		//���� ���õ� �÷��� �ε���
	int					m_nOverColumn;			// ���� ���콺 ������ �÷� �ε���
	int					m_nLButtonColomn;
	int					m_nEnableEditColumn;

	BOOL				m_bColumnGradation;		//�÷� �׶��̼�ó�� �Ұ���������
	BOOL				m_bRowGradation;			//���õ� ���� ��ü Gradation
	UINT				m_nColumnHeight;		//�÷� ����
	UINT				m_nColumnFontSize;		//�÷� ��Ʈ������
	int					m_nColumnTrackingIndex;	//�÷� �����϶� �ε���
	int					m_nMinColumnWidth;		//�÷��� �ּ� ������
	BOOL				m_bColumnTracking;		//���� �÷��� �����̴� ������
	CPoint				m_ptClick;				//���콺 Ŭ���� ����Ʈ
	CPtrArray			m_arrColumnInfo;		//�÷� ����
	bool				m_bExplorerColumn;		// ����Ʈ �̺�Ʈ���� Column �������� ���� ��ų������?


	//list ����
	COLORREF			m_colorBKLine;			//����Ʈ ������ ����
	COLORREF			m_colorGroupLine;		//�׷���� ����
	COLORREF			m_colorItemLine;		//�������� ���� ����
	COLORREF			m_colorSelectGroup;		//�׷��� ���õȰ�� ����
	COLORREF			m_colorGradation;			//�׷��� Gradation ����
	COLORREF			m_colorGroupFont;				// �׷��� ��Ʈ ����
	COLORREF			m_colorItemFont;				// �������� ��Ʈ ����
	COLORREF			m_colorItemRow;				// �������� �� ����

	//COLORREF			m_colorSelectItem;		
	Color				m_colorSelectItem;		//������ ���õǾ������ ���� gdi+

	BOOL				m_bItemLine;			//�������� ������ �׸��� ����
	BOOL				m_bSubItemLine;			//����������� ������ �׸��� ����
	int					m_nExpandGroupIndex;	

	CPtrArray			m_arrGroupInfo;
	eTextFormat			m_eItemFormat;
	int					m_nGroupHeight;
	int					m_nGroupFontSize;
	int					m_nItemHeight;
	
	CScrollBar			*m_pVerticalScroll;
	CScrollBar			*m_pHorizontalScroll;
	
	//edit����
	int					m_nCurEditGroupIndex;
	int					m_nCurEditItemIndex;
	int					m_nCurEditSubItemIndex;
	CEdit				*m_pEditCtrl;

	// ComboBox
	CComboBox*			m_pComboBox;

	// JSK
	int					m_nScrollBarId;
	
	CString				m_strFontName;

	BOOL				m_bMultiLine;

	BOOL				m_bClickCtrl;
	BOOL				m_bClickShift;

	int					m_nStartIndex;
	int					m_nEndIndex;

	bool				m_bClickLButton;
	bool				m_bActiveLButton;

	BOOL				m_bUseClickEvent;
	
	// SJH
	int					m_nUNQID;		//�ش� ��ü ���� ID, ������ �׷츮��Ʈ ���� �ش� ������ � ��Ʈ������ ������..(ä�� ���ý� �ʿ�)

	CPen	m_colorGroupLinePen;
	CPen	m_colorItemLinePen;
	CPen	m_ColumnPen;
	CPen	m_colorBKLinePen;
	CFont   m_Font;

	int		m_nBatteryStatusIndex;
	int		m_nTest;

	bool	m_bUseInterlock;

	bool	m_bIternlockCyclerChannel;
	bool	m_bInterlockReservePause;
	bool	m_bInterlockReserveComplete;
	bool	m_bInterlockReserveJump;
};