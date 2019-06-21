#include "PlayScene.h"
#include <FilePathMgr.h>
#include <ResourceMgr.h>
#include <Texture.h>
#include <Sprite.h>
#include <ClientConnectMgr.h>
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
#include <DBDRule.h>
#include <Terrain.h>
#include <DeadByDaylightGlobal.h>
#include <SSDRandom.h>
#include <ParticleRenderer.h>

#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

#include <FogFilter.h>
#include <DepthFieldFilter.h>

CPlayScene::CPlayScene(CScene* _PairScene)
	:CSceneScript(_PairScene), m_fAccTime(0.f)
	, m_eLoadStep(ASYNC_WAIT), m_LocalPlayerID(0), m_UpdateCount(0)
	, m_LocalPlayer(nullptr), m_GameRule(nullptr)
{

}


CPlayScene::~CPlayScene()
{
}
const bool CPlayScene::IsAsyncLoadingComplete()
{
	CCriSectionObject<CPlayScene>();
	return (m_eLoadStep == ASYNC_LOAD_COMPLETE);
}
const ASYNC_LOADING_STEP CPlayScene::GetLoadingStep()
{
	CCriSectionObject<CPlayScene>();
	return m_eLoadStep;
}

const void CPlayScene::CompleteAsyncLoading()
{
	CCriSectionObject<CPlayScene>();
	m_eLoadStep = ASYNC_LOAD_COMPLETE;
}
const bool CPlayScene::Loading()
{
	if (false == m_bLoading)
	{
		LoadTexture();
		LoadFBX();
	}

	//SPTR<CCamera> MainCam = GetMainCamera();

	//MainCam->ACTOR->AddCom<CFreeCameraScript>();
	//MainCam->ACTOR->AddCom<CMouseScript>();
	////MainCam->AddFilter<CDepthFieldFilter>();

	//MainCam->SetPos(Vec3(0.f, 350.f, -1350.f));
	//MainCam->SetPerspectiveMode();

	LinkCollsionChannel(COLLISION_3D, L"Player", L"Default");
	LinkCollsionChannel(COLLISION_3D, L"Player", L"PlayerDetect");
	//LinkCollsionChannel(COLLISION_3D, L"Attack", L"PlayerBody");

	

	return true;
}


const bool CPlayScene::AsyncLoading(void* _Arg)
{
	InitializeScene();
	Loading();

	if (nullptr == _Arg)
	{
		TASSERT(true);
		return false;
	}

	SPTR<CCamera> Cam1, Cam2;

	Cam1 = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL)->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_OBJECT, 45.f ,1.f, 50000.f });
	Cam1->SetPerspectiveMode();
	Cam1->SetWheelZoom(true);
	Cam1->SettingViewLayer(RENDER_LAYER_MAP, RENDER_LAYER_OBJECT, RENDER_LAYER_BACKEFFECT, RENDER_LAYER_PLAYER, RENDER_LAYER_FRONTEFFECT, RENDER_LAYER_FIRST, RENDER_LAYER_ALPHA);
	SPTR<CFogFilter> FogFillter = Cam1->AddFilter<CFogFilter>(Cam1);
	Cam1->AddFilter<CDepthFieldFilter>();
	FogFillter->SetFogStartDepth(500.f);
	Cam1->SetPos(Vec3(3200.f, 200.f, 5840.f));
	m_pPairScene->RegisteredMainCamera(Cam1);

	Cam2 = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL)->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_OBJECT, 45.f ,1.f, 50000.f });
	Cam2->SetPerspectiveMode();
	Cam2->SetWheelZoom(true);
	Cam2->SettingViewLayer(RENDER_LAYER_MAP, RENDER_LAYER_OBJECT, RENDER_LAYER_BACKEFFECT, RENDER_LAYER_PLAYER, RENDER_LAYER_FRONTEFFECT, RENDER_LAYER_FIRST, RENDER_LAYER_ALPHA);
	FogFillter = Cam2->AddFilter<CFogFilter>(Cam2);
	Cam2->AddFilter<CDepthFieldFilter>();
	FogFillter->SetFogStartDepth(500.f);
	Cam2->SetPos(Vec3(3200.f, 200.f, 560.f));
	Cam2->TRANS->SetRotate(Vec3(0.f, 180.f, 0.f));
	m_pPairScene->RegisteredMainCamera(Cam2);


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
	Terrain->SetCellSize(50.f);
	Terrain->CreateFloor(L"Floor2");

	DBD::pMainTerrain = Terrain;

	DBD::ReadScene(CFilePathMgr::GetPath(L"Scene", L"060-0408-Final.scene").c_str(), m_pPairScene);
	DBD::pMainTerrain->BakeNavigationMesh();


	SPTR<CActor> RuleActor = CreateActor(L"RuleActor", UPDATE_LAYER_NORMAL);
	m_GameRule = RuleActor->AddCom<CDBDRule>();
	m_GameRule->CreateSceneUI();
	m_GameRule->SetTerrain(Terrain);
	m_GameRule->DebugOn();
	

	WorldData* Data = (WorldData*)_Arg;
	m_GameRule->InitializeSceneProcessing(Data);
	m_GameRule->PushEventCamera(Cam1, Cam2);

	SPTR<CActor> PTest1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	m_MapSceneParticle = PTest1->AddCom<CParticleRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
	m_MapSceneParticle->TRANS->SetScale(Vec3(1000.f, 1000.f, 1000.f));
	m_MapSceneParticle->TRANS->SetPos(Vec3(3200.f, 0.f, 2500.f));

	m_MapSceneParticle->PushParticleTex(L"FogParticle", L"FadeParticle");
	DBD::pMainParticle = m_MapSceneParticle;

	for (size_t i = 0; i < 120; ++i)
	{
		m_MapSceneParticle->PushParticle(PARTICLEVTX{
			Vec3(SSDRandom::RandomFloat(500.f, 6000.f), 0.f, SSDRandom::RandomFloat(500.f, 6000.f)),
			Vec3(),
			(float)SSDRandom::RandomInt(3),
			600.f + SSDRandom::RandomFloat(-100.f, 200.f), 300.f + SSDRandom::RandomFloat(-200.f, 500.f),
			Vec2((SSDRandom::RandomBool(0.5f)) ? 1.f : -1.f, SSDRandom::RandomFloat(0.02f, 0.05f)),
			Vec4(1.f, 0.f, 0.f, 0.f), Vec3(), Vec3() });
	}

	//Bush
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
	m_MapSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 0.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	m_MapSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, -50.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });

	m_MapSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6400.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	m_MapSceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6450.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });


	m_GameRule->CreateEscapeDoor();
	GETSINGLE(CClientConnectMgr).RequestSceneObject();
	

	return true;
}


