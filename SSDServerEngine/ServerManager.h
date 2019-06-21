#pragma once
#include "SSDServerCore.h"

class CServerManager
{
protected:
	CServerCS								SyncCS;
	CPacketQueue							MainPacketQueue;

public:
	virtual void Update(const float& _DeltaTime) = 0;
	virtual void PushPacket(CServerUser* _pUser,const unsigned int& _DataLength) = 0;
	virtual void PushPacket(CServerUser* _pUser) = 0;
	virtual void DisConnectUser(CServerUser* _pUser) = 0;
public:
	// 가장 기본적인 처리들(핑,연결 과 같은 서버차원에서의 기본 처리 작업
	void PacketPreProcessing(PACKET& _Pack);

	

public:
	CServerManager();
	virtual ~CServerManager();
};

