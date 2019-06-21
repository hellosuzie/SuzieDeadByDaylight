#pragma once
#include <ServerManager.h>
#include "MiddleStruct.h"



class DBDServerManager : public CServerManager
{
	std::vector<class CRoom*>				m_vecRoom;
	unsigned int							m_iActivcRoomCount;

public:
	void Update(const float& _DeltaTime) override;
	void PushPacket(CServerUser* _pUser, const unsigned int& _DataLength) override;
	void PushPacket(CServerUser* _pUser) override;
	void DisConnectUser(CServerUser* _pUser) override;

private:
	void PacketProcessing(const float& _DeltaTime,const PACKET& _Pack);


public:
	const int EnterRoom(const RoomEnterData& _EnterData);
	const int RandomMatching();
	void ExitRoom(CServerUser* _pUser);

public:
	DBDServerManager();
	~DBDServerManager();
};

