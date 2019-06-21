#pragma once
#include "afxext.h"

class CFixSpWnd : public CSplitterWnd
{
private:
	bool			m_bLock;

public:
	void Lock() { m_bLock = true; }
	void UnLock() { m_bLock = false; }



public:
	CFixSpWnd();
	~CFixSpWnd();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

