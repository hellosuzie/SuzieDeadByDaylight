#pragma once
#include "SSDServerCore.h"


typedef class CNetworkClient : public CServerUser
{
	friend unsigned int _stdcall ClientThreadRecv(void* _Arg);
private:
	const wchar_t*								m_ConnectIP;
	UINT										m_PORT;
	bool										m_bConnect;
	bool										m_bRecivePacket;


public:
	void NonReceivePacket() 
	{
		m_bRecivePacket = false;
	}
	void ReceivePacket() 
	{
		m_bRecivePacket = true;
	}
	void ReceivePacket(const bool& _Receive)
	{
		m_bRecivePacket = _Receive;
	}

	const bool IsReceivePacket() 
	{
		return m_bRecivePacket; 
	}
public:
	unsigned int(_stdcall *PacketRecv)(CNetworkClient* _Arg);

public:
	bool IsConnect()
	{
		return m_bConnect;
	}
	void DisConnect() 
	{
		m_bConnect = false;
	}


public:
	void CreateBasicSock();
	const bool Connect(UINT _PORT, const wchar_t* _IP = nullptr);
	void ThreadConnect(UINT _PORT, const wchar_t* _IP = nullptr);

public:
	const WORD CheckHeaderFlag();

public:
	void PacketProgressStart(unsigned int(_stdcall *PacketRecv)(CNetworkClient* _Arg));
	

public:
	CNetworkClient();
	~CNetworkClient();
}NetClient;

