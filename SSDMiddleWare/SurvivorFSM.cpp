#include "SurvivorFSM.h"
#include "Player.h"
#include <TransForm.h>
#include <Actor.h>
#include <SkeletalFbxRenderer.h>
#include <DissolveSkeletalRenderer.h>
#include <OBBCollider.h>
#include <SphereCollider.h>
#include <KeyMgr.h>
#include <SSDRandom.h>
#include <TimerMgr.h>
#include <Scene.h>
#include "FootPrints.h"
#include "BloodScript.h"
#include "DBDRule.h"
#include "MagicCircle.h"
#include "ClientConnectMgr.h"
#include "Hook.h"
#include <SoundPlayer.h>
#include <Sound.h>

//enum PLAYER_STATE
//{
//	PLAYER_INIT,
//	PLAYER_IDLE,
//	PLAYER_WALK,
//	PLAYER_RUN,
//	PLAYER_ATTACK,
//	PLAYER_DAMAGED,
//	PLAYER_REPIRE,
//	PLAYER_HEALING,
//	PLAYER_GROGGY,
//	PLAYER_SUMMONS,
//  PLAYER_GENBREAK,
//	PLAYER_SELFHEALING,
//  PLAYER_HOOK,
//	PLAYER_RESCUE,
//	PLAYER_RESCUED
//  PLAYER_DEAD
//	MAX_PLAYER_STATE
//};

bool	CSurvivorFSM::SurvivorStateMap[MAX_PLAYER_STATE][MAX_PLAYER_STATE] = 
{  //  INIT   IDLE   WALK   RUN    ATK    DMG    REP    HEAL   GRO    SUM    GATK   SHEAL  HOOK   RES    RESED  DEAD                                               
	 { false, true , true , true , true , true , true , true , true , true , false, true , true , true , false, false } 		// INIT
	,{ false, false, true , true , true , true , true , true , true , false, false, true , false, true , false, false }			// IDLE
	,{ false, true , false, true , true , true , true , true , true , false, false, true , false, true , false, false }			// WALK
	,{ false, true , true , false, true , true , true , true , true , false, false, true , false, true , false, false }			// RUN
	,{ false, true , true , true , false, true , true , true , true , false, false ,false, false, false, false, false }			// ATTACK
	,{ false, true , true , true , true , false, true , true , true , false, false ,false, false, false, false, false }			// DEMAGED
	,{ false, true , true , true , true , true , false, true , true , false, false ,false, false, false, false, false }			// GENERATOR_REPAIR
	,{ false, true , true , true , true , true , true , false, true , false, false ,false, false, false, false, false }			// HEALING
	,{ false, true , true , false, false, false, false, false, false, true , false ,false, true , false, false, true  }			// GROGGY
	,{ false, false, false, false, false, false, false, false, false, false, false ,false, true , false, false, false }			// SUMMONS
	,{ false, false, false, false, false, false, false, false, false, false, false ,false, false, false, false, false }			// GENBREAK
	,{ false, true , true , true , false, false, false, false, false, false, false ,false, false, false, false, false }			// SELFHEALING
	,{ false, true , true , true , false, false, false, false, false, false, false ,false, false, false, true , true  }			// HOOK
	,{ false, true , true , true , false, false, false, false, false, false, false ,false, false, false, false, false }			// RESCUE
	,{ false, true , true , true , false, false, false, false, false, false, false ,false, false, false, false, false }			// RESCUED
	,{ false, false, false, false, false, false, false, false, false, false, false ,false, false, false, false, false }			// DEAD
};


CSurvivorFSM::CSurvivorFSM()
	: m_SurvivorRen(nullptr), m_bGroggyMotion(false), m_SurvivorHook(nullptr)
{
}

CSurvivorFSM::CSurvivorFSM(CPlayer* _pPlayer)
	:CPlayerFSM(_pPlayer), m_SurvivorRen(nullptr), m_bGroggyMotion(false)
{

}
CSurvivorFSM::~CSurvivorFSM()
{
}

