#include "Door.h"
#include "DeadByDaylightGlobal.h"
#include <ParticleRenderer.h>
#include <Sound.h>
#include <SoundPlayer.h>
#include <OBBCollider.h>
#include <Actor.h>
#include "Player.h"
#include "ClientConnectMgr.h"

CDoor::CDoor()
{
	ZeroMemory(m_pDoorCol, sizeof(m_pDoorCol));
}


CDoor::~CDoor()
{
}

const bool CDoor::Init()
{
	//¹® °á°è
	DBD::pMainParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 0.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	DBD::pMainParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, -50.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });

	DBD::pMainParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6400.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	DBD::pMainParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6450.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });


	m_pDoorCol[FRONT_DOOR] = ACTOR->AddCom<COBBCollider>(COLDATA{ L"PlayerDetect",COLLISION_3D });
	m_pDoorCol[FRONT_DOOR]->SetColFi(Vec3(3200.f, 0.f, 6400.f), Vec3(300.f, 1000.f, 200.f));
	m_pDoorCol[FRONT_DOOR]->Off();
	m_pDoorCol[FRONT_DOOR]->SetEnterCallBack(this, &CDoor::EscapeEvent);

	m_pDoorCol[BACK_DOOR] = ACTOR->AddCom<COBBCollider>(COLDATA{ L"PlayerDetect",COLLISION_3D });
	m_pDoorCol[BACK_DOOR]->SetColFi(Vec3(3200.f, 0.f,  0.f), Vec3(300.f, 1000.f, 200.f));
	m_pDoorCol[BACK_DOOR]->Off();
	m_pDoorCol[BACK_DOOR]->SetEnterCallBack(this, &CDoor::EscapeEvent);

	return true;
}

void CDoor::Update(const float & _DeltaTime)
{
}

void CDoor::LastUpdate(const float & _DeltaTime)
{
}

void CDoor::DebugRender()
{
}

void CDoor::OpenEscapeDoor()
{
	m_pDoorCol[FRONT_DOOR]->On();
	m_pDoorCol[BACK_DOOR]->On();
}
void CDoor::EscapeEvent(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	if (false == _Other->ExistTag(L"Survivor")) 
	{
		return;
	}

	SPTR<CPlayer> EsapePlayer =  _Other->ACTOR->FindCom<CPlayer>();

	if (PLAYER_SURVIVOR != EsapePlayer->GetPlayerRole())
	{
		return;
	}

	PlayerSignal Signal;
	Signal.PlayerID = EsapePlayer->GetPlayerID();
	Signal.TargetID = EsapePlayer->GetPlayerID();
	GETSINGLE(CClientConnectMgr).SendData((char*)&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_ESCAPE);

}