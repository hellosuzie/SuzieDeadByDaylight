#include "PreHeader.h"
#include "KillerLobbyScene.h"
#include <DXFont.h>
#include <Terrain.h>
#include <Skybox.h>
#include <FreeCameraScript.h>
#include <TransForm.h>
#include <Fbx.h>
#include <SkeletalFbxRenderer.h>
#include <DissolveSkeletalRenderer.h>
#include <KeyMgr.h>
#include <FbxRenderer.h>
#include <SSDRandom.h>
#include <FogFilter.h>
#include <DepthFieldFilter.h>
#include <ParticleRenderer.h>
#include <GameWindow.h>
#include "MainPlayScene.h"
#include "MainLoadingScene.h"
#include <SoundPlayer.h>

Vec3							CKillerLobbyScene::KillerLobbySurvivorPosition[4] = { Vec3(650.f,0.f, 90.f), Vec3(550.f,0.f,240.f) , Vec3(730.f,0.f,160.f) , Vec3(660.f,0.f,320.f) };
Vec3							CKillerLobbyScene::KillerLobbySurvivorUIPosition[4] = {  Vec3(-490.f,130.f, 100.f), Vec3(-200.f,115.f,100.f) , Vec3(-70.f,130.f,100.f) , Vec3(145.f,115.f,100.f) } ;

CKillerLobbyScene::CKillerLobbyScene(CScene* _PairScene) 
	: CSceneScript(_PairScene), m_bReady(false), m_UserCount(0)
{
}


CKillerLobbyScene::~CKillerLobbyScene()
{
}

