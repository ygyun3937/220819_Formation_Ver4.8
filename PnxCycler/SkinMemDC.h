#pragma once

#include "afxwin.h"


class CSkinMemDC : public CDC
{
private:
	CBitmap m_bitmap; 
	CBitmap* m_oldBitmap; 
	CDC* m_pDC; 
	CRect m_rect; 
	BOOL m_bMemDC; 

public:
	
	CSkinMemDC(CDC* pDC, CRect rect = CRect(0, 0, 0, 0), BOOL bCopyFirst = FALSE) : CDC(), m_oldBitmap(NULL), m_pDC(pDC)
	{
		ASSERT(m_pDC != NULL); // If you asserted here, you passed in a NULL CDC.

		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC){
			// Create a Memory DC
			CreateCompatibleDC(pDC);
			if (rect == CRect(0, 0, 0, 0))
				pDC->GetClipBox(&m_rect);
			else
				m_rect = rect;

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
			if (bCopyFirst)
			{
				this->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					m_pDC, m_rect.left, m_rect.top, SRCCOPY);
			}
		}
		else {

			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	~CSkinMemDC()
	{
		if (m_bMemDC) {

			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);

			SelectObject(m_oldBitmap);
		}
		else {

			m_hDC = m_hAttribDC = NULL;
		}
	}

	CSkinMemDC* operator->() { return this; }

	operator CSkinMemDC*() { return this; }
};