void CSurvivorFSM::Init()
{
	//PLAYERSCRIPT->TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));
	//PLAYERSCRIPT->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));
	PLAYERSCRIPT->SetPlayerRole(PLAYER_SURVIVOR);
	m_SurvivorRen = PLAYERACTOR->AddCom<CDissolveSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_SurvivorRen->SetFbx(L"survivor");
	m_SurvivorRen->SetAllFbxMesh();

	m_SurvivorRen->CreateBoneAnimation(L"Idle", 0);
	m_SurvivorRen->CreateBoneAnimation(L"Crouch_Walk", 1, 0, 20);
	m_SurvivorRen->CreateBoneAnimation(L"Herted_Walk", 2, 0, 39);
	m_SurvivorRen->CreateBoneAnimation(L"Walk", 3, 0, 61)->SetAnimationSpeed(1.8f);
	m_SurvivorRen->CreateBoneAnimation(L"Run", 4, 0, 19);
	m_SurvivorRen->CreateBoneAnimation(L"Herted_Run", 5, 0, 19);
	m_SurvivorRen->CreateBoneAnimation(L"Herted_Run2", 6, 0, 29);
	m_SurvivorRen->CreateBoneAnimation(L"Hit1", 7);
	m_SurvivorRen->CreateBoneAnimation(L"Hit2", 8);
	m_SurvivorRen->CreateBoneAnimation(L"Healing", 9);
	m_SurvivorRen->CreateBoneAnimation(L"Groggy", 10);
	m_SurvivorRen->CreateBoneAnimation(L"SelfHealing", 11);
	m_SurvivorRen->CreateBoneAnimation(L"Hurted_Idle", 13);
	m_SurvivorRen->CreateBoneAnimation(L"Herted_Crouch", 14);
	m_SurvivorRen->CreateBoneAnimation(L"hookedfree", 15);
	m_SurvivorRen->CreateBoneAnimation(L"hookedidle", 16);
	m_SurvivorRen->CreateBoneAnimation(L"hookedstruggle", 17);
	m_SurvivorRen->CreateBoneAnimation(L"Repair", 18);
	m_SurvivorRen->CreateBoneAnimation(L"RepairFail", 19);
	m_SurvivorRen->CreateBoneAnimation(L"Killedspider_In", 20);
	m_SurvivorRen->CreateBoneAnimation(L"Killedspider_Loop", 21);
	m_SurvivorRen->CreateBoneAnimation(L"Killedspider_Out", 22);
	m_SurvivorRen->CreateBoneAnimation(L"Rescue", 23);
	m_SurvivorRen->CreateBoneAnimation(L"Rescuehooked", 24);

	m_SurvivorRen->SetAnimationSubTrans(L"Rescuehooked", Vec3(0.f, 50.f, 0.f), Vec3(0.f, 180.f, 0.f));
	m_SurvivorRen->SetAnimationSubTrans(L"Killedspider_In", Vec3(0.f, -65.f, 0.f), Vec3());
	m_SurvivorRen->SetAnimationSubTrans(L"Killedspider_Loop", Vec3(0.f,-65.f, 0.f), Vec3());
	m_SurvivorRen->SetAnimationSubTrans(L"Killedspider_Out", Vec3(0.f, -65.f, 0.f), Vec3());

	m_SurvivorRen->ChangeBoneAnimation(L"Idle");

	InitializeSurvivorCollider();
	PLAYERSCRIPT->CreateFootPrintDummy();

	CPlayerFSM::Init();
}

