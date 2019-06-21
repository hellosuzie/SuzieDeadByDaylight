#include "PlayerFSM.h"
#include "Player.h"
#include <KeyMgr.h>
#include <TransForm.h>
#include <Camera.h>
#include <Actor.h>
#include <TransForm.h>
#include <Collider.h>
#include <TimerMgr.h>
#include <OBBCollider.h>
#include <SphereCollider.h>
#include "MagicCircle.h"
#include "DeadByDaylightGlobal.h"
#include "DBDRule.h"
#include "Hook.h"
#include "ClientConnectMgr.h"
#include "SurvivorFSM.h"
#include "KillerFSM.h"
#include <Navigator.h>



CPlayerFSM::CPlayerFSM()
	:m_pPairPlayer(nullptr), m_CurPlayerState(PLAYER_INIT), m_fInputInterval(0.f)
	, m_GenViewDir(Vec3()), m_TreatPlayer(nullptr), m_HookPlayer(nullptr)
	, m_RescuedPlayer(nullptr), m_SurvivorHook(nullptr), m_fCurSpeed(0.f)
{
}
CPlayerFSM::CPlayerFSM(CPlayer* _pPlayer)
	:m_pPairPlayer(_pPlayer), m_CurPlayerState(PLAYER_INIT), m_fInputInterval(0.f)
	, m_fAccTime(0.f), m_GenViewDir(Vec3()), m_TreatPlayer(nullptr), m_HookPlayer(nullptr)
	, m_RescuedPlayer(nullptr), m_SurvivorHook(nullptr), m_fCurSpeed(0.f)
{

}

CPlayerFSM::~CPlayerFSM()
{
}
const bool CPlayerFSM::CameraVisionUpdate(const float& _DeltaTime)
{
	Vec3 MRot,Rot;
	MRot.x = -CKeyMgr::GetMouseDir().y;
	MRot.y = CKeyMgr::GetMouseDir().x;

	if (MRot == Vec3())
		return false;

	Rot = m_pPairPlayer->m_PlayerCamDummy->TRANS->GetRotate();
	Rot.x = DXMath::Clamp(Rot.x + MRot.x * VISIONROTSPEEDX * _DeltaTime, -10.f, 20.f);
	Rot.y = Rot.y + MRot.y * VISIONROTSPEEDY * _DeltaTime;

	while (Rot.y >= 360.f) 
	{
		Rot.y -= 360.f;
	}

	while (Rot.y <= -360.f)
	{
		Rot.y += 360.f;
	}

	m_pPairPlayer->m_PlayerCamDummy->TRANS->SetRotate(Rot);

	m_CurCamDir = m_pPairPlayer->m_PlayerCamDummy->TRANS->GetBack();
	m_CurCamDir.y = 0.f;
	m_CurCamDir.Normalize();

	//CalCameraPivot(m_pPairPlayer->m_CameraFrustumCollider);
	return true;
}
const bool CPlayerFSM::CheckMoveKeyInput()
{
	if (false == KEY("UP") && false == KEY("DOWN") && false == KEY("LEFT") && false == KEY("RIGHT"))
		return false;

	return true;
}

const bool CPlayerFSM::CheckSelfHealing()
{
	if (PLAYER_KILLER == PLAYERSCRIPT->m_eRoleType)
	{
		return false;
	}

	if (false == KEYDOWN("SelfHealing")) 
	{
		return false;
	}


	return PLAYERSCRIPT->m_LifeCount == 1;

}

void CPlayerFSM::SetLocalPlayer() 
{

}
void CPlayerFSM::SetRemotePlayer() 
{

}
void CPlayerFSM::Init()
{
	m_pPairPlayer->InitPlayerCamera();
	
	PLAYERSCRIPT->TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));
	PLAYERSCRIPT->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));

	ChangePlayerState(PLAYER_IDLE);
}

