#include "KillerFSM.h"
#include "Player.h"
#include <TransForm.h>
#include <Actor.h>
#include <SkeletalFbxRenderer.h>
#include <OBBCollider.h>
#include <SphereCollider.h>
#include <KeyMgr.h>
#include "DBDRule.h"
#include "ClientConnectMgr.h"

#include <Sound.h>
#include <SoundPlayer.h>

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
//  PLAYER_SELFHEALING,
//  PLAYER_HOOK,
//  PLAYER_RESCUE,
//  PLAYER_RESCUED,
//	MAX_PLAYER_STATE
//};

bool	CKillerFSM::KillerStateMap[MAX_PLAYER_STATE][MAX_PLAYER_STATE] =
{	//  INIT   IDLE   WALK   RUN    ATK    DMG    REP    HEAL   GRO    SUM    GATK   SHEAL  HOOK   RES    RESED  DEAD  
	 { false, true,	 true,  true,  true,  true, true, true , true, true, true, false, false, false, false, false}							// INIT
	,{ false, false, false, true,  true,  true, false, false , false, true, true, false, false, false, false, false }					// IDLE
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// WALK
	,{ false, true,  false, false, true,  true, false, false, false, true, true, false, false, false, false, false }						// RUN
	,{ false, true,  false, true,  false, false, false, false, false, true, false, false, false, false, false, false }					// ATTACK
	,{ false, true,  false, true,  false, false, false, false, false, true, false, false, false, false, false, false }					// DEMAGED
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// GENERATOR_REPAIR
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// HEALING
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// GROGGY
	,{ false, true,  false, true,  false, false, false, false, false, false, false, false, false, false, false, false }					// SUMMONS
	,{ false, true,  false, true,  false, false, false, false, false, false, false, false, false, false, false, false }					// GENBREAK
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// SELFHEALING
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// HOOK
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// RESCUE
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// RESCUED
	,{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }				// DEAD
};


CKillerFSM::CKillerFSM()
	:m_KillerRen(nullptr)
{
}

CKillerFSM::CKillerFSM(CPlayer* _pPlayer)
	:CPlayerFSM(_pPlayer)
{

}
CKillerFSM::~CKillerFSM()
{
}

void CKillerFSM::Init()
{
	//PLAYERSCRIPT->TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));
	//PLAYERSCRIPT->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));
	PLAYERSCRIPT->SetPlayerRole(PLAYER_KILLER);
	m_KillerRen = PLAYERACTOR->AddCom<CSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_KillerRen->SetFbx(L"killer");

	m_KillerRen->CreateBoneAnimation(L"Idle", 0);
	m_KillerRen->CreateBoneAnimation(L"Walk", 1, 0, 20);
	m_KillerRen->CreateBoneAnimation(L"Run", 2, 0, 20);
	m_KillerRen->CreateBoneAnimation(L"Attack", 3);
	m_KillerRen->CreateBoneAnimation(L"GenAttack", 4);
	m_KillerRen->CreateBoneAnimation(L"Stun", 5); 
	m_KillerRen->CreateBoneAnimation(L"ExceedWindow", 12);
	m_KillerRen->CreateBoneAnimation(L"Summons", 13);


	m_KillerRen->ChangeBoneAnimation(L"Idle");

	InitializeKillerCollider();
	PLAYERSCRIPT->CreatePlayerEyeLight();
	CPlayerFSM::Init();
}

const bool CKillerFSM::CheckTransferableState(const PLAYER_STATE& _State)
{
	return KillerStateMap[m_CurPlayerState][_State];
}
void CKillerFSM::Idle(const float& _DeltaTime)
{
	CPlayerFSM::Idle(_DeltaTime);

	if (true == KEY("Attack")) 
	{
		ChangePlayerState(PLAYER_ATTACK); 
		return;
	}

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
	{
		return;
	}

	if (true == PLAYERSCRIPT->KillerFocusingUpdate())
	{
		PLAYERSCRIPT->m_pRule->ChangeFocusingObjEvent_Killer(PLAYERSCRIPT);
	}

	if (true == KEY("Space"))
	{
		switch (PLAYERSCRIPT->m_eFocusing)
		{
		case FOCUSING_SUMMONS_PLAYER:
		{
			m_HookPlayer = PLAYERSCRIPT->m_FocusPlayer;
			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = PLAYERSCRIPT->m_FocusPlayer->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_SUMMONSPLAYER);
			//ChangePlayerState(PLAYER_SUMMONS);
			
		}
		break;
		case FOCUSING_GENERATOR:
		case FOCUSING_TRAP:
			//ChangePlayerState(PLAYER_GENBREAK);
			break;
		break;
		case FOCUSING_NONE:
			break;
		default:
			break;
		}
	}
}


