#include "FreeCameraScript.h"
#include <KeyMgr.h>
#include <TransForm.h>
#include <SSDCore.h>
#include <Light.h>
#include <Actor.h>

CFreeCameraScript::CFreeCameraScript()
	:m_fRotSpeed(180.f), m_fMoveSpeed(500.f)
{

}


CFreeCameraScript::~CFreeCameraScript()
{

}

const bool CFreeCameraScript::Init()
{
	//m_CamLight=  ACTOR->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	//m_CamLight->SetLightColor(Vec4::White);
	//m_CamLight->SetLightDir(TRANS->GetFront());
	

	return true;
}
void CFreeCameraScript::Update(const float& _DeltaTime)
{
	//m_CamLight->SetLightDir(TRANS->GetFront());
	FreeMoveUpdate(_DeltaTime);
	FreeRotateUpdate(_DeltaTime);
}
void CFreeCameraScript::PostUpdate(const float& _DeltaTime)
{
	if (true == KEYDOWN("CAM_RESET"))
	{
		TRANS->SetPos(Vec3());
		TRANS->SetRotate(Vec3());
		return;
	}
}
void CFreeCameraScript::FreeMoveUpdate(const float& _DeltaTime) 
{
	Vec3 MoveDir = Vec3();

	if (true == KEY("CAM_FRONT"))
		MoveDir += TRANS->GetFront();

	if (true == KEY("CAM_BACK"))
		MoveDir += TRANS->GetBack();

	//if (true == KEY("CAM_UP"))
	//	MoveDir += TRANS->GetUp();

	//if (true == KEY("CAM_DOWN"))
	//	MoveDir += TRANS->GetDown();

	if (true == KEY("CAM_LEFT"))
		MoveDir += TRANS->GetLeft();

	if (true == KEY("CAM_RIGHT"))
		MoveDir += TRANS->GetRight();

	if (MoveDir == Vec3())
		return;

	if (true == KEY("CAM_BOOST"))
		TRANS->Move(MoveDir, MOVEBOOST*m_fMoveSpeed, _DeltaTime);
	else
		TRANS->Move(MoveDir, m_fMoveSpeed, _DeltaTime);
}
void CFreeCameraScript::FreeRotateUpdate(const float& _DeltaTime)
{
	if (false == KEY("CAM_HOLD") || false == Core::CheckMouseOnScreen())
		return;

	Vec3 Rot;
	Rot.x = CKeyMgr::GetMouseDir().y;
	Rot.y = CKeyMgr::GetMouseDir().x;

	if (Rot == Vec3())
		return;

	TRANS->AccRotate(Rot * m_fRotSpeed *_DeltaTime);
}