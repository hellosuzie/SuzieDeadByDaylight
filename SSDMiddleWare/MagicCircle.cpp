#include "MagicCircle.h"
#include <FbxRenderer.h>
#include <Actor.h>
#include <TransForm.h>
#include <SphereCollider.h>
#include <Material.h>
#include "Player.h"
#include "PlayerFSM.h"
#include <TimerMgr.h>
#include <Sound.h>
#include <SoundPlayer.h>

CMagicCircle::CMagicCircle()
	:m_CircleColor(MC_MAX), m_PlayerDetectCol(nullptr), m_MagicSphere(nullptr), m_CircleRen(nullptr)
	, m_GeneratorCount(0), m_eGeneratorState(MAX_MAGICCIRCLE_GENSTATE), m_pMagicCircleUpdate(nullptr)
	, m_fAccTime(0.f), m_eBoardState(MAX_MAGICCIRCLE_BOARDSTATE), m_KillerAttackCol(nullptr)
	, m_CircleID(-1), m_bServerFlag(false)
{
}


CMagicCircle::~CMagicCircle()
{
}

const bool CMagicCircle::Init(MAGICCIRCLE_COLOR _Color)
{
	m_CircleColor = _Color;
	m_CircleRen = ACTOR->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_ALPHA });

	switch (m_CircleColor)
	{
	case MC_GREEN:
		m_CircleRen->SetFbx(L"Green");
		m_CircleRen->SetRenSize(Vec3(43.f, 43.f, 43.f));
		m_CircleRen->SetRenPivot(Vec3(0.f, 10.f, 0.f));
	
		m_MagicSphere = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"Default", COLLISION_3D });
		m_MagicSphere->SetColSize(Vec3(45.f, 45.f, 45.f));
		m_MagicSphere->SetDebugColor(Vec4::PastelGreen.vec3);

		m_PlayerDetectCol = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"PlayerDetect", COLLISION_3D });
		m_PlayerDetectCol->SetColSize(Vec3(100.f, 100.f, 100.f));
		m_PlayerDetectCol->SetDebugColor(Vec4::PastelGreen.vec3);
		m_PlayerDetectCol->SetEnterCallBack(this, &CMagicCircle::GeneratorActiveEvent);
		m_PlayerDetectCol->SetExitCallBack(this, &CMagicCircle::GeneratorActiveEnd);

		m_pMagicCircleUpdate = &CMagicCircle::GeneratorUpdate;
		m_eGeneratorState = GENERATOR_IDLE;
		break;
	case MC_RED:
		m_CircleRen->SetFbx(L"Red");
		m_CircleRen->SetRenSize(Vec3(8.f, 8.f, 8.f));
		m_CircleRen->SetRenPivot(Vec3(0.f, 10.f, 0.f));
		
		m_MagicSphere = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"Default", COLLISION_3D });
		m_MagicSphere->SetColSize(Vec3(55.f, 55.f, 55.f));
		m_MagicSphere->SetDebugColor(Vec4::PastelRed.vec3);
		m_MagicSphere->Off();

		m_PlayerDetectCol = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"PlayerDetect", COLLISION_3D });
		m_PlayerDetectCol->SetColSize(Vec3(70.f, 70.f, 70.f));
		m_PlayerDetectCol->SetDebugColor(Vec4::PastelRed.vec3);
		m_PlayerDetectCol->SetEnterCallBack(this, &CMagicCircle::BoardActiveEvent);
		m_PlayerDetectCol->SetExitCallBack(this, &CMagicCircle::BoardActiveEnd);

		m_KillerAttackCol = ACTOR->AddCom<CSphereCollider>(COLDATA{ L"PlayerDetect", COLLISION_3D });
		m_KillerAttackCol->SetColSize(Vec3(70.f, 70.f, 70.f));
		m_KillerAttackCol->SetDebugColor(Vec4::PastelMag.vec3);
		m_KillerAttackCol->SetEnterCallBack(this, &CMagicCircle::BoardTrapAttackEvent);
		m_KillerAttackCol->Off();

		m_pMagicCircleUpdate = &CMagicCircle::BoardUpdate;	
		m_eBoardState = BOARD_IDLE;
		//m_CircleRen->SetAllFbxMesh(L"ForwardFBXMaterial");
		break;
	default:
		break;
	}
	//m_CircleRen->SetAllFbxMesh(L"ForwardFBXMaterial");
	m_CircleRen->SetAllFbxMesh();

	return true;
}
 
