#include "Room.h"
#include "SSDServerCore.h"
#include "DBDServerManager.h"
#include <Actor.h>
#include "Player.h"
#include <SSDRandom.h>
#include <TransForm.h>
#include "PlayerFSM.h"
#include <PacketHeader.h>
#include "IOCPServerMgr.h"
#include <Terrain.h>
#include <ReadStream.h>
#include "DeadByDaylightGlobal.h"
#include <FilePathMgr.h>
#include <Device.h>
#include <Actor.h>
#include <TransForm.h>
#include <NavigationMesh.h>
#include <OBBCollider.h>
#include <SphereCollider.h>
#include "NavigationObject.h"
#include "MagicCircle.h"
#include "DBDRule.h"
#include <SSDRandom.h>

////////////////////////////////////////////				Game World				///////////////////////////////////
CGameWorld::CGameWorld(CScene* _PairScene) 
	:CSceneScript(_PairScene),m_pRule(nullptr),m_pTerrain(nullptr)
{

}
CGameWorld::~CGameWorld()
{
	m_listWorldNavi.clear();
}



const bool CGameWorld::Loading() 
{
	// 맵오브젝트 및 충돌체 , 지형 등등을 설치한다.
	LinkCollsionChannel(COLLISION_3D, L"Player", L"Default");
	LinkCollsionChannel(COLLISION_3D, L"Player", L"PlayerDetect");
	LinkCollsionChannel(COLLISION_3D, L"Attack", L"PlayerBody");

	SPTR<CActor> TerrainActor = CreateActor(UPDATE_LAYER_NORMAL, L"TerrainActor");
	m_pTerrain = TerrainActor->AddCom<CTerrain>(false);
	m_pTerrain->CreateTerrain(128, 128, 30.f);
	m_pTerrain->SetCellSize(50.f);
	m_pTerrain->Off();

	SPTR<CActor> RuleActor = CreateActor(L"RuleActor", UPDATE_LAYER_NORMAL);
	m_pRule = RuleActor->AddCom<CDBDRule>(SERVER_RULE);
	m_pRule->SetTerrain(m_pTerrain);

	CollocateSceneObject();
	m_pTerrain->LoadTerrainData(DBD::m_TerrainLoadData,&m_listWorldNavi);
	m_pTerrain->BakeNavigationMeshInServer();
	return true;
}

void CGameWorld::PreUpdate(const float& _DeltaTime) 
{

}
void CGameWorld::PostUpdate(const float& _DeltaTime) 
{

}
void CGameWorld::LastUpdate(const float& _DeltaTime) 
{

}
void CGameWorld::CollocateSceneObject()
{
	// 1. 파일에서 읽어들인 액터 정보를 적용
	for (size_t k = 0; k < DBD::m_vecActorLoadData.size(); ++k)
	{
		if (nullptr != DBD::m_vecActorLoadData[k])
		{
			ACTOR_UPDATE_LAYER Layer = (ACTOR_UPDATE_LAYER)DBD::m_vecActorLoadData[k]->UpdateLayer;
			bool IsLight = DBD::m_vecActorLoadData[k]->IsLight;
			bool IsCircle = DBD::m_vecActorLoadData[k]->IsMagicCircle;

			//라이트 엑터
			if (true == IsLight)
			{
				continue;
			}
			//마법진 엑터
			else if (true == IsCircle)
			{
					
				SPTR<CActor> NewActor = m_pPairScene->CreateActor(m_pPairScene, Layer, L"LightActor");
				m_pRule->CreateServerMagicCircle(m_pPairScene->CreateActor(m_pPairScene, Layer, L"CircleActor"), (int)DBD::m_vecActorLoadData[k]->MagicCircleColor, DBD::m_vecActorLoadData[k]->ActorPos, DBD::m_vecActorLoadData[k]->ActorRot.y);
			}
			else
			{
				SPTR<CActor> NewActor = m_pPairScene->CreateActor(m_pPairScene, Layer, L"NewActor");
				NewActor->TRANS->SetPos(DBD::m_vecActorLoadData[k]->ActorPos);
				NewActor->TRANS->SetScale(DBD::m_vecActorLoadData[k]->ActorSize);
				NewActor->TRANS->SetRotate(DBD::m_vecActorLoadData[k]->ActorRot);
	
				//충돌체
				std::vector<PrefabData::ColSaveData> ColSaveVector = DBD::m_vecActorLoadData[k]->vecColData;
				for (size_t j = 0; j < ColSaveVector.size(); ++j)
				{
					if (COL_TYPE::COLTYPE_OBB == DBD::m_vecActorLoadData[k]->vecColData[j].ColType)
					{
						SPTR<COBBCollider> TempColl = NewActor->AddCom<COBBCollider>(COL3D_DATA{ COLDATA{ DBD::m_vecActorLoadData[k]->vecColData[j].ColGroupName, COLLISION_3D } ,false});
						TempColl->SetColSize(DBD::m_vecActorLoadData[k]->vecColData[j].ColSize);
						TempColl->SetColPivot(DBD::m_vecActorLoadData[k]->vecColData[j].ColPos);
					}
					else if (COL_TYPE::COLTYPE_SPHERE == DBD::m_vecActorLoadData[k]->vecColData[j].ColType)
					{
						SPTR<CSphereCollider> TempColl = NewActor->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ DBD::m_vecActorLoadData[k]->vecColData[j].ColGroupName, COLLISION_3D } ,false });
						TempColl->SetColSize(DBD::m_vecActorLoadData[k]->vecColData[j].ColSize);
						TempColl->SetColPivot(DBD::m_vecActorLoadData[k]->vecColData[j].ColPos);
					}
				}

				// 네비게이션 메쉬
				std::vector<PrefabData::NavigationData>& NaviDataVec = DBD::m_vecActorLoadData[k]->vecNaviData;
				size_t NaviCount = NaviDataVec.size();
				for (size_t i = 0; i < NaviCount; i++)
				{
					SPTR<CNavigationMesh> Navi = NewActor->AddCom<CNavigationMesh>();
					Navi->SetNaviData(NAVIMESH_DATA{ (RENDER_LAYER)NaviDataVec[i].RenderLayer, NaviDataVec[i].NaviSerialNumber, NaviDataVec[i].NaviObjName });
					m_listWorldNavi.push_back(Navi);
				}
			}
		}
	}

}