void CKillerLobbyScene::WakeUp()
{
	//CSoundPlayer::PlayBGM(L"LobbyScene", 0.1f);
	LobbyEnterData CurLData = DBD::m_RecvRoomData;

	m_Killer.LobbyID = CurLData.LobbyID;
	CurLData.EnterRData.RoomUserCount;
	m_bReady = false;

	switch (CurLData.EnterRData.RoomNumber)
	{
	case 1:
		m_RoomNumUI[ROOM_NUMBER]->SetUITexture(L"Number1");
		break;
	case 2:
		m_RoomNumUI[ROOM_NUMBER]->SetUITexture(L"Number2");
		break;
	case 3:
		m_RoomNumUI[ROOM_NUMBER]->SetUITexture(L"Number3");
		break;
	case 4:
		m_RoomNumUI[ROOM_NUMBER]->SetUITexture(L"Number4");
		break;
	case 5:
		m_RoomNumUI[ROOM_NUMBER]->SetUITexture(L"Number5");
		break;
	default:
		break;
	}

	switch (CurLData.EnterRData.RoomUserCount)
	{
	case 1:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number1");
		break;
	case 2:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number2");
		break;
	case 3:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number3");
		break;
	case 4:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number4");
		break;
	case 5:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number5");
		break;
	default:
		break;
	}
	
	m_UserCount = CurLData.EnterRData.RoomUserCount;
	m_RoomButton[ROOM_BUTTON_READY]->SetUITexture(L"Ready");
	m_RoomButton[ROOM_BUTTON_EXIT]->SetUITexture(L"Back");
	if (nullptr != m_Killer.LobbyRen)
	{
		m_Killer.LobbyRen->SetReverseDissolve(1.5f);
	}
}
const bool CKillerLobbyScene::Loading()
{
	SPTR<CCamera> MainCam = GetMainCamera();
	MainCam->TRANS->SetPos(Vec3(930.f, 84.f, 0.f));
	MainCam->TRANS->SetRotate(Vec3(0.5f, -47.5f, 0.f));
	SPTR<CFogFilter> TempFog = MainCam->AddFilter<CFogFilter>(MainCam);
	TempFog->SetFogStartDepth(600.f);
	MainCam->AddFilter<CDepthFieldFilter>();

	SPTR<CCamera> UICam = GetUICamera();

	TreeTypeName[BUSH1] = L"Bush01";
	TreeTypeName[BUSH2] = L"Bush02";
	TreeTypeName[BUSH3] = L"Bush03";
	TreeTypeName[BUSH4] = L"Bush04";
	TreeTypeName[BUSH5] = L"Bush05";
	TreeTypeName[BUSH6] = L"Bush06";
	TreeTypeName[TREE1] = L"LV_Campagna_Tree05";
	TreeTypeName[TREE2] = L"LV_Campagna_Tree04";

	//디폴트 라이트
	SPTR<CActor> DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	DLight->SetLightColor(Vec4::PastelBlue);
	DLight->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	DLight->SetLightPower(1.f);
	DLight->SetSpecPower(200.f);

	//터레인
	SPTR<CActor> TerrainActor = CreateActor(UPDATE_LAYER_NORMAL, L"TerrainActor");
	SPTR<CTerrain> Terrain = TerrainActor->AddCom<CTerrain>();
	Terrain->CreateTerrain(L"BaseFloor", L"TerrainDiff", L"TerrainNormal", 20, 20, 30.f);
	Terrain->CreateFloor(L"Floor2");
	Terrain->SetCellSize(50.f);
	DBD::pMainTerrain = Terrain;

	// 스카이 박스
	SPTR<CActor> SkyActor = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL);
	SPTR<CSkybox> SkyBox = SkyActor->AddCom<CSkybox>(RENDATA{ RENDER_LAYER_MAP });
	SkyBox->SetSkyBoxSize(10000.f);
	SkyBox->SetTexture(L"SkyboxNight");

	//킬러
	SPTR<CActor> TestActor = CreateActor(L"TestMeshActor", UPDATE_LAYER_NORMAL);
	TestActor->TRANS->SetRotate(Vec3(-90.f, 140.f, 0.f));
	TestActor->TRANS->SetPos(Vec3(900.f, 0.f, 60.f));
	TestActor->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));
	
	m_Killer.LobbyRen = TestActor->AddCom<CDissolveSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_Killer.LobbyRen->SetFbx(L"killer");
	m_Killer.LobbyRen->SetAllFbxMesh();
	m_Killer.LobbyRen->CreateBoneAnimation(L"Idle", 0, 0, 60);
	m_Killer.LobbyRen->ChangeBoneAnimation(L"Idle");
	m_Killer.ReadySignUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"PlayerNonReady" });
	m_Killer.ReadySignUI->SetSubSize(Vec3(90.f, 90.f, 0.f));
	m_Killer.ReadySignUI->SetSubPivot(Vec3(430.f,110.f,100.f));
	
	// 생존자 
	for (size_t i = 0; i < 4; i++)
	{
		LobbyPlayer Player;
		Player.Idx = (UINT)i;
		TestActor = CreateActor(L"SurvivorActor", UPDATE_LAYER_NORMAL);
		TestActor->TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));
		TestActor->TRANS->SetRotate(Vec3(-90.f, 140.f, 0.f));
		TestActor->TRANS->SetPos(KillerLobbySurvivorPosition[Player.Idx]);

		Player.LobbyRen = TestActor->AddCom<CDissolveSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
		Player.LobbyRen->SetFbx(L"survivor");
		Player.LobbyRen->SetAllFbxMesh();
		Player.LobbyRen->CreateBoneAnimation(L"Idle",0);
		Player.LobbyRen->ChangeBoneAnimation(L"Idle");

		Player.ReadySignUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"PlayerNonReady" });
		Player.ReadySignUI->SetSubSize(Vec3(90.f, 90.f, 0.f));
		Player.ReadySignUI->SetSubPivot(KillerLobbySurvivorUIPosition[Player.Idx]);

		// 플레이어 위치 & 회전 변경 필요하여 위치값을 바로 확인하고 싶은 경우 랜더러와 UI를 Off 하지 마세요.
		// 위치 값은 SetSubPivot 함수 안에 있는 변수 혹은 변수 배열 안에 있습니다(Static 맴버 변수입니다.)
		Player.LobbyRen->Off();
		Player.ReadySignUI->Off();
		m_quSurvivor.push(Player);
	}


	//풀 깔기
	for (size_t i = 0; i < 30; ++i)
	{
		int type = SSDRandom::RandomInt(0, 5);
		int size = SSDRandom::RandomInt(2, 3);
		SPTR<CActor> BushActor = CreateActor(L"BushActor", UPDATE_LAYER_NORMAL);
		BushActor->TRANS->SetRotate(Vec3(0.f, SSDRandom::RandomFloat(0.f, 360.f), 0.f));
		BushActor->TRANS->SetPos(Vec3(SSDRandom::RandomFloat(0.f, 400.f), 0.f, SSDRandom::RandomFloat(100.f, 700.f)));
		BushActor->TRANS->SetScale(Vec3(2.f, 2.f, 2.f));

		SPTR<CStaticFbxRenderer> TempBush = BushActor->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
		TempBush->SetFbx(TreeTypeName[type].c_str());
		TempBush->SetAllFbxMesh();
	}

	for (size_t i = 0; i < 30; ++i)
	{
		int type = SSDRandom::RandomInt(0, 5);
		int size = SSDRandom::RandomInt(2, 3);
		SPTR<CActor> BushActor = CreateActor(L"BushActor", UPDATE_LAYER_NORMAL);
		BushActor->TRANS->SetRotate(Vec3(0.f, SSDRandom::RandomFloat(0.f, 360.f), 0.f));
		BushActor->TRANS->SetPos(Vec3(SSDRandom::RandomFloat(0.f, 900.f), 0.f, SSDRandom::RandomFloat(600.f, 800.f)));
		BushActor->TRANS->SetScale(Vec3(2.f, 2.f, 2.f));

		SPTR<CStaticFbxRenderer> TempBush = BushActor->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
		TempBush->SetFbx(TreeTypeName[type].c_str());
		TempBush->SetAllFbxMesh();
	}

	//나무 깔기
	for (size_t i = 0; i < 30; ++i)
	{
		int type = SSDRandom::RandomInt(6, 7);
		SPTR<CActor> TreeActor = CreateActor(L"TreeActor", UPDATE_LAYER_NORMAL);
		TreeActor->TRANS->SetRotate(Vec3(0.f, SSDRandom::RandomFloat(0.f, 360.f), 0.f));
		TreeActor->TRANS->SetPos(Vec3(SSDRandom::RandomFloat(-200.f, 300.f), 0.f, SSDRandom::RandomFloat(100.f, 700.f)));

		SPTR<CStaticFbxRenderer> TempTree = TreeActor->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
		TempTree->SetFbx(TreeTypeName[type].c_str());
		TempTree->SetAllFbxMesh();
	}

	for (size_t i = 0; i < 50; ++i)
	{
		int type = SSDRandom::RandomInt(6, 7);
		SPTR<CActor> TreeActor = CreateActor(L"TreeActor", UPDATE_LAYER_NORMAL);
		TreeActor->TRANS->SetRotate(Vec3(0.f, SSDRandom::RandomFloat(0.f, 360.f), 0.f));
		TreeActor->TRANS->SetPos(Vec3(SSDRandom::RandomFloat(0.f, 900.f), 0.f, SSDRandom::RandomFloat(700.f, 1100.f)));

		SPTR<CStaticFbxRenderer> TempTree = TreeActor->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
		TempTree->SetFbx(TreeTypeName[type].c_str());
		TempTree->SetAllFbxMesh();
	}

	//안개파티클
	SPTR<CActor> PTest1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	SPTR<CParticleRenderer> PRender1 = PTest1->AddCom<CParticleRenderer>(RENDATA{ RENDER_LAYER::RENDER_LAYER_PLAYER });
	PRender1->PushParticleTex(L"FogParticle", L"FadeParticle");
	DBD::pMainParticle = PRender1;

	for (size_t i = 0; i < 10; ++i)
	{
		PRender1->PushParticle(PARTICLEVTX{
			Vec3(SSDRandom::RandomFloat(-100.f, 500.f), 200.f, SSDRandom::RandomFloat(200.f, 400.f)),
			Vec3(),
			(float)SSDRandom::RandomInt(3),
			400.f + SSDRandom::RandomFloat(-100.f, 200.f), 400.f + SSDRandom::RandomFloat(-100.f, 200.f),
			Vec2((SSDRandom::RandomBool(0.5f)) ? 1.f : -1.f, SSDRandom::RandomFloat(0.02f, 0.05f)),
			Vec4(1, 0, 0, 0), Vec3(), Vec3(), Vec4() });
	}

	for (size_t i = 0; i < 12; ++i)
	{
		PRender1->PushParticle(PARTICLEVTX{
			Vec3(SSDRandom::RandomFloat(300.f, 800.f), 200.f, SSDRandom::RandomFloat(600, 1200.f)),
			Vec3(),
			(float)SSDRandom::RandomInt(3),
			400.f + SSDRandom::RandomFloat(-100.f, 200.f), 400.f + SSDRandom::RandomFloat(-100.f, 200.f),
			Vec2((SSDRandom::RandomBool(0.5f)) ? 1.f : -1.f, SSDRandom::RandomFloat(0.02f, 0.05f)),
			Vec4(1, 0, 0, 0), Vec3(), Vec3(), Vec4() });
	}

	//볼륨라이트
	SPTR<CActor> VLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	VLightActor->TRANS->SetPos(Vec3(600.f, 500.f, 350.f));
	SPTR<CLight> VLight = VLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, SPOT_LIGHT, true, L"Cone_Lv360" });
	VLight->SetLightSize(Vec3(600.f, 600.f, 600.f));
	VLight->SetLightColor(Vec4::Pink);
	VLight->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	VLight->SetLightPower(0.5f);
	VLight->SetSpecPower(200.f);
	VLight->SetLightAtenuation(Vec4(1.5f, 0.3f, 0.f, 0.f));
	VLight->SetAmbiFactor(0.f);

	SPTR<CActor> VLightActorK = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	VLightActorK->TRANS->SetPos(Vec3(900.f, 300.f, 60.f));
	SPTR<CLight> VLightK = VLightActorK->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, SPOT_LIGHT, true, L"Cone_Lv360" });
	VLightK->SetLightSize(Vec3(400.f, 400.f, 400.f));
	VLightK->SetLightColor(Vec4::PastelRed);
	VLightK->SetLightDir(Vec3(-1.f, -1.f, 0.f));
	VLightK->SetLightPower(0.5f);
	VLightK->SetSpecPower(40.f);
	VLightK->SetLightAtenuation(Vec4(1.f, 0.3f, 0.f, 0.f));
	VLightK->SetAmbiFactor(0.f);

	

	for (size_t i = ROOM_NUMBER_SIGNATURE; i < MAX_ROOM_NUMBER_UI; ++i)
	{
		m_RoomNumUI[i] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	}

	for (size_t i = ROOM_ACCESS_USER; i < MAX_USER_COUNT_UI; ++i)
	{
		m_RoomUserUI[i] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	}

	for (size_t i = ROOM_BUTTON_READY; i < MAX_ROOM_BUTTON_UI; ++i)
	{
		m_RoomButton[i] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Number1" });
	}

	m_RoomNumUI[ROOM_NUMBER_SIGNATURE]->SetUITexture(L"Shop");
	m_RoomNumUI[ROOM_NUMBER_SIGNATURE]->SetSubSize(Vec3(60.f, 150.f, 0.f));
	m_RoomNumUI[ROOM_NUMBER_SIGNATURE]->SetSubPivot(Vec3(-750.f, 370.f, 300.f));
	m_RoomNumUI[ROOM_NUMBER_SIGNATURE]->SetUIColorFactor(Vec4::Yellow);

	m_RoomNumUI[ROOM_NUMBER]->SetSubSize(Vec3(60.f, 100.f, 0.f));
	m_RoomNumUI[ROOM_NUMBER]->SetSubPivot(Vec3(-680.f, 370.f, 300.f));
	m_RoomNumUI[ROOM_NUMBER]->SetUIColorFactor(Vec4::Yellow);

	m_RoomUserUI[ROOM_ACCESS_USER]->SetSubSize(Vec3(60.f, 100.f, 0.f));
	m_RoomUserUI[ROOM_ACCESS_USER]->SetSubPivot(Vec3(580.f, 370.f, 300.f));

	m_RoomUserUI[ROOM_USER_SLASH]->SetUITexture(L"Slash");
	m_RoomUserUI[ROOM_USER_SLASH]->SetSubSize(Vec3(60.f, 160.f, 0.f));
	m_RoomUserUI[ROOM_USER_SLASH]->SetSubPivot(Vec3(650.f, 370.f, 300.f));

	m_RoomUserUI[ROOM_MAXACCESS_USER]->SetUITexture(L"Number5");
	m_RoomUserUI[ROOM_MAXACCESS_USER]->SetSubSize(Vec3(60.f, 100.f, 0.f));
	m_RoomUserUI[ROOM_MAXACCESS_USER]->SetSubPivot(Vec3(720.f, 370.f, 300.f));

	m_RoomButton[ROOM_BUTTON_READY]->SetUITexture(L"Ready");
	m_RoomButton[ROOM_BUTTON_READY]->SetSubSize(Vec3(200.f, 59.f, 0.f));
	m_RoomButton[ROOM_BUTTON_READY]->SetSubPivot(Vec3(-650.f, -370.f, 300.f));
	m_RoomButton[ROOM_BUTTON_READY]->SetCollSize(Vec3(230.f, 60.f, 0.f));
	m_RoomButton[ROOM_BUTTON_READY]->SetCollPivot(Vec3(-650.f, -370.f, 300.f));
	m_RoomButton[ROOM_BUTTON_READY]->SetEnterCallBack(this, &CKillerLobbyScene::ReadyEnter);
	m_RoomButton[ROOM_BUTTON_READY]->SetStayCallBack(this, &CKillerLobbyScene::ReadyStay);
	m_RoomButton[ROOM_BUTTON_READY]->SetExitCallBack(this, &CKillerLobbyScene::ReadyExit);

	m_RoomButton[ROOM_BUTTON_EXIT]->SetUITexture(L"Back");
	m_RoomButton[ROOM_BUTTON_EXIT]->SetSubSize(Vec3(200.f, 59.f, 0.f));
	m_RoomButton[ROOM_BUTTON_EXIT]->SetSubPivot(Vec3(650.f, -370.f, 300.f));
	m_RoomButton[ROOM_BUTTON_EXIT]->SetCollSize(Vec3(230.f, 60.f, 0.f));
	m_RoomButton[ROOM_BUTTON_EXIT]->SetCollPivot(Vec3(650.f, -370.f, 300.f));
	m_RoomButton[ROOM_BUTTON_EXIT]->SetEnterCallBack(this, &CKillerLobbyScene::BackEnter);
	m_RoomButton[ROOM_BUTTON_EXIT]->SetStayCallBack(this, &CKillerLobbyScene::BackStay);
	m_RoomButton[ROOM_BUTTON_EXIT]->SetExitCallBack(this, &CKillerLobbyScene::BackExit);
	return true;
}

