#include "ClientSession.h"
#include "ClientConnectMgr.h"
#include "ClientReceiver.h"
#include "ClientLogMgr.h"
#include "ClientSender.h"

void _stdcall CClientSession::SessionThreadProcess(void* _pArg)
{
	CClientSession* Session = reinterpret_cast<CClientSession*>(_pArg);

	HANDLE SessionHandle = Session->m_SessionIOCPHandle;

	DWORD Signal = 0;
	ULONG_PTR ComKey = NULL;
	OVERLAPPED* POVERLAPPED = nullptr;


	BOOL  Result;
	std::chrono::system_clock::time_point Ping;
	std::chrono::system_clock::time_point Pong = std::chrono::system_clock::now();
	std::chrono::duration<float> Time;


	while (true) 
	{
		// 1. 핑을 서버에 전송
		GETSINGLE(CClientConnectMgr).m_Sender.SendPing();
		// 2. 핑패킷을 샌더가 다쓸때까지 대기
		Result = GetQueuedCompletionStatus(SessionHandle, &Signal, (PULONG_PTR)&ComKey, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == Signal && ENDSIGNAL == ComKey  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (Signal != PINGSIGNAL) 
		{
			Sleep(1000);
			continue;
		}

		// 3. 이때 타이머를 작동시키고 리시버가 퐁 패킷을 받을때까지 대기(5초 대기)
		Ping = std::chrono::system_clock::now();
		Result = GetQueuedCompletionStatus(SessionHandle, &Signal, (PULONG_PTR)&ComKey, &POVERLAPPED, 5000);

		if (Result == FALSE)
		{
			Sleep(1000);
			continue;
		}

		if (ENDSIGNAL == Signal && ENDSIGNAL == ComKey  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (Signal != PONGSIGNAL)
		{
			Sleep(1000);
			continue;
		}

		//4. 퐁 패킷을 받으면 타이머를 정지시키고 측정된 시간을 클라이언트 커넥터 한테 전송한다.
		Pong = std::chrono::system_clock::now();
		Time = Pong - Ping;
		GETSINGLE(CClientConnectMgr).RecordPing(Time.count());


		Sleep(1000);
	}
	
	

	
}

CClientSession::CClientSession()
{
}


CClientSession::~CClientSession()
{
}

void CClientSession::Init()
{
	m_SessionIOCPHandle = m_SessionIOCP.CreateIOCP();
	MakeThread(SessionThreadProcess, this);
}

void CClientSession::Release()
{
	m_SessionIOCP.TerminateIOCP();
	m_SessionIOCPHandle = NULL;
	CThreadMaker::Release();
}

void CClientSession::SendPingEvent() 
{
	PostQueuedCompletionStatus(m_SessionIOCPHandle, PINGSIGNAL, NULL, NULL);
}
void CClientSession::RecvPongEvent()
{
	PostQueuedCompletionStatus(m_SessionIOCPHandle, PONGSIGNAL, NULL, NULL);
}