void CPlayerFSM::InitializeSurvivorCollider(const bool& _Debug /*= true*/)
{
	SPTR<COBBCollider> PlayerCol = m_pPairPlayer->GetPlayerObstacleCollider();
	PlayerCol->AddTag(L"Survivor");
	PlayerCol->SetColFi(Vec3(0.f, 0.f, 50.f), Vec3(130.f, 130.f, 400.f));
	PlayerCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::ObstacleCollsionEvent);
	PlayerCol->SetStayCallBack((CPlayerFSM*)this, &CPlayerFSM::ObstacleCollsionEvent);

	SPTR<COBBCollider> BodyCol = m_pPairPlayer->GetPlayerBodyCollider();
	BodyCol->AddTag(L"Survivor");
	BodyCol->SetDebugColor(Vec4::PastelGreen.vec3);
	BodyCol->SetColFi(Vec3(0.f, 0.f, 50.f), Vec3(80.f, 80.f, 400.f));
	BodyCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::SurvivorDamagedEvent);

	SPTR<CSphereCollider> DetectedCol = m_pPairPlayer->CreatePlayerDetectedCollider(_Debug);
	DetectedCol->AddTag(L"Survivor");
	DetectedCol->SetColFi(Vec3(0.f, 0.f, 0.f), Vec3(240.f, 240.f, 240.f));
	DetectedCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::FocusingPlayerEvent);
	DetectedCol->SetExitCallBack((CPlayerFSM*)this, &CPlayerFSM::FocusingPlayerEnd);

	SPTR<CSphereCollider> BoundCol = m_pPairPlayer->GetPlayerBoundaryCollider();
	BoundCol->AddTag(L"Survivor");
	BoundCol->SetColFi(Vec3(0.f, 0.f, 0.f), Vec3(210.f, 210.f, 210.f));

}
void CPlayerFSM::InitializeKillerCollider(const bool& _Debug /*= true*/)
{
	SPTR<COBBCollider> PlayerCol = m_pPairPlayer->GetPlayerObstacleCollider();
	PlayerCol->AddTag(L"Killer");
	PlayerCol->SetColFi(Vec3(0.f, 0.f, 50.f), Vec3(130.f, 130.f, 400.f));
	PlayerCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::ObstacleCollsionEvent);
	PlayerCol->SetStayCallBack((CPlayerFSM*)this, &CPlayerFSM::ObstacleCollsionEvent);


	SPTR<COBBCollider> BodyCol = m_pPairPlayer->GetPlayerBodyCollider();
	BodyCol->AddTag(L"Killer");
	BodyCol->SetDebugColor(Vec4::PastelGreen.vec3);
	BodyCol->SetColFi(Vec3(0.f, 0.f, 50.f), Vec3(80.f, 80.f, 400.f));
	BodyCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::KillerDamagedEvent);


	SPTR<COBBCollider> AtkCol = m_pPairPlayer->CreatePlayerAttackCollider(_Debug);
	AtkCol->AddTag(L"Killer");
	AtkCol->SetDebugColor(Vec4::PastelGreen.vec3);
	AtkCol->SetColFi(Vec3(0.f, 80.f, 60.f), Vec3(120.f, 300.f, 60.f));
	AtkCol->Off();
	AtkCol->IsIgnoreCollider(BodyCol);

	SPTR<CSphereCollider> DetectedCol = m_pPairPlayer->CreatePlayerDetectedCollider(_Debug);
	DetectedCol->AddTag(L"Killer");
	DetectedCol->SetColFi(Vec3(0.f, 0.f, 0.f), Vec3(240.f, 240.f, 240.f));
	DetectedCol->SetEnterCallBack((CPlayerFSM*)this, &CPlayerFSM::FocusingPlayerEvent);
	DetectedCol->SetExitCallBack((CPlayerFSM*)this, &CPlayerFSM::FocusingPlayerEnd);


	SPTR<CSphereCollider> BoundCol = m_pPairPlayer->GetPlayerBoundaryCollider();
	BoundCol->AddTag(L"Survivor");
	BoundCol->SetColFi(Vec3(0.f, 0.f, 0.f), Vec3(180.f, 180.f, 180.f));
}
void CPlayerFSM::PreUpdate(const float& _DeltaTime)
{

}
void CPlayerFSM::Update(const float& _DeltaTime)
{
	CBaseFSM::Update(_DeltaTime);
}

const bool CPlayerFSM::CheckTransferableState(const PLAYER_STATE& _State)
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR)
		return CSurvivorFSM::SurvivorStateMap[m_CurPlayerState][_State];
	else 
		return CKillerFSM::KillerStateMap[m_CurPlayerState][_State];
}



