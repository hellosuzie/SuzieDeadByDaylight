#include "MainPlayScene.h"
#include <DeadByDaylightGlobal.h>
#include <DXFont.h>
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
#include <FreeCameraScript.h>
#include <TransForm.h>
#include <Grid3DRenderer.h>
#include <KeyMgr.h>
#include <Fbx.h>
#include <Skybox.h>
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
#include <SoundPlayer.h>
#include <Sound.h>
#include <Door.h>

#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

#include <FogFilter.h>
#include <DepthFieldFilter.h>

CMainPlayScene::CMainPlayScene(CScene* _PairScene) 
	: CSceneScript(_PairScene)
{
}


CMainPlayScene::~CMainPlayScene()
{
}


const bool CMainPlayScene::Loading()
{


	return true;
}

const bool CMainPlayScene::AsyncLoading(void* _Arg)
{
	InitializeScene();

	LinkCollsionChannel(COLLISION_3D, L"Player", L"Default");
	LinkCollsionChannel(COLLISION_3D, L"Player", L"PlayerDetect");
	//LinkCollsionChannel(COLLISION_3D, L"Attack", L"PlayerBody");


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


	SPTR<CActor> DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	DLight->SetLightColor(Vec4::Mint);
	DLight->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	DLight->SetLightPower(2.f);
	DLight->SetSpecPower(8.f);

	DBD::ReadScene(CFilePathMgr::GetPath(L"Scene", L"060-0408-Final.scene").c_str(), m_pPairScene);
	DBD::pMainTerrain->BakeNavigationMesh();


	SPTR<CActor> RuleActor = CreateActor(L"RuleActor", UPDATE_LAYER_NORMAL);
	m_GameRule = RuleActor->AddCom<CDBDRule>();
	m_GameRule->SetTerrain(Terrain);
	m_GameRule->DebugOn();

	//파티클
	SPTR<CActor> PTest1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	m_SceneParticle = PTest1->AddCom<CParticleRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });

	m_SceneParticle->PushParticleTex(L"FogParticle", L"FadeParticle");
	DBD::pMainParticle = m_SceneParticle;

	for (size_t i = 0; i < 120; ++i)
	{
		m_SceneParticle->PushParticle(PARTICLEVTX{
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

	SPTR<CActor> EmTest = CreateActor(L"EmTest", UPDATE_LAYER_NORMAL);
	SPTR<CFbxRenderer> Temp = EmTest->AddCom<CFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });




	////문 결계
	m_SceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 0.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	m_SceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, -50.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });

	m_SceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6400.f), Vec3(), 4.f, 280.f, 280.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });
	m_SceneParticle->PushParticle(PARTICLEVTX{ Vec3(3200.f, 130.f, 6450.f), Vec3(), 5.f, 250.f, 250.f, Vec2(0.f, 0.f),
		Vec4(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f) });


	SoundTypeName[BACK1] = L"Back1";
	SoundTypeName[BACK2] = L"Back2";
	SoundTypeName[BACK3] = L"Back3";
	SoundTypeName[BACK4] = L"Back4";
	SoundTypeName[BACK5] = L"Back5";
	SoundTypeName[BACK6] = L"Back6";

	m_GameRule->CreateSceneUI();
	m_GameRule->CreateEscapeDoor();
	m_GameRule->InitializeSceneProcessing(&DBD::m_RecvWorldData);
	m_GameRule->PushEventCamera(Cam1, Cam2);

	return true;
}

void CMainPlayScene::CreateWorldObjData(const WorldObjData& _Data)
{
	CActor* CirlceActor = nullptr;

	for (size_t i = 0; i < _Data.Header.ObjCount; i++)
	{
		CirlceActor =  CreateActor(UPDATE_LAYER_NORMAL, L"CircleActor");
		m_GameRule->CreateMagicCircle(CirlceActor, _Data.arrCircleData[i]);
	}
}
void CMainPlayScene::PreUpdate(const float & _DeltaTime)
{
	m_SoundAccTime += _DeltaTime;
	//사운드 넣기
	if (CTimerMgr::TimingCheck(m_SoundAccTime, 10.f))
	{
		CSoundPlayer::GlobalPlaySound(SoundTypeName[SSDRandom::RandomInt(0, 5)]);
		CSoundPlayer::SetBGMVolume(0.05f);
	}

	GETSINGLE(ClientConnector).SendPlayerState(_DeltaTime);
	ScenePacketProcessing(_DeltaTime);
}

void CMainPlayScene::PostUpdate(const float & _DeltaTime)
{
}

void CMainPlayScene::LastUpdate(const float & _DeltaTime)
{
	
}

void CMainPlayScene::DebugRender()
{

}



void CMainPlayScene::ScenePacketProcessing(const float& _DeltaTime)
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
			m_GameRule->CollsionPacketProcessing(Pack);
			break;
		default:
			break;
		}

		//// 2. 패킷에서 게임 동작에 관련된 정보를 뽑아낸다.
		//switch (Pack.Header.DataType)
		//{
		//	// 현재 접속중인 리모트 플레이어들의 상태들을 업데이트
		//case FLAG_DATA_ROOM_WORLD:
		//	m_GameRule->RoomDataProcessing(Pack);
		//	break;
		//case FLAG_DATA_SYNCRO_WORLD:
		//	m_GameRule->SyncroWorldProcessing(Pack);
		//	break;
		//	// 게임 중 접속이 끊기거나 게임종료을 한 플레이어 처리
		//case FLAG_GAME_EXIT_USER:
		//	m_GameRule->ExitPlayerProcessing(Pack);
		//	break;
		//case FLAG_DATA_BROADCATE_REMOTE:
		//	m_GameRule->RemotePlayerProcessing(Pack);
		//default:
		//	break;
		//}
	}


}

void CMainPlayScene::WakeUp()
{
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PlayScene.mp3"));
	CSoundPlayer::PlayBGM(L"PlayScene");
	CSoundPlayer::SetBGMVolume(0.05f);

}
