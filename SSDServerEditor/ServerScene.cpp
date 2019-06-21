#include "stdafx.h"
#include "ServerScene.h"
#include <FilePathMgr.h>
#include <ResourceMgr.h>
#include <Texture.h>
#include <Sprite.h>
#include <Actor.h>
#include <Sprite2DRenderer.h>
#include <Animator2D.h>
#include <Base3DRenderer.h>
#include <Material.h>
#include <DeadByDaylightGlobal.h>
#include <FreeCameraScript.h>
#include <TimerMgr.h>
#include <DXFont.h>
#include <Grid3DRenderer.h>
#include <KeyMgr.h>
#include <GlobalDebug.h>
#include <Base3DRenderer.h>
#include <Fbx.h>


CServerScene::CServerScene(CScene* _PairScene)
	:CSceneScript(_PairScene), m_DebugFont(nullptr)
{

}


CServerScene::~CServerScene()
{

}

const bool CServerScene::Loading()
{
	m_DebugFont = CResourceMgr<CDXFont>::Find(L"³ª´®¹Ù¸¥Ææ");

	SPTR<CCamera> MainCam = GetMainCamera();

	MainCam->ACTOR->AddCom<CFreeCameraScript>();
	MainCam->SetPerspectiveMode();

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"spectiles.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"test.png"));

	CResourceMgr<CSprite>::Create(L"spectiles", { L"spectiles" , POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f) ,16,16,0, ALLFRAME });
	CResourceMgr<CSprite>::Create(L"test", { L"test" , POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f) ,1,1,0, ALLFRAME });
	CResourceMgr<CSprite>::Create(L"test", { L"test" , POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f) ,1,1,0, ALLFRAME });


	SPTR<CActor> MeshTestActor = CreateActor(L"TestMeshActor", UPDATE_LAYER_NORMAL);
	SPTR<Grid3DRenderer> TempMeshRender = MeshTestActor->AddCom<Grid3DRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	TempMeshRender->SetColorFactor(VEC4::Pink);

	SPTR<CActor> Test = CreateActor(L"Test", UPDATE_LAYER_NORMAL);
	SPTR<CBase3DRenderer> Sphere = Test->AddCom<CBase3DRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	Sphere->PushMesh(L"Sphere_Lv8");
	Sphere->PushMaterial(L"ColorBase3D");
	Sphere->GetMaterial(0)->SetRasterizerState(L"WireFrame");
	Sphere->SetRenSize(Vec3(100.f, 100.f, 100.f));
	Sphere->ACTOR->SetPos(Vec3(0.f, 0.f, 200.f));
	Sphere->SetColorFactor(Vec4::PastelMag);
	Sphere->GetMaterial(0)->SetDepthStancilState(L"IgnoreDepth");



	return true;
}

void CServerScene::DebugRender()
{
	wchar_t	DebugBuff[MAXSTRING];

	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);
	Vec3 CamPos = GetMainCamera()->GetPos();

	swprintf_s(DebugBuff, L"FPS : %.f", FPS);

	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(5.f, 0.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Camera Pos  : (%.1f , %.1f , %.1f)", CamPos.x, CamPos.y, CamPos.z);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(250.f, 0.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Mouse Pos : (%.1f , %.1f)", Core::GetScreenMousePos().x, Core::GetScreenMousePos().y);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(0.f, 25.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Screen Size : (%.1f , %.1f)", (float)Core::GetWindowWidth(), (float)Core::GetWindowHeight());
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(250.f, 25.f), Vec4::Pink);


	if (Core::CheckMouseOnScreen())
		swprintf_s(DebugBuff, L"Mouse In Screen");
	else
		swprintf_s(DebugBuff, L"Mouse Out of Screen");

	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(500.f, 25.f), Vec4::Pink);

}