void CKillerFSM::Run(const float& _DeltaTime) 
{
	CPlayerFSM::Run(_DeltaTime);

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER)
	{
		return;
	}

	if (true == KEY("Attack"))
	{
		ChangePlayerState(PLAYER_ATTACK);
		return;
	}

	if (true == PLAYERSCRIPT->KillerFocusingUpdate())
	{
		PLAYERSCRIPT->m_pRule->ChangeFocusingObjEvent_Killer(PLAYERSCRIPT);
	}

	if (true == KEY("Space"))
	{
		switch (PLAYERSCRIPT->m_eFocusing)
		{
		case FOCUSING_SUMMONS_PLAYER:
		{
			m_HookPlayer = PLAYERSCRIPT->m_FocusPlayer;
			PlayerSignal Signal;
			Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
			Signal.TargetID = PLAYERSCRIPT->m_FocusPlayer->GetPlayerID();
			GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_SUMMONSPLAYER);
		}
		break;
		case FOCUSING_GENERATOR:
		case FOCUSING_TRAP:
			//ChangePlayerState(PLAYER_GENBREAK);
			break;
			break;
		case FOCUSING_NONE:
			break;
		default:
			break;
		}
	}
}

void CKillerFSM::Attack(const float& _DeltaTime)
{
	CPlayerFSM::Attack(_DeltaTime);

	if (true == m_KillerRen->CheckAnimationEnd()) 
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

}
void CKillerFSM::Damaged(const float& _DeltaTime)
{

	CPlayerFSM::Damaged(_DeltaTime);
}
void CKillerFSM::GenBreak(const float& _DeltaTime)
{
	if (true == m_KillerRen->CheckAnimationEnd())
	{
		ChangePlayerState(PLAYER_IDLE);
		return;
	}

	CPlayerFSM::GenBreak(_DeltaTime);
}
void CKillerFSM::Summons(const float& _DeltaTime)
{
	CPlayerFSM::Summons(_DeltaTime);

	if (PLAYERSCRIPT->m_eNetType == REMOTE_PLAYER) 
	{
		if (true == m_KillerRen->CheckAnimationEnd()) 
		{
			ChangePlayerState(PLAYER_IDLE);
			return;
		}

		return;
	}

	if (true == TIMING(m_fAccTime, _DeltaTime, 1.5f)) 
	{
		PlayerSignal Signal;
		Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
		Signal.TargetID = m_HookPlayer->GetPlayerID();
		GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_DISSOLVEPLAYER);
	}


	if (m_fAccTime >= 3.5f)
	{
		PlayerSignal Signal;
		Signal.PlayerID = PLAYERSCRIPT->GetPlayerID();
		Signal.TargetID = m_HookPlayer->GetPlayerID();
		GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_HOOKPLAYER);
		ChangePlayerState(PLAYER_IDLE);
		m_HookPlayer = nullptr;
		return;
	}
}
void CKillerFSM::ChangeIdleEvent()
{
	m_KillerRen->ChangeBoneAnimation(L"Idle",0.25f);
	CPlayerFSM::ChangeIdleEvent();

}

void CKillerFSM::ChangeRunEvent()
{
	m_KillerRen->ChangeBoneAnimation(L"Run", 0.15f);
	CPlayerFSM::ChangeRunEvent();
}

void CKillerFSM::ChangeAttackEvent()
{
	CSoundPlayer::GlobalPlaySound(L"KHitFail", 0.6f);
	m_KillerRen->ChangeBoneAnimation(L"Attack", 0.15f);
	CPlayerFSM::ChangeAttackEvent();
}

void CKillerFSM::ChangeGenBreakEvent()
{
	m_KillerRen->ChangeBoneAnimation(L"GenAttack", 0.15f);
	CPlayerFSM::ChangeGenBreakEvent();
}
void CKillerFSM::ChangeDamagedEvent()
{
	CSoundPlayer::GlobalPlaySound(L"KKillerHit");
	m_KillerRen->ChangeBoneAnimation(L"Stun", 0.15f);
	CPlayerFSM::ChangeDamagedEvent();
}
void CKillerFSM::ChangeSummonsEvent()
{
	CSoundPlayer::GlobalPlaySound(L"KPlayerHang");
	m_KillerRen->ChangeBoneAnimation(L"Summons", 0.2f);
	CPlayerFSM::ChangeSummonsEvent();
}