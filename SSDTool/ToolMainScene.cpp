#include "ToolMainScene.h"
#include "stdafx.h"
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
#include <Skybox.h>
#include <TransForm.h>
#include <FbxRenderer.h>
#include <SkeletalFbxRenderer.h> 
#include <RenderTarget.h>
#include <PlayerCamScript.h>
#include <BloomFilter.h>
#include <GaussianBlur.h>
#include <TerrainRenderer.h>
#include <Light.h>
#include <Camera.h>
#include "SSDToolGlobal.h"
#include "EditDlg.h"
#include <MouseScript.h>
#include <Gizmo.h>	
#include <SurvivorFSM.h>
#include <Player.h>
#include <TraceScript.h>
#include <Navigator.h>
#include <Terrain.h>
#include <Scene.h>
#include <GameWindow.h>
#include "SSDToolGlobal.h"
#include "NaviDlg.h"
#include <FogFilter.h>
#include <MagicCircle.h>
#include <Scene.h>
#include <UIBase.h>
#include <ParticleRenderer.h>
#include <DecalRenderer.h>
#include <DBDRule.h>
#include <FootPrints.h>
#include <SSDRandom.h>

#include "ControlDlg.h"

CToolMainScene::CToolMainScene(CScene* _PairScene)
	:CSceneScript(_PairScene), m_DebugFont(nullptr), m_ToolPlayer(nullptr), m_PlayerNavigator(nullptr)
{
	SSDToolGlobal::ToolMainScene = this;
}


CToolMainScene::~CToolMainScene()
{

}