void CSurvivorFSM::PostUpdate(const float& _DeltaTime)
{
	if (REMOTE_PLAYER == PLAYERSCRIPT->m_eNetType) 
	{
		return;
	}


	switch (PLAYERSCRIPT->m_eFocusing)
	{
	case FOCUSING_HEALING_PLAYER:
	{
		if (nullptr != PLAYERSCRIPT->m_FocusPlayer)
		{
			PLAYERSCRIPT->m_pRule->SetMainGauge(PLAYERSCRIPT->m_FocusPlayer->m_HealingCount / MAXHEALINGCOUNT);
		}
	}
	break;
	case FOCUSING_RESCUE_PLAYER:
	{
		if (nullptr != PLAYERSCRIPT->m_FocusPlayer)
		{
			PLAYERSCRIPT->m_pRule->SetMainGauge(PLAYERSCRIPT->m_FocusPlayer->m_HookCount / MAXHOOKCOUNT);
		}
	}
	break;
	case FOCUSING_GENERATOR: 
	{	
		if (nullptr != PLAYERSCRIPT->m_FocusGenMagicCircle) 
		{
			PLAYERSCRIPT->m_pRule->SetMainGauge(PLAYERSCRIPT->m_FocusGenMagicCircle->GetGeneratorGauge());
		}
	}
		break;
	default:
		break;
	}
}
const bool CSurvivorFSM::CheckTransferableState(const PLAYER_STATE& _State)
{
	return SurvivorStateMap[m_CurPlayerState][_State];
}

void CSurvivorFSM::Idle(const float& _DeltaTime) 
{
	CPlayerFSM::Idle(_DeltaTime);

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
	{
		return;
	}

	if (true == PLAYERSCRIPT->SurvivorFocusingUpdate())
	{
		PLAYERSCRIPT->m_pRule->ChangeFocusingObjEvent_Survivor(PLAYERSCRIPT);
	}

	if (true == KEYDOWN("Space")) 
	{
		switch (PLAYERSCRIPT->m_eFocusing)
		{
		case FOCUSING_HEALING_PLAYER:
		{
			m_TreatPlayer = PLAYERSCRIPT->m_FocusPlayer;
			ChangePlayerState(PLAYER_HEALING);
		}
		break; 
		case FOCUSING_RESCUE_PLAYER:
		{
			m_RescuedPlayer = PLAYERSCRIPT->m_FocusPlayer;
			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = m_RescuedPlayer->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUEPLAYER);
		}
		break;
		case FOCUSING_GENERATOR:
		{
			ChangePlayerState(PLAYER_REPIRE);
		}
		break;
		case FOCUSING_TRAP:
		{
			CircleData Data;
			Data.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Data.CircleID = PLAYERSCRIPT->m_FocusTrapMagicCircle->GetCircleID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_TRAP);
			PLAYERSCRIPT->m_FocusTrapMagicCircle->ActiveTrap();
		}
		break;
		case FOCUSING_NONE:
		{

		}
		break;
		default:
			break;
		}
	}
	
}

