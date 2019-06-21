#include "Player.h"
#include <Actor.h>
#include <OBBCollider.h>
#include <TransForm.h>
#include <SSDServerCore.h>
#include "PlayerFSM.h"
#include <Scene.h>
#include "TraceScript.h"
#include <Gizmo.h>
#include <Navigator.h>
#include <FrustumCollider.h>
#include <SSDCore.h>
#include <SphereCollider.h>
#include <DepthFieldFilter.h>
#include <FogFilter.h>
#include "DeadByDaylightGlobal.h"
#include "DBDRule.h"
#include "Hook.h"
#include <Light.h>
#include <functional>
#include <SSDRandom.h>


bool Player_Compare(std::tuple<float, CPlayer*>& _Left, std::tuple<float, CPlayer*>& _Right)
{
	return std::get<0>(_Left) < std::get<0>(_Right);
}

bool Circle_Compare(std::tuple<float,SPTR<CMagicCircle>>& _Left, std::tuple<float, SPTR<CMagicCircle>>& _Right)
{
	return std::get<0>(_Left) < std::get<0>(_Right);
}

CPlayer::CPlayer()
	:m_eNetType(MAX_PLAYER_NETWORK_TYPE), m_PlayerObstacleCollider(nullptr)
	, m_eRoleType(MAX_PLAYER_ROLE_TYPE), m_pPlayerFSM(nullptr)
	, m_bBroadSync(false), m_bEnter(false), m_pPairUser(nullptr)
	, m_bBroadState(false), m_PlayerCamera(nullptr)
	, m_PlayerCamDummy(nullptr), m_PlayerNavigator(nullptr)
	, m_CameraFrustumCollider(nullptr),m_CameraPivot(Vec3())
	, m_MaxCamDist(0.f), m_PlayerBodyCollider(nullptr)
	, m_PlayerAttackCollider(nullptr), m_LifeCount(0), m_DeathCount(0)
	, m_FocusGenMagicCircle(nullptr), m_FocusTrapMagicCircle(nullptr)
	, m_PlayerDetectedCollider(nullptr), m_HealingCount(0.f)
	, m_bHealing(false), m_PlayerBoundaryCollider(nullptr)
	, m_HookCount(0.f), m_bRescued(false), m_PlayerCamActor(nullptr)
	, m_pRule(nullptr), m_bSyncro(false), m_bBuildComplete(false)
	, m_PlayerLightActor(nullptr), m_PlayerLight(nullptr), m_PlayerFootPrintDummy(nullptr)
	, m_eFocusing(FOCUSING_NONE), m_bServerFlag(false), m_SummonsCount(0.f), m_SurvivorID(-1)
	, m_bGamePlayEnd(false)
{
}


CPlayer::~CPlayer()
{
	m_pPairUser = nullptr;
}

void CPlayer::SetPlayerID(class CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	m_PlayerID = (unsigned __int64)std::hash<CServerUser*>()(_pUser);

}

void CPlayer::SetRule(CDBDRule* _pRule)
{
	if (nullptr == _pRule)
		return;

	m_pRule = _pRule;
}

void CPlayer::SetLocalPlayer()
{
	m_eNetType = LOCAL_PLAYER;
	
	m_PlayerCamDummy = CreateActor(UPDATE_LAYER_NORMAL, L"CameraDummy");

	m_PlayerCamActor = CreateActor(UPDATE_LAYER_NORMAL, L"PlayerCamera");
	m_PlayerCamera = m_PlayerCamActor->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_OBJECT, 45.f ,1.f, 10000.f });
	m_PlayerCamera->SettingViewLayer(RENDER_LAYER_MAP, RENDER_LAYER_OBJECT, RENDER_LAYER_BACKEFFECT, RENDER_LAYER_PLAYER, RENDER_LAYER_FRONTEFFECT, RENDER_LAYER_FIRST, RENDER_LAYER_ALPHA);
	m_PlayerCamera->SetPerspectiveMode();
	m_PlayerCamera->TRANS->SetRotate(Vec3(13.f, 180.f, 0.f));
	m_PlayerCamDummy->SetChild(m_PlayerCamera->ACTOR);
	m_PlayerCamera->AddFilter<CFogFilter>(m_PlayerCamera);
	m_PlayerCamera->AddFilter<CDepthFieldFilter>();

	////발자국 더미
	//m_PlayerFootPrintDummy = CreateActor(UPDATE_LAYER_NORMAL, L"FootPrintDummy");

	InitPlayerCamera();
	AddPlayerCameraCollider();

	SCENE->RegisteredMainCamera(GetPlayerCamera());
	SCENE->ChangeMainCamera(GetPlayerCamera());

}
void CPlayer::SetRemotePlayer()
{
	m_eNetType = REMOTE_PLAYER;
}


