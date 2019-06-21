#pragma once
#include <Script.h>
#include "MiddleStruct.h"
#include <Camera.h>

#define CAMERAPIVOT Vec3(0.f, 80.f, 220.f) 
#define SURVIVORCAMERAPIVOT Vec3(0.f, 80.f, 220.f)
#define KILLERCAMERAPIVOT Vec3(0.f, 20.f, 60.f) 
 

#define DUMMYPIVOT Vec3(0.f,70.f,0.f)

#define MAXSURVIVORCAMERADISTANCE 200.f
#define MAXKILLERCAMERADISTANCE 70.f

#define MINDISTFACTOR 0.1f
#define CAMFRUSTUMANGLE 20.f
#define CAMFRUSTUMNEAR 10.f

#define MAXSURVIVORLIFE 3
#define MAXKILLERLIFE 0xffffffff
#define SURVIVORDEATHCOUNT 3

#define MAXHEALINGCOUNT 30.f

#define MAXHOOKCOUNT 60.f
#define HALFHOOKCOUNT 30.f
#define MAXRESCUECOUNT 5.f


enum FOCUSING_OBJ_TYPE
{
	FOCUSING_NONE,
	FOCUSING_HEALING_PLAYER,	
	FOCUSING_RESCUE_PLAYER,
	FOCUSING_SUMMONS_PLAYER,
	FOCUSING_GENERATOR,
	FOCUSING_TRAP,
	MAX_FOCUSING_OBJ_TYPE,
};


class CMagicCircle;
class CPlayer : public CScript
{
private:
	friend class CRoom;
private:
	bool											m_bServerFlag;
	// 로비룸에 있는 플레이어들이 준비상태인지 알기위한 변수
	bool											m_bServerReady;
	bool											m_bBuildComplete;
	PLAYER_NETWORK_TYPE								m_eNetType;
	PLAYER_ROLE_TYPE								m_eRoleType;
	// 클라이언트에서 서버로 데이터를 요구할때 true가 되는 변수
	bool											m_bEnter;
	bool											m_bBroadSync;
	CServerUser*									m_pPairUser;

private:
	unsigned int									m_SurvivorID;

public:
	const bool IsBroadSync() { return m_bBroadSync; }

private:
	friend class CPlayerFSM;
	friend class CSurvivorFSM;
	friend class CKillerFSM;
	friend class CDBDRule;

	CPlayerFSM*											m_pPlayerFSM;
	unsigned __int64									m_PlayerID;
	// 클라이언트의 로컬 플레이어의 상태가 바뀌였을때 서버로 신호를 보내기 위한 변수
	bool												m_bBroadState;
	// 서버로 부터 동기화 패킷을 받아 기하상태를 동기화 했을때의 변수
	// 이 변수가 true이면 이동함수에서 Move를 수행하지 않는다.
	bool												m_bSyncro;

protected:
	SPTR<class COBBCollider>							m_PlayerObstacleCollider;
	SPTR<COBBCollider>									m_PlayerBodyCollider;
	SPTR<COBBCollider>									m_PlayerAttackCollider;
	SPTR<class CSphereCollider>							m_PlayerDetectedCollider;
	SPTR<CSphereCollider>								m_PlayerBoundaryCollider;

	SPTR<class CActor>									m_PlayerCamActor;
	SPTR<class CCamera>									m_PlayerCamera;
	SPTR<class CNavigator>								m_PlayerNavigator;
	SPTR<class CFrustumCollider>						m_CameraFrustumCollider;


public:
	std::list<SPTR<class CCollider>>					m_listCamCol;


private:
	SPTR<class CActor>									m_PlayerCamDummy;
	Vec3												m_CameraPivot;
	float												m_MaxCamDist;

	SPTR<class CActor>									m_PlayerLightActor;
	SPTR<class CLight>									m_PlayerLight;