void CSurvivorFSM::Walk(const float& _DeltaTime)
{
	
	CPlayerFSM::Walk(_DeltaTime);
	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
	{
		return;
	}

	if (true == PLAYERSCRIPT->SurvivorFocusingUpdate())
	{
		PLAYERSCRIPT->m_pRule->ChangeFocusingObjEvent_Survivor(PLAYERSCRIPT);
	}

	if (true == KEY("Space"))
	{
		switch (PLAYERSCRIPT->m_eFocusing)
		{
		case FOCUSING_HEALING_PLAYER:
		{
			m_TreatPlayer = PLAYERSCRIPT->m_FocusPlayer;
			ChangePlayerState(PLAYER_HEALING);
		}
		break; 
		case FOCUSING_RESCUE_PLAYER:
		{
			m_RescuedPlayer = PLAYERSCRIPT->m_FocusPlayer;

			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = m_RescuedPlayer->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUEPLAYER);
			//ChangePlayerState(PLAYER_RESCUE);
		}
		break;
		case FOCUSING_GENERATOR: 
		{
			ChangePlayerState(PLAYER_REPIRE);
		}
		break;
		case FOCUSING_TRAP: 
		{
			CircleData Data;
			Data.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Data.CircleID = PLAYERSCRIPT->m_FocusTrapMagicCircle->GetCircleID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_TRAP);
			PLAYERSCRIPT->m_FocusTrapMagicCircle->ActiveTrap();
		}
			break;
		case FOCUSING_NONE: 
		{
			
		}
			break;
		default:
			break;
		}
		
	}

	if (true == KEY("Run"))
	{
		ChangePlayerState(PLAYER_RUN);
		return;
	}
}
void CSurvivorFSM::Run(const float& _DeltaTime)
{
	CPlayerFSM::Run(_DeltaTime);

	if (CTimerMgr::Pulse(0.5f))
	{
		PLAYERSCRIPT->GetFootPrintActor()->AddCom<CFootPrints>(PLAYERACTOR->GetWorldPos());
	}

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
	{
		return;
	}

	if (true == PLAYERSCRIPT->SurvivorFocusingUpdate())
	{
		PLAYERSCRIPT->m_pRule->ChangeFocusingObjEvent_Survivor(PLAYERSCRIPT);
	}

	if (true == KEY("Space"))
	{
		switch (PLAYERSCRIPT->m_eFocusing)
		{
		case FOCUSING_HEALING_PLAYER:
		{
			m_TreatPlayer = PLAYERSCRIPT->m_FocusPlayer;
			ChangePlayerState(PLAYER_HEALING);
		}
		break;
		case FOCUSING_RESCUE_PLAYER:
		{
			m_RescuedPlayer = PLAYERSCRIPT->m_FocusPlayer;
			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = m_RescuedPlayer->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUEPLAYER);
		}
		break;
		case FOCUSING_GENERATOR:
		{
			//PLAYERSCRIPT->SendFocusingCircle();
			ChangePlayerState(PLAYER_REPIRE);
		}
		break;
		case FOCUSING_TRAP:
		{
			//PLAYERSCRIPT->SendFocusingCircle();
			CircleData Data;
			Data.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Data.CircleID = PLAYERSCRIPT->m_FocusTrapMagicCircle->GetCircleID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Data,sizeof(Data),FLAG_PLAYING_RULE_SIGNAL,FLAG_DATA_TRAP);
			PLAYERSCRIPT->m_FocusTrapMagicCircle->ActiveTrap();
		}
		break;
		case FOCUSING_NONE:
		{

		}
		break;
		default:
			break;
		}
	}
	
	if (true == KEYUP("Run"))
	{
		ChangePlayerState(PLAYER_WALK);
		return;
	}
}

void CSurvivorFSM::Repair(const float& _DeltaTime)
{
	CPlayerFSM::Repair(_DeltaTime);
	
	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
		return;

	if (true == CheckMoveKeyInput())
	{
		PLAYERSCRIPT->m_FocusGenMagicCircle->CancelRepairGenerator();
		ChangePlayerState(PLAYER_WALK);
		return;
	}

	if (PLAYERSCRIPT->m_FocusGenMagicCircle->IsMaximumGeneratorCount()) 
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

	CircleData Data;
	Data.PlayerID = m_pPairPlayer->GetPlayerID();
	Data.CircleID = PLAYERSCRIPT->m_FocusGenMagicCircle->GetCircleID();
	Data.Value.Repair = _DeltaTime;
	GETSINGLE(ClientConnectMgr).SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_GANERATOR);

}
void CSurvivorFSM::Damaged(const float& _DeltaTime)
{
	CPlayerFSM::Damaged(_DeltaTime);

	PLAYERSCRIPT->m_pRule->UpdateSurvivorUI(PLAYERSCRIPT);

	if (PLAYERSCRIPT->m_eNetType == LOCAL_PLAYER) 
	{
		PLAYERSCRIPT->m_pRule->UpdateLiftUI(PLAYERSCRIPT);
	}
}
void CSurvivorFSM::Groggy(const float& _DeltaTime)
{
	CPlayerFSM::Groggy(_DeltaTime);

	if (true == m_bGroggyMotion)
		return;

	if (m_fAccTime >= 2.5f) 
	{
		m_SurvivorRen->ChangeBoneAnimation(L"Groggy", 0.5f);
		
		m_bGroggyMotion = true;
	}
	
	m_fAccTime += _DeltaTime;

}
void CSurvivorFSM::Summons(const float& _DeltaTime)
{
	if (true == CTimerMgr::TimingCheck(m_fAccTime, 1.5f)) 
	{
		m_SurvivorRen->SetDissolve(1.5f);
	}

	CPlayerFSM::Summons(_DeltaTime);
}
void CSurvivorFSM::Dead(const float& _DeltaTime)
{
	CPlayerFSM::Dead(_DeltaTime);

	if (true == CTimerMgr::TimingCheck(m_fAccTime, 3.f)) 
	{
		m_SurvivorRen->ChangeBoneAnimation(L"Killedspider_Loop");
	}
	else if (true == CTimerMgr::TimingCheck(m_fAccTime, 5.f)) 
	{
		m_SurvivorRen->ChangeBoneAnimation(L"Killedspider_Out");
	}
	else if (true == CTimerMgr::TimingCheck(m_fAccTime, 15.f)) 
	{
		
		PLAYERACTOR->Off();
		if (PLAYERSCRIPT->m_eNetType == LOCAL_PLAYER) 
		{
			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = PLAYERSCRIPT->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_DEAD_OUT);
		}
		return;
	}
}