/////////////////////////////////////////////				Room					////////////////////////////////////

IOCPServerMgr*						CRoom::pRoomServerMgr = nullptr;
DBDServerManager*					CRoom::pServerManager = nullptr;
Room_PackProc_Function				CRoom::RoomPackFunc[MAX_HEADER_FLAG] = { nullptr,};


Vec3								CRoom::SurvivorInitPos[MAXSURVIVORUSER] = { Vec3(1450.f,0.f,5800.f) , Vec3(500.f,0.f,1300.f) ,Vec3(5300.f,0.f,1800.f) ,Vec3(5680.f,0.f,5680.f) };
Vec3								CRoom::KillerInitPos = Vec3(3200.f,0.f,600.f);

void CRoom::Init(IOCPServerMgr* _pMgr)
{
	pRoomServerMgr = _pMgr;
	RoomPackFunc[FLAG_DISCONNECT] = Disconnect;
	RoomPackFunc[FLAG_SCENE_BUILD_COMPLETE] = BuildComplete;
	RoomPackFunc[FLAG_GAME_PLAYING] = GamePlaying;
	RoomPackFunc[FLAG_LOBBY_WAITING] = LobbyWaiting;
	RoomPackFunc[FLAG_PLAYING_RULE_SIGNAL] = RuleProcess;
}

void CRoom::Disconnect(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime) 
{
	_pRoom->ExitRoom(_Pack.pUser);
}
void CRoom::BuildComplete(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime) 
{
	_pRoom->m_FindUser = _pRoom->m_mapRoomUser.find(_Pack.pUser);
	if (_pRoom->m_FindUser != _pRoom->m_UserEnd)
	{
		if (false == _pRoom->m_FindUser->second->m_bBroadSync)
		{
			_pRoom->m_FindUser->second->m_bBroadSync = true;
		}
	}
}
void CRoom::LobbyWaiting(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime)
{
	switch (_Pack.Header.DataType)
	{

	case FLAG_DATA_ROOMENTER_USER:
	{
		_pRoom->BroadcateLobbyEnterUser(_Pack.pUser);
	}
	break;
	case FLAG_DATA_READY:
	{
		_pRoom->BroadcateLobbyReadyUser(_Pack.pUser);
	}
	break;
	case FLAG_DATA_READY_CANCLE:
	{
		_pRoom->BroadcateLobbyReadyCancleUser(_Pack.pUser);
	}
	break;
	case FLAG_DATA_ROOM_EXIT:
	{
		_pRoom->ExitRoom(_Pack.pUser);
	}
	break;
	case FLAG_DATA_WORLD_OBJECT:
	{
		_pRoom->BroadcateWorldObjectData(_Pack.pUser);
	}
	break;
	case FLAG_DATA_WORLD_BUILD_COMPLETE:
	{
		_pRoom->RecvBuildCompete(_Pack.pUser);
	}
	break;
	default:
		break;
	}
}

