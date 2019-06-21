#include "MouseScript.h"
#include <Actor.h>
#include <RayCollider.h>
#include <Camera.h>
#include <SSDCore.h>
#include <GameWindow.h>

CMouseScript::CMouseScript()
	:m_MouseCol(nullptr)
{
}


CMouseScript::~CMouseScript()
{
}


const bool CMouseScript::Init() 
{
	m_MouseCol = ACTOR->AddCom<CRayCollider>(COLDATA{L"Mouse",COLLISION_3D});

	return true;
}
void CMouseScript::MountingCameraEvent()
{
	m_MouseCol->SetRayLength(m_pPairCamera->GetFar());

}
void CMouseScript::LastCamScriptUpdate(const float& _DeltaTime)
{
	m_MouseCol->SetRayOrigin(m_pPairCamera->GetPos());
	Vec4 SMousePos = Core::GetScreenMousePos();

	D3D11_VIEWPORT ViewPort = m_pPairCamera->GetCameraViewPort();
	Vec4 Dir;

	float Near = m_pPairCamera->GetNear();
	Mat Proj = m_pPairCamera->GetProjMatrix();
	Mat InvView = m_pPairCamera->GetViewMatrix().GetInverseMat();
	Dir.x = (((SMousePos.x - ViewPort.TopLeftX)* 2.0f / ViewPort.Width) - 1.0f);

	Dir.x -= Proj._31;
	Dir.x /= Proj._11;

	Dir.y = -(((SMousePos.y - ViewPort.TopLeftY) * 2.0f / ViewPort.Height) - 1.0f);
	Dir.y -= Proj._32;
	Dir.y /= Proj._22;

	Dir.z = 1.f;


	Dir = Dir * InvView;
	m_MouseCol->SetRayDir(Dir.GetNormalizeVec());
}

SPTR<CRayCollider> CMouseScript::GetMouseRay()
{
	return m_MouseCol;
}