const bool CPlayerFSM::ChangePlayerState(const PLAYER_STATE& _State)
{
	if (false == CheckTransferableState(_State))
		return false;


	m_pPairPlayer->SetBroadFlag(true);
	m_CurPlayerState = _State;

	m_fCurSpeed = 0.f;
	switch (m_CurPlayerState)
	{
	case PLAYER_IDLE:
		ChangeIdleEvent();
		ChangeCurFunction(this, &CPlayerFSM::Idle);
		break;
	case PLAYER_WALK:
		ChangeWalkEvent();
		ChangeCurFunction(this, &CPlayerFSM::Walk);
		break;
	case PLAYER_RUN:
		ChangeRunEvent();
		ChangeCurFunction(this, &CPlayerFSM::Run);
		break;
	case PLAYER_ATTACK:
		ChangeAttackEvent();
		ChangeCurFunction(this, &CPlayerFSM::Attack);
		break;
	case PLAYER_DAMAGED:
		ChangeDamagedEvent();
		ChangeCurFunction(this, &CPlayerFSM::Damaged);
		break;
	case PLAYER_REPIRE:
		ChangeRepairEvent();
		ChangeCurFunction(this, &CPlayerFSM::Repair);
		break;
	case PLAYER_HEALING:
		ChangeHealingEvent();
		ChangeCurFunction(this, &CPlayerFSM::Healing);
		break;
	case PLAYER_GROGGY:
		ChangeGroggyEvent();
		ChangeCurFunction(this, &CPlayerFSM::Groggy);
		break;
	case PLAYER_SUMMONS:
		ChangeSummonsEvent();
		ChangeCurFunction(this, &CPlayerFSM::Summons);
		break;
	case PLAYER_GENBREAK:
		ChangeGenBreakEvent();
		ChangeCurFunction(this, &CPlayerFSM::GenBreak);
		break;
	case PLAYER_SELFHEALING:
		ChangeSelfHealingEvent();
		ChangeCurFunction(this, &CPlayerFSM::SelfHealing);
		break;
	case PLAYER_HOOK:
		ChangeHookEvent();
		ChangeCurFunction(this, &CPlayerFSM::Hook);
		break;
	case PLAYER_RESCUE:
		ChangeRescueEvent();
		ChangeCurFunction(this, &CPlayerFSM::Rescue);
		break;
	case PLAYER_RESCUED:
		ChangeRescuedEvent();
		ChangeCurFunction(this, &CPlayerFSM::Rescued);
		break;
	case PLAYER_DEAD:
		ChangeDeadEvent();
		ChangeCurFunction(this, &CPlayerFSM::Dead);
		break;
	case MAX_PLAYER_STATE:
	default:
		return false;
	}

	m_fAccTime = 0.f;
	return true;
}


void CPlayerFSM::Idle(const float& _DeltaTime) 
{

	// 입력키가 눌렸을때 
	if (m_pPairPlayer->m_eNetType == LOCAL_PLAYER)
	{
		if (true == CheckMoveKeyInput()) 
		{
			if (PLAYER_SURVIVOR ==  PLAYERSCRIPT->m_eRoleType) 
			{
				ChangePlayerState(PLAYER_WALK);
				return;
			}
			else if (PLAYER_KILLER == PLAYERSCRIPT->m_eRoleType) 
			{
				ChangePlayerState(PLAYER_RUN);
				return;
			}
		}
		else if (true == CheckSelfHealing()) 
		{
			ChangePlayerState(PLAYER_SELFHEALING);
			return;
		}
		
	}

	//if (true == PLAYERSCRIPT->m_bHealing) 
	//{
	//	PLAYERSCRIPT->PlayerHealing(_DeltaTime);
	//}


}
void CPlayerFSM::Walk(const float& _DeltaTime) 
{

	if (LOCAL_PLAYER == m_pPairPlayer->m_eNetType)
	{

		if(true == CheckSelfHealing())
		{
			ChangePlayerState(PLAYER_SELFHEALING);
			return;
		}


		if (false == CheckMoveKeyInput()) 
		{		
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		if (true == PLAYERSCRIPT->m_bSyncro)
		{
			return;
		}


		Vec3 MoveDir = UpdateMoveDir(_DeltaTime);
		
		if (MoveDir == Vec3())
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		if (m_PlayerMoveDir != MoveDir)
		{
			m_PlayerMoveDir = MoveDir;
			m_pPairPlayer->SetBroadFlag(true);
		}
	}


	Vec3 Pos = PLAYERSCRIPT->TRANS->Move(m_PlayerMoveDir, m_fCurSpeed, _DeltaTime);
}

void CPlayerFSM::Run(const float& _DeltaTime) 
{
	if (LOCAL_PLAYER == m_pPairPlayer->m_eNetType)
	{

		if (true == CheckSelfHealing())
		{
			ChangePlayerState(PLAYER_SELFHEALING);
			return;
		}

		if (false == CheckMoveKeyInput()) 
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		if (true == PLAYERSCRIPT->m_bSyncro)
		{
			return;
		}

		Vec3 MoveDir = UpdateMoveDir(_DeltaTime);

		if (MoveDir == Vec3())
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		if (m_PlayerMoveDir != MoveDir)
		{
			m_PlayerMoveDir = MoveDir;
			m_pPairPlayer->SetBroadFlag(true);
		}
	}

	Vec3 Pos = PLAYERSCRIPT->TRANS->Move(m_PlayerMoveDir, m_fCurSpeed, _DeltaTime);
}
void CPlayerFSM::Attack(const float& _DeltaTime) 
{
	if (TIMING(m_fAccTime,_DeltaTime,0.4f))
	{
		PLAYERSCRIPT->m_PlayerAttackCollider->On();
	}
	else if (TIMING(m_fAccTime, _DeltaTime, 0.8f))
	{
		PLAYERSCRIPT->m_PlayerAttackCollider->Off();
	}

	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::Damaged(const float& _DeltaTime) 
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR) 
	{
		--PLAYERSCRIPT->m_LifeCount;

		if (PLAYERSCRIPT->m_LifeCount <= 0)
		{
			ChangePlayerState(PLAYER_GROGGY);
			return;
		}
		else
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}
	}
	else 
	{
		if (m_fAccTime >= 2.7f)
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		m_fAccTime += _DeltaTime;
	}

}

