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
	//column ����
	void				SetColumnGradation(BOOL bGradation);
	void				SetColumnHeight(UINT nHeight);
	void				SetMinColumnWidth(int nMinColumnWidth);
	void				InsertColumn(sColumnInfo info);
	void				InsertColumn(CString strText, int nWidth, eTextFormat eFormat);
	
	//list ����
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
	//column ����
	void				DrawColumn(CDC* pDC);
	void				RemoveAllColumn();
	UINT				GetTextFormat(eTextFormat eFormat);
	void				GetColumnRect(int nColumnIndex, CRect &rect);
	void				CheckSelectColumn(CPoint point);
	int					GetAllColumnWidth();


	//list ����
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

private:
	ULONG_PTR			m_gpToken;

	//column ����
	COLORREF			m_colorColumn;			//�÷� ����
	COLORREF			m_colorColumnSelect;	//�÷��� ���õɶ��� ����
	int					m_nSelectColumn;		//���� ���õ� �÷��� �ε���

	BOOL				m_bColumnGradation;		//�÷� �׶��̼�ó�� �Ұ���������
	UINT				m_nColumnHeight;		//�÷� ����
	UINT				m_nColumnFontSize;		//�÷� ��Ʈ������
	int					m_nColumnTrackingIndex;	//�÷� �����϶� �ε���
	int					m_nMinColumnWidth;		//�÷��� �ּ� ������
	BOOL				m_bColumnTracking;		//���� �÷��� �����̴� ������
	CPoint				m_ptClick;				//���콺 Ŭ���� ����Ʈ
	CPtrArray			m_arrColumnInfo;		//�÷� ����


	//list ����
	COLORREF			m_colorBKLine;			//����Ʈ ������ ����
	COLORREF			m_colorGroupLine;		//�׷���� ����
	COLORREF			m_colorItemLine;		//�������� ���� ����
	COLORREF			m_colorSelectGroup;		//�׷��� ���õȰ�� ����
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

	int					m_nUNQID;

	CPen	m_colorGroupLinePen;
	CPen	m_colorItemLinePen;
	CPen	m_ColumnPen;
	CPen	m_colorBKLinePen;
	CFont   m_Font;
};