void CKillerLobbyScene::PreUpdate(const float & _DeltaTime)
{

	if (GETSINGLE(CClientConnectMgr).EmptyPacketQueue())
		return;

	PACKET Pack;

	while (GETSINGLE(CClientConnectMgr).ExistPacket())
	{
		// 1. 서버에서 전달받은 패킷을 가져온다
		GETSINGLE(CClientConnectMgr).GetPacket(Pack);

		if (FLAG_LOBBY_WAITING != Pack.Header.HeaderFlag)
			continue;

		// 2. 패킷에서 게임 동작에 관련된 정보를 뽑아낸다.
		switch (Pack.Header.DataType)
		{
			// 현재 접속중인 리모트 플레이어들의 상태들을 업데이트
		case FLAG_DATA_ROOMENTER_USER:
		{
			unsigned __int64 ID = ExtractionData<unsigned __int64>(Pack);
			EnterUser(ID);
		}
		break;
		case FLAG_DATA_READY:
		{
			unsigned __int64 ID = ExtractionData<unsigned __int64>(Pack);
			ReadyUser(ID);
		}
		break;
		case FLAG_DATA_READY_CANCLE:
		{
			unsigned __int64 ID = ExtractionData<unsigned __int64>(Pack);
			ReadyCancleUser(ID);
		}
		break;
		case FLAG_DATA_ROOM_EXIT: 
		{
			unsigned __int64 ID = ExtractionData<unsigned __int64>(Pack);
			ExitUser(ID);
		}
			break;
		case FLAG_DATA_BUILD_GAMEWORLD:
		{
			WorldData BuildData;

			unsigned int* DataSize = (unsigned int*)(Pack.Packet);

			BuildData = ExtractionData<WorldData>(Pack, *DataSize);
			BuildGameWorld(BuildData);
		}
			break;
		default:
			break;
		}
	}

	
}

