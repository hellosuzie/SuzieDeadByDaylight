#pragma once
#include <ServerConnectMgr.h>
#include <DXMacro.h>
#include <ReferenceBase.h>
#include "MiddleStruct.h"
#include "ClientReceiver.h"
#include "ClientSender.h"
#include "ClientLogMgr.h"
#include "ClientSession.h"

#define FAILRECVPONG -1.f
#define MAXWAITPONGTIME 5.f


typedef class CClientConnectMgr : public CServerConnectMgr
{
	SINGLETONBODY(CClientConnectMgr)
	
private:
	friend CClientReceiver;
	friend CClientSender;
	friend CClientSession;

private:
	static CPacketQueue								ClientPacketQueue;
	static unsigned int _stdcall PacketRecvProcess(CNetworkClient* _Arg);
	static float									fPacketRecvTime;
	
public:
	static void Init();

public:
	CClientLogMgr									m_LogMgr;

private:
	CClientReceiver									m_Receiver;
	CClientSender									m_Sender;
	CClientSession									m_Session;

	float											m_fPing;
	float											m_fMaxPing;
	float											m_fPingAccTime;
	//float											m_fMaxPing;
	bool											m_bCompleteConnect;
	bool											m_bRoomEnter;
	long											m_RoomDataRequestCount;
	unsigned int									m_SendCount;


	void(CClientConnectMgr::*pPingUpdate)(const float&);

private:
	SPTR<class CPlayer>								m_ClientLocalPlayer;


public:
	void SetLocalPlayer(CPlayer* _pPlayer);

public:
	const long GetRoomDataRequestCount() { return m_RoomDataRequestCount; }

public:
	const bool IsCompleteConnect() { return m_bCompleteConnect; }
	const bool IsRoomEnter() { return m_bRoomEnter; }


public:
	const bool ExistPacket();
	const bool EmptyPacketQueue();
	const bool GetPacket(PACKET& _Packet);
	void ReceivePacket(const bool _Receive);
	void ClearPacketQueue();
	

public:
	// 패킷 큐에서 패킷을 뽑아온뒤 처리하는 함수
	void CheckConnectServer(const float& _DeltaTime);
	const int ReceiveRoomPacket(WorldData* _BuildData);
	

public:
	void ConnectLogicServer();
	void SendPlayerState(const float& _DeltaTime);

public:
	void RequestEnterRoom(const PLAYER_ROLE_TYPE& _Type,const unsigned int& _Room = -1);
	void RequestEnterLobbyRoom(const PLAYER_ROLE_TYPE& _Type, const unsigned int& _Room = -1);
	void RequestEnterTestRoom();
	void NotifyBuildComplete();
	void RequestRoomData();
	void RequestActiveRoomList();
	void NotifyReadyGame();
	void NotifyCancleReady();
	void NotifyLobbyExit();
	void NotifyWorldBuildComplete();
	void RequestSceneObject();


public:

private:
	void TryServerConnect();

public:
	void DisConnectLogicServer();

public:
	void OperationSessionThread();
	void SendPingEvent();
	void RecvPongEvent();
	void  RecordPing(const float& _Ping);
	const float GetPing() 
	{
		return m_fPing * 1000.f; 
	}
	const float GetPingTime() { return m_fPing; }
	const float GetMaxPing() { return m_fMaxPing* 1000.f; }


}ClientConnectMgr,ClientConMgr,ClientConnector;

