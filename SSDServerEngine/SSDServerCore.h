#pragma once
#include "SyncContainer.h"
#include "SSDConsole.h"

#define SERVER_IP L"192.168.1.146"


class CServerUser : public CServerObject
{
public:
	enum WORK_TYPE
	{
		WORK_CONNET,
		WORK_RECV,
		WORK_SEND,
		WORK_ACCEPT,
		WORK_REUSE,
	};

	class OVERBUFFER 
	{
	public:
		OVERLAPPED						m_Over;
		SOCKET							m_UserSock;
		WORK_TYPE						m_eType;
		DWORD							m_Flag;
		WSABUF							m_WSABuf;
		unsigned long					m_PacketSize;
		char							m_Packet[PACKET_DOUDLESIZE];
		CServerUser*					m_User;

	public:
		const bool RecvPacket();

	public:
		const bool SendPacket(char * _pBuffer, unsigned int _iSize);
		const bool AyncSendPacket(char * _pBuffer, unsigned int _iSize);

	};

public:
	SOCKET							m_UserSock;
	SOCKADDR_IN						m_AddLocal;
	SOCKADDR_IN						m_AddRemote;
	OVERBUFFER						m_RecvBuffer;
	OVERBUFFER						m_SendBuffer;
	UINT							m_RoomNumber;

public:
	CServerCS						m_UserCS;
	float							m_fAccCriticalTime;

public:
	char							m_Local_IP[16];
	char							m_Remote_IP[16];

public:
	char							m_UserRecvStream[STREAM_SIZE];
	int								m_iRemainSize;
	std::queue<PACKET>				m_quUserPacket;


public:
	void ASyncInit();
	void ReallocateUserSocket();


public:
	void ConnectEvent();

public:
	const char* GetLocalIPAddressString();
	const char* GetRemoteIPAddressString();
	const int GetLocalPortNumber();
	const int GetRemotePortNumber();


public:
	void RecvPacket();

public:
	const PACKET MakePacket(void* _pData, unsigned int _iSize, const unsigned int& _iHeaderType = 0, const unsigned int& _iDataType = 0);
	void SendData(void* _pData,unsigned int _iSize, const unsigned int& _iHeaderType = 0, const unsigned int& _iDataType = 0);
	
public:
	void SendPing();
	void SendPong();
public:
	void RequestCompleteConnect();
	void RequestDisConnect();

	void NotifyCompleteConnect();
	void NotifyDisConnect();

public:
	void ResetCriticalTime();
	const float AddCriticalTime(const float& _DeltaTime);


public:
	const bool DispatchPacket(const DWORD& _Size);
	const bool ExistUserPacket();
	const PACKET GetUserPacket();

public:
	CServerUser();
	~CServerUser();
	
};



class SSDServerCore
{
public:
	enum SOCK_TYPE
	{
		SOCK_TYPE_OVERLAPPED = 0x01,
		SOCK_TYPE_MULTIPOINT_C_ROOT = 0x02,
		SOCK_TYPE_MULTIPOINT_C_LEAF = 0x04,
		SOCK_TYPE_MULTIPOINT_D_ROOT = 0x08,
		SOCK_TYPE_MULTIPOINT_D_LEAF = 0x10,
		SOCK_TYPE_ACCESS_SYSTEM_SECURITY = 0x40,
		SOCK_TYPE_NO_HANDLE_INHERIT = 0x80,
		SOCK_TYPE_REGISTERED_IO = 0x100
	};


public:
	class CServerUpdater
	{
		friend class SSDServerCore;

	protected:
		bool										m_bServerLive;
		CPacketQueue								m_ServerPacketQueue;


	public:
		virtual void ServerStart() = 0;
		virtual void ServerEnd() {}
		virtual void PrevUpdate(const float& _DeltaTime) {}
		virtual void Update(const float& _DeltaTime) {}
		virtual void PostUpdate(const float& _DeltaTime) {}
	public:
		void UpdaterOff() 
		{
			m_bServerLive = false;
		}
	public:
		bool IsUpdate() {
			return m_bServerLive;
		}

		virtual const bool IsInitComplete() { return true; }
	public:
		CServerUpdater()
			:m_bServerLive(false) {}
		virtual ~CServerUpdater() = 0 {}

	};

	static bool									bServerProgress;
	static CServerUpdater*						ServerUpdator;
	static class CServerManager*				pServerManager;

	static int									ServerPort;
	static unsigned int							UserCount;
	static SOCKET								AcceptSock;

	//static CSyncUMap<SOCKET,CServerUser*>		mapUser;
	static CSyncUMap<CServerUser*,SOCKET>		mapUser;
	
public:
	static bool IsUpdate() { return bServerProgress; }

public:
	static void ServerOperation(unsigned int _PORT,const bool& _Con = true);
	static void ServerQuit();
	static void CoreRelease();

public:
	static SOCKET GetASyncServerSock(unsigned int _WaitQueueSize,const SOCK_TYPE& _eSock = SOCK_TYPE_OVERLAPPED);
	static SOCKET GetASyncSock(const SOCK_TYPE& _eSock);

public:
	static CServerUser* CreateASyncUser();
	static CServerUser* ServerUserASyncAccept();

public:


public:
	static CServerUser* WaitToConnetUser(SOCKET _Sock);
	static const bool UpdateUserSocket(CServerUser* _pUser);

	template<typename SERVERTYPE>
	static SERVERTYPE* ServerOperation(unsigned int _PORT, const bool& _Con /*= true*/)
	{
		WSADATA WData;
		WSAStartup(MAKEWORD(2, 2), &WData);

		ServerPort = _PORT;

		SERVERTYPE* NewServer = new SERVERTYPE();
		ServerUpdator = NewServer;

		if (nullptr == ServerUpdator)
			return nullptr;

		bServerProgress = true;

		if (true == _Con)
			SSDConsole::ConsoleOperation();

		ServerUpdator->ServerStart();
		return NewServer;
	}

public:
	SSDServerCore() {}
	virtual ~SSDServerCore();
};