void CRoom::GamePlaying(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime) 
{
	switch (_Pack.Header.DataType)
	{
	case FLAG_DATA_LOCALPLAYER_STATE:
	{
		PlayerData Data = ExtractionData<PlayerData>(_Pack);

		if (true == _pRoom->m_pGameRule->UpdatePlayer(Data)) 
		{
			_pRoom->PushRecvData(Data);
		}

	}
	default:
		break;
	}
}
void  CRoom::RuleProcess(CRoom* _pRoom, const PACKET& _Pack, const float& _DeltaTime)
{
	switch (_Pack.Header.DataType)
	{
	case FLAG_DATA_GANERATOR:
	{
		CircleData Data = ExtractionData<CircleData>(_Pack);
		_pRoom->m_pGameRule->UpdateGernerator(Data);
	}
		break;
	case FLAG_DATA_TRAP: 
	{		
		CircleData Data = ExtractionData<CircleData>(_Pack);
		_pRoom->m_pGameRule->UpdateTrap(Data);
	}
		break;
	case FLAG_DATA_SUMMONSPLAYER:
	case FLAG_DATA_HEALINGPLAYER:
	case FLAG_DATA_RESCUEPLAYER:
	case FLAG_DATA_SELFHEALING:
	case FLAG_DATA_DISSOLVEPLAYER:
	case FLAG_DATA_HOOKPLAYER:
	case FLAG_DATA_ACCHOOK:
	case FLAG_DATA_RESCUED_COMPETE:
	case FLAG_DATA_ESCAPE:
	case FLAG_DATA_DEAD_OUT:
	{
		PlayerSignal Signal = ExtractionData<PlayerSignal>(_Pack);
		_pRoom->m_pGameRule->UpdatePlayer(Signal, _Pack.Header.DataType);
	}
	break;
		break;
	default:
		break;
	}
}
CRoom::CRoom()
	: m_RoomNumber(-1), m_CurUserCount(0), m_bRoomActive(true)
	, m_RoomGameScene(nullptr), m_GameWorld(nullptr)
	, m_pGameRule(nullptr), m_pRoomMaster(nullptr)
	, m_fRoomDeltaTime(0.f), m_fAccSyncroTime(SYNCROTIME)
	, m_bGamePlaying(false)
{
	m_mapRoomUser.clear();
	m_RoomPacketQueue.Init();
	m_UserEnd =  m_mapRoomUser.end();
}


CRoom::~CRoom()
{

}

void CRoom::PushRecvData(const PlayerData& _Data)
{
	m_RecvStart = m_listRecvPlayerData.begin();
	m_RecvEnd = m_listRecvPlayerData.end();

	for (; m_RecvStart!= m_RecvEnd; ++m_RecvStart)
	{
		if (m_RecvStart->PlayerID == _Data.PlayerID && _Data.Count > m_RecvStart->Count) 
		{
			m_RecvStart->PlayerState = _Data.PlayerState;
			m_RecvStart->Pos = _Data.Pos;
			m_RecvStart->Rot = _Data.Rot;
			return;
		}
	}

	m_listRecvPlayerData.push_back(_Data);
}
const unsigned __int64 CRoom::GetPlayerID(CServerUser* _pUser)
{
	m_FindUser = m_mapRoomUser.find(_pUser);
	if (m_FindUser == m_UserEnd) 
	{
		TASSERT(true);
		return 0;
	}

	return m_FindUser->second->GetPlayerID();
}
const ActiveRoomData CRoom::GetRoomData()
{
	ActiveRoomData Data;
	ZeroMemory(&Data, sizeof(ActiveRoomData));

	Data.RoomNumber = m_RoomNumber;
	Data.RoomUserCount = (unsigned int)m_listRoomUser.Size();
	Data.bGamePlaying = m_bGamePlaying;

	unsigned int Count = 1;
	for (std::unordered_map<CServerUser*,SPTR<CPlayer>>::value_type User : m_mapRoomUser)
	{
		if (User.second->GetPlayerRole() == PLAYER_KILLER) 
		{
			Data.LobbyUserID[0] = User.second->GetPlayerID();
		}
		else 
		{
			Data.LobbyUserID[Count++] = User.second->GetPlayerID();
		}
	}
	
	/*
	std::list<CServerUser*>::iterator Start = m_listRoomUser.m_List.begin();
	std::list<CServerUser*>::iterator End = m_listRoomUser.m_List.end();
	
	for (; Start != End; ++Start)
	{
		if (m_mapRoomUser.find(*Start)->second->GetPlayerRole() == PLAYER_KILLER) 
		{
			Data.LobbyUserID[0] = m_mapRoomUser.find(*Start)->second->GetPlayerID();
		}
		else 
		{
			Data.LobbyUserID[Count++] = m_mapRoomUser.find(*Start)->second->GetPlayerID();
		}
	}
	*/

	return Data;
}
void CRoom::Update(const float& _DeltaTime)
{
	m_fRoomDeltaTime += _DeltaTime;

	if (m_bGamePlaying) 
	{
		if (m_fRoomDeltaTime >= GAMEUPDATETIME)
		{
			GameLogicProcessing(m_fRoomDeltaTime);
			m_fRoomDeltaTime = 0.f;
		}

		BroadcastSyncroPacket(_DeltaTime);
	}
	else 
	{
		GameLobbyProcessing(_DeltaTime);
	}

}

void CRoom::TestRoomUpdate(const float& _DeltaTime)
{
	m_fRoomDeltaTime += _DeltaTime;

	if (m_fRoomDeltaTime >= GAMEUPDATETIME) 
	{
		GameLogicProcessing(m_fRoomDeltaTime);
		m_fRoomDeltaTime = 0.f;
	}


	BroadcastSyncroPacket(_DeltaTime);
}