const bool CPlayer::Init()
{
	m_PlayerNavigator = ACTOR->AddCom<CNavigator>();
	CScript::CreateFSM();
	return true;
}
const bool CPlayer::Init(const PlayerInitData& _InitData)
{
	CPlayer::Init();

	// 서버 상의 모든 플레이어는 리모트 플레이어로 취급
	if (true == _InitData.ServerPlayer) 
	{
		m_PlayerObstacleCollider = ACTOR->AddCom<COBBCollider>(COL3D_DATA{ COLDATA{ L"Player", COLLISION_3D },false });
		m_PlayerBodyCollider = ACTOR->AddCom<COBBCollider>(COL3D_DATA{ COLDATA{ L"PlayerBody", COLLISION_3D },false });
		m_PlayerBoundaryCollider = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"Default", COLLISION_3D },false });

		m_PlayerObstacleCollider->AddIgnoreCollider(m_PlayerBoundaryCollider);

		//m_PlayerCamDummy = CreateActor(UPDATE_LAYER_NORMAL, L"CameraDummy");

	}
	else 
	{
		m_PlayerObstacleCollider = ACTOR->AddCom<COBBCollider>( COLDATA{ L"Player", COLLISION_3D } );
		m_PlayerBodyCollider = ACTOR->AddCom<COBBCollider>(COLDATA{ L"PlayerBody", COLLISION_3D } );
		m_PlayerBoundaryCollider = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"Default", COLLISION_3D } );

		m_PlayerObstacleCollider->AddIgnoreCollider(m_PlayerBoundaryCollider);

		//DBD::pGameRule->PushPlayer(this);

		//	//m_PlayerCamera->AddFilter<CDepthFieldFilter>();
		//	//m_PlayerCamera->AddFilter<CFogFilter>(m_PlayerCamera);
		//m_PlayerCamera->AddFilter<CFogFilter>(m_PlayerCamera);
		//m_PlayerCamera->AddFilter<CDepthFieldFilter>();

		//	m_PlayerCamDummy = CreateActor(UPDATE_LAYER_NORMAL, L"CameraDummy");
		//	m_PlayerCamDummy->SetChild(m_PlayerCamera->ACTOR);
		//}


		
		//m_PlayerCamera->TRANS->SetPos(CAMERAPIVOT);
		//AddPlayerCameraCollider();
		//m_PlayerCamera->ACTOR->AddCom<CGizmo>(RENDATA{ RENDER_LAYER_DEBUG });
		//m_PlayerCamera->TRANS->SetRotate(Vec3(-13.f, 0.f, 0.f));
		//m_PlayerCamera->ChangeViewAxisInvZ();
		//ACTOR->AddCom<CGizmo>(RENDATA{ RENDER_LAYER_DEBUG });

	}

	return CScript::Init();
}


void CPlayer::Update(const float& _DeltaTime)
{
	if (m_eNetType == LOCAL_PLAYER && Core::IsClientFocus())   
	{
		m_pPlayerFSM->CameraVisionUpdate(_DeltaTime);
		m_PlayerCamDummy->SetPos(TRANS->GetWorldPos() + DUMMYPIVOT);
	}

	CScript::Update(_DeltaTime);
}
void CPlayer::LastUpdate(const float& _DeltaTime)
{
	m_bSyncro = false;

	// 서버쪽의 플레이어의 경우 카메라 피봇을 조정하는 로직을 수행하지않는다.
	if (nullptr == m_PlayerCamera)
		return;

	UpdateCameraPivot();
}