	SPTR<class CActor>									m_PlayerFootPrintDummy;
	SPTR<class CActor>									m_PlayerFootPrintDummyRight;
	SPTR<class CActor>									m_PlayerFootPrintDummyLeft;

private:
	UINT												m_LifeCount;
	UINT												m_DeathCount;
	bool												m_bHealing;
	float												m_HealingCount;
	float												m_HookCount;
	float												m_RescueCount;
	bool												m_bRescued;
	float												m_SummonsCount;

private:
	bool												m_bGamePlayEnd;

private:
	std::list<std::tuple<float, SPTR<CMagicCircle>>>	m_listDetectMagicCircle;
	SPTR<CMagicCircle>									m_FocusGenMagicCircle;
	SPTR<CMagicCircle>									m_FocusTrapMagicCircle;
	
	std::list<std::tuple<float,CPlayer*>>				m_listFocusPlayer;
	CPlayer*											m_FocusPlayer;

	FOCUSING_OBJ_TYPE									m_eFocusing;


private:
	CDBDRule*											m_pRule;
	bool												m_bDemegeFlag;

private:
	void SetBroadFlag(const bool& _Flag)
	{
		if (m_eNetType == REMOTE_PLAYER)
			return;

		m_bBroadState = _Flag;
	}

public:
	const bool IsBroadFlag() { return m_bBroadState; }
	void InitBroadFlag() { m_bBroadState = false; }

private:
	void SetPlayerID(class CServerUser* _pUser);
public:
	void SetRule(CDBDRule* _pRule);
public:
	void SetPlayerID(const unsigned __int64& _ID)
	{
		m_PlayerID = _ID;
	}
	const unsigned __int64 GetPlayerID()
	{
		return m_PlayerID;
	}
public:
	template<typename PLAYERFSM>
	PLAYERFSM* MountPlayerFSM()
	{
		if (nullptr == m_FSMDriver)
			return nullptr;

		if (false == Dynamic_Cast_Check<CPlayerFSM, PLAYERFSM>::IsParent)
			return nullptr;

		PLAYERFSM*  MountFSM = SettingFSM<PLAYERFSM>(this);
		m_pPlayerFSM = MountFSM;
		//m_pPlayerFSM->Init();
		return MountFSM;
	}

