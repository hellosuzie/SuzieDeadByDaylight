#include "TestScene.h"
#include <Grid3DRenderer.h>
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
#include <TransForm.h>
#include <Grid3DRenderer.h>
#include <KeyMgr.h>
#include <Fbx.h>
#include <Skybox.h>
#include <DXFont.h>
#include <TimerMgr.h>
#include "Test.h"
#include <SphereCollider.h>
#include <OBBCollider.h>
#include <FbxRenderer.h>
#include <SkeletalFbxRenderer.h>
#include <MouseScript.h>
#include <DXMacro.h>
#include <GlobalDebug.h>
#include <SSDThread.h>
#include <GaussianBlur.h>
#include <BloomFilter.h>
#include <Player.h>
#include <SurvivorFSM.h>
#include <KillerFSM.h>
#include <NavigationMesh.h>
#include <Navigator.h>
#include <TerrainRenderer.h>
#include <TraceScript.h>
#include <Terrain.h>
#include <NavigationObject.h>
#include <UIBase.h>
#include <Sprite2DRenderer.h>
#include <SubTrans.h>
#include <RectCollider.h>
#include <DecalRenderer.h>
#include <SphereCollider.h>
#include <OBBCollider.h>
#include <DepthFieldFilter.h>
#include <MagicCircle.h>
#include <Hook.h>
#include <DBDRule.h>

#include <DepthFieldFilter.h>
#include <FogFilter.h>
#include <ParticleRenderer.h>

#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <SSDRandom.h>

#include <SoundPlayer.h>
#include <TimerMgr.h>

CTestScene::CTestScene(CScene* _pScene)
	:CSceneScript(_pScene), m_SurvivorPlayer(nullptr), m_KillerPlayer(nullptr)
	, m_ControlableCamera(nullptr), m_GenMagicCircle(nullptr), m_TrapMagicCircle(nullptr)
	, m_TestSurvivorPlayer(nullptr), m_TestSurvivor(nullptr)
	, m_GameRule(nullptr)
{

}