void CPlayer::InitializePlayer(const PlayerData& _Data)
{
	SetPlayerID(_Data.PlayerID);
	TRANS->SetPos(_Data.Pos);
	TRANS->SetRotate(_Data.Rot);
}

const bool CPlayer::UpdatePlayer(const PlayerData& _Data)
{
	if (nullptr != m_pPlayerFSM)
		return m_pPlayerFSM->UpdatePlayerData(_Data);

	return false;
}
void CPlayer::SyncroPlayer(const PlayerData& _Data)
{
	if (nullptr != m_pPlayerFSM) 
	{
		m_pPlayerFSM->SynchronizationPlayer(_Data);
	}
}

void CPlayer::SetPos(const Vec3& _Pos)
{
	TRANS->SetPos(_Pos);
}
const Vec3 CPlayer::GetWorldPos()
{
	return TRANS->GetWorldPos();
}
const Vec3 CPlayer::GetPrevPos()
{
	if (nullptr == m_PlayerNavigator)
	{
		TASSERT(true);
		return Vec3();
	}

	return m_PlayerNavigator->GetNaviPrevPos();
}
const PlayerData CPlayer::GetPlayerData()
{
	PlayerData StateData;
	StateData.PlayerID = m_PlayerID;
	StateData.PlayerRole = m_eRoleType;
	StateData.PlayerState = (int)m_pPlayerFSM->m_CurPlayerState;
	StateData.Pos = TRANS->GetWorldPos();
	StateData.Rot = TRANS->GetRotate();
	return StateData;
}

SPTR<CCamera> CPlayer::GetPlayerCamera()
{
	return m_PlayerCamera;
}
SPTR<CNavigator> CPlayer::GetPlayerNavigator()
{
	return m_PlayerNavigator;
}

const Vec3 CPlayer::GetPlayerCamDir() 
{ 
	return m_pPlayerFSM->m_CurCamDir;
}
const Vec3 CPlayer::GetPlayerDir()
{ 
	return TRANS->GetUp(); 
}

const Vec3 CPlayer::GetCamDummyRot()
{
	return m_PlayerCamDummy->TRANS->GetRotate();
}

void CPlayer::PlayerActorOn()
{
	ACTOR->On();
	m_PlayerCamDummy->On();
}
void CPlayer::PlayerActorOff()
{
	ACTOR->Off();
	m_PlayerCamDummy->Off();
}
const bool CPlayer::IsPlayerActive()
{
	return ACTOR->IsActive();
}
void CPlayer::InitPlayerCamera()
{
	if (m_eNetType == REMOTE_PLAYER)
		return;


	if (nullptr == m_PlayerCamActor)
		return;

	m_PlayerCamActor->TRANS->SetPos(m_CameraPivot);
}
void CPlayer::AddPlayerCameraCollider()
{
	if (nullptr != m_PlayerCamera) 
	{
		m_CameraFrustumCollider = m_PlayerCamera->ACTOR->AddCom<CFrustumCollider>(COLDATA{ L"Player",COLLISION_3D });
		m_CameraFrustumCollider->SetFrustumFi(m_PlayerCamera, CAMFRUSTUMANGLE, 1.f, CAMFRUSTUMNEAR, m_MaxCamDist);
		m_CameraFrustumCollider->SetEnterCallBack(m_pPlayerFSM, &CPlayerFSM::CamObstacleCollsionEvent);
	}
}

void CPlayer::ChangePlayerCamFrustum(const float& _Near, const float& _Far)
{
	m_CameraFrustumCollider->SetFrustumFi(m_PlayerCamera, CAMFRUSTUMANGLE, 1.f, _Near, _Far);
}