void CRoom::PacketProcessing(const float& _DeltaTime)
{
	while (!m_RoomPacketQueue.Empty())
	{
		PACKET Pack;
		m_RoomPacketQueue.Pop(&Pack);

		if (Pack.pUser->m_RoomNumber != m_RoomNumber)
			continue;

		if (nullptr != PacketProcessing::PacketProcFuntion[Pack.Header.HeaderFlag]) 
		{
			PacketProcessing::PacketProcFuntion[Pack.Header.HeaderFlag](Pack);
		}
		
		if (nullptr != RoomPackFunc[Pack.Header.HeaderFlag]) 
		{
			RoomPackFunc[Pack.Header.HeaderFlag](this, Pack, _DeltaTime);
		}
	}

	if (true != m_listRecvPlayerData.empty()) 
	{
		BroadcastRecvPacket(_DeltaTime);
	}
}
void CRoom::GameLobbyProcessing(const float& _DeltaTime)
{

}
void CRoom::GameLogicProcessing(const float& _DeltaTime)
{
	m_RoomGameScene->Update(_DeltaTime);
	m_RoomGameScene->Collision3D();
	BroadcastServerCollisionEvent();
	m_RoomGameScene->LastUpdate(_DeltaTime);
	m_RoomGameScene->ReduceRelease();

}
void CRoom::BroadcastRecvPacket(const float& _DeltaTime)
{
	WorldData BroadData;

	m_UserStart = m_mapRoomUser.begin();
	BroadData.Header.CurPlayerCount = (unsigned int)m_listRecvPlayerData.size();

	m_RecvStart =  m_listRecvPlayerData.begin();
	m_RecvEnd = m_listRecvPlayerData.end();

	int Idx = 0;
	for (;m_RecvStart != m_RecvEnd ; ++m_RecvStart)
	{
		BroadData.arrPlayerData[Idx] = (*m_RecvStart);
	}


	// 3. 현재 접속중인 모든 플레이어들에게 전송
	int PackSize = sizeof(PlayerData) * BroadData.Header.CurPlayerCount + sizeof(WorldDataHeader);
	BroadData.Header.DataSize = PackSize;

	m_UserStart = m_mapRoomUser.begin();

	for (; m_UserStart != m_UserEnd; ++m_UserStart)
	{
		CServerUser* pUser = m_UserStart->first;
		SPTR<CPlayer> pPlayer = m_UserStart->second;

		if (m_listRecvPlayerData.size() == 1 && m_listRecvPlayerData.front().PlayerID == pPlayer->GetPlayerID())
			continue;

		BroadData.Header.LocalPlayerID = pPlayer->m_PlayerID;
		
		// 기존 유저들에게 패킷 전송
		if (nullptr != pUser && true == pPlayer->m_bBroadSync) 
		{
			pUser->SendData(&BroadData, PackSize, FLAG_GAME_PLAYING, FLAG_DATA_BROADCATE_REMOTE);
		}
	}

	m_listRecvPlayerData.clear();
}

const bool CRoom::BroadcastSyncroPacket(const float& _DeltaTime)
{
	if (m_CurUserCount <= 0)
		return false;

	if (m_fAccSyncroTime < SYNCROTIME)
	{
		m_fAccSyncroTime += _DeltaTime;
		return false;
	}

	m_fAccSyncroTime = 0.f;

	// 1. 기존 유저들의 상태들을 패킷으로 준비한다.
	WorldData BroadData;

	m_UserStart = m_mapRoomUser.begin();
	BroadData.Header.CurPlayerCount = 0;

	int Idx = 0;

	for (; m_UserStart != m_UserEnd; ++m_UserStart, ++Idx)
	{
		BroadData.arrPlayerData[Idx] = m_UserStart->second->GetPlayerData();
		++BroadData.Header.CurPlayerCount;
	}

	// 2. 입장한 유저가 있는경우 
	// 유저의 플레이어를 씬에 배치하고 기존 유저들에게 새로운 유저가 들어왔음을 통지한다.
	// 추후 GameLogicProcessing 이전에 입장한 유저를 생성해야 할 수도 있다.
	while (false == m_quEnterData.Empty())
	{
		RoomEnterData Data = m_quEnterData.Pop();
		CPlayer* NewPlayer =  CreatePlayer(Data);
		
		Vec3 SpwanPos;

		//if (NewPlayer->m_eRoleType == PLAYER_KILLER)
		//{
		//	SpwanPos = KillerInitPos;
		//
		//}
		//else 
		//{
		//	SpwanPos = SurvivorInitPos[SSDRandom::RandomInt(0, 3)];
		//}

		SpwanPos = Vec3(SSDRandom::RandomFloat(3100.f, 3300.f), 0.f, SSDRandom::RandomFloat(2500.f, 2900.f));

		NewPlayer->TRANS->SetPos(SpwanPos);
		m_GameWorld->m_pTerrain->PushNavigator(NewPlayer->GetPlayerNavigator());
		BroadData.arrPlayerData[Idx] = NewPlayer->GetPlayerData();
		++BroadData.Header.CurPlayerCount;
	}

	// 3. 데이터를 요청한 유저들에게 접속중인 유저들의 데이터를 전송한다.
	int PackSize = sizeof(PlayerData) * BroadData.Header.CurPlayerCount + sizeof(WorldDataHeader);
	BroadData.Header.DataSize = PackSize;
	
	m_UserStart = m_mapRoomUser.begin();

	for (; m_UserStart != m_UserEnd; ++m_UserStart)
	{
		CServerUser* pUser = m_UserStart->first;
		SPTR<CPlayer> pPlayer = m_UserStart->second;
		
		BroadData.Header.LocalPlayerID = pPlayer->m_PlayerID;
		if (false == pPlayer->m_bEnter)
		{
			// 기존 유저들에게 패킷 전송
			if (nullptr != pUser && true == pPlayer->m_bBroadSync)
				pUser->SendData(&BroadData, PackSize, FLAG_GAME_PLAYING, FLAG_DATA_SYNCRO_WORLD);
		}
		else 
		{
			// 새로 들어온 유저들에게 패킷 전송
			if (nullptr != pUser )
			{
				pUser->SendData(&BroadData, PackSize, FLAG_ENTER_ROOM_SUCCESS, FLAG_DATA_ROOM_WORLD);
				pPlayer->m_bEnter = false;
			}
		}
	}

	return true;
}

