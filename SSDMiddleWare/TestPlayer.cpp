#include "TestPlayer.h"
#include <Base3DRenderer.h>
#include <Actor.h>
#include <TransForm.h>

CTestPlayer::CTestPlayer()
	:m_TestRen(nullptr)
{
}


CTestPlayer::~CTestPlayer()
{
}


const bool CTestPlayer::Init()
{
	m_TestRen = ACTOR->AddCom<CBase3DRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_TestRen->PushMesh(L"Sphere_Lv64");
	m_TestRen->PushMaterial(L"Base3D");
	m_TestRen->SetRenSize(Vec3(100.f, 100.f, 100.f));

	return CPlayer::Init();
}