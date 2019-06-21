#include "NetworkClient.h"
#include "ServerThread.h"
#include <WS2tcpip.h>
#include <atlstr.h>



unsigned int _stdcall ClientThreadRecv(void* _Arg)
{
	CNetworkClient* Ptr = (CNetworkClient*)_Arg;

	//char ArrBuf[1024];

	while ( Ptr->IsConnect())
	{
		//int Result = WSARecv(Ptr->m_UserSock, &Ptr->m_RecvBuffer.m_WSABuf, 1, &(Ptr->m_RecvBuffer.m_PacketSize), &Ptr->m_RecvBuffer.m_Flag, NULL, NULL);
		int Result = recv(Ptr->m_UserSock, Ptr->m_RecvBuffer.m_Packet, sizeof(PACKET) , 0);
		
		if (-1 != Result && true == Ptr->m_bRecivePacket)
		{
			Ptr->m_RecvBuffer.m_PacketSize = Result;
			
			if (FLAG_DISCONNECT == Ptr->CheckHeaderFlag())
				break;

			Ptr->PacketRecv(Ptr);
		}
	
	}


	closesocket(Ptr->m_UserSock);
	return 0;
}




CNetworkClient::CNetworkClient()
	: CServerUser(), m_PORT(-1), m_bConnect(false), m_ConnectIP(nullptr), m_bRecivePacket(true)
{
}


CNetworkClient::~CNetworkClient()
{
	m_bConnect = false;
}

void CNetworkClient::CreateBasicSock() 
{
	WSADATA WData;
	WSAStartup(MAKEWORD(2, 2), &WData);

	ASyncInit();

	if (0 == m_UserSock)
	{
		assert(nullptr);
		return;
	}

	// IPv4를 이용
	m_AddRemote.sin_family = AF_INET;
	m_AddRemote.sin_port = htons(m_PORT);

	if (nullptr == m_ConnectIP)
	{
		m_AddRemote.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		InetPton(m_AddRemote.sin_family, m_ConnectIP, &m_AddRemote.sin_addr.s_addr);
	}

}



const bool CNetworkClient::Connect(UINT _PORT, const wchar_t* _IP /*= nullptr*/)
{
	m_PORT = _PORT;
	m_ConnectIP = _IP;
	CreateBasicSock();

	// IOCP 워커 쓰레드들에게 Connect 신호를 보낸다.
	int Check = WSAConnect(m_UserSock, (sockaddr*)&m_AddRemote, sizeof(SOCKADDR_IN),NULL, NULL, NULL, NULL);
	
	if (-1 == Check)
	{
		m_bConnect = false;
	}
	else 
	{
		m_bConnect = true;
	}

	return m_bConnect;
}


void CNetworkClient::ThreadConnect(UINT _PORT, const wchar_t* _IP /*= nullptr*/)
{
	m_PORT = _PORT;
	m_ConnectIP = _IP;

	CreateBasicSock();
}

void CNetworkClient::PacketProgressStart(unsigned int(_stdcall *_PacketRecv)(CNetworkClient* _Arg))
{
	PacketRecv = _PacketRecv;

	CServerThread::Processing(L"Recv", ClientThreadRecv, this);
}


const WORD CNetworkClient::CheckHeaderFlag()
{
	if (m_RecvBuffer.m_PacketSize < sizeof(PacketHeader))
		return 0;

	PacketHeader* pHeader = (PacketHeader*)(m_RecvBuffer.m_Packet);
	return pHeader->HeaderFlag;
}