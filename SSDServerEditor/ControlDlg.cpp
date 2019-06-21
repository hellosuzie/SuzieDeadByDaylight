// ControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SSDServerEditor.h"
#include "ControlDlg.h"
#include "afxdialogex.h"


// CControlDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CControlDlg, CDialogEx)

CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONTROLDLG, pParent)
{

}

CControlDlg::~CControlDlg()
{
}

void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialogEx)
END_MESSAGE_MAP()


// CControlDlg 메시지 처리기입니다.


BOOL CControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
