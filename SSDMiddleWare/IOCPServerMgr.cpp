#include "IOCPServerMgr.h"
#include <IOCP.h>
#include "Room.h"


unsigned int _stdcall IOCPServerManagerProcess(void* _Arg)
{
	IOCPServerMgr* p = (IOCPServerMgr*)_Arg;

	if (nullptr != p)
		p->OperationIOCPManager();

	return 0;
}




/////////////////////////////////////////////////////////			IOCP Manager		////////////////////////////////


NetworkManager*				IOCPServerMgr::pNetworkMgr = nullptr;
CommandManager*				IOCPServerMgr::pCommandMgr = nullptr;
LogManager*					IOCPServerMgr::pLogMgr = nullptr;
unsigned int				IOCPServerMgr::InitCompeteManagerCount = 0;

unsigned int				IOCPServerMgr::RoomCount = 0;
int							IOCPServerMgr::AcceptNumber  = 0;
CSyncList<CServerUser*>		IOCPServerMgr::AllConnectUser;



const bool IOCPServerMgr::PrePacketProcessing(CServerUser*  _pUser, const UINT& _Size)
{
	if (nullptr == _pUser || _Size < sizeof(PacketHeader))
		return true;

	_pUser->ResetCriticalTime();

	return false;
}
void IOCPServerMgr::PacketProcessing(PACKET& _Pack)
{
	switch (_Pack.Header.HeaderFlag)
	{
	case FLAG_PING:
	{
		if (nullptr == _Pack.pUser)
			return;

		_Pack.pUser->ResetCriticalTime();
		PrintLog(LogManager::MakeLogString("Ping  Client IP : ", _Pack.pUser->GetRemoteIPAddressString()));
	}
	break;
	case FLAG_DISCONNECT:
	{
		AllConnectUser.Erase(_Pack.pUser);
		_Pack.pUser->ReallocateUserSocket();
		//DisConnectUser(_Pack.pUser);

	}
	break;
	default:
		return;
	}
}


IOCPServerMgr::IOCPServerMgr()
{
}


IOCPServerMgr::~IOCPServerMgr()
{
}


void IOCPServerMgr::LogError()
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




void IOCPServerMgr::OperationIOCPManager()
{
	pNetworkMgr = new NetworkManager(this);
	pCommandMgr = new CommandManager(this);
	pLogMgr = new LogManager(this);

	//RoomCount = std::thread::hardware_concurrency() * 2;
	RoomCount = MAXROOMCOUNT;

	pLogMgr->Init();
	pNetworkMgr->Init(std::thread::hardware_concurrency() * 2);
	pCommandMgr->Init(RoomCount);

	// 억셉트 소켓을 네트워크 입출력 완료 포트에 연결
	SOCKET AcceptSock = SSDServerCore::GetASyncServerSock(SOMAXCONN);
	pNetworkMgr->m_NetworkIOCP->BindingSocket((HANDLE)AcceptSock, (ULONG_PTR)0, 0);

	for (size_t i = 0; i < INITUSER; i++)
	{
		SSDServerCore::ServerUserASyncAccept();
	}


	while (this->m_bServerLive)
	{
		Sleep(1);
	}

	pNetworkMgr->Release();
	pCommandMgr->Release();
	pLogMgr->Release();

	
	return;
}



void IOCPServerMgr::ServerStart()
{
	m_bServerLive = true;
	CServerThread::Processing(L"IOCPServerManagerProcess", IOCPServerManagerProcess, this);
}
void IOCPServerMgr::ServerEnd()
{
	pNetworkMgr->Release();
	pCommandMgr->Release();

	AllConnectUser.Clear();
}