void CPlayer::PlayerCameraCollisionEvent(SPTR<CCollider> _Col)
{
	for (SPTR<CCollider> Col : m_listCamCol)
	{
		if (Col == _Col)
			return;
	}

	m_listCamCol.push_back(_Col);

}
void CPlayer::UpdateCameraPivot()
{
	if (true == m_listCamCol.empty())
	{
		return;
	}

	m_PlayerCamera->TRANS->SetPos(m_CameraPivot);
	m_CameraFrustumCollider->SetFrustumFi(m_PlayerCamera, CAMFRUSTUMANGLE, 1.f, CAMFRUSTUMNEAR, m_MaxCamDist);

	std::list<SPTR<CCollider>>::iterator Start = m_listCamCol.begin();
	std::list<SPTR<CCollider>>::iterator End = m_listCamCol.end();
	
	for (; Start !=End ; )
	{
		if (false == m_CameraFrustumCollider->CollisionCheck(*Start))
		{
			Start = m_listCamCol.erase(Start);
		}
		else 
		{
			++Start;
		}
	}

	if (true == m_listCamCol.empty())
	{
		return;
	}

	Start = m_listCamCol.begin();
	End = m_listCamCol.end();

	float DistFactor = 0.1f;
	bool Check = false;

	while (DistFactor < 1.f)
	{
		m_PlayerCamera->TRANS->SetPos(m_CameraPivot*DistFactor);
		m_CameraFrustumCollider->SetFrustumFi(m_PlayerCamera, CAMFRUSTUMANGLE, 1.f, CAMFRUSTUMNEAR, m_MaxCamDist*DistFactor);

		for (;Start != End ;++Start)
		{
			if (true == m_CameraFrustumCollider->CollisionCheck(*Start)) 
			{
				Check = true;
				break;
			}
		}

		if (true == Check) 
		{
			DistFactor -= 0.01f;
			m_PlayerCamera->TRANS->SetPos(m_CameraPivot*DistFactor);
			m_CameraFrustumCollider->SetFrustumFi(m_PlayerCamera, CAMFRUSTUMANGLE, 1.f, CAMFRUSTUMNEAR, m_MaxCamDist*DistFactor);
			break;	
		}

		DistFactor += 0.01f;
		Start = m_listCamCol.begin();
	}

	
}
void CPlayer::SetHookCount(const float& _Count)
{
	if (m_HookCount < HALFHOOKCOUNT && _Count >= HALFHOOKCOUNT)
		m_pPlayerFSM->HalfHookEvent();

	m_HookCount = _Count;
}
SPTR<COBBCollider> CPlayer::GetPlayerObstacleCollider()
{
	return m_PlayerObstacleCollider;
}

SPTR<COBBCollider> CPlayer::GetPlayerBodyCollider()
{
	return m_PlayerBodyCollider;
}
SPTR<COBBCollider> CPlayer::GetPlayerAttackCollider()
{
	return m_PlayerAttackCollider;
}
SPTR<CSphereCollider> CPlayer::GetPlayerDetectedCollider()
{
	return m_PlayerDetectedCollider;
}
SPTR<CSphereCollider> CPlayer::GetPlayerBoundaryCollider()
{
	return  m_PlayerBoundaryCollider;
}
SPTR<COBBCollider> CPlayer::CreatePlayerAttackCollider(const bool& _Debug)
{
	m_PlayerAttackCollider = ACTOR->AddCom<COBBCollider>(COL3D_DATA{ COLDATA{ L"Attack", COLLISION_3D } ,_Debug});
	return m_PlayerAttackCollider;
}