CTestScene::~CTestScene()
{

}
void CTestScene::WakeUp()
{
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PlayScene.mp3"));
	CSoundPlayer::PlayBGM(L"PlayScene");
	CSoundPlayer::SetBGMVolume(0.05f);

	CTimerMgr::SetFixFrame(60.1f);
}
const bool CTestScene::Loading()
{
	if (false == m_bLoading)
	{
		LoadTexture();
		LoadFBX();
	}

	SPTR<CCamera> MainCam = GetMainCamera();

	MainCam->ACTOR->AddCom<CFreeCameraScript>();
	MainCam->ACTOR->AddCom<CMouseScript>();
	SPTR<CFogFilter> FogFillter = MainCam->AddFilter<CFogFilter>(MainCam);
	MainCam->AddFilter<CDepthFieldFilter>();
	FogFillter->SetFogStartDepth(500.f);

	MainCam->SetPos(Vec3(3200.f, 200.f, 5840.f));
	MainCam->SetPerspectiveMode();

	LinkCollsionChannel(COLLISION_3D, L"Player", L"Default");
	LinkCollsionChannel(COLLISION_3D, L"Player", L"PlayerDetect");
	LinkCollsionChannel(COLLISION_3D, L"Attack", L"PlayerBody");

	// 스카이 박스 & 터레인 만들기
	// 스카이 박스
	SPTR<CActor> SkyActor = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL);
	SPTR<CSkybox> SkyBox = SkyActor->AddCom<CSkybox>(RENDATA{ RENDER_LAYER_MAP });
	SkyBox->SetSkyBoxSize(10000.f);
	SkyBox->SetTexture(L"SkyboxNight");

	//// 터레인 (터레인 랜더러 + 기타 객체들) 
	SPTR<CActor> TerrainActor = CreateActor(UPDATE_LAYER_NORMAL, L"TerrainActor");
	SPTR<CTerrain> Terrain = TerrainActor->AddCom<CTerrain>();
	Terrain->CreateTerrain(L"BaseFloor", L"TerrainDiff", L"TerrainNormal", 128, 128, 30.f);
	Terrain->CreateFloor(L"Floor2");
	Terrain->SetCellSize(50.f);

	DBD::pMainTerrain = Terrain;


	SPTR<CActor> DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	DLight->SetLightColor(Vec4::Mint);
	DLight->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	DLight->SetLightPower(2.f);
	DLight->SetSpecPower(8.f);

	//DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	//DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	//DLight->SetLightColor(Vec4::Mint);
	//DLight->SetLightDir(Vec3(1.f, -1.f, 0.f));
	//DLight->SetLightPower(2.f);
	//DLight->SetSpecPower(8.f);

	//DeadByDaylightGlobal::ReadScene((CFilePathMgr::GetPath(L"Scene") + L"060-0408-Final.scene").c_str(), m_pPairScene);

	SPTR<CActor> RuleActor = CreateActor(L"RuleActor", UPDATE_LAYER_NORMAL);
	m_GameRule = RuleActor->AddCom<CDBDRule>();
	m_GameRule->SetTerrain(Terrain);


	SPTR<CActor> PlayerActor = nullptr;
	// 3. 유저 객체들 생성( 생성자 or 살인자 )
	// 생존자
	/*PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	m_SurvivorPlayer = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	m_SurvivorPlayer->SetRule(m_GameRule);
	m_Survivor = m_SurvivorPlayer->MountPlayerFSM<CSurvivorFSM>();
	m_SurvivorPlayer->SetLocalPlayer();
	m_pPairScene->RegisteredMainCamera(m_SurvivorPlayer->GetPlayerCamera());
	PlayerActor->TRANS->SetPos(SURVIVORINITPOS);
	DBD::pMainTerrain->PushNavigator(m_SurvivorPlayer->GetPlayerNavigator());

	m_eSceneControl = CONTROL_SURVIVOR;
	m_ControlableCamera = m_SurvivorPlayer->GetPlayerCamera();*/


	PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	m_TestSurvivorPlayer = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	m_TestSurvivorPlayer->SetRule(m_GameRule);
	m_TestSurvivor = m_TestSurvivorPlayer->MountPlayerFSM<CSurvivorFSM>();
	PlayerActor->TRANS->SetPos(Vec3(3200.f, 0.f, 3000.f));
	m_TestSurvivorPlayer->SetRemotePlayer();
	m_TestSurvivorPlayer->PlayerDamagedEvent();

	// 살인자
	PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	m_KillerPlayer = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	m_Killer = m_KillerPlayer->MountPlayerFSM<CKillerFSM>();
	m_KillerPlayer->SetLocalPlayer();
	m_KillerPlayer->SetRule(m_GameRule);
	m_pPairScene->RegisteredMainCamera(m_KillerPlayer->GetPlayerCamera());
	PlayerActor->TRANS->SetPos(KILLERINITPOS);
	DBD::pMainTerrain->PushNavigator(m_KillerPlayer->GetPlayerNavigator());

	m_eSceneControl = CONTROL_KILLER;
	m_ControlableCamera = m_KillerPlayer->GetPlayerCamera();



	m_GenMagicCircle = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL)->AddCom<CMagicCircle>(MC_GREEN);
	m_GenMagicCircle->TRANS->SetPos(Vec3(3000.f, 0.f, 3000.f));

	m_TrapMagicCircle = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL)->AddCom<CMagicCircle>(MC_RED);
	m_TrapMagicCircle->TRANS->SetPos(Vec3(3400.f, 0.f, 3000.f));

	//UItest
	//SPTR<CUIBase> Base = CreateActor(L"UI", UPDATE_LAYER_NORMAL)->AddCom<CUIBase>(UI_DATA{ RENDER_LAYER_UI , UCT_BASE , L"FocusPause"});
	//Base->GetActor()->SetSize(Vec3(50.f, 50.f, 0.f));
	//Base->GetActor()->SetPos(Vec3(50.f, 50.f, 0.f));

	//파티클
	SPTR<CActor> PTest1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	m_TestSceneParticle = PTest1->AddCom<CParticleRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });

	m_TestSceneParticle->PushParticleTex(L"FogParticle", L"FadeParticle");

	for (size_t i = 0; i < 120; ++i)
	{
		m_TestSceneParticle->PushParticle(PARTICLEVTX{
			Vec3(SSDRandom::RandomFloat(500.f, 6000.f), 0.f, SSDRandom::RandomFloat(500.f, 6000.f)),
			Vec3(),
			(float)SSDRandom::RandomInt(3),
			600.f + SSDRandom::RandomFloat(-100.f, 200.f), 300.f + SSDRandom::RandomFloat(-200.f, 500.f),
			Vec2((SSDRandom::RandomBool(0.5f)) ? 1.f : -1.f, SSDRandom::RandomFloat(0.02f, 0.05f)),
			Vec4(1.f, 0.f, 0.f, 0.f), Vec3(), Vec3() });
	}

	// Bush
	TreeTypeName[BUSH1] = L"Bush01";
	TreeTypeName[BUSH2] = L"Bush02";
	TreeTypeName[BUSH3] = L"Bush03";
	TreeTypeName[BUSH4] = L"Bush04";
	TreeTypeName[BUSH5] = L"Bush05";
	TreeTypeName[BUSH6] = L"Bush06";
	TreeTypeName[TREE1] = L"LV_Campagna_Tree05";
	TreeTypeName[TREE2] = L"LV_Campagna_Tree04";

	for (size_t i = 0; i < 600; ++i)
	{
		Vec3 TempPos = Vec3(SSDRandom::RandomFloat(100.f, 6300.f), 0.f, SSDRandom::RandomFloat(100.f, 6300.f));
		while (false == DBD::pMainTerrain->CheckMoveablePosition(TempPos))
		{
			TempPos = Vec3(SSDRandom::RandomFloat(100.f, 6300.f), 0.f, SSDRandom::RandomFloat(100.f, 6300.f));
		}

		int type = SSDRandom::RandomInt(0, 5);
		float size = SSDRandom::RandomFloat(1, 2.5);
		SPTR<CActor> BushActor = CreateActor(L"BushActor", UPDATE_LAYER_NORMAL);
		BushActor->TRANS->SetRotate(Vec3(0.f, SSDRandom::RandomFloat(0.f, 360.f), 0.f));
		BushActor->TRANS->SetPos(TempPos);
		BushActor->TRANS->SetScale(Vec3(size, size, size));

		SPTR<CStaticFbxRenderer> TempBush = BushActor->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
		TempBush->SetFbx(TreeTypeName[type].c_str());
		TempBush->SetAllFbxMesh();
	}

	//문 결계
	m_TestSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 0.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f)});
	m_TestSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, -50.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f)});

	m_TestSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6400.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	m_TestSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6450.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });


	//사운드
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back1.mp3"));
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back2.mp3"));
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back3.mp3"));
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back4.mp3"));
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back5.mp3"));
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PHang.mp3"));



	//m_GameRule->CreateSceneUI();
	return true;
}

