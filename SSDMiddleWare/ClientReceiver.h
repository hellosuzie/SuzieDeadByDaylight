#pragma once
#include <ThreadMaker.h>
#include <IOCP.h>

class CClientReceiver : public CThreadMaker
{
private:
	friend class CClientConnectMgr;
	friend class CClientSession;

	static void _stdcall RecvThreadProcess(void* _pArg);

private:
	CIOCP										m_RecvIOCP;
	HANDLE										m_RecvIOCPHandle;

private:
	void Init();
	void Release() override;

public:
	CClientReceiver();
	~CClientReceiver();
};