void CRoom::SendPacketToSyncPlayer(const float& _DeltaTime)
{
	if (m_CurUserCount <= 0)
		return;

	if (true == m_quBroadUser.empty() && true == m_quEnterData.Empty())
		return;


	// 1. 기존 유저들의 상태들을 패킷으로 준비한다.
	WorldData BroadData;

	m_UserStart = m_mapRoomUser.begin();
	BroadData.Header.CurPlayerCount = 0;

	int Idx = 0;

	for (; m_UserStart != m_UserEnd; ++m_UserStart, ++Idx)
	{
		BroadData.arrPlayerData[Idx].PlayerID = m_UserStart->second->m_PlayerID;
		BroadData.arrPlayerData[Idx].Pos = m_UserStart->second->GetWorldPos();
		BroadData.arrPlayerData[Idx].PlayerRole = m_UserStart->second->m_eRoleType;
		++BroadData.Header.CurPlayerCount;
	}

	// 2. 입장한 유저가 있는경우 
	// 유저의 플레이어를 씬에 배치하고 기존 유저들에게 새로운 유저가 들어왔음을 통지한다.
	// 추후 GameLogicProcessing 이전에 입장한 유저를 생성해야 할 수도 있다.
	while (false == m_quEnterData.Empty())
	{
		RoomEnterData Data = m_quEnterData.Pop();
		CPlayer* NewPlayer = CreatePlayer(Data);
		NewPlayer->m_bBroadSync = true;
		m_quBroadUser.push(NewPlayer);
		Vec3 SpwanPos = Vec3(SSDRandom::RandomFloat(2900.f, 3500.f), 0.f, SSDRandom::RandomFloat(2900.f, 3500.f));
		NewPlayer->TRANS->SetPos(SpwanPos);
		BroadData.arrPlayerData[BroadData.Header.CurPlayerCount].Pos = NewPlayer->GetWorldPos();
		BroadData.arrPlayerData[BroadData.Header.CurPlayerCount].PlayerID = NewPlayer->m_PlayerID;
		BroadData.arrPlayerData[BroadData.Header.CurPlayerCount].PlayerRole = NewPlayer->m_eRoleType;
		++BroadData.Header.CurPlayerCount;
	}

	// 3. 데이터를 요청한 유저들에게 접속중인 유저들의 데이터를 전송한다.
	int PackSize = sizeof(PlayerData) * BroadData.Header.CurPlayerCount + sizeof(WorldDataHeader);
	BroadData.Header.DataSize = PackSize;

	while (false == m_quBroadUser.empty())
	{
		SPTR<CPlayer> BroadPlayer = m_quBroadUser.front();
		m_quBroadUser.pop();

		BroadData.Header.LocalPlayerID = BroadPlayer->m_PlayerID;
		if (false == BroadPlayer->m_bEnter)
		{
			// 기존 유저들에게 패킷 전송

			if (nullptr != BroadPlayer->m_pPairUser)
				BroadPlayer->m_pPairUser->SendData(&BroadData, PackSize, FLAG_GAME_PLAYING, FLAG_DATA_ROOM_WORLD);
		}
		else
		{
			// 새로 들어온 유저들에게 패킷 전송
			if (nullptr != BroadPlayer->m_pPairUser)
			{
				BroadPlayer->m_pPairUser->SendData(&BroadData, PackSize, FLAG_ENTER_ROOM_SUCCESS, FLAG_DATA_ROOM_WORLD);
				BroadPlayer->m_bEnter = false;
			}
		}

		BroadPlayer->m_bBroadSync = false;
	}
}