void CTestScene::EnterTest()
{
	Tem->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 1.f));
}

void CTestScene::StayTest()
{
	Tem->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.5f));
}

void CTestScene::ExitTest()
{
	Tem->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 1.f));
}

void CTestScene::PostUpdate(const float& _DeltaTime)
{
	//m_SoundAccTime += _DeltaTime;
	////사운드 넣기
	//if (CTimerMgr::TimingCheck(m_SoundAccTime, 10.f))
	//{
	//	CSoundPlayer::GlobalPlaySound(SoundTypeName[SSDRandom::RandomInt(0, 5)]);
	//	CSoundPlayer::SetBGMVolume(0.07f);
	//}


	if (true == KEYDOWN("NaviBakeKey"))
	{
		if (false == DBD::pMainTerrain->IsNavigationBake())
		{
			m_SurvivorPlayer->SetPos(SURVIVORINITPOS);
			//m_KillerPlayer->SetPos(KILLERINITPOS);

			DBD::pMainTerrain->BakeNavigationMesh();
		}
		else
		{
			DBD::pMainTerrain->DeleteNavigationMesh();
		}
	}

	if (true == KEYDOWN("NaviAreaKey"))
	{
		CNaviMeshArea::ChangeAreaRendering();
	}
}

void CTestScene::LastUpdate(const float& _DeltaTime)
{
	if (true == KEYDOWN("CameraKey"))
	{
		CKeyMgr::ChangeMouseClipMode();

		SPTR<CCamera> MainCam = m_pPairScene->GetMainViewCamera();

		if (GetMainCamera() != MainCam)
		{
			CKeyMgr::SetMouseClip(false);
			m_pPairScene->ChangeMainCamera(GetMainCamera());
			GetMainCamera()->SetCameraBluring(1.5f, m_ControlableCamera, 3.f);
		}
		else
		{
			CKeyMgr::SetMouseClip(true);
			m_pPairScene->ChangeMainCamera(m_ControlableCamera);
			m_ControlableCamera->SetCameraBluring(1.5f, GetMainCamera(), 3.f);
		}

	}
}
void CTestScene::LoadTexture()
{
	// 텍스쳐 로드
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"SkyboxNight.dds"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number1.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number3.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number4.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number5.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"PlayerState.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Perk.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Noise.jpg"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Burn.jpg"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Dead.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Exit.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"HitOne.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"HitTwo.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Hook.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Idle.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Back.png"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"DestroyMagic.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Gauge.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"GaugeFull.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Heal.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"RunMagic.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SelfHeal.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"StartMagic.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SummonsHook.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Rescue.png"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog0.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog1.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog3.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Gate1.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Gate2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints_N.tga"));
	CResourceMgr<CMultiTexture>::Create(L"FogParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 6, L"Fog0", L"Fog1", L"Fog2", L"Fog3", L"Gate1", L"Gate2" });

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeBlack.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeRed.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeWhite.png"));
	SPTR<CMultiTexture> TempTex = CResourceMgr<CMultiTexture>::Create(L"FadeParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"FadeWhite", L"FadeBlack", L"FadeRed" });


	// 스프라이트 생성
	//CResourceMgr<CSprite>::Create(L"UIBack", { L"UIBack", POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f), 1, 1, 0, ALLFRAME });
	CResourceMgr<CMultiTexture>::Create(L"Floor2", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"BaseFloor_2", L"TerrainDiff_2", L"TerrainNormal_2" });
}

