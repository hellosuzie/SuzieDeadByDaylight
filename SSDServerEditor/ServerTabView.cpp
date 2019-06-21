// ServerTabView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SSDServerEditor.h"
#include "ServerTabView.h"


// CServerTabView

IMPLEMENT_DYNCREATE(CServerTabView, CView)

CServerTabView::CServerTabView()
{

}

CServerTabView::~CServerTabView()
{
}

BEGIN_MESSAGE_MAP(CServerTabView, CView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CServerTabView 그리기입니다.

void CServerTabView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CServerTabView 진단입니다.

#ifdef _DEBUG
void CServerTabView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CServerTabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CServerTabView 메시지 처리기입니다.


int CServerTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	if (FALSE == m_RootTabDlg.Create(IDD_SERVERTABDLG, this))
	{
		AfxMessageBox(_T("루트 탭 DLG 만들기 오류"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	RECT Rc = { 0,0 };
	this->GetClientRect(&Rc);
	m_RootTabDlg.SetWindowPos(this, Rc.left - 2, Rc.top, Rc.right - 4 * Rc.left, Rc.bottom + 10, SWP_NOZORDER);
	m_RootTabDlg.ShowWindow(SW_SHOW);

	return 0;
}