void CPlayerFSM::Repair(const float& _DeltaTime) 
{
	//CameraVisionUpdate(_DeltaTime);

}
void CPlayerFSM::Healing(const float& _DeltaTime) 
{
	//if (nullptr == m_TreatPlayer) 
	//{
	//	return ChangePlayerState(PLAYER_IDLE);
	//}

	//if (false == m_TreatPlayer->IsHealing()) 
	//{
	//	return ChangePlayerState(PLAYER_IDLE);
	//}

	//m_TreatPlayer->PlayerHealing(_DeltaTime);
	////CameraVisionUpdate(_DeltaTime);

	//if (true == CheckMoveKeyInput())
	//{
	//	m_TreatPlayer->SetHealing(false);
	//	m_TreatPlayer = nullptr;

	//	return	ChangePlayerState(PLAYER_WALK);
	//}
}
void CPlayerFSM::Groggy(const float& _DeltaTime) 
{
	
}
void CPlayerFSM::Summons(const float& _DeltaTime) 
{
	//if (m_pPairPlayer->m_eNetType == LOCAL_PLAYER)
	//{
	//	//CameraVisionUpdate(_DeltaTime);
	//}
	//
	//if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR) 
	//{

	//}
	//else 
	//{
	//	if (m_fAccTime >= 3.5f)
	//	{
	//		ChangePlayerState(PLAYER_IDLE);
	//		m_HookPlayer->m_pPlayerFSM->ChangePlayerState(PLAYER_HOOK);
	//		m_HookPlayer = nullptr;
	//		return;
	//	}
	//}

	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::GenBreak(const float& _DeltaTime)
{
	if (true == CTimerMgr::TimingCheck(m_fAccTime, 1.5f)) 
	{
		if (nullptr != PLAYERSCRIPT->m_FocusGenMagicCircle) 
		{
			PLAYERSCRIPT->m_FocusGenMagicCircle->BreakGenerator();
		}
		else if ( PLAYERSCRIPT->m_FocusTrapMagicCircle) 
		{
			PLAYERSCRIPT->m_FocusTrapMagicCircle->StopTrap();
		}
	}

	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::SelfHealing(const float& _DeltaTime)
{
	//CameraVisionUpdate(_DeltaTime);

	PLAYERSCRIPT->PlayerHealing(_DeltaTime);

	if (false == PLAYERSCRIPT->IsHealing()) 
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

	if (true == CheckMoveKeyInput())
	{
		ChangePlayerState(PLAYER_WALK);
		return	;
	}
	

}
void CPlayerFSM::Hook(const float& _DeltaTime)
{
	
}
void CPlayerFSM::Rescue(const float& _DeltaTime)
{
	//CameraVisionUpdate(_DeltaTime);
	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::Rescued(const float& _DeltaTime)
{
	//CameraVisionUpdate(_DeltaTime);

	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::Dead(const float& _DeltaTime)
{
	//CameraVisionUpdate(_DeltaTime);

	m_fAccTime += _DeltaTime;
}
void CPlayerFSM::ChangeIdleEvent()
{


}
void CPlayerFSM::ChangeWalkEvent()
{
	PLAYERSCRIPT->SetHealing(false);
	m_fCurSpeed = SURVIVORWALK;
}
void CPlayerFSM::ChangeRunEvent()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR) 
	{
		PLAYERSCRIPT->SetHealing(false);
		if (PLAYERSCRIPT->m_LifeCount > 1) 
		{
			m_fCurSpeed = SURVIVORRUN;
		}
		else 
		{
			m_fCurSpeed = SURVIVORSLOWRUN;
		}
	}
	else 
	{
		m_fCurSpeed = KILLERRUN;
	}
}

void CPlayerFSM::ChangeAttackEvent()
{

}
void CPlayerFSM::ChangeDamagedEvent()
{
	PLAYERSCRIPT->SetHealing(false);
}
void CPlayerFSM::ChangeRepairEvent()
{
	//PLAYERSCRIPT->m_FocusGenMagicCircle->RepairGenerator();

	//m_GenViewDir = (PLAYERSCRIPT->m_FocusGenMagicCircle->TRANS->GetWorldPos() - PLAYERSCRIPT->TRANS->GetWorldPos()).GetNormalizeVec();
}
void CPlayerFSM::ChangeHealingEvent()
{
	//m_TreatPlayer->SetHealing(true);
}
void CPlayerFSM::ChangeGroggyEvent()
{

}
void CPlayerFSM::ChangeSummonsEvent()
{
	/*
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_KILLER) 
	{
		m_HookPlayer->m_pPlayerFSM->ChangePlayerState(PLAYER_SUMMONS);
	}
	*/
}

void CPlayerFSM::ChangeGenBreakEvent()
{
	
}
void CPlayerFSM::ChangeSelfHealingEvent()
{
	PLAYERSCRIPT->SetHealing(true);
}
void CPlayerFSM::ChangeHookEvent()
{

}
void CPlayerFSM::ChangeRescueEvent()
{
	//m_RescuedPlayer->m_pPlayerFSM->ChangePlayerState(PLAYER_RESCUED);

}
void CPlayerFSM::ChangeRescuedEvent()
{

}
void CPlayerFSM::ChangeDeadEvent()
{
	
}
const bool CPlayerFSM::CheckGeneratorRepair()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_KILLER) 
	{
		return false;
	}

	if (nullptr == PLAYERSCRIPT->m_FocusGenMagicCircle || true == PLAYERSCRIPT->m_FocusGenMagicCircle->CheckRepairComplete())
	{
		return false;
	}

	return CheckLookAtObject(PLAYERSCRIPT->m_FocusGenMagicCircle->TRANS->GetWorldPos());
}