void IOCPServerMgr::ObserverConnectUser(const float& _DeltaTime)
{
	if (true == AllConnectUser.Empty())
		return;

	LocalCS Lock(&AllConnectUser.m_SyncObj);

	std::list<CServerUser*>::iterator Start = AllConnectUser.m_List.begin();
	std::list<CServerUser*>::iterator End = AllConnectUser.m_List.end();

	CServerUser* pUser = nullptr;

	for (; Start != End;)
	{
		pUser = (*Start);

		if (MAXCRITICALTIME <= (*Start)->AddCriticalTime(_DeltaTime))
		{
			Start = AllConnectUser.m_List.erase(Start);
			
			closesocket(pUser->m_UserSock);
			PrintLog(LogManager::MakeLogString("Time Over  Client IP : ", pUser->GetRemoteIPAddressString()
				, " , Connect Users : ", (int)AllConnectUser.Size()));
	
			//PostQueuedCompletionStatus(pNetworkMgr->m_hNetIOCP, 0, (ULONG_PTR)pUser, (LPOVERLAPPED)&pUser->m_RecvBuffer);
			//pCommandMgr->ExitRoom(pUser);
			//printf("Time Over  Client IP : %s , Connect Users : %d \n", pUser->GetRemoteIPAddressString(), (int)AllConnectUser.Size());
			// 연결이 끊긴 유저가 룸안에 있는경우 룸안에 유저를 제거한다.
		}
		else
		{
			++Start;
		}
	}

}
const bool IOCPServerMgr::ConnectUser(CServerUser::OVERBUFFER* _pOverPart)
{
	_pOverPart->m_WSABuf.buf = _pOverPart->m_Packet;
	_pOverPart->m_WSABuf.len = PACKET_PAYLOAD + sizeof(PacketHeader);
	_pOverPart->m_eType = CServerUser::WORK_RECV;

	int SizeLocal;
	int SizeRemote;
	sockaddr_in* Local = nullptr;
	sockaddr_in* Reomote = nullptr;

	// 억셉트로 호출한 소켓도 알아올수 있다.
	// 해당 프로세스의 억셉트ex를 한번이라도 호출해서 돌아가고 있다면의 경우
	// 알아올수 있다.

	// AcceptEx는 IPv6 버젼 IP까지 모두 받기위해 소켓주소크기를 원래 구조체보다 16바이트 더 잡아먹는다.
	GetAcceptExSockaddrs(&_pOverPart->m_Packet[0], 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16
		, (sockaddr**)&Local, &SizeLocal
		, (sockaddr**)&Reomote, &SizeRemote);
	memcpy_s(&_pOverPart->m_User->m_AddLocal, sizeof(sockaddr_in), Local, sizeof(sockaddr_in));
	memcpy_s(&_pOverPart->m_User->m_AddRemote, sizeof(sockaddr_in), Reomote, sizeof(sockaddr_in));

	CServerUser* ConnUser = SSDServerCore::WaitToConnetUser(_pOverPart->m_UserSock);

	// Aceppt 된 유저의 소켓을 IOCP와 연결(지금 부터 IOCP가 유저 소켓을 감지)
	CreateIoCompletionPort((HANDLE)_pOverPart->m_UserSock, pNetworkMgr->m_hNetIOCP, (ULONG_PTR)ConnUser, 0);

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

void IOCPServerMgr::PrevUpdate(const float& _DeltaTime)
{
	ObserverConnectUser(_DeltaTime);
	pCommandMgr->PacketProgress(_DeltaTime);
	//pCommandMgr->PacketProgress(_DeltaTime);
}

void IOCPServerMgr::Update(const float& _DeltaTime)
{
	// 여기서 커맨드 쓰레드를 통한 게임 로직을 실행한다.
	//pCommandMgr->GameProgress(_DeltaTime);
}

void IOCPServerMgr::PostUpdate(const float& _DeltaTime)
{

}
void IOCPServerMgr::PrintLog(const std::string& _StrLog, const LOG_FLAG& _Flag /*= LOG_PRINT_CONSOL*/)
{
	pLogMgr->PrintLog(_StrLog, _Flag);
}
void IOCPServerMgr::RecvUserPacket(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	pCommandMgr->RecvUserPacket(_pUser);
}

const int IOCPServerMgr::EnterRoom(const RoomEnterData& _EnterData)
{
	return pCommandMgr->EnterRoom(_EnterData);
}
void IOCPServerMgr::SendActiveRoomList(CServerUser* _pUser)
{
	CServerUser* ReqUser = AllConnectUser.Find(_pUser);

	if (nullptr == ReqUser)
		return;

	pCommandMgr->SendActiveRoomList(_pUser);
}
void IOCPServerMgr::ExitRoom(CServerUser* _pUser)
{
	pCommandMgr->ExitRoom(_pUser);
}


void IOCPServerMgr::DisConnectUser(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	_pUser->ASyncInit(); 
	_pUser->ResetCriticalTime();

	_pUser->m_RecvBuffer.m_eType = CServerUser::WORK_CONNET;

	int Return = AcceptEx(SSDServerCore::AcceptSock, _pUser->m_UserSock
		, &_pUser->m_RecvBuffer.m_Packet, 0
		, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16
		, &_pUser->m_RecvBuffer.m_PacketSize, &_pUser->m_RecvBuffer.m_Over);

	if (TRUE != Return)
	{
		DWORD errCode = WSAGetLastError();

		if (ERROR_IO_PENDING != errCode)
		{
			LPVOID lpMsgBuf = nullptr;
			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
			std::wstring LastError = (wchar_t*)lpMsgBuf;
			LocalFree(lpMsgBuf);

			assert(nullptr);
		}

	}


	if (true == SSDServerCore::UpdateUserSocket(_pUser))
	{
		PrintLog(LogManager::MakeLogString("Success Socket Update , New Socket : ", (int)_pUser->m_UserSock));
	}
	else
	{
		PrintLog(LogManager::MakeLogString("Fail Socket Update"));
	}

}