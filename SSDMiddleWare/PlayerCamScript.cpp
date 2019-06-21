#include "PlayerCamScript.h"
#include <Camera.h>
#include <Scene.h>
#include <TransForm.h>

CPlayerCamScript::CPlayerCamScript() : m_PlayerCam(nullptr), m_PlayerCamActor(nullptr)
{
}


CPlayerCamScript::~CPlayerCamScript()
{
}

const bool CPlayerCamScript::Init(SPTR<class CActor> _ParentActor)
{
	m_PlayerCamActor = CreateActor(UPDATE_LAYER_NORMAL, L"TestMeshActor");
	m_PlayerCam = m_PlayerCamActor->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_OBJECT, 45.f ,1.f, 50000.f });
	m_PlayerCam->SettingViewLayer(RENDER_LAYER_MAP, RENDER_LAYER_OBJECT, RENDER_LAYER_BACKEFFECT, RENDER_LAYER_PLAYER, RENDER_LAYER_FRONTEFFECT, RENDER_LAYER_FIRST);
	m_PlayerCam->SetPerspectiveMode();

	//_ParentActor->SetChild(m_PlayerCamActor);
	Vec3 TempVec = _ParentActor->TRANS->GetBack();
	m_PlayerCamActor->TRANS->SetPos(Vec3(TempVec.x, TempVec.y, TempVec.z * 500.f));
	//m_PlayerCamActor->TRANS->SetRotate(Vec3(90.f, 0.f, 0.f));

	return true;
}

void CPlayerCamScript::Update(const float & _DeltaTime)
{
}

void CPlayerCamScript::LastUpdate(const float & _DeltaTime)
{
}

void CPlayerCamScript::DebugRender()
{
}