void CRoom::BroadcastExitPlayer(SPTR<CPlayer> _ExitPlayer)
{
	if (nullptr == _ExitPlayer)
		return;

	unsigned __int64 ExitPlayerID = _ExitPlayer->GetPlayerID();

	m_UserStart = m_mapRoomUser.begin();

	for (;m_UserStart != m_UserEnd ; ++m_UserStart)
	{
		if (nullptr != m_UserStart->first) 
		{
			CServerUser* pUser = m_UserStart->first;
			if (m_bGamePlaying) 
			{
				pUser->SendData(&ExitPlayerID, sizeof(unsigned __int64), FLAG_GAME_PLAYING, FLAG_GAME_EXIT_USER);
			}
			else 
			{
				pUser->SendData(&ExitPlayerID, sizeof(unsigned __int64), FLAG_LOBBY_WAITING, FLAG_DATA_ROOM_EXIT);
			}
		}
	}
}
const bool CRoom::EnterRoom(CServerUser* _pUser)
{
	if (m_CurUserCount >= MAXROOMUSER)
		return false;

	m_listRoomUser.push_back(_pUser);
	_pUser->m_RoomNumber = m_RoomNumber;
	m_CurUserCount = (unsigned int)m_listRoomUser.Size();

	// 방에 처음 입장한 경우 해당 유저는 반드시 살인자여야 한다.
	RoomEnterData Data;
	
	if ( nullptr == m_pRoomMaster )
	{
		Data.RoleType = PLAYER_KILLER;
		m_pRoomMaster = _pUser;
	}
	else 
	{
		Data.RoleType = PLAYER_SURVIVOR;
	}

	Data.RoomNumber = m_RoomNumber;
	Data.pEnterUser = _pUser;

	 CreatePlayer(Data);

	if (false == m_bRoomActive)
		m_bRoomActive = true;

	return true;
}
const bool CRoom::EnterRoom(const RoomEnterData& _EnterData)
{
	if (m_CurUserCount >= MAXROOMUSER || nullptr == _EnterData.pEnterUser)
		return false;

	if (_EnterData.RoleType == PLAYER_KILLER) 
	{
		m_pRoomMaster = _EnterData.pEnterUser;
	}

	m_listRoomUser.push_back(_EnterData.pEnterUser);
	_EnterData.pEnterUser->m_RoomNumber = m_RoomNumber;
	m_quEnterData.Push(_EnterData);
	
	m_CurUserCount = (unsigned int)m_listRoomUser.Size();

	if (false == m_bRoomActive)
		m_bRoomActive = true;

	return true;
}

const bool CRoom::ExitRoom(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return false;

	if (nullptr == m_listRoomUser.Find(_pUser))
		return false;
	
	if (false == m_bGamePlaying &&  m_pRoomMaster == _pUser) 
	{
		return ClearRoom();
	}

	m_listRoomUser.Erase(_pUser);
	m_FindUser =  m_mapRoomUser.find(_pUser);
	
	m_CurUserCount = (unsigned int)m_listRoomUser.Size();

	if (m_FindUser != m_UserEnd)
	{
		SPTR<CPlayer> P = m_FindUser->second;
		m_pGameRule->DeletePlayer(P);
		P->m_pPairUser = nullptr;
		m_FindUser = m_mapRoomUser.erase(m_FindUser);
		m_UserEnd = m_mapRoomUser.end();
		pRoomServerMgr->PrintLog(LogManager::MakeLogString("Exit Player - Room Number : ", (int)m_RoomNumber
			, " , Player ID : ", P->GetPlayerID()
			, ", Cur Player Count : ", (int)m_CurUserCount));
		BroadcastExitPlayer(P);
	}

	_pUser->m_RoomNumber = -1;

	if (_pUser == m_pRoomMaster)
		m_pRoomMaster = nullptr;

	if (m_CurUserCount == 0 && true == m_bRoomActive)
	{
		m_bRoomActive = false;
		m_bGamePlaying = false;
		m_pGameRule->m_CircleCount = 0;
		m_pGameRule->InitWorldObj();
	}

	return true;
}

const bool CRoom::ClearRoom()
{
	m_FindUser = m_mapRoomUser.find(m_pRoomMaster);
	unsigned __int64 ExitID = m_FindUser->second->GetPlayerID();

	for (std::unordered_map<CServerUser*,SPTR<CPlayer>>::value_type User : m_mapRoomUser)
	{
		User.first->SendData(&ExitID, sizeof(ExitID), FLAG_LOBBY_WAITING, FLAG_DATA_ROOM_EXIT);
		User.first->m_RoomNumber = -1;
		User.second->m_pPairUser = nullptr;
		m_pGameRule->DeletePlayer(User.second);
		m_GameWorld->m_pPairScene->ReduceRelease();
	}
	
	m_mapRoomUser.clear();
	m_listRoomUser.Clear();
	m_UserEnd = m_mapRoomUser.end();

	m_pRoomMaster = nullptr;
	m_CurUserCount = 0;
	m_bRoomActive = false;
	m_bGamePlaying = false;

	pRoomServerMgr->PrintLog(LogManager::MakeLogString("Room #", (int)m_RoomNumber, " : Room Master Exit"));
	return true;
}

const bool CRoom::IsAllUserReady()
{
	for (std::unordered_map<CServerUser*, SPTR<CPlayer>>::value_type User : m_mapRoomUser)
	{
		if (false == User.second->m_bServerReady)
			return false;
	}
	return true;
}

const bool CRoom::IsAllUserBuildComplete()
{
	for (std::unordered_map<CServerUser*, SPTR<CPlayer>>::value_type User : m_mapRoomUser)
	{
		if (false == User.second->m_bBroadSync)
			return false;
	}
	return true;
}