const bool CMagicCircle::Init(const MCData& _InitData)
{
	if (false == _InitData.bServerObj) 
	{
		return Init(_InitData.CircleColor);
	}


	m_CircleColor = _InitData.CircleColor;


	switch (m_CircleColor)
	{
	case MC_GREEN:
		m_MagicSphere = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"Default", COLLISION_3D},false });
		m_MagicSphere->SetColSize(Vec3(45.f, 45.f, 45.f));

		m_PlayerDetectCol = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"PlayerDetect", COLLISION_3D },false });
		m_PlayerDetectCol->SetColSize(Vec3(100.f, 100.f, 100.f));
		m_PlayerDetectCol->SetEnterCallBack(this, &CMagicCircle::GeneratorActiveEvent);
		m_PlayerDetectCol->SetExitCallBack(this, &CMagicCircle::GeneratorActiveEnd);

		m_pMagicCircleUpdate = &CMagicCircle::GeneratorUpdate;
		m_eGeneratorState = GENERATOR_IDLE;
		break;
	case MC_RED:
		m_MagicSphere = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"Default", COLLISION_3D },false });
		m_MagicSphere->SetColSize(Vec3(55.f, 55.f, 55.f));
		m_MagicSphere->Off();

		m_PlayerDetectCol = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"PlayerDetect", COLLISION_3D },false });
		m_PlayerDetectCol->SetColSize(Vec3(70.f, 70.f, 70.f));
		m_PlayerDetectCol->SetEnterCallBack(this, &CMagicCircle::BoardActiveEvent);
		m_PlayerDetectCol->SetExitCallBack(this, &CMagicCircle::BoardActiveEnd);

		m_KillerAttackCol = ACTOR->AddCom<CSphereCollider>(COL3D_DATA{ COLDATA{ L"Attack", COLLISION_3D },false });
		m_KillerAttackCol->SetColSize(Vec3(70.f, 70.f, 70.f));
		m_KillerAttackCol->SetEnterCallBack(this, &CMagicCircle::BoardTrapAttackEvent);
		m_KillerAttackCol->Off();

		m_pMagicCircleUpdate = &CMagicCircle::BoardUpdate;
		m_eBoardState = BOARD_IDLE;
		//m_CircleRen->SetAllFbxMesh(L"ForwardFBXMaterial");
		break;
	default:
		break;
	}


	return true;
}
void CMagicCircle::Update(const float & _DeltaTime)
{
	ACTOR->GetTrans()->AccRotate(AXIS_Y, _DeltaTime * 5.f);

	if (nullptr == m_pMagicCircleUpdate)
	{
		return;
	}

	(this->*m_pMagicCircleUpdate)(_DeltaTime);
}

void CMagicCircle::LastUpdate(const float & _DeltaTime)
{

}

void CMagicCircle::GeneratorUpdate(const float& _DeltaTime) 
{
	switch (m_eGeneratorState)
	{
	case GENERATOR_IDLE:
		break;
	case GENERATOR_REPAIL: 
	{

		m_GeneratorCount += _DeltaTime;
		if (m_GeneratorCount >= MAXGENERATORCOUNT) 
		{
			m_GeneratorCount = MAXGENERATORCOUNT;
			m_eGeneratorState = GENERATOR_REPAILCOMPLETE;
			return;
		}
	}
		break;
	case GENERATOR_DEMAGED: 
	{
		m_GeneratorCount -= _DeltaTime * BREAKFACTOR;
		if (m_GeneratorCount <= 0.f)
		{
			m_GeneratorCount = 0.f;
			m_eGeneratorState = GENERATOR_IDLE;
			m_MagicSphere->SetDebugColor(Vec4::PastelGreen.vec3);
		}
	}
		break;
	case GENERATOR_REPAILCOMPLETE:
	{
	}
	break;
	default:
		return;
	}
}
void CMagicCircle::BoardUpdate(const float& _DeltaTime) 
{
	switch (m_eBoardState)
	{
	case BOARD_IDLE: 
	{
	
	}
		break;
	case BOARD_TRAPACTIVE: 
	{
		if (true == CTimerMgr::TimingCheck(m_fAccTime,0.5f)) 
		{
			m_KillerAttackCol->Off();
			return;
		}

		m_fAccTime += _DeltaTime;
	}
		break;
	case BOARD_STOP: 
	{
		if (m_fAccTime >= RECORVERYTIME) 
		{
			m_fAccTime = 0.f;
			RecoveryCompleteTrap();
			return;
		}

		m_fAccTime += _DeltaTime;
	}
		break;
	default:
		break;
	}
}

