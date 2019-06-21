#pragma once
#include "SSDServerCore.h"
#include "ServerMacro.h"
#include "ServerNetworkManager.h"
#include "ServerCommendManager.h"
#include "ServerLogManager.h"

#define COMPLETECOUNT 3

class IOCPServerMgr : public SSDServerCore::CServerUpdater
{
private:
	friend class CServerManager;

public:
	static void LogError();

public:
	const bool PrePacketProcessing(CServerUser* _pUser, const UINT& _Size);
	// 이 함수 내에서 패킷처리를 완료했으면 서버 매니져의 패킷큐에 패킷을 추가하지 않는다.
	void PacketProcessing(PACKET& _Pack);


public:
	static  NetworkManager*									pNetworkMgr;
	static  CommandManager*									pCommandMgr;
	static  LogManager*										pLogMgr;
	static  unsigned int									InitCompeteManagerCount;

public:
	static void InitComplete() 
	{
		CServerCS Lock;
		Lock.Enter();
		++InitCompeteManagerCount;
		Lock.Leave();
	}
	// 로그 매니져는 추후에 추가

	static unsigned int										RoomCount;
	static int												AcceptNumber;
	static CSyncList<CServerUser*>							AllConnectUser;

public:
	void OperationIOCPManager();


public:
	const bool IsInitComplete() override
	{
		return InitCompeteManagerCount >= COMPLETECOUNT;
	}
	void ServerStart() override;
	void ServerEnd()override;
	void PrevUpdate(const float& _DeltaTime) override;
	void Update(const float& _DeltaTime)override;
	void PostUpdate(const float& _DeltaTime) override;


public:
	void PrintLog(const std::string& _StrLog  ,const LOG_FLAG& _Flag = LOG_PRINT_CONSOL );

public:
	void RecvUserPacket(CServerUser* _pUser);
	const int EnterRoom(const RoomEnterData& _EnterData);
	void SendActiveRoomList(CServerUser* _pUser);
	void ExitRoom(CServerUser* _pUser);

private:
	void ObserverConnectUser(const float& _DeltaTime);

public:
	static const bool ConnectUser(CServerUser::OVERBUFFER* _pOverPart);
	void DisConnectUser(CServerUser* _pUser);



public:
	IOCPServerMgr();
	~IOCPServerMgr() override;
};

