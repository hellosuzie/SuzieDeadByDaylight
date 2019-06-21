#include "ClientSender.h"
#include <IOCP.h>
#include "ClientConnectMgr.h"
#include <NetworkClient.h>

void CClientSender::SendThreadProcess(void* _pArg)
{
	CClientSender* Sender = reinterpret_cast<CClientSender*>(_pArg);

	HANDLE SendIOCP = Sender->m_SendIOCPHandle;

	DWORD ByteSize = 0;
	CServerUser* pUser = nullptr;
	PACKET* TempPack;
	PACKET SendPack;
	OVERLAPPED* POVERLAPPED = nullptr;

	while (true) 
	{
		int  Result = GetQueuedCompletionStatus(SendIOCP, &ByteSize, (PULONG_PTR)&TempPack, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == ByteSize && ENDSIGNAL == TempPack  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (0 == ByteSize)
		{
			continue;
		}

		SendPack = *TempPack;
		CServerUser::OVERBUFFER* pOver = DeductionPointer(CServerUser::OVERBUFFER*, POVERLAPPED, m_Over);
		pOver->SendPacket((char*)&SendPack, ByteSize);
		if (SendPack.Header.HeaderFlag == FLAG_PING) 
		{
			GETSINGLE(CClientConnectMgr).SendPingEvent();
		}
		
	}


}

CClientSender::CClientSender()
	:m_SendIOCP(CIOCP()), m_SendIOCPHandle(NULL)
{

}

CClientSender::~CClientSender()
{
}

void CClientSender::Init()
{
	m_SendIOCPHandle = m_SendIOCP.CreateIOCP();

	
	MakeThread(SendThreadProcess,this);
	

}

void CClientSender::Release()
{
	m_SendIOCP.TerminateIOCP();
	m_SendIOCPHandle = NULL;
	CThreadMaker::Release();
}
void CClientSender::SendPacket(const PACKET& _Pack) 
{
	m_CurSendPack = _Pack;
	PostQueuedCompletionStatus(m_SendIOCPHandle, m_CurSendPack.Header.Size, (ULONG_PTR)&m_CurSendPack, &GETSINGLE(CClientConnectMgr).pLogicClient->m_SendBuffer.m_Over);
}
void CClientSender::SendPing()
{
	PACKET PingPack;
	PingPack.Header.Size = sizeof(PacketHeader);
	PingPack.Header.HeaderFlag = FLAG_PING;
	PingPack.Header.DataType = 0;
	SendPacket(PingPack);
}
void CClientSender::SendPong()
{
	PACKET PingPack;
	PingPack.Header.Size = sizeof(PacketHeader);
	PingPack.Header.HeaderFlag = FLAG_PONG;
	PingPack.Header.DataType = 0;
	SendPacket(PingPack);
}