void CRoom::RecvBuildCompete(CServerUser* _pUser)
{
	if (true == m_bGamePlaying)
		return;

	m_FindUser = m_mapRoomUser.find(_pUser);

	if (m_FindUser != m_UserEnd) 
	{
		//m_FindUser->second->m_bBuildComplete = true;
		m_FindUser->second->m_bBroadSync = true;
		if (true == IsAllUserBuildComplete()) 
		{
			m_bGamePlaying = true;
			pRoomServerMgr->PrintLog(LogManager::MakeLogString("Room #", (int)m_RoomNumber, " : Game Start"));
			BroadcateGameStart();
			return;
		}
	}
	
}
void CRoom::BroadcateLobbyEnterUser(CServerUser* _pUser)
{
	// 현재 접속중인 모든 플레이어들에게 새로운 유저가 접속함을 알려준다.
	m_FindUser = m_mapRoomUser.find(_pUser);
	if (m_FindUser == m_mapRoomUser.end())
		return;

	unsigned __int64 EnterID = m_FindUser->second->GetPlayerID();

	std::list<CServerUser*>::iterator Start =   m_listRoomUser.m_List.begin();
	std::list<CServerUser*>::iterator End = m_listRoomUser.m_List.end();


	for (;Start != End;++Start )
	{
		if (_pUser == *Start)
			continue;

		(*Start)->SendData(&EnterID, sizeof(EnterID), FLAG_LOBBY_WAITING, FLAG_DATA_ROOMENTER_USER);
	}
}
void CRoom::BroadcateLobbyReadyUser(CServerUser* _pUser) 
{
	// 현재 접속중인 모든 플레이어들에게 레디 버튼을 눌렀음을 알려준다.
	m_FindUser = m_mapRoomUser.find(_pUser);
	if (m_FindUser == m_mapRoomUser.end())
		return;

	m_FindUser->second->m_bServerReady = true;
	unsigned __int64 ReadyID = m_FindUser->second->GetPlayerID();
	std::list<CServerUser*>::iterator Start = m_listRoomUser.m_List.begin();
	std::list<CServerUser*>::iterator End = m_listRoomUser.m_List.end();

	for (; Start != End; ++Start)
	{
		(*Start)->SendData(&ReadyID, sizeof(ReadyID), FLAG_LOBBY_WAITING, FLAG_DATA_READY);
	}


	if (true == IsAllUserReady()) 
	{
		BroadcateGameWorldData();
	}
}
void CRoom::BroadcateLobbyReadyCancleUser(CServerUser* _pUser)
{
	m_FindUser = m_mapRoomUser.find(_pUser);
	if (m_FindUser == m_mapRoomUser.end())
		return;

	m_FindUser->second->m_bServerReady = false;
	unsigned __int64 CancleID = m_FindUser->second->GetPlayerID();

	// 현재 접속중인 모든 플레이어들에게 해당유저가 준비를 해제되었음을 알려준다.
	std::list<CServerUser*>::iterator Start = m_listRoomUser.m_List.begin();
	std::list<CServerUser*>::iterator End = m_listRoomUser.m_List.end();

	for (; Start != End; ++Start)
	{
		(*Start)->SendData(&CancleID, sizeof(CancleID), FLAG_LOBBY_WAITING, FLAG_DATA_READY_CANCLE);
	}

}
void CRoom::BroadcateLobbyExitUser(CServerUser* _pUser)
{	
	ExitRoom(_pUser);
}
void CRoom::BroadcateGameWorldData()
{
	// 1. 현재 로비에 접속중인 플레이어들의 위치를 초기화 한다.
	std::vector<int> Temp;
	Temp.reserve(4);
	Temp.push_back(0);
	Temp.push_back(1);
	Temp.push_back(2);
	Temp.push_back(3);
	SSDRandom::ShuffleVector(Temp);

	Vec3 SpwanPos; 
	WorldData BroadData;

	m_UserStart = m_mapRoomUser.begin();
	BroadData.Header.CurPlayerCount = 0;

	int Idx = 0;

	for (std::unordered_map<CServerUser*, SPTR<CPlayer>>::value_type P : m_mapRoomUser)
	{
		if (P.second->GetPlayerRole() == PLAYER_SURVIVOR)
		{
			SpwanPos = SurvivorInitPos[Temp.back()];
			Temp.pop_back();
		}
		else
		{
			SpwanPos = KillerInitPos;
		}

		P.second->TRANS->SetPos(SpwanPos);
		m_GameWorld->m_pTerrain->PushNavigator(P.second->GetPlayerNavigator());
		BroadData.arrPlayerData[Idx++] = P.second->GetPlayerData();
		++BroadData.Header.CurPlayerCount;
	}

	// 2. 유저들에게 접속중인 유저들의 데이터를 전송한다.
	int PackSize = sizeof(PlayerData) * BroadData.Header.CurPlayerCount + sizeof(WorldDataHeader);
	BroadData.Header.DataSize = PackSize;

	m_UserStart = m_mapRoomUser.begin();

	for (; m_UserStart != m_UserEnd; ++m_UserStart)
	{
		CServerUser* pUser = m_UserStart->first;
		SPTR<CPlayer> pPlayer = m_UserStart->second;

		BroadData.Header.LocalPlayerID = pPlayer->m_PlayerID;
		pUser->SendData(&BroadData, PackSize, FLAG_LOBBY_WAITING, FLAG_DATA_BUILD_GAMEWORLD);
	}
}

