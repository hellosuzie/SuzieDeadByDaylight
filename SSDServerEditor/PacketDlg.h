#pragma once
#include "afxwin.h"


// CPacketDlg 대화 상자입니다.

class CPacketDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPacketDlg)

public:
	CPacketDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPacketDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PACKETDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_PacketLogList;
	afx_msg void OnClose();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