SPTR<CSphereCollider> CPlayer::CreatePlayerDetectedCollider(const bool& _Debug )
{
	m_PlayerDetectedCollider = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"PlayerDetect", COLLISION_3D }, _Debug});
	m_PlayerDetectedCollider->SetDebugColor(Vec4::PastelBlue.vec3);
	m_PlayerObstacleCollider->AddIgnoreCollider(m_PlayerDetectedCollider);

	return m_PlayerDetectedCollider;
}
CActor* CPlayer::CreateFootPrintDummy()
{
	//발자국 더미
	m_PlayerFootPrintDummy = CreateActor(UPDATE_LAYER_NORMAL, L"FootPrintDummy");
	return m_PlayerFootPrintDummy;
}
void CPlayer::CreatePlayerEyeLight()
{
	if (PLAYER_ROLE_TYPE::PLAYER_SURVIVOR == m_eRoleType)
		return;

	//살인자 안광
	m_PlayerLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"PlayerLight");
	m_PlayerLightActor->TRANS->SetPos(Vec3(0.f, 70.f, 300.f));
	m_PlayerLightActor->TRANS->SetRotate(Vec3(40.f, 0.f, 180.f));
	ACTOR->SetChild(m_PlayerLightActor);

	m_PlayerLight = m_PlayerLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, SPOT_LIGHT, true, L"Cone_Lv360" });
	m_PlayerLight->SetLightSize(Vec3(900.f, 1000.f, 900.f));
	m_PlayerLight->SetLightColor(Vec4::Red);
	m_PlayerLight->SetLightPower(0.8f);
	m_PlayerLight->SetSpecPower(200.f);
	m_PlayerLight->SetLightAtenuation(Vec4(1.0f, 0.4f, 0.f, 0.f));
	m_PlayerLight->SetAmbiFactor(0.f);
}
void CPlayer::PushDetectMagicCircle(SPTR<CMagicCircle> _MC)
{
	if (nullptr == _MC)
	{
		return;
	}

	std::list<std::tuple<float, SPTR<CMagicCircle>>>::iterator Start = m_listDetectMagicCircle.begin();
	std::list<std::tuple<float, SPTR<CMagicCircle>>>::iterator End = m_listDetectMagicCircle.end();

	for (;Start != End ;++Start)
	{
		if ( std::get<1>(*Start) == _MC)
		{
			return;
		}
	}

	m_listDetectMagicCircle.push_back(std::make_tuple(0.f, _MC));

}
void CPlayer::PopDetectMagicCircle(SPTR<CMagicCircle> _MC)
{
	if (nullptr == _MC)
	{
		return;
	}

	std::list<std::tuple<float, SPTR<CMagicCircle>>>::iterator Start = m_listDetectMagicCircle.begin();
	std::list<std::tuple<float, SPTR<CMagicCircle>>>::iterator End = m_listDetectMagicCircle.end();

	for (; Start != End; )
	{
		if (std::get<1>(*Start) == _MC)
		{
			Start = m_listDetectMagicCircle.erase(Start);
		}
		else 
		{
			++Start;
		}
	}

}
void CPlayer::FocusingGenerator(SPTR<CMagicCircle> _MC)
{
	if (nullptr == _MC) 
	{
		return;
	}

	m_FocusGenMagicCircle = _MC;
}
void CPlayer::FocusingTrap(SPTR<CMagicCircle> _MC) 
{
	if (nullptr == _MC)
	{
		return;
	}

	m_FocusTrapMagicCircle = _MC;
}
void CPlayer::FocusingEndGenerator() 
{
	m_FocusGenMagicCircle = nullptr;
}
void CPlayer::FocusingEndTrap() 
{
	m_FocusTrapMagicCircle = nullptr;
}
const bool CPlayer::IsFocusingGenerator() 
{
	return nullptr != m_FocusGenMagicCircle;
}
const bool CPlayer::IsFocusingTrap()
{
	return nullptr != m_FocusTrapMagicCircle;
}
void CPlayer::PlayerDamagedEvent()
{
	m_pPlayerFSM->ChangePlayerState(PLAYER_DAMAGED);
}			
void CPlayer::AddDeathCount()
{
	++m_DeathCount;

	switch (m_DeathCount)
	{
	case 1:
		m_HookCount = 0.f;
		break;
	case 2:
		m_HookCount = 29.8f;
		break;
	case 3:
		m_HookCount = 59.8f;
		break;
	default:
		break;
	}
}
void CPlayer::SetDeathCount(const unsigned int& _Count)
{
	m_DeathCount = _Count;

	switch (m_DeathCount)
	{
	case 1:
		m_HookCount = 0.f;
		break;
	case 2:
		m_HookCount = 29.8f;
		break;
	case 3:
		m_HookCount = 59.8f;
		break;
	default:
		break;
	}
}
void CPlayer::PushFocusingPlayer(SPTR<CPlayer> _FocusPlayer) 
{
	if (nullptr == _FocusPlayer)
		return;
	
	std::list<std::tuple<float,CPlayer*>>::iterator Start = m_listFocusPlayer.begin();
	std::list<std::tuple<float,CPlayer*>>::iterator End = m_listFocusPlayer.end();

	for (; Start != End; ++Start)
	{
		if (std::get<1>(*Start) == _FocusPlayer)
		{
			return;
		}
	}

	m_listFocusPlayer.push_back(std::make_tuple(0.f, _FocusPlayer.PTR));
}
void CPlayer::PopFocusingPlayer(SPTR<CPlayer> _FocusPlayer)
{
	std::list<std::tuple<float, CPlayer*>>::iterator Start = m_listFocusPlayer.begin();
	std::list<std::tuple<float, CPlayer*>>::iterator End = m_listFocusPlayer.end();

	for (; Start != End; )
	{
		if (std::get<1>(*Start) == _FocusPlayer) 
		{
			m_listFocusPlayer.erase(Start);
			return;
		}
		else 
		{
			++Start;
		}
	}
}
const bool CPlayer::IsTreatable()
{
	if (m_eRoleType == PLAYER_KILLER)
		return false;

	if (m_LifeCount >= MAXSURVIVORLIFE  || m_LifeCount<= 0)
	{
		return false;
	}

	return true;
}
const bool CPlayer::IsIdle()
{
	return PLAYER_IDLE == m_pPlayerFSM->m_CurPlayerState;
}

