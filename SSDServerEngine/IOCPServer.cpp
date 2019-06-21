#include "IOCPServer.h"
#include "SSDServerCore.h"
#include "ServerManager.h"
#include <algorithm>

HANDLE										CIOCPServer::IOCPCOM = 0;
int											CIOCPServer::AcceptNumber = 0;
std::vector<std::thread>					CIOCPServer::vecThreadPool;
std::vector<HANDLE>							CIOCPServer::vecThreadHandle;
UINT										CIOCPServer::IOCPThreadCount = 0;
CSyncList<CServerUser*>						CIOCPServer::AllConnectUser;

// IOCP 서버 프로세스(서버 메인 쓰레드)
unsigned int _stdcall IOCPServerProcess(void* _Arg) 
{
	CIOCPServer* p = (CIOCPServer*)_Arg;
	
	if(nullptr != p)
		p->Update();

	return 0;
}


// IOCP 쓰레드 프로세스(워커 쓰레드)
unsigned int _stdcall CIOCPServer::IOCPThreadProcess(void* _Arg)
{
	CIOCPServer* IOCPServer = (CIOCPServer*)_Arg;

	if (nullptr == IOCPServer)
	{
		std::wstring ErrorText = L"Create IOCP Work Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	DWORD SIZE = 0;
	CServerUser* pUser = nullptr;
	OVERLAPPED* POVERLAPPED = nullptr;
	
	while ( true )
	{
		int  Result = GetQueuedCompletionStatus(IOCPCOM, &SIZE, (PULONG_PTR)&pUser, &POVERLAPPED, INFINITE);
		
		if (SIZE == 0 && pUser == nullptr &&  POVERLAPPED == nullptr)
		{
			break;
		}
		
		CServerUser::OVERBUFFER* pOverPart = DeductionPointer(CServerUser::OVERBUFFER*, POVERLAPPED, m_Over);
		//pOverPart = reinterpret_cast<CServerUser::OVERBUFFER*>(reinterpret_cast<char*>(POVERLAPPED) - reinterpret_cast<ULONG_PTR>(&reinterpret_cast<CServerUser::OVERBUFFER*>(0)->m_Over));
	
		if (nullptr == pOverPart || nullptr == pOverPart->m_User )
			continue;

		CServerUser* pUser = pOverPart->m_User;

		switch (pOverPart->m_eType)
		{
			// 클라이언트 유저가 connect 함수를 호출하는 순간 
		case CServerUser::WORK_TYPE::WORK_CONNET:
		{
			if (true == IOCPServer->ConnectUser(pOverPart) )
			{
				pUser->ConnectEvent();
				printf("Connect Client IP : %s , UserSocket : %d \n", pUser->GetRemoteIPAddressString(), (int)pUser->m_UserSock);
				pUser->NotifyCompleteConnect();
				pUser->RecvPacket();
			}
			else 
			{
				printf("Connect Fail Remote IP : %s , Remote  Port Number : %d \n", pUser->GetRemoteIPAddressString(), pUser->GetRemotePortNumber());
			}
		}
			break;
		case CServerUser::WORK_TYPE::WORK_RECV:
		{
			// 패킷 전처리 함수에서 패킷을 처리하지 않았으면 패킷큐에 패킷을 넣고
			// 패킷 처리를 서버 매니져가 수행하도록 한다.
			printf("Recv Packet Size : %ld  , Send IP : %s ,Socket Number : %d , Work Thread ID : %lu \n", SIZE, pUser->GetRemoteIPAddressString(),(int)pUser->m_UserSock,GetCurrentThreadId());
			pUser->DispatchPacket(SIZE);
			SSDServerCore::pServerManager->PushPacket(pUser);
			pUser->RecvPacket();
		}


			break;
		case CServerUser::WORK_TYPE::WORK_SEND:
		{

		}
			break;
		default:
			break;
		}
	}



	return 0;
}
const bool CIOCPServer::PrePacketProcessing(CServerUser*  _pUser, const UINT& _Size)
{
	if (nullptr == _pUser || _Size < sizeof(PacketHeader))
		return true;

	_pUser->ResetCriticalTime();

	PacketHeader* Header = (PacketHeader*)(_pUser->m_RecvBuffer.m_Packet);

	//switch (Header->HeaderFlag)
	//{
	//case FLAG_PING:
	//	
	//	return true;
	//default:
	//	return false;
	//}


	return false;
}

void CIOCPServer::LogError()
{
	DWORD errCode = WSAGetLastError();

	if (ERROR_IO_PENDING == errCode)
	{
		return;
	}

	LPVOID lpMsgBuf = nullptr;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	wchar_t Msg[256] = L"";

	swprintf_s(Msg, L"%ls", (wchar_t*)lpMsgBuf);
	printf("%ls\n", (wchar_t*)lpMsgBuf);
	
}



CIOCPServer::CIOCPServer()
{
}


CIOCPServer::~CIOCPServer()
{
	
}


void CIOCPServer::ServerStart()
{
	m_bServerLive = true;
	CServerThread::Processing(L"IOCPMainProcess", IOCPServerProcess, this);
}
void CIOCPServer::ServerEnd()
{
	for (size_t i = 0; i < vecThreadPool.size(); i++)
	{
		if (true == vecThreadPool[i].joinable())
		{
			vecThreadPool[i].join();
		}
	}

	vecThreadHandle.clear();
	vecThreadPool.clear();


	AllConnectUser.Clear();
}

void CIOCPServer::Update()
{
	// INVALID_HANDLE_VALUE용도, nullptr(감시하려는 핸들()), 0, 0
	IOCPCOM = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

	// 쓰레드 갯수 조절 : 서버 CPU 하드웨어 코어 갯수 * 2 = IOCP 쓰레드 갯수
	IOCPThreadCount = std::thread::hardware_concurrency() * 2;

	for (size_t i = 0; i < IOCPThreadCount; ++i)
	{
		vecThreadPool.push_back(std::thread(IOCPThreadProcess, this));
		vecThreadHandle.push_back(vecThreadPool[i].native_handle());
	}
	

	SOCKET AcceptSock = SSDServerCore::GetASyncServerSock(SOMAXCONN);
	// 억셉트 소켓을 입출력 완료 포트에 연결
	CreateIoCompletionPort((HANDLE)AcceptSock, IOCPCOM, (ULONG_PTR)0, 0);


	for (size_t i = 0; i < INITUSER; i++)
	{
		SSDServerCore::ServerUserASyncAccept();
	}


	while (this->m_bServerLive)
	{

	}


	for (size_t i = 0; i < vecThreadPool.size(); i++)
	{
		PostQueuedCompletionStatus(IOCPCOM, 0, 0, NULL);
	}

	WaitForMultipleObjects(IOCPThreadCount, &vecThreadHandle[0], TRUE, INFINITE);
	return;

}
void CIOCPServer::PreUpdate(const float& _DeltaTime) 
{
	ObserverConnectUser(_DeltaTime);
	
	
}
void CIOCPServer::PostUpdate(const float& _DeltaTime) 
{

}
void CIOCPServer::ObserverConnectUser(const float& _DeltaTime)
{
	if (true == AllConnectUser.Empty())
		return;

	LocalCS Lock(&AllConnectUser.m_SyncObj);

	std::list<CServerUser*>::iterator Start = AllConnectUser.m_List.begin();
	std::list<CServerUser*>::iterator End = AllConnectUser.m_List.end();
	
	CServerUser* pUser = nullptr ;

	for (;Start != End ;)
	{
		pUser = (*Start);

		if (MAXCRITICALTIME <= (*Start)->AddCriticalTime(_DeltaTime))
		{
			Start = AllConnectUser.m_List.erase(Start);
			DisConnectUser(pUser);
			printf("Time Over  Client IP : %s , Connect Users : %d \n", pUser->GetRemoteIPAddressString(), (int)AllConnectUser.Size());
			SSDServerCore::pServerManager->DisConnectUser(pUser);
		}
		else
		{
			++Start;
		}
	}

}
const bool CIOCPServer::ConnectUser(CServerUser::OVERBUFFER* _pOverPart)
{
	_pOverPart->m_WSABuf.buf = _pOverPart->m_Packet;
	_pOverPart->m_WSABuf.len = PACKET_SIZE + sizeof(PacketHeader);
	_pOverPart->m_eType = CServerUser::WORK_RECV;

	int SizeLocal;
	int SizeRemote;
	sockaddr_in* Local = nullptr;
	sockaddr_in* Reomote = nullptr;

	// 억셉트로 호출한 소켓도 알아올수 있다.
	// 해당 프로세스의 억셉트ex를 한번이라도 호출해서 돌아가고 있다면의 경우
	// 알아올수 있다.

	// AcceptEx는 IPv6 버젼 IP까지 모두 받기위해 소켓주소크기를 원래 구조체보다 16바이트 더 잡아먹는다.
	GetAcceptExSockaddrs(&_pOverPart->m_Packet[0], _pOverPart->m_PacketSize, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16
		, (sockaddr**)&Local, &SizeLocal
		, (sockaddr**)&Reomote, &SizeRemote);
	memcpy_s(&_pOverPart->m_User->m_AddLocal, sizeof(sockaddr_in), Local, sizeof(sockaddr_in));
	memcpy_s(&_pOverPart->m_User->m_AddRemote, sizeof(sockaddr_in), Reomote, sizeof(sockaddr_in));

	CServerUser* ConnUser = SSDServerCore::WaitToConnetUser(_pOverPart->m_UserSock);

	// Aceppt 된 유저의 소켓을 IOCP와 연결(지금 부터 IOCP가 유저 소켓을 감지)
	CreateIoCompletionPort((HANDLE)_pOverPart->m_UserSock, IOCPCOM, (ULONG_PTR)&ConnUser, 0);
	
	LINGER linger;
	linger.l_linger = 0;
	linger.l_onoff = 1;

	if (SOCKET_ERROR == setsockopt(_pOverPart->m_UserSock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(LINGER)))
	{
		assert(nullptr);
		return false;
	}

	AllConnectUser.push_back(_pOverPart->m_User);

	return true;
}

void CIOCPServer::DisConnectUser(CServerUser* _pUser)
{
	_pUser->NotifyDisConnect();

	_pUser->ReallocateUserSocket();

	int Return = AcceptEx(SSDServerCore::AcceptSock, _pUser->m_UserSock
		, &_pUser->m_RecvBuffer.m_Packet, 0
		, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16
		, &_pUser->m_RecvBuffer.m_PacketSize, &_pUser->m_RecvBuffer.m_Over);

	if (TRUE != Return)
	{
		LogError();
	}

	SSDServerCore::UpdateUserSocket(_pUser);
}

void CIOCPServer::PacketProcessing(PACKET& _Pack)
{
	switch (_Pack.Header.HeaderFlag)
	{
	case FLAG_PING: 
	{
		if (nullptr == _Pack.pUser)
			return;

		_Pack.pUser->ResetCriticalTime();
		printf("Ping  Client IP : %s\n", _Pack.pUser->GetRemoteIPAddressString());
	}
		break;
	case FLAG_DISCONNECT:
	{
		AllConnectUser.Erase(_Pack.pUser);
		DisConnectUser(_Pack.pUser);
		printf("Disconnect  Client IP : %s , Connect Users : %d \n", _Pack.pUser->GetRemoteIPAddressString(), (int)AllConnectUser.Size());
	}
		break;
	default:
		return;
	}
}