void CPlayScene::WakeUp()
{
	//GETSINGLE(CClientConnectMgr).ReceivePacket(true);
	GETSINGLE(CClientConnectMgr).NotifyBuildComplete();
	GETSINGLE(CClientConnectMgr).OperationSessionThread();
	CTimerMgr::SetFixFrame(60.f);
}
void CPlayScene::PreUpdate(const float & _DeltaTime)
{
	//GETSINGLE(ClientConnector).SendPlayerState(_DeltaTime);
	GETSINGLE(ClientConnector).SendPlayerState(_DeltaTime);
	ScenePacketProcessing(_DeltaTime);
}

void CPlayScene::PostUpdate(const float & _DeltaTime)
{
}

void CPlayScene::LastUpdate(const float & _DeltaTime)
{
	
}


void CPlayScene::ScenePacketProcessing(const float& _DeltaTime)
{
	if (GETSINGLE(CClientConnectMgr).EmptyPacketQueue())
		return;

	PACKET Pack;



	while (GETSINGLE(CClientConnectMgr).ExistPacket())
	{
		// 1. 서버에서 전달받은 패킷을 가져온다
		GETSINGLE(CClientConnectMgr).GetPacket(Pack);

		if (Pack.Header.HeaderFlag == FLAG_PLAYING_RULE_SIGNAL)
		{
			m_GameRule->RulePacketProcessing(Pack);
		}

		// 2. 패킷에서 게임 동작에 관련된 정보를 뽑아낸다.
		switch (Pack.Header.DataType)
		{
			// 현재 접속중인 리모트 플레이어들의 상태들을 업데이트
		case FLAG_DATA_ROOM_WORLD:
			m_GameRule->RoomDataProcessing(Pack);
			//RoomDataProcessing(_DeltaTime, Pack);
			break;
		case FLAG_DATA_SYNCRO_WORLD:
			m_GameRule->SyncroWorldProcessing(Pack);
			break;
		// 게임 중 접속이 끊기거나 게임종료을 한 플레이어 처리
		case FLAG_GAME_EXIT_USER:
			m_GameRule->ExitPlayerProcessing(Pack);
			//ExitPlayerProcessing(_DeltaTime, Pack);
			break;
		case FLAG_DATA_BROADCATE_REMOTE:
			m_GameRule->RemotePlayerProcessing(Pack);
			break;
		case FLAG_DATA_GANERATOR_COMPLETE:
			m_GameRule->RulePacketProcessing(Pack);
			break;
		case FLAG_DATA_COLLISION:
		{
			m_GameRule->CollsionPacketProcessing(Pack);
		}
			break;
		default:
			break;
		}
	}


}
void CPlayScene::LoadTexture()
{
	// 텍스쳐 로드
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"SkyboxNight.dds"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number0.png"));
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
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Defeat.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"MainLoading.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Win.png"));


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

void CPlayScene::LoadFBX()
{
	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor", CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
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
void CPlayScene::DebugRender()
{
	SSDDebug::DrawRenderTarget(L"Diffuse", 10, 5, 0, 3);
	SSDDebug::DrawRenderTarget(L"Position", 10, 5, 1, 3);
	SSDDebug::DrawRenderTarget(L"Normal", 10, 5, 2, 3);
	SSDDebug::DrawRenderTarget(L"Depth", 10, 5, 3, 3);

	SSDDebug::DrawRenderTarget(L"LightDiffuse", 10, 5, 0, 4);
	SSDDebug::DrawRenderTarget(L"LightSpecular", 10, 5, 1, 4);

	wchar_t	DebugBuff[MAXSTRING];

	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);
	Vec3 CamPos = GetMainCamera()->GetPos();

	swprintf_s(DebugBuff, L"FPS : %.f    Ping : %.2f ms   Max Ping : %.2f ms"
		, CTimerMgr::FPS
		, GETSINGLE(CClientConnectMgr).GetPing()
		, GETSINGLE(CClientConnectMgr).GetMaxPing());

	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 0.f), Vec4::Pink);
}

void CPlayScene::CreateWorldObjData(const WorldObjData& _Data)
{
	CActor* CirlceActor = nullptr;

	for (size_t i = 0; i < _Data.Header.ObjCount; i++)
	{
		CirlceActor = CreateActor(UPDATE_LAYER_NORMAL, L"CircleActor");
		m_GameRule->CreateMagicCircle(CirlceActor, _Data.arrCircleData[i]);
	}
}