const bool CPlayerFSM::CheckGeneratorAttack()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR)
	{
		return false;
	}


	if (nullptr == PLAYERSCRIPT->m_FocusGenMagicCircle 
		|| true == PLAYERSCRIPT->m_FocusGenMagicCircle->CheckNoRepair()
		|| true == PLAYERSCRIPT->m_FocusGenMagicCircle->CheckRepairComplete()
		|| GENERATOR_DEMAGED == PLAYERSCRIPT->m_FocusGenMagicCircle->GetGenState())
	{
		return false;
	}

	return CheckLookAtObject(PLAYERSCRIPT->m_FocusGenMagicCircle->TRANS->GetWorldPos());

}

const bool CPlayerFSM::CheckBoardTrapActive()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_KILLER)
	{
		return false;
	}

	if (nullptr == PLAYERSCRIPT->m_FocusTrapMagicCircle
		|| false == PLAYERSCRIPT->m_FocusTrapMagicCircle->CheckTrapAvailable())
	{
		return false;
	}

	return true;
}
const bool CPlayerFSM::CheckBoardTrapAttack()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR)
	{
		return false;
	}

	if (nullptr == PLAYERSCRIPT->m_FocusTrapMagicCircle
		|| false == PLAYERSCRIPT->m_FocusTrapMagicCircle->CheckTrapActive())
	{
		return false;
	}

	//Vec3 GenDir = (PLAYERSCRIPT->m_FocusTrapMagicCircle->TRANS->GetWorldPos() - PLAYERSCRIPT->TRANS->GetWorldPos()).GetNormalizeVec();
	//GenDir.y = 0.f;

	//if (DXMath::Vector3Dot(GenDir, PLAYERSCRIPT->TRANS->GetUp()) >= 0.65f)
	//	return true;

	return CheckLookAtObject(PLAYERSCRIPT->m_FocusTrapMagicCircle->TRANS->GetWorldPos());
}
const bool CPlayerFSM::CheckSurvivorHealing() 
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_KILLER)
	{
		return false;
	}

	if (true  == PLAYERSCRIPT->m_listFocusPlayer.empty())
	{
		return false;
	}

	m_TreatPlayer = FindTreatablePlayer();

	return nullptr != m_TreatPlayer;
}
const bool CPlayerFSM::CheckHookSummons()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_SURVIVOR)
	{
		return false;
	}

	if (true == PLAYERSCRIPT->m_listFocusPlayer.empty())
	{
		return false;
	}

	m_HookPlayer = FindHookPlayer();

	return nullptr != m_HookPlayer;
}
const bool CPlayerFSM::CheckSurvivorRescued()
{
	if (PLAYERSCRIPT->m_eRoleType == PLAYER_KILLER)
	{
		return false;
	}

	if (true == PLAYERSCRIPT->m_listFocusPlayer.empty())
	{
		return false;
	}

	m_RescuedPlayer = FindRescuedPlayer();

	return nullptr != m_RescuedPlayer;
}
const bool CPlayerFSM::CheckLookAtObject(const Vec3& _ObjPos) 
{
	Vec3 LookDir = (_ObjPos - PLAYERSCRIPT->TRANS->GetWorldPos()).GetNormalizeVec();
	Vec3 FrontDir = PLAYERSCRIPT->TRANS->GetUp();
	
	LookDir.y = 0.f;
	FrontDir.y = 0.f;
	
	if (DXMath::Vector2Dot(LookDir.XZ(), FrontDir.XZ()) >= LOOKATMAGICNUMBER)
		return true;

	return false;
}