void CKillerLobbyScene::PostUpdate(const float & _DeltaTime)
{
}

void CKillerLobbyScene::LastUpdate(const float & _DeltaTime)
{
}

void CKillerLobbyScene::DebugRender()
{
	wchar_t	DebugBuff[MAXSTRING];
	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);

	swprintf_s(DebugBuff, L"Killer Lobby Scene");
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 0.f), Vec4::Pink);

	Vec3 CamPos = GetMainCamera()->GetPos();
	Vec3 CamRot = GetMainCamera()->GetRot();
	swprintf_s(DebugBuff, L"Camera Pos  : (%.1f , %.1f , %.1f)", CamPos.x, CamPos.y, CamPos.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 20.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Camera Rot  : (%.1f , %.1f , %.1f)", CamRot.x, CamRot.y, CamRot.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 40.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Mouse Pos  : (%d , %d )", (int)CGameWindow::GetMousePos().x, (int)CGameWindow::GetMousePos().y);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 60.f), Vec4::Pink);
}

void CKillerLobbyScene::ReadyEnter() 
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomButton[ROOM_BUTTON_READY]->SetUIAplha(0.3f);
}
void CKillerLobbyScene::ReadyStay() 
{
	if (true == KEYDOWN("LMouseClick")) 
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		// 서버로 준비가 버튼을 눌렀음을 알리는 패킷을 보낸다.
		// 현재 레디상태가 아닌경우 서버에 게임 대기 상태임을 알려주는 패킷을 보낸다
		if (false == m_bReady) 
		{
			GETSINGLE(CClientConnectMgr).NotifyReadyGame();
		}
		// 현재 레디 상태인경우 서버에 게임 대기하지 않음을 알려주는 패킷을 전송
		else 
		{
			GETSINGLE(CClientConnectMgr).NotifyCancleReady();
		}
	}
}
void CKillerLobbyScene::ReadyExit() 
{
	m_RoomButton[ROOM_BUTTON_READY]->SetUIAplha(1.f);
}
void CKillerLobbyScene::BackEnter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomButton[ROOM_BUTTON_EXIT]->SetUIAplha(0.3f);
}
void CKillerLobbyScene::BackStay()
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		// 서버로 방을 나가겠다는 패킷을 전송한다.
		GETSINGLE(CClientConnectMgr).NotifyLobbyExit();
	}
}
void CKillerLobbyScene::BackExit()
{
	m_RoomButton[ROOM_BUTTON_EXIT]->SetUIAplha(1.f);
}

