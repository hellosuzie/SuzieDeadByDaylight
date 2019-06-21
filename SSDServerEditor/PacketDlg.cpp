// PacketDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SSDServerEditor.h"
#include "PacketDlg.h"
#include "afxdialogex.h"


// CPacketDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPacketDlg, CDialogEx)

CPacketDlg::CPacketDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PACKETDLG, pParent)
{

}

CPacketDlg::~CPacketDlg()
{
}

void CPacketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_PacketLogList);
}


BEGIN_MESSAGE_MAP(CPacketDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPacketDlg 메시지 처리기입니다.


void CPacketDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return;
	CDialogEx::OnClose();
}


void CPacketDlg::OnOK()
{
	return;
}


BOOL CPacketDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) 
		return TRUE;


	return CDialogEx::PreTranslateMessage(pMsg);
}
