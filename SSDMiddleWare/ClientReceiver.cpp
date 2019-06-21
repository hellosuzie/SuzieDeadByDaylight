#include "ClientReceiver.h"
#include <NetworkClient.h>
#include "MiddleMacro.h"
#include "ClientConnectMgr.h"
#include <chrono>


void  CClientReceiver::RecvThreadProcess(void* _pArg) 
{
	CClientReceiver* pReceiver = reinterpret_cast<CClientReceiver*>(_pArg);

	HANDLE RecvIOCP = pReceiver->m_RecvIOCPHandle;
	DWORD ByteSize = 0;
	CNetworkClient* pNetClient;
	OVERLAPPED* POVERLAPPED = nullptr;

	/*
	std::chrono::system_clock::time_point Cur;
	std::chrono::system_clock::time_point Prev = std::chrono::system_clock::now();
	std::chrono::duration<float> Time;
	*/

	while (true)
	{
		int  Result = GetQueuedCompletionStatus(RecvIOCP, &ByteSize, (PULONG_PTR)&pNetClient, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == ByteSize && ENDSIGNAL == pNetClient  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (0 == ByteSize)
		{
			continue;
		}

		if (true == pNetClient->DispatchPacket(ByteSize))
		{
			GETSINGLE(CClientConnectMgr).RecvPongEvent();
		}
	
		GETSINGLE(CClientConnectMgr).ClientPacketQueue.PushUserPacket(pNetClient);
		pNetClient->RecvPacket();

		//GETSINGLE(CClientConnectMgr).m_LogMgr.PrintLog(CClientLogMgr::MakeLogString("Recv Packet Size : ", ByteSize, " Delay Time : ", Time.count()));
	}


}
CClientReceiver::CClientReceiver()
	:m_RecvIOCP(CIOCP()), m_RecvIOCPHandle(NULL)
{
}



CClientReceiver::~CClientReceiver()
{
}


void CClientReceiver::Init() 
{
	m_RecvIOCPHandle = m_RecvIOCP.CreateIOCP();
	m_RecvIOCP.BindingSocket((HANDLE)GETSINGLE(CClientConnectMgr).pLogicClient->m_UserSock
		, (ULONG_PTR)GETSINGLE(CClientConnectMgr).pLogicClient);

	MakeThread(RecvThreadProcess, this);

}
void CClientReceiver::Release()
{
	m_RecvIOCP.TerminateIOCP();
	m_RecvIOCPHandle = NULL;
	CThreadMaker::Release();
}