void CRoom::BroadcateWorldObjectData(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	WorldObjData SData;
	ZeroMemory(&SData, sizeof(SData));

	bool LastCheck = false;
	int StartIdx = 0;
	int CurIdx, NextIdx;

	while (false == LastCheck)
	{
		SData.Header.ObjCount = 0;

		for (int i = 0; i < MAXMAGICCIRCLE; ++i)
		{
			CurIdx = StartIdx + i;
			NextIdx = CurIdx + 1;
			SData.arrCircleData[i] = m_pGameRule->m_vecMagicCircle[StartIdx + i]->GetCircleData();
			++SData.Header.ObjCount;
			
			if (NextIdx >= (int)m_pGameRule->m_vecMagicCircle.size()) 
			{
				LastCheck = true;
				SData.Header.LastData = true;
				SData.Header.DataSize = sizeof(WorldObjHeader) + sizeof(MagicCircleData) * SData.Header.ObjCount;
				_pUser->SendData(&SData, SData.Header.DataSize, FLAG_LOBBY_WAITING, FLAG_DATA_WORLD_OBJECT);
				return;
			}
		}

		if (false == LastCheck) 
		{
			SData.Header.DataSize = sizeof(WorldObjHeader) + sizeof(MagicCircleData) * SData.Header.ObjCount;
			_pUser->SendData(&SData, SData.Header.DataSize, FLAG_LOBBY_WAITING, FLAG_DATA_WORLD_OBJECT);
		}
	}
	


}

void CRoom::BroadcateGameStart()
{
	for (std::unordered_map<CServerUser*, SPTR<CPlayer>>::value_type P : m_mapRoomUser)
	{
		P.first->SendData(nullptr, 0, FLAG_GAME_PLAYING, 0);
	}
}
void CRoom::PushPacket(CServerUser* _pUser, const unsigned int& _DataLength)
{
	if (nullptr == _pUser)
		return;
	
	m_RoomPacketQueue.AddData(_pUser->m_RecvBuffer.m_Packet, _DataLength, _pUser);
}
void CRoom::PushPacket(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	m_RoomPacketQueue.PushUserPacket(_pUser);
}

void CRoom::PushPacket(const PACKET& _Pack)
{
	m_RoomPacketQueue.Push(_Pack);
}
void CRoom::CreateGameWorld()
{
	// 기존의 씬이 있는경우
	if (nullptr != m_RoomGameScene)
		return;

	m_RoomGameScene = CResourceMgr<CScene>::Create();
	m_RoomGameScene->SetName(L"RoomGameScene");

	// 서버에서는 카메라를 만들지 않는다.
	//m_RoomGameScene->Init();
	CSceneScript* InitScript = new CGameWorld(m_RoomGameScene);

	m_RoomGameScene->AddSceneScript(InitScript,false);
	m_RoomGameScene->Loading();
	m_GameWorld = (CGameWorld*)InitScript;
	m_pGameRule = m_GameWorld->m_pRule;

}
CPlayer* CRoom::CreatePlayer(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return nullptr;

	CActor* NewActor = m_GameWorld->CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	CPlayer* NewPlayer =  NewActor->AddCom<CPlayer>(PlayerInitData{ true });

	NewPlayer->m_bEnter = true;
	NewPlayer->m_bBroadSync = false;
	NewPlayer->m_pPairUser = _pUser;
	NewPlayer->SetPlayerID(_pUser);
	NewPlayer->SetRemotePlayer();
	m_mapRoomUser.insert(std::unordered_map<CServerUser*, SPTR<class CPlayer>>::value_type(_pUser, NewPlayer));
	m_UserEnd = m_mapRoomUser.end();
	return NewPlayer;
}
CPlayer* CRoom::CreatePlayer(const RoomEnterData& _EnterData)
{
	if (nullptr == _EnterData.pEnterUser)
		return nullptr;

	CPlayer* NewPlayer = m_pGameRule->CreateServerPlayer(_EnterData);
	m_mapRoomUser.insert(std::unordered_map<CServerUser*, SPTR<class CPlayer>>::value_type(_EnterData.pEnterUser, NewPlayer));

	m_UserEnd = m_mapRoomUser.end();
	pRoomServerMgr->PrintLog(LogManager::MakeLogString("Create Player -  Room Number  : ", (int)m_RoomNumber
		, " , Player ID : ", NewPlayer->GetPlayerID()
		, ", Cur Player Count : ", (int)m_CurUserCount));

	return NewPlayer;
}

void CRoom::BroadcastServerCollisionEvent()
{
	bool Check = false;
	WorldData Data;
	ZeroMemory(&Data, sizeof(Data));

	for (auto P : m_mapRoomUser)
	{
		if (PLAYER_DAMAGED == P.second->m_pPlayerFSM->m_CurPlayerState) 
		{
			Data.arrPlayerData[Data.Header.CurPlayerCount] = P.second->GetPlayerData();
			++Data.Header.CurPlayerCount;
		}
	}

	if (0 == Data.Header.CurPlayerCount)
		return;

	Data.Header.DataSize = sizeof(Data.Header) + sizeof(PlayerData)*Data.Header.CurPlayerCount;

	for (auto P : m_mapRoomUser)
	{
		P.first->SendData(&Data, Data.Header.DataSize, FLAG_GAME_PLAYING, FLAG_DATA_COLLISION);
	}

}