void CKillerLobbyScene::EnterUser(const unsigned __int64& _ID) 
{
	if (m_Killer.LobbyID == _ID || true == m_quSurvivor.empty())
	{
		return;
	}

	LobbyPlayer EnterPlayer = m_quSurvivor.front();
	EnterPlayer.LobbyID = _ID;
	m_listRemoteSurvivor.push_back(EnterPlayer);
	EnterPlayer.LobbyRen->SetReverseDissolve(0.5f);
	EnterPlayer.ReadySignUI->On();

	m_quSurvivor.pop();

	++m_UserCount;
	switch (m_UserCount)
	{
	case 1:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number1");
		break;
	case 2:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number2");
		break;
	case 3:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number3");
		break;
	case 4:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number4");
		break;
	case 5:
		m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number5");
		break;
	default:
		break;
	}

}
void CKillerLobbyScene::ReadyUser(const unsigned __int64& _ID) 
{
	if (m_Killer.LobbyID == _ID) 
	{
		// Ready를 표현하는 UI와 Ready 버튼을 변경
		m_bReady = true;
		m_RoomButton[ROOM_BUTTON_READY]->SetUIColorFactor(Vec4::PastelRed.vec3);
		m_Killer.ReadySignUI->SetUITexture(L"PlayerReady");
	}
	else 
	{
		std::list<LobbyPlayer>::iterator Start = m_listRemoteSurvivor.begin();
		std::list<LobbyPlayer>::iterator End = m_listRemoteSurvivor.end();

		for (;Start!= End; ++Start)
		{
			if (Start->LobbyID == _ID) 
			{
				// 리포트 플레이어의 레디 UI를 변경
				Start->ReadySignUI->SetUITexture(L"PlayerReady");
				return;
			}
		}
	}
}
void CKillerLobbyScene::ReadyCancleUser(const unsigned __int64& _ID) 
{
	if (m_Killer.LobbyID == _ID)
	{
		// Ready를 표현하는 UI와 Ready 버튼을 변경
		m_bReady = false;
		m_RoomButton[ROOM_BUTTON_READY]->SetUIColorFactor(Vec3(1.f, 1.f, 1.f));
		m_Killer.ReadySignUI->SetUITexture(L"PlayerNonReady");
	}
	else
	{
		std::list<LobbyPlayer>::iterator Start = m_listRemoteSurvivor.begin();
		std::list<LobbyPlayer>::iterator End = m_listRemoteSurvivor.end();

		for (; Start != End; ++Start)
		{
			if (Start->LobbyID == _ID)
			{
				// 리포트 플레이어의 레디 UI를 변경
				Start->ReadySignUI->SetUITexture(L"PlayerNonReady");
				return;
			}
		}
	}
}
void CKillerLobbyScene::ExitUser(const unsigned __int64& _ID)
{
	if (m_Killer.LobbyID == _ID)
	{
		// Ready를 표현하는 UI와 Ready 버튼을 변경
		m_bReady = false;
		m_RoomButton[ROOM_BUTTON_READY]->SetUIColorFactor(Vec3(1.f, 1.f, 1.f));
		CScene::ChangeScene(L"SelectScene");
	}
	else
	{
		std::list<LobbyPlayer>::iterator Start = m_listRemoteSurvivor.begin();
		std::list<LobbyPlayer>::iterator End = m_listRemoteSurvivor.end();

		for (; Start != End; ++Start)
		{
			if (Start->LobbyID == _ID)
			{
				// 리포트 플레이어의 아바타와 UI를 비 활성화
				m_quSurvivor.push(*Start);
				m_quSurvivor.back().LobbyID = 0;
				m_quSurvivor.back().LobbyRen->SetDissolve(1.f);
				m_quSurvivor.back().ReadySignUI->Off();
				m_listRemoteSurvivor.erase(Start);

				--m_UserCount;
				switch (m_UserCount)
				{
				case 1:
					m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number1");
					break;
				case 2:
					m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number2");
					break;
				case 3:
					m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number3");
					break;
				case 4:
					m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number4");
					break;
				case 5:
					m_RoomUserUI[ROOM_ACCESS_USER]->SetUITexture(L"Number5");
					break;
				default:
					break;
				}

				return;
			}
		}


	}
}

void CKillerLobbyScene::BuildGameWorld(const WorldData& _Data)
{
	DBD::m_RecvWorldData = _Data;
	CScene::ChangeScene(L"MainLoadingScene");
}

void CKillerLobbyScene::InitializeLobby()
{
	m_bReady = false;
	m_RoomButton[ROOM_BUTTON_READY]->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_RoomButton[ROOM_BUTTON_EXIT]->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 1.f));

	while (false == m_listRemoteSurvivor.empty())
	{
		m_listRemoteSurvivor.front().LobbyID = 0;
		m_listRemoteSurvivor.front().ReadySignUI->SetUITexture(L"PlayerNonReady");
		m_listRemoteSurvivor.front().LobbyRen->Off();
		m_listRemoteSurvivor.front().ReadySignUI->Off();
		m_quSurvivor.push(m_listRemoteSurvivor.front());
	}

}