void CTestScene::LoadFBX()
{
	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });

	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor", CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	//CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"SM_ST_OfficeRoom.fbx"), CFbx::LOAD_DESC{ FBX_STATIC });

	// 갈고리 FBX 로드
	CResourceMgr<CFbx>::Load(L"killedspider", CFilePathMgr::GetPath(L"Animation", L"killedspider.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });


	_finddata_t fd;
	intptr_t handle;
	int result = 1;
	std::string FileName;
	std::string Path = CFilePathMgr::GetSTRPath(L"Architecture");
	Path += "\\*.sfbx";
	handle = _findfirst(Path.c_str(), &fd);

	while (result != -1)
	{
		FileName = fd.name;
		std::wstring temp = CA2W(FileName.c_str(), CP_UTF8);
		CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", temp), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
		result = _findnext(handle, &fd);
	}
}

void CTestScene::DebugRender()
{

	SSDDebug::DrawRenderTarget(L"Diffuse", 12, 7, 0, 6);
	SSDDebug::DrawRenderTarget(L"Position", 12, 7, 1, 6);
	SSDDebug::DrawRenderTarget(L"Normal", 12, 7, 2, 6);
	SSDDebug::DrawRenderTarget(L"Depth", 12, 7, 3, 6);
	SSDDebug::DrawRenderTarget(L"Specular", 12, 7, 4, 6);
	SSDDebug::DrawRenderTarget(L"Emissive", 12, 7, 5, 6);

	SSDDebug::DrawRenderTarget(L"LightDiffuse", 12, 7, 0, 5);
	SSDDebug::DrawRenderTarget(L"LightSpecular", 12, 7, 1, 5);
	SSDDebug::DrawRenderTarget(L"EmvFactor", 12, 7, 2, 5);
	SSDDebug::DrawRenderTarget(GetUICamera(), 12, 7, 3, 5);


	//SSDDebug::DrawRenderTarget(L"LightDiffuse", 4, 2, 0, 0);
	//SSDDebug::DrawRenderTarget(L"Diffuse", 4, 2, 1, 0);
	//SSDDebug::DrawRenderTarget(L"Normal", 4, 2, 2, 0);
	//

	wchar_t	DebugBuff[MAXSTRING];

	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);
	Vec3 CamPos = GetMainCamera()->GetPos();

	swprintf_s(DebugBuff, L"FPS : %.f", CTimerMgr::FPS);

	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 0.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Camera Pos  : (%.1f , %.1f , %.1f)", CamPos.x, CamPos.y, CamPos.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(250.f, 0.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Survivor Player Pos : ( %.2f ,%.2f ,%.2f ) ",
	//	m_SurvivorPlayer->TRANS->GetWorldPos().x, m_SurvivorPlayer->TRANS->GetWorldPos().y, m_SurvivorPlayer->TRANS->GetWorldPos().z);
	//DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 20.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Killer Player Pos : ( %.2f ,%.2f ,%.2f ) ",
	//	m_KillerPlayer->TRANS->GetWorldPos().x, m_KillerPlayer->TRANS->GetWorldPos().y, m_KillerPlayer->TRANS->GetWorldPos().z);
	//DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 60.f), Vec4::Pink);

	if (nullptr != m_Survivor)
	{
		swprintf_s(DebugBuff, L"P1 Life : %u, P1 Death : %u, P1 Healing : %0.2f , P1 Hook : %0.2f"
			, m_Survivor->GetLifeCount()
			, m_Survivor->GetDeathCount()
			, m_Survivor->GetHealingCount()
			, m_Survivor->GetHookCount()
		);

		DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(4.f, 20.f), Vec4::Pink);

		swprintf_s(DebugBuff, L"P1 Pos : ( %.2f, %.2f, %.2f )"
			, m_SurvivorPlayer->TRANS->GetWorldPos().x
			, m_SurvivorPlayer->TRANS->GetWorldPos().y
			, m_SurvivorPlayer->TRANS->GetWorldPos().z
		);

		DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(4.f, 40.f), Vec4::Pink);

	}

	swprintf_s(DebugBuff, L"P2 Life : %u, P2 Death : %u, P2 Healing : %0.2f , P2 Hook : %0.2f"
		, m_TestSurvivor->GetLifeCount()
		, m_TestSurvivor->GetDeathCount()
		, m_TestSurvivor->GetHealingCount()
		, m_TestSurvivor->GetHookCount()
	);

	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(4.f, 60.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Generator Count  :  %.2f",  m_GenMagicCircle->GetGeneratorCount());
	//DBD::pPrettyFont->DrawString(DebugBuff, 20.f, Vec2(4.f, 100.f), Vec4::Pink);

}

void CTestScene::PreUpdate(const float & _DeltaTime)
{

}