void CPlayerFSM::ActiveFocusBoardTrap()
{
	PLAYERSCRIPT->m_FocusTrapMagicCircle->ActiveTrap();
}
void CPlayerFSM::AttackFocusBoardTrap() 
{
	PLAYERSCRIPT->m_FocusTrapMagicCircle->StopTrap();
}
void CPlayerFSM::UpdatePlayerDir(const float& _DeltaTime)
{
	m_CurFront = PLAYERSCRIPT->TRANS->GetUp() * m_CurMoveFlag.y + PLAYERSCRIPT->TRANS->GetRight() * m_CurMoveFlag.x;
	m_CurFront.Normalize();

	float Cross = m_CurFront.z * m_CurCamDir.x - m_CurFront.x * m_CurCamDir.z;
	float Dot = m_CurFront.x * m_CurCamDir.x + m_CurFront.z * m_CurCamDir.z;
	Dot = DXMath::Clamp(Dot, -1.f, 1.f);

	float MaxAngle = acos(Dot) * DXMath::RtD;
	
	if (MaxAngle <= 2.f)
		return;
	
	float Angle = DXMath::Clamp(_DeltaTime * PLAYERROTSPEED, 0.f, MaxAngle);

	if (Cross <= 0.f)
	{
		PLAYERTRANS->AccRotate(Vec3(0.f, -Angle, 0.f));
	}
	else 
	{
		PLAYERTRANS->AccRotate(Vec3(0.f, Angle, 0.f));
	}
}

const Vec3 CPlayerFSM::UpdateMoveDir(const float& _DeltaTime)
{
	//if (PLAYERTRANS->IsRotate()) 
	//{
	//	UpdatePlayerDir();
	//}


	m_CurMoveFlag = Vec2();

	// 앞,뒤 방향키는 플레이어의 현재 Forward,Back 방향으로 이동
	if (true == KEY("UP"))
	{
		m_CurMoveFlag.y += 1.f;
	}
	if (true == KEY("DOWN"))
	{
		m_CurMoveFlag.y -= 1.f;
	}
	// 좌,우 방향키는 플레이어의 현재 Left, Right 방향으로 이동
	if (true == KEY("LEFT"))
	{
		m_CurMoveFlag.x -= 1.f;
	}

	if (true == KEY("RIGHT"))
	{
		m_CurMoveFlag.x += 1.f;
	}

	//Vec3 Front = PLAYERSCRIPT->TRANS->GetUp() * MoveFlag.z , Right = PLAYERSCRIPT->TRANS->GetRight() * MoveFlag.x;
	//Vec3 MoveDir =  DXMath::EpsilonCheck( Front + Right);
	//Vec3 MoveDir = DXMath::EpsilonCheck(m_CurFront * m_CurMoveFlag.y + m_CurRight * m_CurMoveFlag.x);
	//MoveDir.Normalize();
	
	UpdatePlayerDir(_DeltaTime);
	return PLAYERSCRIPT->TRANS->GetUp();
}