void CSurvivorFSM::Healing(const float& _DeltaTime)
{
	//CPlayerFSM::Healing(_DeltaTime);

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
		return;

	if (true == CheckMoveKeyInput())
	{
		ChangePlayerState(PLAYER_WALK);
		return;
	}

	if (nullptr == m_TreatPlayer || false == m_TreatPlayer->IsIdle() )
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

	if (m_TreatPlayer->m_HealingCount >= MAXHEALINGCOUNT || m_TreatPlayer->m_LifeCount >= MAXSURVIVORLIFE) 
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}
	
	PlayerSignal Signal;
	Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
	Signal.TargetID = m_TreatPlayer->GetPlayerID();
	Signal.Value.DeltaHeal = _DeltaTime;
	GETSINGLE(ClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_HEALINGPLAYER);
	
}
void CSurvivorFSM::Hook(const float& _DeltaTime)
{
	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
		return;

	if (m_fAccTime <= 1.8f)
	{
		m_fAccTime += _DeltaTime;
		return;
	}

	if (true == PLAYERSCRIPT->IsRescued())
		return;


	PlayerSignal Signal;
	Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
	Signal.TargetID = PLAYERSCRIPT->GetPlayerID();
	Signal.Value.Hook = _DeltaTime;
	GETSINGLE(ClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_ACCHOOK);

}

void CSurvivorFSM::Rescue(const float& _DeltaTime) 
{
	CPlayerFSM::Rescue(_DeltaTime);

	if (m_fAccTime >= 1.8f)
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

}
void CSurvivorFSM::Rescued(const float& _DeltaTime) 
{
	m_fAccTime += _DeltaTime;

	if (TIMING(m_fAccTime,_DeltaTime,1.8f) && PLAYERSCRIPT->m_eNetType == LOCAL_PLAYER )
	{
		PlayerSignal Signal;
		Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
		Signal.TargetID = PLAYERSCRIPT->GetPlayerID();
		GETSINGLE(CClientConnectMgr).SendData((char*)&Signal,sizeof(Signal),FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUED_COMPETE);
		return;
	}

}
void CSurvivorFSM::ChangeIdleEvent()
{
	CPlayerFSM::ChangeIdleEvent();
	switch (PLAYERSCRIPT->m_LifeCount)
	{
	case 3:
		m_SurvivorRen->ChangeBoneAnimation(L"Idle", 0.2f);
		break;
	case 2:
	case 1:
		m_SurvivorRen->ChangeBoneAnimation(L"Hurted_Idle", 0.2f);
		break;
	default:
		break;
	}
}

void CSurvivorFSM::ChangeWalkEvent()
{
	CPlayerFSM::ChangeWalkEvent();
	switch (PLAYERSCRIPT->m_LifeCount)
	{
	case 3:
		m_SurvivorRen->ChangeBoneAnimation(L"Walk", 0.2f);
		break;
	case 2:
	case 1:
		m_SurvivorRen->ChangeBoneAnimation(L"Herted_Walk", 0.2f);
		break;
	default:
		break;
	}
}