	void SetPlayerRole(const PLAYER_ROLE_TYPE& _Role)
	{
		m_eRoleType = _Role;
		switch (m_eRoleType)
		{
		case PLAYER_SURVIVOR:
		{
			m_CameraPivot = SURVIVORCAMERAPIVOT;
			m_MaxCamDist = MAXSURVIVORCAMERADISTANCE;
			m_LifeCount = MAXSURVIVORLIFE;
			m_DeathCount = 0;
			m_HealingCount = 0.f;
			m_HookCount = 0.f;
		}
		break;
		case PLAYER_KILLER:
		{
			m_CameraPivot = KILLERCAMERAPIVOT;
			m_MaxCamDist = MAXKILLERCAMERADISTANCE;
			m_LifeCount = MAXKILLERLIFE;
			m_DeathCount = 0;
			m_HookCount = 0.f;
		}
		break;
		default:
			break;
		}

	}
	void SetLocalPlayer();
	void SetRemotePlayer();
	PLAYER_ROLE_TYPE GetPlayerRole() { return m_eRoleType; }


public:
	const bool Init() override;
	const bool Init(const PlayerInitData& _InitData);
	void Update(const float& _DeltaTime)override;
	void LastUpdate(const float& _DeltaTime)override;

public:
	void InitializePlayer(const PlayerData& _Data);
	const bool UpdatePlayer(const PlayerData& _Data);
	void SyncroPlayer(const PlayerData& _Data);


public:
	void SetPos(const Vec3& _Pos);
	const Vec3 GetWorldPos();
	const Vec3 GetPrevPos();
	const PlayerData GetPlayerData();
	SPTR<CCamera> GetPlayerCamera();
	SPTR<CNavigator> GetPlayerNavigator();
	const Vec3 GetPlayerCamDir();
	const Vec3 GetPlayerDir();
	const Vec3 GetCamDummyRot();

public:
	void PlayerActorOn();
	void PlayerActorOff();
	const bool IsPlayerActive();

public:
	void InitPlayerCamera();
	void AddPlayerCameraCollider();
	void ChangePlayerCamFrustum(const float& _Near, const float& _Far);
public:
	void PlayerCameraCollisionEvent(SPTR<CCollider> _Col);
	void UpdateCameraPivot();

public:
	void SetHookCount(const float& _Count);

public:
	CPlayerFSM*		   GetPlayerFSM() { return m_pPlayerFSM; }
	SPTR<COBBCollider> GetPlayerObstacleCollider();
	SPTR<COBBCollider> GetPlayerBodyCollider();
	SPTR<COBBCollider> GetPlayerAttackCollider();
	SPTR<CSphereCollider> GetPlayerDetectedCollider();
	SPTR<CSphereCollider> GetPlayerBoundaryCollider();
	SPTR<COBBCollider> CreatePlayerAttackCollider(const bool& _Debug);
	SPTR<CSphereCollider> CreatePlayerDetectedCollider(const bool& _Debug );
	CActor* CreateFootPrintDummy();
	void CreatePlayerEyeLight();

public:
	void PushDetectMagicCircle(SPTR<CMagicCircle> _MC);
	void PopDetectMagicCircle(SPTR<CMagicCircle> _MC);
	void FocusingGenerator(SPTR<CMagicCircle> _MC);
	void FocusingTrap(SPTR<CMagicCircle> _MC);
	void FocusingEndGenerator();
	void FocusingEndTrap();
	const bool IsFocusingGenerator();
	const bool IsFocusingTrap();

public:
	void PlayerDamagedEvent();

public:
	void AddDeathCount();
	void SetDeathCount(const unsigned int& _Count);
	void PushFocusingPlayer(SPTR<CPlayer> _FocusPlayer);
	void PopFocusingPlayer(SPTR<CPlayer> _FocusPlayer);
	const bool IsTreatable();
	const bool IsIdle();
	const bool IsLifeZero();
	void PlayerHealing(const float& _HealingCount);
	void AddHookCount(const float& _HookCount);
	void SetHealing(const bool& _Healing) { m_bHealing = _Healing; }
	const bool IsHealing() { return m_bHealing; }
	const bool IsFullHookCount() { return m_HookCount >= MAXHOOKCOUNT; }
	const bool IsHalfHookCount() { return m_HookCount >= MAXHOOKCOUNT * 0.5f; }
	void SetRescued(const bool& _Rescued) { m_bRescued = _Rescued; }
	const bool IsRescued() { return m_bRescued; }
	const bool TriggerHookSpider(const float& _HookCount)
	{
		if (m_HookCount < HALFHOOKCOUNT && m_HookCount + _HookCount >= HALFHOOKCOUNT)
			return true;

		return false;
	}

	SPTR<class CActor> GetFootPrintActor()
	{
		return m_PlayerFootPrintDummy;
	}
	SPTR<class CActor> GetFootPrintActorRight()
	{
		return m_PlayerFootPrintDummyRight;
	}
	SPTR<class CActor> GetFootPrintActorLeft()
	{
		return m_PlayerFootPrintDummyLeft;
	}

public:
	const bool IsBeforeExecution();
	const bool IsGroggy();
	const bool IsHook();
public:
	const bool SurvivorFocusingUpdate();
	const bool ChangeSurvivorFocusingObj(FOCUSING_OBJ_TYPE _PrevType);
	const bool KillerFocusingUpdate();
	const bool ChangeKillerFocusingObj(FOCUSING_OBJ_TYPE _PrevType);
	void SortingDetectObj();
	const bool ExistPlayerList(CPlayer* _pPlayer);
	const bool ExistMagicCircle(SPTR<CMagicCircle> _Circle);

public:
	void EndHealingEvent();

public:
	CPlayer();
	~CPlayer();
};

