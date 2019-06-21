#pragma once
#include "ServerObject.h"
#include <list>
#include "SyncContainer.h"
#include <Scene.h>
#include <array>
#include <PacketHeader.h>
#include "MiddleMacro.h"
#include "MiddleStruct.h"



class CDBDRule;
class CGameWorld : public CSceneScript 
{
private:
	friend class CRoom;
	SPTR<class CTerrain>							m_pTerrain;
	CDBDRule*										m_pRule;
	std::list<SPTR<class CNavigationMesh>>			m_listWorldNavi;
public:
	const bool Loading() override;
	void PreUpdate(const float& _DeltaTime)override;
	void PostUpdate(const float& _DeltaTime)override;
	void LastUpdate(const float& _DeltaTime)override;

public:
	void CollocateSceneObject();

public:
	CGameWorld(CScene* _PairScene);
	~CGameWorld();

};


typedef void(_stdcall*   Room_PackProc_Function)(CRoom* _pRoom,const PACKET& _Pack,const float& _DeltaTime);

class CRoom : public CServerObject
{
private:
	friend class IOCPServerMgr;
	static IOCPServerMgr*													pRoomServerMgr;
	static Room_PackProc_Function											RoomPackFunc[MAX_HEADER_FLAG];

	static std::map<int, std::list<SPTR<CActor>>>							m_mapAllLoadActor;
	static TerrainData														m_TerrainLoadData;

private:
	static Vec3																SurvivorInitPos[MAXSURVIVORUSER];
	static Vec3																KillerInitPos;

public:
	static void Init(IOCPServerMgr* _pMgr);
	static void _stdcall Disconnect(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime);
	static void _stdcall BuildComplete(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime);
	static void _stdcall LobbyWaiting(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime);
	static void _stdcall GamePlaying(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime);
	static void _stdcall RuleProcess(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime);

private:
	friend class DBDServerManager;
	static  DBDServerManager*												pServerManager;

private:
	friend class CIOCPServer;

private:
	unsigned int															m_RoomNumber;
	unsigned int															m_CurUserCount;
	

private:
	// Server Part
	bool																	m_bRoomActive;
	bool																	m_bGamePlaying;
	CSyncList<class CServerUser*>											m_listRoomUser;
	CServerUser*															m_pRoomMaster;
	CSyncQueue<RoomEnterData>												m_quEnterData;
	CPacketQueue															m_RoomPacketQueue;

	float																	m_fRoomDeltaTime;
	float																	m_fAccSyncroTime;
	float																	m_fAccGameLogicTime;
private:
	// InGame Part
	SPTR<CScene>															m_RoomGameScene;
	CGameWorld*																m_GameWorld;
	CDBDRule*																m_pGameRule;
	std::unordered_map<CServerUser*, SPTR<class CPlayer>>					m_mapRoomUser;
	std::unordered_map<CServerUser*, SPTR<class CPlayer>>::iterator			m_FindUser;
	std::unordered_map<CServerUser*, SPTR<class CPlayer>>::iterator			m_UserStart;
	std::unordered_map<CServerUser*, SPTR<class CPlayer>>::iterator			m_UserEnd;
	std::queue<SPTR<CPlayer>>												m_quBroadUser;
	
private:
	std::list<PlayerData>													m_listRecvPlayerData;
	std::list<PlayerData>::iterator											m_RecvStart;
	std::list<PlayerData>::iterator											m_RecvEnd;


	Vec3																	m_SurvivorInitPos[MAXSURVIVORUSER];
	Vec3																	m_KillerInitPos;

public:
	void PushRecvData(const PlayerData& _Data);

public:
	void ActiveRoom() { m_bRoomActive = true; }
	void UnActiveRoom() { m_bRoomActive = false; }
	const bool IsActive() { return m_bRoomActive; }
	const bool IsFullRoom() { return m_listRoomUser.Size() >= MAXROOMUSER; };

	const ActiveRoomData GetRoomData();
	

public:
	void Update(const float& _DeltaTime);
	void TestRoomUpdate(const float& _DeltaTime);

public:
	void PacketProcessing(const float& _DeltaTime);
	void GameLobbyProcessing(const float& _DeltaTime);
	void GameLogicProcessing(const float& _DeltaTime);
	// 서버로 전송된 로컬 플레이어들의 정보를 다른 플레이어들에게 중계하는 함수
	void BroadcastRecvPacket(const float& _DeltaTime);
	// 단위 시간마다 서버에 접속중인 모든 플레이어에게 데이터를 전송
	const bool BroadcastSyncroPacket(const float& _DeltaTime);
	// 정보전달을 요청한 클라이언트 에게만 데이터를 전송
	void SendPacketToSyncPlayer(const float& _DeltaTime);
	void BroadcastExitPlayer(SPTR<CPlayer> _ExitPlayer);

public:
	void SetTestRoom() 
	{
		m_bGamePlaying = true;
	}
	void SetRoomNumber(const unsigned int& _Number)
	{
		m_RoomNumber = _Number;
	}
	const unsigned int GetRoomNumber() 
	{
		return m_RoomNumber;
	}
	const unsigned int GetUserCount()
	{
		return (unsigned int)m_listRoomUser.Size();
	}
	const unsigned __int64 GetPlayerID(CServerUser* _pUser);
	const bool IsGamePlaying() 
	{
		return m_bGamePlaying;
	}


public:
	const bool EnterRoom(CServerUser* _pUser);
	const bool EnterRoom(const RoomEnterData& _EnterData);
	const bool ExitRoom(CServerUser* _pUser);
	const bool ClearRoom();
	const bool IsAllUserReady();
	const bool IsAllUserBuildComplete();
	void RecvBuildCompete(CServerUser* _pUser);

public:
	void BroadcateLobbyEnterUser(CServerUser* _pUser);
	void BroadcateLobbyReadyUser(CServerUser* _pUser);
	void BroadcateLobbyReadyCancleUser(CServerUser* _pUser);
	void BroadcateLobbyExitUser(CServerUser* _pUser);
	void BroadcateGameWorldData();
	void BroadcateWorldObjectData(CServerUser* _pUser);
	void BroadcateGameStart();
public:
	void PushPacket(CServerUser* _pUser, const unsigned int& _DataLength);
	void PushPacket(CServerUser* _pUser);
	void PushPacket(const PACKET& _Pack);

public:
	void CreateGameWorld();
	CPlayer* CreatePlayer(CServerUser* _pUser);
	CPlayer* CreatePlayer(const RoomEnterData& _EnterData);

public:
	void BroadcastServerCollisionEvent();

public:
	CRoom();
	~CRoom();
};