const bool CToolMainScene::Loading()
{
	m_DebugFont = CResourceMgr<CDXFont>::Find(L"나눔바른펜");
	SPTR<CCamera> MainCam = GetMainCamera();

	MainCam->ACTOR->AddCom<CFreeCameraScript>();
	MainCam->ACTOR->AddCom<CMouseScript>();
	MainCam->SetPerspectiveMode();

	LinkCollsionChannel(COLLISION_3D, L"Mouse", L"NaviTri");

	//TempBloom = MainCam->AddFilter<CBloomFilter>(MINIMIZE_BUFFER_TYPE::MINI_64);
	//TempBloom->SetBloomResTarget(MainCam);
	//TempBloom->SetMiniBufferAmount(0.6f);
	//TempBloom->SetMiniBufferLuminance(Vec3(0.2125f, 0.7154f, 0.0721f));
	//TempBloom->SetMiniBufferMultiple(2.f);
	//TempBloom->SetRange(5);
	//TempBloom->SetSigma(50.f);

	//SPTR<CGaussianBlur> Gau = MainCam->AddFilter<CGaussianBlur>();
	//Gau->SetGaussianBuffer(0.8f, 7);

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"SkyboxNight.dds"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints_N.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Noise.jpg"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Burn.jpg"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Shadow.tga"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"PlayerBlood.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"KillerBlood.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"MoveBlood.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"ColorBase.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"MagicEffect.png"));
	CResourceMgr<CMultiTexture>::Create(L"ParticleTest", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 2, L"ColorBase", L"MagicEffect" });

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeBlack.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeRed.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeWhite.png"));
	SPTR<CMultiTexture> TempTex = CResourceMgr<CMultiTexture>::Create(L"FadeParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"FadeWhite", L"FadeBlack", L"FadeRed" });

	//스카이박스
	SPTR<CActor> SkyActor = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL);
	SPTR<CSkybox> SkyBox = SkyActor->AddCom<CSkybox>(RENDATA{ RENDER_LAYER_MAP });
	SkyBox->SetSkyBoxSize(10000.f);
	SkyBox->SetTexture(L"SkyboxNight");
	//SkyBox->SetTexture(L"NightMoon");

	////디폴트 라이트
	SPTR<CActor> DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	DLight->SetLightColor(Vec4::Mint);
	DLight->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	DLight->SetLightPower(0.5f);
	DLight->SetSpecPower(200.f);


	////터레인
	CResourceMgr<CMultiTexture>::Create(L"Floor2", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"BaseFloor_2", L"TerrainDiff_2", L"TerrainNormal_2" });

	//// New 터레인
	SPTR<CActor> TerrainActor = CreateActor(UPDATE_LAYER_NORMAL, L"TerrainActor");
	SPTR<CTerrain> Terrain = TerrainActor->AddCom<CTerrain>();
	Terrain->CreateTerrain(L"BaseFloor", L"TerrainDiff", L"TerrainNormal", 128, 128, 30.f);
	//Terrain->CreateTerrain(L"BaseFloor", L"TerrainDiff", L"TerrainNormal", 50, 50, 30.f);
	Terrain->CreateFloor(L"Floor2");
	Terrain->SetCellSize(50.f);
	DBD::pMainTerrain = Terrain;
	DBD::pMainTerrain->SetTerrainColRay(MainCam->FindCameraScript<CMouseScript>()->GetMouseRay());


	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });

	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor", CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	//CreateToolPlayer();

	//UITest
	SPTR<CCamera> UICam = GetUICamera();
	SPTR<CUIBase> Tem = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Burn" });
	Tem->SetSubSize(Vec3(200.f, 200.f, 200.f));
	Tem->SetSubPivot(Vec3(-400.f, 100.f, 100.f));
	Tem->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 1.f));

	SPTR<CUIBase> Tem3 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Burn" });
	Tem3->SetSubSize(Vec3(200.f, 200.f, 200.f));
	Tem3->SetSubPivot(Vec3(400.f, 100.f, 100.f));
	Tem3->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.5f));

	//SPTR<CActor> TempTestActor = CreateActor(L"FadeTest", UPDATE_LAYER_NORMAL);
	//TempFade = TempTestActor->AddCom<CFadeScript>(m_SurvivorPlayer->GetPlayerCamera());

	//파티클 테스트
	SPTR<CActor> PTest1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	SPTR<CParticleRenderer> PRender1 = PTest1->AddCom<CParticleRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
	PRender1->PushParticleTex(L"ParticleTest", L"FadeParticle");

	int Index = PRender1->PushParticle(PARTICLEVTX{ Vec3(100.f, 100.f, 100.f), Vec3(), 1.f, 200.f, 2000.f, Vec2(0.f, 0.f),
	Vec2(1.f, 1.f), Vec3(), Vec3(), Vec4()});
	PRender1->PushParticleAnimation(Index, PARTICLEANIM(true, Vec2(5.f, 3.f), 15));

	DBD::pMainParticle = PRender1;
	//PRender1->PushParticleTex(L"ParticleTest");
	//PRender1->PushParticle(PARTICLEVTX{ Vec3(500.f, 100.f, 100.f), Vec3(), 1.f, 500.f, 500.f, Vec2(0.f, 0.f),
	//Vec2(1.f, 0.f), Vec3(), Vec3(), Vec4() });

	//PRender1->PushParticle(PARTICLEVTX{
	//Vec3(100.f, 100.f, 100.f),
	//Vec3(),
	//0.f,
	//600.f, 300.f,
	//Vec2(),
	//Vec2(1.f, 0.f), Vec3(), Vec3(), Vec4() });

	//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Gate2.png"));
	//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Burn.jpg"));
	//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints.tga"));
	//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints_N.tga"));


	//SPTR<CActor> DTest = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	//SPTR<CDecalRenderer> DRender = DTest->AddCom<CDecalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//DRender->SetRenSize(Vec3(100.f, 100.f, 100.f));
	//DRender->SetRenPivot(Vec3(100.f, 10.f, 100.f));
	//DRender->SetDecalTex(L"FootPrints");
	//DRender->SetDecalNormalTex(L"FootPrints_N");

	//SPTR<CActor> DTest2 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	//SPTR<CDecalRenderer> DRender2 = DTest2->AddCom<CDecalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//DRender2->SetRenSize(Vec3(200.f, 200.f, 200.f));
	//DRender2->SetRenPivot(Vec3(300.f, 10.f, 300.f));
	//DRender2->SetDecalTex(L"Gate2");

	SPTR<CActor> PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	m_SurvivorPlayer = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	m_Survivor = m_SurvivorPlayer->MountPlayerFSM<CSurvivorFSM>();
	m_SurvivorPlayer->SetLocalPlayer();
	m_pPairScene->RegisteredMainCamera(m_SurvivorPlayer->GetPlayerCamera());
	PlayerActor->TRANS->SetPos(Vec3(100.f, 0.f, 100.f));
	DBD::pMainTerrain->PushNavigator(m_SurvivorPlayer->GetPlayerNavigator());

	//PlayerActor->AddCom<CFootPrints>(m_SurvivorPlayer->GetFootPrintActor());
	m_ControlableCamera = m_SurvivorPlayer->GetPlayerCamera();

	return true;
}

