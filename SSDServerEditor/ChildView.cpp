
// ChildView.cpp : CChildView 클래스의 구현
//
#include "stdafx.h"
#include "SSDServerEditor.h"
#include "ChildView.h"
#include "GlobalServerViewer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_LOGIC_CONNECT, &CChildView::OnLogicConnect)
	ON_COMMAND(ID_LOGIC_DISCONNECT, &CChildView::OnLogicDisconnect)
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}



void CChildView::OnLogicConnect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (true == GlobalServerViewer::ConnectToLogicServer())
	{
		AfxMessageBox(_T("로직 서버 연결 성공"), MB_OK | MB_ICONEXCLAMATION);
	}
	else 
	{
		AfxMessageBox(_T("로직 서버 연결 실패"), MB_OK | MB_ICONEXCLAMATION);
	}

}


void CChildView::OnLogicDisconnect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (nullptr == GlobalServerViewer::pLogicClient)
	{
		AfxMessageBox(_T("로직 서버와 연결되어있지 않습니다."), MB_OK | MB_ICONEXCLAMATION);
	}

	if ( true == GlobalServerViewer::DisConnectToLogicServer())
	{
		AfxMessageBox(_T("로직 서버 연결 해제 성공"), MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		AfxMessageBox(_T("로직 서버 연결 해제 실패"), MB_OK | MB_ICONEXCLAMATION);
	}
	
}