const bool CPlayer::IsLifeZero()
{
	return m_LifeCount <= 0;
}
void CPlayer::PlayerHealing(const float& _HealingCount)
{
	if (false == m_bHealing)
		return;
		
	m_HealingCount += _HealingCount;

	if (m_HealingCount >= MAXHEALINGCOUNT) 
	{
		m_LifeCount = (UINT)DXMath::Clamp((int)m_LifeCount + 1, 0, MAXSURVIVORLIFE);
		m_HealingCount = 0.f;
		m_bHealing = false;
		m_pPlayerFSM->ChangeIdleEvent();
	}
}

void CPlayer::AddHookCount(const float& _HookCount)
{
	m_HookCount += _HookCount;

}
const bool CPlayer::IsBeforeExecution()
{
	if (nullptr == m_FocusPlayer)
		return false;
	
	return m_FocusPlayer->m_pPlayerFSM->m_CurPlayerState == PLAYER_HOOK;
}
const bool CPlayer::IsGroggy()
{
	return m_pPlayerFSM->m_CurPlayerState == PLAYER_GROGGY;
}
const bool CPlayer::IsHook()
{
	return m_pPlayerFSM->m_CurPlayerState == PLAYER_HOOK;
}
const bool CPlayer::SurvivorFocusingUpdate()
{
	SortingDetectObj();
	
	switch (m_eFocusing)
	{
	case FOCUSING_HEALING_PLAYER:
	{
		if ( false == ExistPlayerList(m_FocusPlayer)
			|| false == m_pPlayerFSM->CheckLookAtObject(m_FocusPlayer->TRANS->GetWorldPos())
			|| m_FocusPlayer->IsTreatable())
		{
			m_FocusPlayer = nullptr;
		}
		else 
		{
			return false;
		}
	}
		break;
	case FOCUSING_RESCUE_PLAYER:
	{
		if (false == ExistPlayerList(m_FocusPlayer)
			|| false == m_pPlayerFSM->CheckLookAtObject(m_FocusPlayer->TRANS->GetWorldPos())
			|| false == m_FocusPlayer->IsHook())
		{
			m_FocusPlayer = nullptr;
		}
		else
		{
			return false;
		}
	}
	break;
	case FOCUSING_GENERATOR: 
	{
		if (false == ExistMagicCircle(m_FocusGenMagicCircle)
			|| false == m_pPlayerFSM->CheckLookAtObject(m_FocusGenMagicCircle->TRANS->GetWorldPos())
			|| true == m_FocusGenMagicCircle->CheckRepairComplete()	)
		{
			m_FocusGenMagicCircle = nullptr;
		}
		else
		{
			return false;
		}
	}
		break;
	case FOCUSING_TRAP: 
	{
		if (false == ExistMagicCircle(m_FocusTrapMagicCircle) || 
			false == m_FocusTrapMagicCircle->CheckTrapAvailable())
		{
			m_FocusTrapMagicCircle = nullptr;
		}
		else
		{
			return false;
		}
	}
		break;
	default:
		break;
	}

	return ChangeSurvivorFocusingObj(m_eFocusing);
}
const bool CPlayer::ChangeSurvivorFocusingObj(FOCUSING_OBJ_TYPE _PrevType)
{
	for (std::tuple<float, CPlayer*> P : m_listFocusPlayer)
	{
		if (false == m_pPlayerFSM->CheckLookAtObject(std::get<1>(P)->TRANS->GetWorldPos()))
			continue;

		if (true == std::get<1>(P)->IsTreatable() )
		{
			m_FocusPlayer = std::get<1>(P);
			m_eFocusing = FOCUSING_HEALING_PLAYER;
			return _PrevType != m_eFocusing;
		}
		else if (true == std::get<1>(P)->IsHook()) 
		{
			m_FocusPlayer = std::get<1>(P);
			m_eFocusing = FOCUSING_RESCUE_PLAYER;
			return _PrevType != m_eFocusing;
		}
	}

	for (std::tuple<float, SPTR<CMagicCircle>> C : m_listDetectMagicCircle)
	{
		switch (std::get<1>(C)->GetMagicCircleColor())
		{
		case MC_GREEN:
		{
			if (true == m_pPlayerFSM->CheckLookAtObject(std::get<1>(C)->TRANS->GetWorldPos())
				&& false == std::get<1>(C)->CheckRepairComplete())
			{
				m_eFocusing = FOCUSING_GENERATOR;
				m_FocusGenMagicCircle = std::get<1>(C);
				return _PrevType != m_eFocusing;
			}
		}
		break;
		case MC_RED:
		{
			if (true == m_pPlayerFSM->CheckLookAtObject(std::get<1>(C)->TRANS->GetWorldPos())
				&& true == std::get<1>(C)->CheckTrapAvailable())
			{
				m_eFocusing = FOCUSING_TRAP;
				m_FocusTrapMagicCircle = std::get<1>(C);
				return _PrevType != m_eFocusing;
			}
		}
		break;
		default:
			break;
		}
	}

	m_eFocusing = FOCUSING_NONE;
	return _PrevType != m_eFocusing;

}
const bool CPlayer::KillerFocusingUpdate()
{
	SortingDetectObj();

	switch (m_eFocusing)
	{
	case FOCUSING_SUMMONS_PLAYER:
	{
		if (false == ExistPlayerList(m_FocusPlayer)
			|| false == m_pPlayerFSM->CheckLookAtObject(m_FocusPlayer->TRANS->GetWorldPos())
			|| false == m_FocusPlayer->IsGroggy())
		{
			m_FocusPlayer = nullptr;
		}
		else
		{
			return false;
		}
	}
	break;
	case FOCUSING_GENERATOR:
	{
		if (false == ExistMagicCircle(m_FocusGenMagicCircle)
			|| false == m_pPlayerFSM->CheckLookAtObject(m_FocusGenMagicCircle->TRANS->GetWorldPos()))
		{
			m_FocusGenMagicCircle = nullptr;
		}
		else
		{
			return false;
		}
	}
	break;
	case FOCUSING_TRAP:
	{
		if (false == ExistMagicCircle(m_FocusTrapMagicCircle) ||
			false == m_pPlayerFSM->CheckLookAtObject(m_FocusTrapMagicCircle->TRANS->GetWorldPos()))
		{
			m_FocusGenMagicCircle = nullptr;
		}
		else
		{
			return false;
		}
	}
	break;
	default:
		break;
	}

	return ChangeKillerFocusingObj(m_eFocusing);
}