void CToolMainScene::DebugRender()
{
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Diffuse"), 12, 7, 0, 6);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Position"), 12, 7, 1, 6);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Normal"), 12, 7, 2, 6);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Depth"), 12, 7, 3, 6);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Specular"), 12, 7, 4, 6);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Emissive"), 12, 7, 5, 6);

	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"LightDiffuse"), 12, 7, 0, 5);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"LightSpecular"), 12, 7, 1, 5);
	SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"EmvFactor"), 12, 7, 2, 5);

	//SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Minimize64"), 10, 5, 0, 2);
	//SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Minimize128"), 10, 5, 1, 2);
	//SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Minimize256"), 10, 5, 2, 2);
	//SSDDebug::DrawRenderTarget(CResourceMgr<RT>::Find(L"Minimize512"), 10, 5, 3, 2);

	//wchar_t	DebugBuff[MAXSTRING];

	//memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);
	//Vec3 CamPos = GetMainCamera()->GetPos();

	//swprintf_s(DebugBuff, L"FPS : %.f", FPS);

	//m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(5.f, 0.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Camera Pos  : (%.1f , %.1f , %.1f)", CamPos.x, CamPos.y, CamPos.z);
	//m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(250.f, 0.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Mouse Pos : (%.1f , %.1f)", Core::GetScreenMousePos().x, Core::GetScreenMousePos().y);
	//m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(0.f, 25.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Screen Size : (%.1f , %.1f)", (float)Core::GetWindowWidth(), (float)Core::GetWindowHeight());
	//m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(250.f, 25.f), Vec4::Pink);

	//if (Core::CheckMouseOnScreen())
	//	swprintf_s(DebugBuff, L"Mouse In Screen");
	//else
	//	swprintf_s(DebugBuff, L"Mouse Out of Screen");

	//m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(500.f, 25.f), Vec4::Pink);


}

void CToolMainScene::TestFunc(void* _Para)
{
	//m_Light->SetLightColor(Vec4::PastelRed);
}

void CToolMainScene::TestFunc2(void* _Para)
{
	//m_Light->SetLightColor(Vec4::PastelGreen);
}

void CToolMainScene::PreUpdate(const float & _DeltaTime)
{
	//if (nullptr == LightActor)
	//{
	//	return;
	//}

	//Vec3 MoveDir = Vec3();

	//if (true == KEY("CAM_UP"))
	//	LightActor->TRANS->AccRotate(Vec3(0.f, 0.f, 360.f*_DeltaTime));

	//if (true == KEY("CAM_DOWN"))
	//	LightActor->TRANS->AccRotate(Vec3(0.f, 0.f, -360.f*_DeltaTime));

	//if (true == KEY("UP"))
	//	MoveDir -= LightActor->TRANS->GetUp();

	//if (true == KEY("DOWN"))
	//	MoveDir -= LightActor->TRANS->GetDown();

	//if (true == KEY("LEFT"))
	//	MoveDir += LightActor->TRANS->GetLeft();

	//if (true == KEY("RIGHT"))
	//	MoveDir += LightActor->TRANS->GetRight();


	//if (MoveDir == Vec3())
	//	return;


	//LightActor->TRANS->Move(MoveDir, 1000.f, _DeltaTime);


}

