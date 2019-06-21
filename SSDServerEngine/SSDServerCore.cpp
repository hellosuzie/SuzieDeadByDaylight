#include "SSDServerCore.h"
#include "IOCPServer.h"
#include <crtdbg.h>
#include "SSDConsole.h"
#include "ServerManager.h"

#include <WS2tcpip.h>



///////////////////////////////////////				OverPart				//////////////////////////////
const bool CServerUser::OVERBUFFER::RecvPacket()
{
	DWORD dwFlag = 0;
	m_WSABuf.len = PACKET_SIZE + sizeof(PacketHeader);
	m_WSABuf.buf = m_Packet;

	WSARecv(m_UserSock, &m_WSABuf, 1, &m_PacketSize, &dwFlag, &m_Over, NULL);

	return true;
}

const bool CServerUser::OVERBUFFER::SendPacket(char * _pBuffer, unsigned int _iSize)
{
	DWORD dwFlag = 0;
	m_WSABuf.len = _iSize;
	memcpy(m_Packet, _pBuffer, _iSize);
	m_WSABuf.buf = m_Packet;
	WSASend(m_UserSock, &m_WSABuf, 1, &m_PacketSize, dwFlag, NULL, NULL);
	return true;
}


const bool CServerUser::OVERBUFFER::AyncSendPacket(char * _pBuffer, unsigned int _iSize)
{
	DWORD dwFlag = 0;
	m_WSABuf.len = _iSize;
	memcpy(m_Packet, _pBuffer, _iSize);
	m_WSABuf.buf = m_Packet;
	WSASend(m_UserSock, &m_WSABuf, 1, &m_PacketSize, dwFlag, &m_Over, NULL);
	return true;
}