const bool CPlayerFSM::UpdatePlayerData(const PlayerData& _Data)
{
	PLAYERSCRIPT->TRANS->SetPos(_Data.Pos);
	PLAYERSCRIPT->TRANS->SetRotate(_Data.Rot);
	m_PlayerMoveDir = PLAYERSCRIPT->TRANS->GetUp();
	return ChangePlayerState((PLAYER_STATE)_Data.PlayerState);
}


void CPlayerFSM::SynchronizationPlayer(const PlayerData& _Data)
{
	Vec3 Pos = PLAYERSCRIPT->GetWorldPos();

	PLAYERSCRIPT->m_bSyncro = true;
	PLAYERSCRIPT->TRANS->SetRotate(_Data.Rot);
	m_PlayerMoveDir = PLAYERSCRIPT->TRANS->GetUp();

	PLAYERSCRIPT->TRANS->SetPos(_Data.Pos);
	PLAYERSCRIPT->m_PlayerNavigator->SetPrevPos(_Data.Pos);

	GETSINGLE(CClientConnectMgr).m_LogMgr.PrintLog(CClientLogMgr::MakeLogString(" Prev SyncPos : ( ", Pos.x, ", ", Pos.z, ")    ", "Sync Pos : ( ", _Data.Pos.x, ", ", _Data.Pos.z, " )   "));
}


void CPlayerFSM::ObstacleCollsionEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{
	if (_Other->GetChannel() != L"Default") 
	{
		return;
	}

	Vec2 ColDir = (_Other->GetColPos() - PLAYERTRANS->GetWorldPos()).GetNormalizeVec().XZ();
	float Check = DXMath::Vector2Dot(ColDir, m_PlayerMoveDir.XZ());
	
	// 플레이어의 움직이는 방향과 충돌체 방향이 90도 이상인 경우 
	// 플레이어를 밀치는 로직을 수행하지 않는다.
	if (Check <= 0.f) 
	{
		return;
	}

	Vec3 MoveablePos = m_pPairPlayer->GetPrevPos();
	Vec3 MoveDir;

	Vec3 Front = PLAYERTRANS->GetUp() * DXMath::Vector3Dot(PLAYERTRANS->GetUp(), m_PlayerMoveDir) ;
	Vec3 Right = PLAYERTRANS->GetRight() *DXMath::Vector3Dot(PLAYERTRANS->GetRight(), m_PlayerMoveDir) ;

	float T1 = 0.f, T2 = 0.f, T3 = 0.f;

	for ( ; T1 <= 1.f; T1 += COLCHECKUNIT)
	{
		MoveDir = m_PlayerMoveDir * T1;
		PLAYERTRANS->SetPos(MoveablePos + MoveDir);

		if (true == _This->CollisionCheck(_Other)) 
		{
			T1 -= COLCHECKUNIT;
			MoveDir = m_PlayerMoveDir * T1;
			MoveablePos = MoveablePos + MoveDir;
			break;
		}
	}

	for (; T2 <= 1.f - T1; T2 += COLCHECKUNIT)
	{
		PLAYERTRANS->SetPos(MoveablePos + Front * T2);

		if (true == _This->CollisionCheck(_Other))
		{
			T2 -= COLCHECKUNIT;
			break;
		}
	}

	for (; T3 <= 1.f - T1; T3 += COLCHECKUNIT)
	{
		PLAYERTRANS->SetPos(MoveablePos + Right * T3);

		if (true == _This->CollisionCheck(_Other))
		{
			T3 -= COLCHECKUNIT;
			break;
		}
	}

	MoveablePos += (Front * T2 + Right * T3);
	PLAYERTRANS->SetPos(MoveablePos);
	

}
void CPlayerFSM::ObstacleCollsionEnd(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	int a = 0;
}
void CPlayerFSM::SurvivorDamagedEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{
	if (_Other->ExistTag(L"Survivor"))
	{
		return;
	}

	ChangePlayerState(PLAYER_DAMAGED);

}
void CPlayerFSM::KillerDamagedEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	if (_Other->ExistTag(L"Killer")) 
	{
		return;
	}

	ChangePlayerState(PLAYER_DAMAGED);
}