void CMagicCircle::GeneratorActiveEvent(SPTR<class CCollider> _This, SPTR<class CCollider> _Other) 
{
	if (true == _Other->ExistTag(L"Survivor"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PushDetectMagicCircle(this);
		//ColPlayer->FocusingGenerator(this);
	}
	else if (true == _Other->ExistTag(L"Killer"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PushDetectMagicCircle(this);
		//ColPlayer->FocusingGenerator(this);
	}

	
}
void CMagicCircle::GeneratorActiveEnd(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	if (true == _Other->ExistTag(L"Survivor"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PopDetectMagicCircle(this);
		//ColPlayer->FocusingEndGenerator();
	}
	else if (true == _Other->ExistTag(L"Killer"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PopDetectMagicCircle(this);
		//ColPlayer->FocusingEndGenerator();
	}

}
void CMagicCircle::BoardActiveEvent(SPTR<class CCollider> _This, SPTR<class CCollider> _Other) 
{
	if (true == _Other->ExistTag(L"Survivor") || true == _Other->ExistTag(L"Killer"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PushDetectMagicCircle(this);
		//ColPlayer->FocusingTrap(this);
	}
}
void CMagicCircle::BoardActiveEnd(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	if (true == _Other->ExistTag(L"Survivor") || true == _Other->ExistTag(L"Killer"))
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		ColPlayer->PopDetectMagicCircle(this);
		//ColPlayer->FocusingEndTrap();
	}
}
void CMagicCircle::BoardTrapAttackEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other)
{
	if (true == _Other->ExistTag(L"Killer")) 
	{
		SPTR<CPlayer> ColPlayer = _Other->ACTOR->FindCom<CPlayer>();
		
		if (nullptr == ColPlayer)
			return;

		ColPlayer->PlayerDamagedEvent();
	}
}

void CMagicCircle::RepairGenerator()
{
	m_eGeneratorState = GENERATOR_REPAIL;
	m_MagicSphere->SetDebugColor(Vec4::PastelGreen.vec3);
}

void CMagicCircle::CancelRepairGenerator()
{
	if (m_eGeneratorState == GENERATOR_REPAILCOMPLETE)
		return;

	m_eGeneratorState = GENERATOR_IDLE;

}

void CMagicCircle::BreakGenerator()
{
	m_eGeneratorState = GENERATOR_DEMAGED;
	m_MagicSphere->SetDebugColor(Vec4::PastelMag.vec3);

}

void CMagicCircle::RepairCompleteEvent()
{
	m_eGeneratorState = GENERATOR_REPAILCOMPLETE;
}
const bool CMagicCircle::CheckRepairComplete()
{
	return m_eGeneratorState == GENERATOR_REPAILCOMPLETE;
}
const bool CMagicCircle::CheckNoRepair() 
{
	return m_GeneratorCount <= 0.f;
}

const bool CMagicCircle::CheckBreakableGenerator()
{
	return (m_GeneratorCount > 0.f) && (m_eGeneratorState != GENERATOR_REPAILCOMPLETE) && (m_eGeneratorState != GENERATOR_DEMAGED);
}

void CMagicCircle::RecoveryCompleteTrap()
{
	m_eBoardState = BOARD_IDLE;

}
void CMagicCircle::ActiveTrap()
{
	if (m_eBoardState == BOARD_TRAPACTIVE) 
	{
		return;
	}

	m_eBoardState = BOARD_TRAPACTIVE;
	m_MagicSphere->On();
	m_KillerAttackCol->On();
	m_fAccTime = 0.f;
}
void CMagicCircle::StopTrap()
{
	m_eBoardState = BOARD_STOP;
	m_MagicSphere->Off();
	m_fAccTime = 0.f;
}

const bool CMagicCircle::CheckTrapAvailable() 
{
	return m_eBoardState == BOARD_IDLE;
}
const bool CMagicCircle::CheckTrapActive() 
{
	if (true == m_KillerAttackCol->IsActive()) 
	{
		return false;
	}

	return m_eBoardState == BOARD_TRAPACTIVE;
}

SPTR<CFbxRenderer> CMagicCircle::GetMagicCircleRen()
{
	return m_CircleRen;
}

MagicCircleData CMagicCircle::GetCircleData()
{
	MagicCircleData ReturnData;
	ReturnData.Type = (int)m_CircleColor;
	ReturnData.ID = (int)m_CircleID;
	ReturnData.InitPos = TRANS->GetWorldPos();
	ReturnData.InitRotY = TRANS->GetRotate().y;
	return ReturnData;
}