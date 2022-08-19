#pragma once

#include "GroupListDefine.h"
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define WM_GROUP_SELECT				( WM_USER + 100 )
#define WM_GROUP_RCLICK				( WM_USER + 101 )

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
	virtual BOOL 		PreTranslateMessage(MSG* pMsg);
	
public:
	//column 관련
	void				SetColumnGradation(BOOL bGradation);
	void				SetColumnHeight(UINT nHeight);
	void				SetMinColumnWidth(int nMinColumnWidth);
	void				InsertColumn(sColumnInfo info);
	void				InsertColumn(CString strText, int nWidth, eTextFormat eFormat);
	
	//list 관련
	int					GetGroupInfoSize();
	int					GetItemCount(int nGroupIndex);
	CString				GetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex);

	void				SetBkLineColor(COLORREF colorBKLine);
	
	int					InsertGroup(sGroupInfo *pInfo);
	int					InsertGroup(CString strText);

	int					InsertItem(int nGroupIndex, sItemInfo itemInfo);
	int					InsertItem(int nGroupIndex, CString strText);

	//void				RemoveItem(int nGroupIndex, int nItemIndex);
	void				RemoveAllItems();

	void				SetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex, CString strText);
	void				SetSubItemColor(int nGroupIndex, int nItemIndex, int nSubItemIndex, COLORREF subItemColor, BOOL bUseColor);
	void				SetItemColor(int nGroupIndex, int nItemIndex, COLORREF itemColor);
	void				SetItemSelected(int nGroupIndex, int nItemIndex);

	BOOL				CreateScrollBar(UINT nVerticalScrollID = 0, UINT nHorizontalScrollID = 0);
	BOOL				CreateEditCtrl(UINT nEditID);

	int					GetAllListHeight();
	void				SetMouseWheel(short zDelta);
	void				SetVScrollPos(int nGroupIndex, int nItemIndex);
	void				SetGroupHeight(int nGroupHeight);

	void				SetFontName( CString strFontName );
	void				SetFontSizeEx( int nFontSize );

	void				SetItemHeight(int nItemHeight);

	void				SetMultiLine( BOOL bMultiLine );

	void				SetUNQID(const int nID);
	int					GetUNQID();

private:
	//column 관련
	void				DrawColumn(CDC* pDC);
	void				RemoveAllColumn();
	UINT				GetTextFormat(eTextFormat eFormat);
	void				GetColumnRect(int nColumnIndex, CRect &rect);
	void				CheckSelectColumn(CPoint point);
	int					GetAllColumnWidth();


	//list 관련
	void				DrawBkLine(CDC *pDC);
	void				DrawGradation(CDC * pDC, COLORREF rgbBegin, COLORREF rgbEnd, CRect rect, int nPos);
	void				DrawItems(CDC *pDC);
	void				DrawItems(int nGroupIndex, CDC *pDC, sGroupInfo	*pGroupInfo, CRect rcGroup);
	void				DrawExpand(CDC *pDC, CRect rect, BOOL bExpand);

	BOOL				IsPtInResizeRect(CPoint point, int *pTrackingIndex = NULL);

	void				SetItemLine(BOOL bItemLine);
	void				SetSubItemLine(BOOL bSubItemLine);
		
	
	void				GetGroupRect(int nGroupIndex, CRect &rect);
	void				GetItemRect(int nGroupIndex, int nItemIndex, CRect &rect);

	void				GetSubItemRect(int nGroupIndex, int nItemIndex, int nSubItemRect, CRect &rect);
	
	
	//스크롤관련
	void				SettingScrollBar();
	void				RemoveScroll();
	void				InvalidateNoScroll();
	void				RedrawNoScroll();
	int					GetScrollPos(CScrollBar* pScrollBar);
	int					GetScrollTrackPos(CScrollBar* pScrollBar);

	//edit 관련
	void				InitEditCtrl();
	void				RemoveEditCtrl();

private:
	ULONG_PTR			m_gpToken;

	//column 관련
	COLORREF			m_colorColumn;			//컬럼 색상
	COLORREF			m_colorColumnSelect;	//컬럼이 선택될때의 색상
	int					m_nSelectColumn;		//현재 선택된 컬럼의 인덱스

	BOOL				m_bColumnGradation;		//컬럼 그라데이션처리 할것인지여부
	UINT				m_nColumnHeight;		//컬럼 높이
	UINT				m_nColumnFontSize;		//컬럼 폰트사이즈
	int					m_nColumnTrackingIndex;	//컬러 움직일때 인덱스
	int					m_nMinColumnWidth;		//컬럼의 최소 사이즈
	BOOL				m_bColumnTracking;		//현재 컬럼을 움직이는 중인지
	CPoint				m_ptClick;				//마우스 클릭된 포인트
	CPtrArray			m_arrColumnInfo;		//컬럼 정보


	//list 관련
	COLORREF			m_colorBKLine;			//리스트 배경라인 색상
	COLORREF			m_colorGroupLine;		//그룹라인 색상
	COLORREF			m_colorItemLine;		//아이템의 라인 색상
	COLORREF			m_colorSelectGroup;		//그룹이 선택된경우 색상
	//COLORREF			m_colorSelectItem;		
	Color				m_colorSelectItem;		//아이템 선택되었을경우 색상 gdi+

	BOOL				m_bItemLine;			//아이템이 라인을 그릴지 여부
	BOOL				m_bSubItemLine;			//서브아이템의 라인을 그릴지 여부
	int					m_nExpandGroupIndex;	

	CPtrArray			m_arrGroupInfo;
	eTextFormat			m_eItemFormat;
	int					m_nGroupHeight;
	int					m_nGroupFontSize;
	int					m_nItemHeight;
	
	CScrollBar			*m_pVerticalScroll;
	CScrollBar			*m_pHorizontalScroll;
	
	//edit관련
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

	int					m_nUNQID;

	CPen	m_colorGroupLinePen;
	CPen	m_colorItemLinePen;
	CPen	m_ColumnPen;
	CPen	m_colorBKLinePen;
	CFont   m_Font;
};


