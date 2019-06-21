#pragma once
#include <ThreadMaker.h>
#include <IOCP.h>

#define PINGSIGNAL 1
#define PONGSIGNAL 2

class CClientSession : public CThreadMaker
{
private:
	friend class CClientConnectMgr;

	static void _stdcall SessionThreadProcess(void* _pArg);

private:
	CIOCP										m_SessionIOCP;
	HANDLE										m_SessionIOCPHandle;
	


private:
	void Init();
	void Release() override;

private:
	void SendPingEvent();
	void RecvPongEvent();
public:
	CClientSession();
	~CClientSession();
};