void CPlayerFSM::FocusingPlayerEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{
	if (true == _Other->ExistTag(L"Survivor")) 
	{
		PLAYERSCRIPT->PushFocusingPlayer( _Other->ACTOR->FindCom<CPlayer>());
		return;
	}
}
void CPlayerFSM::FocusingPlayerEnd(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	if (true == _Other->ExistTag(L"Survivor"))
	{
		CPlayer* OtherPlayer = _Other->ACTOR->FindCom<CPlayer>();
		PLAYERSCRIPT->PopFocusingPlayer(OtherPlayer);
		if (nullptr != m_TreatPlayer && m_TreatPlayer == OtherPlayer)
		{
			m_TreatPlayer = nullptr;
		}
		return;
	}
}

void CPlayerFSM::DetectionGenerator(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{

}
void CPlayerFSM::DetectionPlayer(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{

}
void CPlayerFSM::DetectionTrap(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{

}
CPlayer* CPlayerFSM::FindTreatablePlayer()
{
	if (PLAYERSCRIPT->m_listFocusPlayer.empty()) 
	{
		return nullptr;
	}

	//std::list<CPlayer*>::iterator Start = PLAYERSCRIPT->m_listFocusPlayer.begin();
	//std::list<CPlayer*>::iterator End = PLAYERSCRIPT->m_listFocusPlayer.end();

	//for (; Start != End ; ++Start)
	//{
	//	if (true == (*Start)->IsTreatable() &&  true == CheckLookAtObject((*Start)->TRANS->GetWorldPos()))
	//	{
	//		return *Start;
	//	}
	//}

	return nullptr;
}

CPlayer* CPlayerFSM::FindHookPlayer()
{
	if (PLAYERSCRIPT->m_listFocusPlayer.empty())
	{
		return nullptr;
	}

	//std::list<CPlayer*>::iterator Start = PLAYERSCRIPT->m_listFocusPlayer.begin();
	//std::list<CPlayer*>::iterator End = PLAYERSCRIPT->m_listFocusPlayer.end();

	//for (; Start != End; ++Start)
	//{
	//	if (true == (*Start)->IsLifeZero() 
	//		&& PLAYER_GROGGY == (*Start)->m_pPlayerFSM->m_CurPlayerState
	//		&& true == CheckLookAtObject((*Start)->TRANS->GetWorldPos()))
	//	{
	//		return *Start;
	//	}
	//}

	return nullptr;
}
CPlayer* CPlayerFSM::FindRescuedPlayer()
{
	if (PLAYERSCRIPT->m_listFocusPlayer.empty())
	{
		return nullptr;
	}

	//std::list<CPlayer*>::iterator Start = PLAYERSCRIPT->m_listFocusPlayer.begin();
	//std::list<CPlayer*>::iterator End = PLAYERSCRIPT->m_listFocusPlayer.end();

	//for (; Start != End; ++Start)
	//{
	//	if (PLAYER_HOOK == (*Start)->m_pPlayerFSM->m_CurPlayerState && true == CheckLookAtObject((*Start)->TRANS->GetWorldPos()))
	//	{
	//		return *Start;
	//	}
	//}

	return nullptr;
}
const bool CPlayerFSM::IsFocusingPlayer(CPlayer* _Player)
{
	//std::list<CPlayer*>::iterator Start = PLAYERSCRIPT->m_listFocusPlayer.begin();
	//std::list<CPlayer*>::iterator End = PLAYERSCRIPT->m_listFocusPlayer.end();

	//for (;Start != End ; ++Start)
	//{
	//	if (*Start == _Player)
	//		return true;
	//}

	return false;
	
}
void CPlayerFSM::CamObstacleCollsionEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	m_pPairPlayer->PlayerCameraCollisionEvent(_Other);
}


const UINT CPlayerFSM::GetLifeCount() 
{
	return PLAYERSCRIPT->m_LifeCount;
}
const UINT CPlayerFSM::GetDeathCount()
{
	return PLAYERSCRIPT->m_DeathCount;
}
const float CPlayerFSM::GetHealingCount()
{
	return PLAYERSCRIPT->m_HealingCount;
}
const float CPlayerFSM::GetHealingGauge()
{
	return PLAYERSCRIPT->m_HealingCount / MAXHEALINGCOUNT;
}
const float CPlayerFSM::GetHookCount()
{
	return PLAYERSCRIPT->m_HookCount;
}
const float CPlayerFSM::GetRescuedCount()
{
	return PLAYERSCRIPT->m_RescueCount;
}