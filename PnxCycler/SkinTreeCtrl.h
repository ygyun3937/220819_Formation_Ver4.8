#pragma once

#include "DefTreeCtrl.h"
// CSkinTreeCtrl
#include <GdiPlus.h>
using namespace Gdiplus;

/********************************************************
@brief 트리컨트롤 클래스
@details 
@author ween8
@date 2016-06-13 10:17:00
@version 
********************************************************/
class AFX_EXT_CLASS CSkinTreeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CSkinTreeCtrl)

public:
	CSkinTreeCtrl();
	virtual ~CSkinTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnPaint();
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	
	virtual BOOL		PreTranslateMessage(MSG* pMsg);

public:
	
	//이벤트처리
	//마우스클릭, 더블클릭, 우측버튼클릭시 POS 전달

	//아이템별로 user data setting
	void				SetItemData(POSITION pos, void *pData);
	void*				GetItemData(POSITION pos);
	
	//text 수정하기 및 가져오기
	void				SetItemText(POSITION pos, CString strText);
	CString				GetItemText(POSITION pos);
	stTreeItem*			GetItem(POSITION pos);
	
	//데이터 추가
	POSITION			InsertItem(POSITION posParent, stTreeItem *pTreeItem);
	
	//전체 아이템 삭제
	void				RemoveAllItems(CList<stTreeItem*, stTreeItem*> *pTreeItems);
	void				RemoveAllItems();
	
	//하나의 아이템 삭제
	void				RemoveItem(POSITION pos);
	
	//스크롤바 생성
	BOOL				CreateScrollBar(UINT nVerticalScrollID = 0, UINT nHorizontalScrollID = 0);
	
	//현재 배경색상 가져오기
	inline COLORREF		GetBKColor() { return m_colorBK; }
	inline void			SetBKColor(COLORREF colorBK) { m_colorBK = colorBK; InvalidateNoScroll();}

	//expand 이미지 설정
	void				SetExpandImagePath(CString strNormalImagePath, CString strMoreImagePath);

	void				SetBKLine(BOOL bLineDraw) { m_bBKLine = bLineDraw; InvalidateNoScroll(); }
	void				SetFontSize(int nFontSize) { m_nFontSize = nFontSize; InvalidateNoScroll(); }



	//마우스 휠 이벤트 받기위함
	void				SetMouseWheel(short zDelta);

private:
	stTreeItem*			GetItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, POSITION pos);
	stTreeItem*			PtInItem(CPoint point);
	stTreeItem*			PtInItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, CPoint point, int &nHeight);

	stTreeItem*			PtInItemExpand(CPoint point);
	stTreeItem*			PtInItemExpand(CList<stTreeItem*, stTreeItem*> *pTreeItems, CPoint point, int &nHeight);

	void				AllDeselectItems(CList<stTreeItem*, stTreeItem*> *pTreeItems);

	int					GetParentCnt(stTreeItem *pTreeItem);
	int					GetTotalHeight();
	void				GetTotalHeight(int &nTotalHeight, CList<stTreeItem*, stTreeItem*> *pTreeItems);
	void				GetLongTextWidth(CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nLongTextWidth);
	void				GetChildCount(int &nChildCnt, stTreeItem *pTreeItem);
	void				SetDrawEffect(stTreeItem *pTreeItem, BOOL bDraw, int nSleep);

	BOOL				IsParentExpand(stTreeItem *pTreeItem);

	//draw
	void				DrawItems(CDC *pDC, CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nTop);
	void				DrawExpand(CDC *pDC, CRect rect, BOOL bExpand);
	stTreeItem*			GetFirstDrawItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nTop);
	BOOL				IsUseExpandImage();

	//스크롤 관련
	void				RemoveScroll();
	void				SettingScrollBar();
	void				InvalidateNoScroll();
	void				RedrawNoScroll();
	int					GetScrollPos(CScrollBar* pScrollBar);
	int					GetScrollTrackPos(CScrollBar* pScrollBar);
	int					GetScrollbarSize(CScrollBar* pScrollBar);

private:
	int					m_nFontSize;
	BOOL				m_bBKLine;
	CString				m_strNormalImagePath;
	CString				m_strMoreImagePath;
	Color				m_colorSelectItem;
	Color				m_colorLine;
	COLORREF			m_colorFont;
	COLORREF			m_colorBK;
	CScrollBar			*m_pVerticalScroll;
	CScrollBar			*m_pHorizontalScroll;

	CList<stTreeItem*, stTreeItem*>	m_treeItems;
};