void CToolMainScene::PostUpdate(const float & _DeltaTime)
{
	if (true == KEY("L"))
	{
		DBD::pMainParticle->SetFadeEffect(1.f, FADE_IN, FADE_COLOR_WHITE);
	}

	//if (true == KEYDOWN("f5") ) 
	//{
	//	if (nullptr == m_ToolPlayer->ACTOR) 
	//	{
	//		CreateToolPlayer();
	//	}

	//	if (false == m_ToolPlayer->IsPlayerActive())
	//	{
	//		ChangePlayerVision();
	//	}
	//}

	//if (true == KEYDOWN("EscapeKey") && true == m_ToolPlayer->IsPlayerActive())
	//{
	//	ChangeFreeCameraVision();
	//}

	if (true == KEYDOWN("NaviBakeKey"))
	{

		if (false == DBD::pMainTerrain->IsNavigationBake())
		{
			DBD::pMainTerrain->BakeNavigationMesh();
			m_ToolPlayer->SetPos(Vec3(3200.f, 0.f, 3000.f));
			m_PlayerNavigator->SetCurNaviMesh(DBD::pMainTerrain->GetTerrainRen()->GetTerrainNaviMesh());
		}
		else
		{
			m_PlayerNavigator->InitCurNaviMesh();
			DBD::pMainTerrain->DeleteNavigationMesh();
		}
	}

	//if (true == KEY("R"))
	//{
	//	std::wstring TmpName = CFilePathMgr::GetPath(L"Scene") + L"055-0305.scene";
	//	SSDToolGlobal::ToolControlDlg->NewReadScene(TmpName.c_str());

	//	SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	//	FbxRen1 = Test1->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//	FbxRen1->SetFbx(L"Test");
	//	FbxRen1->SettingFbxMesh(0);
	//	FbxRen1->GetFbxMesh(0)->GetMatStart(0)->SetEmivColor(2, Vec4(5.f, 5.f, 5.f, 1.f));
	//}

	//if (true == SSDToolGlobal::CollEdit)
	//{
	//	if (nullptr == SSDToolGlobal::ToolSelectCollider)
	//	{
	//		return;
	//	}

	//	Vec3 PivotPos = SSDToolGlobal::ToolSelectCollider->GetColliderPivot();
	//	if (true == KEY("R"))
	//	{
	//		//SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	//		//FbxRen1 = Test1->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//		//FbxRen1->SetFbx(L"Test");
	//		//FbxRen1->SettingFbxMesh(0);
	//		//FbxRen1->GetFbxMesh(0)->GetMatStart(0)->SetEmivColor(0, Vec4(5.f, 5.f, 5.f, 1.f));

	//		//SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x + 0.5f, PivotPos.y, PivotPos.z));
	//		//if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		//{
	//		//	SSDToolGlobal::ToolEditDlg->ChangeCollPivotX(PivotPos.x + 0.5f);
	//		//}
	//	}
	//	if (true == KEY("F"))
	//	{
	//		SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x - 0.5f, PivotPos.y, PivotPos.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollPivotX(PivotPos.x - 0.5f);
	//		}
	//	}
	//	if (true == KEY("T"))
	//	{
	//		SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x, PivotPos.y + 0.5f, PivotPos.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollPivotY(PivotPos.y + 0.5f);
	//		}
	//	}
	//	if (true == KEY("G"))
	//	{
	//		SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x, PivotPos.y - 0.5f, PivotPos.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollPivotY(PivotPos.y - 0.5f);
	//		}
	//	}
	//	if (true == KEY("Y"))
	//	{
	//		SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x, PivotPos.y, PivotPos.z + 0.5f));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollPivotZ(PivotPos.z + 0.5f);
	//		}
	//	}
	//	if (true == KEY("H"))
	//	{
	//		SSDToolGlobal::ToolSelectCollider->SetColPivot(Vec3(PivotPos.x, PivotPos.y, PivotPos.z - 0.5f));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollPivotZ(PivotPos.z - 0.5f);
	//		}
	//	}

	//	Vec3 PivotSize = SSDToolGlobal::ToolSelectCollider->GetColliderSize();
	//	if (true == KEY("U"))
	//	{
	//		float Temp = fabsf(PivotSize.x + 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(Temp, PivotSize.y, PivotSize.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeX(PivotSize.x + 0.5f);
	//		}
	//	}
	//	if (true == KEY("J"))
	//	{
	//		float Temp = fabsf(PivotSize.x - 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(Temp, PivotSize.y, PivotSize.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeX(PivotSize.x - 0.5f);
	//		}
	//	}
	//	if (true == KEY("I"))
	//	{
	//		float Temp = fabsf(PivotSize.y + 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(PivotSize.x, Temp, PivotSize.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeY(PivotSize.y + 0.5f);
	//		}
	//	}
	//	if (true == KEY("K"))
	//	{
	//		float Temp = fabsf(PivotSize.y - 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(PivotSize.x, Temp, PivotSize.z));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeY(PivotSize.y - 0.5f);
	//		}
	//	}
	//	if (true == KEY("O"))
	//	{
	//		float Temp = fabsf(PivotSize.z + 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(PivotSize.x, PivotSize.y, Temp));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeZ(PivotSize.z + 0.5f);
	//		}
	//	}
	//	if (true == KEY("L"))
	//	{
	//		float Temp = fabsf(PivotSize.z - 0.5f);
	//		SSDToolGlobal::ToolSelectCollider->SetColSize(Vec3(PivotSize.x, PivotSize.y, Temp));
	//		if (nullptr != SSDToolGlobal::ToolEditDlg)
	//		{
	//			SSDToolGlobal::ToolEditDlg->ChangeCollSizeZ(PivotSize.z - 0.5f);
	//		}
	//	}
	//}


}