void CSurvivorFSM::ChangeRunEvent()
{
	CPlayerFSM::ChangeRunEvent();
	switch (PLAYERSCRIPT->m_LifeCount)
	{
	case 3:
		m_SurvivorRen->ChangeBoneAnimation(L"Run", 0.2f);
		break;
	case 2:
		m_SurvivorRen->ChangeBoneAnimation(L"Herted_Run", 0.2f);
		break;
	case 1:
		m_SurvivorRen->ChangeBoneAnimation(L"Herted_Run2", 0.2f);
		break;
	default:
		break;
	}

}

void CSurvivorFSM::ChangeRepairEvent()
{
	CSoundPlayer::GlobalPlaySound(L"Generator");
	CPlayerFSM::ChangeRepairEvent();
	m_SurvivorRen->ChangeBoneAnimation(L"Repair", 0.3f);
}
void CSurvivorFSM::ChangeDamagedEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PHit");
	CSoundPlayer::GlobalPlaySound(L"KBlood");
	//ÇÇ »Ñ¸®±â
	//PLAYERACTOR->SCENE->GetUICamera()->ACTOR->AddCom<CBloodScript>(BLOODDATA{ PLAYERACTOR->SCENE->GetUICamera(), BLOOD_TYPE::BT_SURVIVOR });
}
void CSurvivorFSM::ChangeGroggyEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PSecondHit");
	m_SurvivorRen->ChangeBoneAnimation(L"Hit1", 0.2f);
	m_bGroggyMotion = false;

	//ÇÇ »Ñ¸®±â
	//PLAYERACTOR->SCENE->GetUICamera()->ACTOR->AddCom<CBloodScript>(BLOODDATA{ PLAYERACTOR->SCENE->GetUICamera(), BLOOD_TYPE::BT_SURVIVOR });
}
void CSurvivorFSM::ChangeSummonsEvent()
{
	
}
void CSurvivorFSM::ChangeHealingEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PBreath");

	m_SurvivorRen->ChangeBoneAnimation(L"Healing", 0.2f);
	CPlayerFSM::ChangeHealingEvent();
}
void CSurvivorFSM::ChangeSelfHealingEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PHeal");
	m_SurvivorRen->ChangeBoneAnimation(L"SelfHealing", 0.2f);
	CPlayerFSM::ChangeSelfHealingEvent();
}

void CSurvivorFSM::ChangeHookEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PHang");

	m_SurvivorRen->ChangeBoneAnimation(L"hookedidle", 0.2f);
	m_SurvivorRen->SetReverseDissolve(1.5f);
	
	m_SurvivorHook = PLAYERSCRIPT->m_pRule->PopHook();
	m_SurvivorHook->HookActive(PLAYERSCRIPT->m_DeathCount, PLAYERSCRIPT);
}
void CSurvivorFSM::ChangeRescueEvent() 
{
	m_SurvivorRen->ChangeBoneAnimation(L"Rescue");
}
void CSurvivorFSM::ChangeRescuedEvent()
{
	m_SurvivorRen->ChangeBoneAnimation(L"Rescuehooked");

	CPlayerFSM::ChangeRescuedEvent();
}
void CSurvivorFSM::ChangeDeadEvent()
{
	CSoundPlayer::GlobalPlaySound(L"PDead");
	m_SurvivorRen->ChangeBoneAnimation(L"Killedspider_In");
	m_SurvivorHook->ChangeHookState(HOOK_SPIDERIN);

}
void CSurvivorFSM::DissolveEvent()
{
	m_SurvivorRen->SetDissolve(1.5f);
}
void CSurvivorFSM::HalfHookEvent() 
{
	if (nullptr == m_SurvivorHook)
		return;

	m_SurvivorHook->SpiderActive();
}

void CSurvivorFSM::RescuedEvent()
{
	if (m_CurPlayerState != PLAYER_RESCUED)
		return;

	

	PLAYERSCRIPT->m_LifeCount = 1;
	m_SurvivorHook->HookExtinction();
	m_SurvivorHook = nullptr;
	ChangePlayerState(PLAYER_IDLE);
}