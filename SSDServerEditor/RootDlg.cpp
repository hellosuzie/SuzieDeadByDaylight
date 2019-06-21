// RootDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SSDServerEditor.h"
#include "RootDlg.h"
#include "afxdialogex.h"
#include "PacketDlg.h"
#include <DXMacro.h>


// CRootDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRootDlg, CDialogEx)

CRootDlg::CRootDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVERTABDLG, pParent)
{

}

CRootDlg::~CRootDlg()
{
}

void CRootDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROOTTAB, m_RootTabCtrl);
}


BEGIN_MESSAGE_MAP(CRootDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRootDlg 메시지 처리기입니다.


BOOL CRootDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CreateTab<CPacketDlg>(IDD_PACKETDLG, L"패킷 로그");

	ShowDlg(PACKETLOG);
	SetBackgroundColor(RGB(0, 0, 0));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CRootDlg::OnDestroy()
{
	for (size_t i = 0; i < m_vecDlg.size(); i++)
	{
		m_vecDlg[i]->SendMessage(WM_DESTROY);
		SAFE_DELETE(m_vecDlg[i]);
	}
	m_vecDlg.clear();


	CDialogEx::OnDestroy();

}


BOOL CRootDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;


	return CDialogEx::PreTranslateMessage(pMsg);
}