///////////////////////////////////////				Server User				/////////////////////////////////
void CServerUser::ASyncInit()
{
	m_UserSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	
	BOOL on = TRUE;
	
	//int OptR = setsockopt(m_UserSock, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));

	//if (0 != OptR)
	//	assert(nullptr);

	int OptR = setsockopt(m_UserSock, IPPROTO_TCP, TF_REUSE_SOCKET, (char *)&on, sizeof(on));

	if (0 != OptR)
		assert(nullptr);

	OptR = setsockopt(m_UserSock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	if (0 != OptR)
		assert(nullptr);

	ZeroMemory(&m_RecvBuffer ,sizeof(OVERBUFFER));
	ZeroMemory(&m_SendBuffer, sizeof(OVERBUFFER));

	m_RecvBuffer.m_UserSock = m_UserSock;
	m_SendBuffer.m_UserSock = m_UserSock;

	m_RecvBuffer.m_User = this;
	m_SendBuffer.m_User = this;
}

void CServerUser::ReallocateUserSocket()
{
	//shutdown(m_UserSock, CF_BOTH);
	//closesocket(m_UserSock);
	//ASyncInit();


	BOOL Check = TransmitFile(m_UserSock, NULL, NULL, NULL, (LPOVERLAPPED)&m_RecvBuffer.m_Over, NULL, TF_DISCONNECT | TF_REUSE_SOCKET);

	if (SOCKET_ERROR == Check)
	{
		assert(true);
	}

	m_RecvBuffer.m_eType = CServerUser::WORK_REUSE;
	m_SendBuffer.m_eType = CServerUser::WORK_SEND;

	ResetCriticalTime();
}
void CServerUser::ConnectEvent()
{
	inet_ntop(AF_INET, &m_AddLocal.sin_addr, m_Local_IP, sizeof(m_Local_IP));
	inet_ntop(AF_INET, &m_AddRemote.sin_addr, m_Remote_IP, sizeof(m_Remote_IP));
}
const char* CServerUser::GetLocalIPAddressString()
{
	return m_Local_IP;
}
const char* CServerUser::GetRemoteIPAddressString()
{
	return m_Remote_IP;
}
const int CServerUser::GetLocalPortNumber()
{
	return ntohs(m_AddLocal.sin_port);
}
const int CServerUser::GetRemotePortNumber()
{
	return ntohs(m_AddRemote.sin_port);
}
void CServerUser::RecvPacket()
{
	m_RecvBuffer.RecvPacket();
}
const PACKET CServerUser::MakePacket(void* _pData, unsigned int _iSize, const unsigned int& _iHeaderType, const unsigned int& _iDataType)
{
	PACKET NewPacket;
	memset(&NewPacket, 0, sizeof(PACKET));

	NewPacket.Header.Size = _iSize + sizeof(PacketHeader) ;
	NewPacket.Header.HeaderFlag = _iHeaderType;
	NewPacket.Header.DataType = _iDataType;

	if (_iSize != 0 && nullptr != _pData) 
	{
		memcpy(NewPacket.Packet, _pData, _iSize);
	}

	return NewPacket;
}

void CServerUser::SendData(void* _pData, unsigned int _iSize, const unsigned int& _iHeaderType, const unsigned int& _iDataType)
{
	PACKET SendPacket = MakePacket(_pData, _iSize, _iHeaderType, _iDataType);
	memset(&SendPacket, 0, sizeof(PACKET));

	SendPacket.Header.Size = _iSize + sizeof(PacketHeader);
	SendPacket.Header.HeaderFlag = _iHeaderType;
	SendPacket.Header.DataType = _iDataType;

	memcpy(SendPacket.Packet, _pData, _iSize);
	m_SendBuffer.SendPacket((char*)&SendPacket, SendPacket.Header.Size );
}

void CServerUser::SendPing() 
{
	PACKET PingPacket = MakePacket(nullptr, 0,FLAG_PING);
	m_SendBuffer.SendPacket((char*)&PingPacket, PingPacket.Header.Size);
}
void CServerUser::SendPong()
{
	PACKET PongPacket = MakePacket(nullptr, 0, FLAG_PONG);
	m_SendBuffer.SendPacket((char*)&PongPacket, PongPacket.Header.Size);
}
void CServerUser::RequestCompleteConnect() 
{
	PACKET Pack = MakePacket(nullptr, 0, FLAG_CONNECT);
	m_SendBuffer.SendPacket((char*)&Pack, Pack.Header.Size);
}
void CServerUser::RequestDisConnect() 
{
	PACKET Pack = MakePacket(nullptr, 0, FLAG_DISCONNECT);
	m_SendBuffer.SendPacket((char*)&Pack, Pack.Header.Size);
}
void CServerUser::NotifyCompleteConnect()
{
	return RequestCompleteConnect();
}
void CServerUser::NotifyDisConnect()
{
	return RequestDisConnect();
}
CServerUser::CServerUser()
	:m_RoomNumber(-1),m_UserSock(INVALID_SOCKET)
	,m_UserCS(CServerCS()),m_fAccCriticalTime(0.f)
	, m_iRemainSize(0)
{
	ZeroMemory(m_Local_IP, sizeof(m_Local_IP));
	ZeroMemory(m_Remote_IP, sizeof(m_Remote_IP));
	ZeroMemory(m_UserRecvStream, sizeof(m_UserRecvStream));
}
CServerUser::~CServerUser() 
{

}

void CServerUser::ResetCriticalTime() 
{
	LocalCS Lock(&m_UserCS);
	m_fAccCriticalTime = 0.f;

}
const float CServerUser::AddCriticalTime(const float& _DeltaTime) 
{
	LocalCS Lock(&m_UserCS);
	m_fAccCriticalTime += _DeltaTime;
	return m_fAccCriticalTime;
}

const bool CServerUser::DispatchPacket(const DWORD& _Size)
{
	if (_Size <= 0)
		return false;

	LocalCS Lock(&m_UserCS);
	
	CopyMemory(m_UserRecvStream + m_iRemainSize, m_RecvBuffer.m_Packet, _Size);
	m_iRemainSize += _Size;
	bool PingCheck = false;


	while (true)
	{
		//남은 길이가 8바이트 보다 작으면 중지
		if (m_iRemainSize < sizeof(DWORD))
			break;

		// 패킷 길이 얻어오기
		int iPacketLen = 0;
		CopyMemory(&iPacketLen, m_UserRecvStream, sizeof(DWORD));

		//남은 길이가 패킷길이보다 작으면 중지
		if (m_iRemainSize < iPacketLen)
			break;


		//패킷으로 데이터 추출
		PACKET NewPacket;
		ZeroMemory(&NewPacket, sizeof(PACKET));

		memcpy(&NewPacket, m_UserRecvStream, iPacketLen);
		NewPacket.pUser = this;

		if (NewPacket.Header.HeaderFlag == FLAG_PING || NewPacket.Header.HeaderFlag == FLAG_PONG)
		{
			PingCheck = true;
		}
		else 
		{
			m_quUserPacket.push(NewPacket);
		
		}
		//패킷 스트림에서 패킷만큼 앞으로 이동
		m_iRemainSize -= iPacketLen;
		MoveMemory(m_UserRecvStream, m_UserRecvStream + iPacketLen, m_iRemainSize);	
	}	

	return PingCheck;

}

const bool CServerUser::ExistUserPacket()
{
	return !m_quUserPacket.empty();
}
const PACKET CServerUser::GetUserPacket()
{
	PACKET ReturnPacket = m_quUserPacket.front();
	m_quUserPacket.pop();
	return ReturnPacket;
}
///////////////////////////////////////				Server Core				/////////////////////////////////

bool								SSDServerCore::bServerProgress = false;
SSDServerCore::CServerUpdater*		SSDServerCore::ServerUpdator = nullptr;
CServerManager*						SSDServerCore::pServerManager = nullptr;
int									SSDServerCore::ServerPort = 0;
unsigned int						SSDServerCore::UserCount = 0;
SOCKET								SSDServerCore::AcceptSock;

//CSyncUMap<SOCKET, CServerUser*>	SSDServerCore::mapUser;
CSyncUMap<CServerUser*,SOCKET>		SSDServerCore::mapUser;

SSDServerCore::~SSDServerCore()
{
	
}
void SSDServerCore::ServerOperation(unsigned int _PORT, const bool& _Con /*= true*/)
{
	WSADATA WData;
	WSAStartup(MAKEWORD(2, 2), &WData);

	ServerPort = _PORT;

	ServerUpdator = new CIOCPServer();


	if (nullptr == ServerUpdator)
		return;

	bServerProgress = true;

	if(true == _Con)
		SSDConsole::ConsoleOperation();

	ServerUpdator->ServerStart();
}


void SSDServerCore::ServerQuit()
{
	ServerUpdator->UpdaterOff();
	CServerThread::End();
	bServerProgress = false;


	UserCount = 0;

	if (0 != AcceptSock)
	{
		closesocket(AcceptSock);
	}
	
	if (nullptr != ServerUpdator)
	{
		ServerUpdator->ServerEnd();
		DELETE_POINTER(ServerUpdator)
	}
	
	
	CoreRelease();
	SSDConsole::ConsoleQuit();
	WSACleanup();
}
void SSDServerCore::CoreRelease()
{
	for (std::unordered_map<CServerUser*,SOCKET>::value_type& UserIter : mapUser.m_UMap)
	{
		CServerUser* pUser = UserIter.first;
		closesocket(UserIter.second);
		DELETE_POINTER(pUser);
	}

	mapUser.Clear();


}
SOCKET SSDServerCore::GetASyncServerSock(unsigned int _WaitQueueSize, const SOCK_TYPE& _eSock/* = SOCK_TYPE_OVERLAPPED*/)
{
	AcceptSock = GetASyncSock(_eSock);

	if (SOCKET_ERROR == listen(AcceptSock, _WaitQueueSize))
		assert(nullptr);
	
	return AcceptSock;
}
SOCKET SSDServerCore::GetASyncSock(const SOCK_TYPE& _eSock)
{
	// 서버 통신을 할건데.
	// 어떤 버전의 어떤 프로토콜 서버통신을 할것인지에 대한 데이터
	AcceptSock = 0;
	AcceptSock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, _eSock);


	if (INVALID_SOCKET == AcceptSock)
		assert(nullptr);

	// 소켓 주소를 만든다 (소켓주소 = IP주소 + 포트 번호)
	SOCKADDR_IN Add;
	// IP4 프로토콜을 사용하겠다.
	Add.sin_family = AF_INET;

	// listen 할 주소를 전체 주소로 잡아준다.
	Add.sin_addr.s_addr = htonl(INADDR_ANY);

	//InetPton(Add.sin_family, SERVER_IP, &Add.sin_addr.s_addr);
	Add.sin_port = htons(ServerPort);

	// 서버쪽 소켓의 로컬 주소와 로컬 포트번호를 결정
	if (SOCKET_ERROR == ::bind(AcceptSock, (SOCKADDR*)&Add, sizeof(SOCKADDR)))
		assert(nullptr);
	
	BOOL on = TRUE;

	int OptR = setsockopt(AcceptSock, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char *)&on, sizeof(on));

	if (0 != OptR)
		assert(nullptr);

	OptR = setsockopt(AcceptSock, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));

	if (0 != OptR)
		assert(nullptr);

	return AcceptSock;
}
CServerUser* SSDServerCore::CreateASyncUser()
{
	CServerUser* NewUser = new CServerUser();
	NewUser->ASyncInit();
	return NewUser;
}
CServerUser* SSDServerCore::ServerUserASyncAccept()
{
	CServerUser* NewUser = CreateASyncUser();

	NewUser->m_RecvBuffer.m_eType = CServerUser::WORK_CONNET;
	mapUser.Insert(NewUser, NewUser->m_UserSock);
	

	int Return = AcceptEx(AcceptSock, NewUser->m_UserSock
		, &NewUser->m_RecvBuffer.m_Packet, 0
		, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16
		, &NewUser->m_RecvBuffer.m_PacketSize, &NewUser->m_RecvBuffer.m_Over);

	if (TRUE != Return)
	{
		DWORD errCode = WSAGetLastError();

		if (ERROR_IO_PENDING == errCode)
			return NewUser;
		

		LPVOID lpMsgBuf = nullptr;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		std::wstring LastError = (wchar_t*)lpMsgBuf;
		LocalFree(lpMsgBuf);
		
		assert(nullptr);
		delete NewUser;
		return nullptr;
	}


	return NewUser;
}

CServerUser* SSDServerCore::WaitToConnetUser(SOCKET _Sock)
{
	CServerCS CS;

	CS.Enter();
	// 동기화 해줘야 할거 같다.
	CServerUser* ConnUser = mapUser.FindKey(_Sock);

	if (nullptr == ConnUser)
	{
		assert(nullptr);
		return nullptr;
	}
	CS.Leave();

	return ConnUser;
}

const bool SSDServerCore::UpdateUserSocket(CServerUser* _pUser)
{
	//mapUser.ReplaceValue(_pUser, _pUser->m_UserSock);

	//if (true == mapUser.ReplaceValue(_pUser, _pUser->m_UserSock)) 
	//{
	//	printf("Success Socket Update , New Socket :%d \n",(int) _pUser->m_UserSock);
	//}
	//else 
	//{
	//	printf("Fail Socket Update\n");
	//
	//}

	return  mapUser.ReplaceValue(_pUser, _pUser->m_UserSock);
}