const bool CPlayer::ChangeKillerFocusingObj(FOCUSING_OBJ_TYPE _PrevType)
{
	for (std::tuple<float, CPlayer*> P : m_listFocusPlayer)
	{
		if (true == m_pPlayerFSM->CheckLookAtObject(std::get<1>(P)->TRANS->GetWorldPos())
			&& true == std::get<1>(P)->IsGroggy() )
		{
			m_FocusPlayer = std::get<1>(P);
			m_eFocusing = FOCUSING_SUMMONS_PLAYER;
			return _PrevType != m_eFocusing;
		}
	}

	for (std::tuple<float, SPTR<CMagicCircle>> C : m_listDetectMagicCircle)
	{
		switch (std::get<1>(C)->GetMagicCircleColor())
		{
		case MC_GREEN:
		{
			if (true == m_pPlayerFSM->CheckLookAtObject(std::get<1>(C)->TRANS->GetWorldPos())
				&& true == std::get<1>(C)->CheckBreakableGenerator())
			{
				m_eFocusing = FOCUSING_GENERATOR;
				m_FocusGenMagicCircle = std::get<1>(C);
				return _PrevType != m_eFocusing;
			}
		}
		break;
		case MC_RED:
		{
			if (true == m_pPlayerFSM->CheckLookAtObject(std::get<1>(C)->TRANS->GetWorldPos())
				&& true == std::get<1>(C)->CheckTrapActive())
			{
				m_eFocusing = FOCUSING_TRAP;
				m_FocusTrapMagicCircle = std::get<1>(C);
				return _PrevType != m_eFocusing;
			}
		}
		break;
		default:
			break;
		}
	}

	m_eFocusing = FOCUSING_NONE;
	return _PrevType != m_eFocusing;
}
void CPlayer::SortingDetectObj()
{
	Vec3 Pos = TRANS->GetWorldPos();

	if(m_listFocusPlayer.size() >= 2)
	{
		for (std::tuple<float, CPlayer*> P : m_listFocusPlayer)
		{
			std::get<0>(P) = (std::get<1>(P)->TRANS->GetWorldPos() - Pos).Length_Square();
		}

		m_listFocusPlayer.sort(Player_Compare);
	}


	if (m_listDetectMagicCircle.size() >= 2)
	{
		for (std::tuple<float, CPlayer*> P : m_listFocusPlayer)
		{
			std::get<0>(P) = (std::get<1>(P)->TRANS->GetWorldPos() - Pos).Length_Square();
		}
		
		m_listDetectMagicCircle.sort(Circle_Compare);
	}
}
const bool CPlayer::ExistPlayerList(CPlayer* _pPlayer) 
{
	if (nullptr == _pPlayer)
		return false;

	for (std::tuple<float,CPlayer*> P : m_listFocusPlayer)
	{
		if (std::get<1>(P) == _pPlayer)
		{
			return true;
		}
	}

	return false;
}
const bool CPlayer::ExistMagicCircle(SPTR<CMagicCircle> _Circle) 
{
	if (nullptr == _Circle)
		return false;

	for (std::tuple<float, SPTR<CMagicCircle>> C: m_listDetectMagicCircle)
	{
		if (std::get<1>(C) == _Circle)
		{
			return true;
		}
	}

	return false;
}

void CPlayer::EndHealingEvent()
{
	m_pPlayerFSM->ChangeIdleEvent();
	m_pPlayerFSM->m_CurPlayerState = PLAYER_IDLE;

}