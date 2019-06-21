#pragma once
#include <ThreadMaker.h>
#include <IOCP.h>


class CClientSender : public CThreadMaker
{
private:
	friend class CClientConnectMgr;
	friend class CClientSession;

	static void _stdcall SendThreadProcess(void* _pArg);

private:
	CIOCP										m_SendIOCP;
	HANDLE										m_SendIOCPHandle;
	PACKET										m_CurSendPack;

private:
	void Init();
	void Release() override;

private:
	void SendPacket(const PACKET& _Pack);
	void SendPing();
	void SendPong();

public:
	CClientSender();
	~CClientSender();
};