void CToolMainScene::LastUpdate(const float & _DeltaTime)
{

}


void CToolMainScene::ChangePlayerVision()
{
	if (m_ToolPlayer->ACTOR == nullptr)
	{
		m_ToolPlayer = nullptr;
		CreateToolPlayer();
	}

	if (m_ToolPlayer->IsActive())
		return;

	m_ToolPlayer->PlayerActorOn();
	//CKeyMgr::SetMouseClip(true);
	//CKeyMgr::SetMouseMiddlePos();
	m_pPairScene->ChangeMainCamera(m_ToolPlayer->GetPlayerCamera());
	ShowCursor(FALSE);
}
void CToolMainScene::ChangeFreeCameraVision()
{
	if (m_ToolPlayer->ACTOR == nullptr)
	{
		m_ToolPlayer = nullptr;
		CreateToolPlayer();
	}

	m_ToolPlayer->PlayerActorOff();
	//CKeyMgr::SetMouseClip(false);
	m_pPairScene->ChangeMainCamera(GetMainCamera());
	ShowCursor(TRUE);
}
void CToolMainScene::CreateToolPlayer()
{
	// 툴 플레이어
	SPTR<CActor> PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	m_ToolPlayer = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	CSurvivorFSM* Fsm = m_ToolPlayer->MountPlayerFSM<CSurvivorFSM>();
	m_PlayerNavigator = PlayerActor->AddCom<CNavigator>();

	PlayerActor->TRANS->SetPos(DBD::pMainTerrain->GetMiddlePos());
	m_ToolPlayer->SetLocalPlayer();
	m_ToolPlayer->PlayerActorOff();
}

//그리드
//SPTR<CActor> MeshTestActor = CreateActor(L"GridActor", UPDATE_LAYER_NORMAL);
//SPTR<Grid3DRenderer> TempMeshRender = MeshTestActor->AddCom<Grid3DRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
//TempMeshRender->SetColorFactor(VEC4::Pink);

////건축물
//CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Architecture", L"LV_Siraka_Town_Dolphin.fbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"Idle" });

//SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
//SPTR<CStaticFbxRenderer> FbxRen1 = Test1->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
//FbxRen1->SetFbx(L"Test");
//FbxRen1->ACTOR->SetPos(Vec3(100.f, 20.f, 100.f));
//FbxRen1->SettingFbxMesh(0);
//FbxRen1->SetRenSize(Vec3(50.f, 50.f, 50.f));

//사각형
//SPTR<CActor> Test3 = CreateActor(L"TestSphereActor", UPDATE_LAYER_NORMAL);
//SPTR<CBase3DRenderer> Sphere1 = Test3->AddCom<CBase3DRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
//Sphere1->PushMesh(L"Sphere_Lv64");
//Sphere1->PushMaterial(L"Base3D");
//Sphere1->SetRenSize(Vec3(100.f, 100.f, 100.f));
//Sphere1->SetRenPivot(Vec3(100.f, 100.f, 100.f));
//Sphere1->ACTOR->SetPos(Vec3(0.f, 0.f, 200.f));
//Sphere1->GetMaterial(0)->PushSampler(0, LINEARSAMPLER);
//Sphere1->DefferredOn();

////CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Animation", L"test.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });

//SPTR<CActor> TestActor = CreateActor(L"TestMeshActor", UPDATE_LAYER_NORMAL);
//TestActor->TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));
//TestActor->TRANS->SetPos(Vec3(500.f, 0.f, 0.f));
//TestActor->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));
//m_AnimRen = TestActor->AddCom<CSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
//m_AnimRen->SetFbx(L"killer");
//m_AnimRen->CreateBoneAnimation(L"walkcrouch", 1);
//m_AnimRen->CreateBoneAnimation(L"walkherted", 2);
//m_AnimRen->CreateBoneAnimation(L"walkslow", 3);
//m_AnimRen->CreateBoneAnimation(L"sprint", 4);
//m_AnimRen->CreateBoneAnimation(L"hertedsprint1", 5);
//m_AnimRen->CreateBoneAnimation(L"hertedsprint2", 6);
//m_AnimRen->CreateBoneAnimation(L"hitcrawl1", 7);
//m_AnimRen->CreateBoneAnimation(L"hitcrawl2", 8);
//m_AnimRen->CreateBoneAnimation(L"healing", 9);
//m_AnimRen->CreateBoneAnimation(L"healdloop", 10);
//m_AnimRen->CreateBoneAnimation(L"selfhealing", 11);
//m_AnimRen->CreateBoneAnimation(L"lockerhide", 12);
//m_AnimRen->CreateBoneAnimation(L"hurtedidle", 13);
//m_AnimRen->CreateBoneAnimation(L"hurtedcrouch", 14);
//m_AnimRen->CreateBoneAnimation(L"hookedfree", 15);
//m_AnimRen->CreateBoneAnimation(L"hookedidle", 16);
//m_AnimRen->CreateBoneAnimation(L"hookedstruggle", 17);
//m_AnimRen->CreateBoneAnimation(L"genrefair", 18);
//m_AnimRen->CreateBoneAnimation(L"genfail", 19);
//m_AnimRen->CreateBoneAnimation(L"killedspiderin", 20);
//m_AnimRen->CreateBoneAnimation(L"killedspiderloop", 21);
//m_AnimRen->CreateBoneAnimation(L"killedspiderout", 22);
//m_AnimRen->CreateBoneAnimation(L"rescue", 23);
//m_AnimRen->CreateBoneAnimation(L"rescuehooked", 24);

//m_AnimRen->ChangeBoneAnimation(L"walkcrouch");
//m_AnimRen->PushBoneAnimEvent(L"walkcrouch", 30, &CToolMainScene::TestFunc, this, nullptr, EVENT_CONSTANT);
//m_AnimRen->PushBoneAnimEvent(L"walkcrouch", 0, &CToolMainScene::TestFunc2, this, nullptr, EVENT_CONSTANT);

//LightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
//SPTR<CLight> Light = LightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, SPOT_LIGHT, false, L"Sphere_Lv16" });
//Light->TRANS->SetScale(Vec3(200.f, 200.f, 200.f));
//Light->TRANS->SetPos(Vec3(0.f, 0.f, 0.f));
//Light->SetLightColor(Vec4::Pink);
//Light->SetLightAtenuation(Vec4(1000.f, 1.f, 1.f, 0.f));
//Light->SetLightRange(2000.f);
//Light->SetLightPower(3.f);
//Light->SetLightDir(Vec3(0.f, 0.f, 1.f));

//볼륨메쉬 테스트
//LightActor = CreateActor(UPDATE_LAYER_NORMAL, L"VolumeLightActor");
//LightActor->TRANS->SetPos(Vec3(1500.f, 494.f, 1500.f));
//SPTR<CLight> Light2 = LightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, SPOT_LIGHT, true, L"Cone_Lv360" });
//Light2->SetLightColor(Vec4::PastelRed);
//Light2->SetLightAtenuation(Vec4(1.f, 1.f, 0.f, 0.f));
//Light2->SetLightPower(1.f);
//Light2->SetLightSize(Vec3(600.f, 600.f, 600.f));
//Light2->SetLightDir(Vec3(0.f, -1.f, 0.f));

//LightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
//SPTR<CLight> Light = LightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, true, L"Sphere_Lv16" });
//Light->ACTOR->SetSize(Vec3(200.f, 200.f, 200.f));
//Light->ACTOR->SetPos(Vec3(0.f, 0.f, 0.f));
//Light->SetLightColor(Vec4::Mint);
//Light->SetLightDir(Vec3(-1.f, -1.f, 1.f));
////Light->SetLightAtenuation(Vec4(2000.f, 1.f, 1.f, 0.f));
////Light->SetLightRange(2000.f);
//Light->SetLightPower(3.f);

//SPTR<CActor> PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
//m_Player = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
//CSurvivorFSM* Fsm = m_Player->MountPlayerFSM<CSurvivorFSM>();
//SPTR<CNavigator> PlayerNavi = PlayerActor->AddCom<CNavigator>();

//m_Player->SetLocalPlayer();
//Fsm->LocalPlayer();
//CResourceMgr<CFbx>::Load(L"Green", CFilePathMgr::GetPath(L"Architecture", L"Green.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC_NONAUTO, L"" });
//SPTR<CActor> TempActor = CreateActor(L"TempActor", UPDATE_LAYER_NORMAL);
//SPTR<CMagicCircle> TestCircle = TempActor->AddCom<CMagicCircle>(MAGICCIRCLE_COLOR::MC_GREEN);
//TempActor->SetSize(Vec3(50.f, 50.f, 50.f));

//CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Architecture", L"Green.fbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"Idle" });
//디퍼드에서 그리기
//SPTR<CActor> Test2 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
//SPTR<CFbxRenderer> FbxRen2 = Test2->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_ALPHA });
//FbxRen2->SetFbx(L"Test");
//FbxRen2->SetAllFbxMesh();
//FbxRen2->GetFbxMesh(0)->GetMatStart(0)->DeferredOn();
//FbxRen2->SetRenSize(Vec3(100.f, 100.f, 100.f));
//FbxRen2->GetFbxMesh(0)->GetMatStart(0)->SetEmivColor(0, Vec4(5.f, 5.f, 5.f, 1.f));

//포워드에서 그리기
//SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
//FbxRen1 = Test1->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
//FbxRen1->SetFbx(L"Test");
//FbxRen1->SetAllFbxMesh(L"ForwardFBXMaterial");
//FbxRen1->GetFbxMesh(0)->GetMatStart(0)->DeferredOff();
//FbxRen1->SetRenSize(Vec3(100.f, 100.f, 100.f));
//FbxRen1->SetRenPivot(Vec3(300.f, 0.f, 300.f));

//SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
//Test1->AddCom<CParticle>();

//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Noise.png"));
//CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"test.tga"));
//CResourceMgr<CMultiTexture>::Create(L"PTest", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 2, L"test", L"Noise" });