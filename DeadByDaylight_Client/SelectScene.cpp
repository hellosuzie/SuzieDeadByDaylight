#include "PreHeader.h"
#include "SelectScene.h"
#include <UIBase.h>
#include <MouseScript.h>
#include <FreeCameraScript.h>
#include <SSDCore.h>
#include <Skybox.h>	
#include <KeyMgr.h>
#include <DeadByDaylightGlobal.h>
#include <DXFont.h>
#include <SSDThread.h>
#include "RoomSelectScene.h"
#include "MainPlayScene.h"
#include "EndingScene.h"
#include "KillerLobbyScene.h"
#include "KillerVictoryScene.h"
#include "SurvivorLobbyScene.h"
#include "SurvivorVictoryScene.h"
#include "MainLoadingScene.h"
#include <Fbx.h>
#include <Sound.h>
#include <SoundPlayer.h>

#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>


unsigned int  _stdcall IntroLoadingProcessing(void* _pArg)
{
	
	CSelectScene* Select = reinterpret_cast<CSelectScene*>(_pArg);
	Select->LoadGameResource();

	Select->m_BuildCount++;
	CScene::CreateScene<CRoomSelectScene>(L"RoomSelectScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CSurvivorLobbyScene>(L"SurvivorLobbyScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CKillerLobbyScene>(L"KillerLobbyScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CSurvivorVictoryScene>(L"SurvivorVictoryScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CKillerVictoryScene>(L"KillerVictoryScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CEndingScene>(L"EndingScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CMainPlayScene>(L"MainPlayScene", true);
	Select->m_BuildCount++;
	CScene::CreateScene<CMainLoadingScene>(L"MainLoadingScene", true);
	Select->m_BuildCount++;

	std::chrono::system_clock::time_point Prev = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point Cur;
	std::chrono::duration<float> Time;

	GETSINGLE(ClientConnector).ConnectLogicServer();

	do 
	{
		Cur = std::chrono::system_clock::now();
		Time = Cur - Prev;
		GETSINGLE(CClientConnectMgr).CheckConnectServer(Time.count());
		Prev = Cur;
	}while (false == GETSINGLE(CClientConnectMgr).IsCompleteConnect());
	Select->m_BuildCount++;


	return 0;
}

CSelectScene::CSelectScene(CScene* _PairScene) 
	: CSceneScript(_PairScene), m_fAccTime(0.f), m_eIntroSelectScene(INTRO_SCENE)
	, m_fLoadingProgress(0.f), m_BuildCount(0)
{
	ZeroMemory(m_IntroSelectUI, sizeof(m_IntroSelectUI));
}


CSelectScene::~CSelectScene()
{
}

// UI로 쓸 텍스쳐를 로딩한다.
const bool CSelectScene::Loading()
{
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"LoadingScene.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"LoadingBack.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SelectScene.png"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SelectKiller.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SelectPlayer.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"None.png"));

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog0.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog1.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Fog3.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Gate1.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Gate2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeBlack.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeRed.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeWhite.png"));


	CResourceMgr<CMultiTexture>::Create(L"FadeParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"FadeWhite", L"FadeBlack", L"FadeRed" });
	CResourceMgr<CMultiTexture>::Create(L"FogParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 6, L"Fog0", L"Fog1", L"Fog2", L"Fog3", L"Gate1", L"Gate2" });
	CResourceMgr<CMultiTexture>::Create(L"Floor2", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"BaseFloor_2", L"TerrainDiff_2", L"TerrainNormal_2" });



	SPTR<CCamera> MainCam = GetMainCamera();
	MainCam->ACTOR->AddCom<CMouseScript>();

	SPTR<CCamera> UICam = GetUICamera();

	m_IntroSelectUI[INTRO_BACKUI] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"LoadingScene" });
	m_IntroSelectUI[INTRO_BACKUI]->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_IntroSelectUI[INTRO_BACKUI]->SetSubPivot(Vec3(0.f, 0.f, 100.f));

	m_IntroSelectUI[INTRO_UI] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"LoadingBack" });
	m_IntroSelectUI[INTRO_UI]->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_IntroSelectUI[INTRO_UI]->SetSubPivot(Vec3(0.f, -500.f, 101.f));

	//플레이어
	m_IntroSelectUI[SELECT_SURVIVOR_UI] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"None" });
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetSubSize(Vec3(400.f, 465.f, 0.f));
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetSubPivot(Vec3(-320.f, 0.f, 105.f));
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetCollSize(Vec3(400.f, 465.f, 0.f));
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetCollPivot(Vec3(-320.f, 0.f, 105.f));
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetEnterCallBack(this, &CSelectScene::SurvivalEnter);
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetStayCallBack(this, &CSelectScene::SurvivalStay);
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetExitCallBack(this, &CSelectScene::SurvivalExit);

	//킬러
	m_IntroSelectUI[SELECT_KILLER_UI] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"None" });
	m_IntroSelectUI[SELECT_KILLER_UI]->SetSubSize(Vec3(400.f, 465.f, 0.f));
	m_IntroSelectUI[SELECT_KILLER_UI]->SetSubPivot(Vec3(315.f, 0.f, 105.f));
	m_IntroSelectUI[SELECT_KILLER_UI]->SetCollSize(Vec3(400.f, 465.f, 0.f));
	m_IntroSelectUI[SELECT_KILLER_UI]->SetCollPivot(Vec3(315.f, 0.f, 105.f));
	m_IntroSelectUI[SELECT_KILLER_UI]->SetEnterCallBack(this, &CSelectScene::KillerEnter);
	m_IntroSelectUI[SELECT_KILLER_UI]->SetStayCallBack(this, &CSelectScene::KillerStay);
	m_IntroSelectUI[SELECT_KILLER_UI]->SetExitCallBack(this, &CSelectScene::KillerExit);

	m_IntroSelectUI[SELECT_BACKUI] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"SelectScene" });
	m_IntroSelectUI[SELECT_BACKUI]->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_IntroSelectUI[SELECT_BACKUI]->SetSubPivot(Vec3(0.f, 0.f, 106.f));

	for (size_t i = SELECT_SURVIVOR_UI; i < MAX_INTRO_SELECT_UI; i++)
	{
		m_IntroSelectUI[i]->Off();
	}


	SSDThread::CreateThread(L"IntroLoading", &IntroLoadingProcessing, (void*)this);


	return true;
}

void CSelectScene::PreUpdate(const float & _DeltaTime)
{
	if (m_BuildCount != COMPLETE_CONNECT_SERVER)
		return;

	if (GETSINGLE(CClientConnectMgr).EmptyPacketQueue())
		return;

	PACKET Pack;

	while (GETSINGLE(CClientConnectMgr).ExistPacket())
	{
		// 1. 서버에서 전달받은 패킷을 가져온다
		GETSINGLE(CClientConnectMgr).GetPacket(Pack);

		// 2. 패킷에서 게임 동작에 관련된 정보를 뽑아낸다.
		switch (Pack.Header.HeaderFlag)
		{
			// 현재 접속중인 리모트 플레이어들의 상태들을 업데이트
		case FLAG_REQUEST_ROOM_LIST: 
		{
			RoomList ListData;

			unsigned int* DataSize = (unsigned int*)(Pack.Packet);

			ListData = ExtractionData<RoomList>(Pack, *DataSize);
			DBD::RecordRoomListData(ListData);
			CScene::ChangeScene(L"RoomSelectScene");
		}
			break;
		case FLAG_ENTER_ROOM_SUCCESS:
		{
			LobbyEnterData LData;

			LData = ExtractionData<LobbyEnterData>(Pack);
			DBD::RecordRecvRoomData(LData);
			CScene::ChangeScene(L"KillerLobbyScene");
		}
		
			break;
		default:
			break;
		}
	}

}

void CSelectScene::PostUpdate(const float & _DeltaTime)
{
	switch (m_eIntroSelectScene)
	{
	case INTRO_SCENE:
		IntroLoadingProgress(_DeltaTime);
		break;
	default:
		break;
	}

}


void CSelectScene::IntroLoadingProgress(const float& _DeltaTime) 
{
	if (m_BuildCount == LOAD_GAMERESOURCE) 
	{
		m_IntroSelectUI[INTRO_UI]->SetSubPivot(Vec3(0.f, LoadingGauge(m_fLoadingProgress / INTRORESOURCECOUNT), 101.f));
	}

	else if (m_BuildCount == COMPLETE_CONNECT_SERVER)
	{
		GETSINGLE(CClientConnectMgr).OperationSessionThread();
		m_eIntroSelectScene = SELECT_SCENE;
		

		for (size_t i = INTRO_BACKUI; i < SELECT_SURVIVOR_UI; i++)
		{
			m_IntroSelectUI[i]->Off();
		}

		for (size_t i = SELECT_SURVIVOR_UI; i < MAX_INTRO_SELECT_UI; i++)
		{
			m_IntroSelectUI[i]->On();
		}	


		CSoundPlayer::PlayBGM(L"LobbyScene", 0.1f);
	}

}
void CSelectScene::SelectProgress(const float& _DeltaTime) 
{

}
float CSelectScene::LoadingGauge(float _Percent)
{
	if (0.f > _Percent)
	{
		_Percent = 0.f;
	}
	if (1.f < _Percent)
	{
		_Percent = 1.f;
	}

	float TempPos = (_Percent * 500.f) - 500.f;
	return TempPos;
}

void CSelectScene::KillerEnter()
{
	m_IntroSelectUI[SELECT_KILLER_UI]->SetUITexture(L"SelectKiller");
	//m_TempUI4->SetUITexture(L"SelectKiller");
	CSoundPlayer::GlobalPlaySound(L"Button");
}

void CSelectScene::KillerStay()
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");

		//CScene::ChangeScene(L"KillerLobbyScene");
		// 룸 배정받는 패킷을 전송한다.
		GETSINGLE(CClientConnectMgr).RequestEnterLobbyRoom(PLAYER_KILLER);
	}
}

void CSelectScene::KillerExit()
{
	m_IntroSelectUI[SELECT_KILLER_UI]->SetUITexture(L"None");
	//m_TempUI4->SetUITexture(L"None");
}

void CSelectScene::SurvivalEnter()
{
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetUITexture(L"SelectPlayer");
	//m_TempUI3->SetUITexture(L"SelectPlayer");
	CSoundPlayer::GlobalPlaySound(L"Button");
}

void CSelectScene::SurvivalStay()
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");

		//CScene::ChangeScene(L"RoomSelectScene");
		// 현재 활성화된 Room의 정보를 요구하는 패킷을 전송
		GETSINGLE(CClientConnectMgr).RequestActiveRoomList();
	}
}

void CSelectScene::SurvivalExit()
{
	m_IntroSelectUI[SELECT_SURVIVOR_UI]->SetUITexture(L"None");
	//m_TempUI3->SetUITexture(L"None");
}
void CSelectScene::SceneRender()
{
	if (m_eIntroSelectScene != INTRO_SCENE)
		return;

	wchar_t	DebugBuff[MAXSTRING];
	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);

	switch (m_BuildCount)
	{
	case BUILD_ROOMSELECT_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 1 / 8");
		break;
	case BUILD_SURVIVORLOBBY_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 2 / 8");
		break;
	case BUILD_KILLERLOBBY_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 3 / 8");
		break;
	case BUILD_SURVIVORVICTORY_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 4 / 8");
		break;
	case BUILD_KILLERVICTORY_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 5 / 8");
		break;
	case BUILD_ENDING_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 6 / 8");
		break;
	case BUILD_MAINLOADING_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 7 / 8");
		break;
	case BUILD_MAINPLAY_SCENE:
		swprintf_s(DebugBuff, L"Build Scene 8 / 8");
		break;
	case TRY_CONNECT_SERVER:
		swprintf_s(DebugBuff, L"Try Connect Server");
		break;
	default:
		break;
	}

	DBD::pPrettyFont->DrawString(DebugBuff, 22.f, Vec2(Core::GetClientWidth() * 0.5f, Core::GetClientHeight() * 0.95f), Vec4::Pink, FW1_TEXT_FLAG::FW1_CENTER);
}

void CSelectScene::LoadGameResource()
{
	if (m_fLoadingProgress != 0.f)
		return;

	LoadFBX();
	LoadTexture();
	LoadSound();
}
void CSelectScene::LoadTexture()
{
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"SkyboxNight.dds"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff.tga"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal.tga"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Burn.jpg"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Noise.jpg"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints.tga"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FootPrints_N.tga"));
	m_fLoadingProgress += 1.f;

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number1.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number2.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number3.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number4.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number5.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"PlayerState.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Perk.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Dead.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Exit.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"HitOne.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"HitTwo.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Hook.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Idle.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Back.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"DestroyMagic.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Gauge.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"GaugeFull.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Heal.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"RunMagic.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SelfHeal.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"StartMagic.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Ready.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"ReadyBase.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"PlayerReady.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"PlayerNonReady.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Shop.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Slash.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"SummonsHook.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Rescue.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Number0.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"RoomScene.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Select.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Back.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"None.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Room.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"RoomEmpty.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"RoomSelect.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Reset.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"ResetSelect.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Defeat.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"MainLoading.png"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"UI", L"Win.png"));
	m_fLoadingProgress += 1.f;

}

void CSelectScene::LoadFBX() 
{
	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	m_fLoadingProgress += 1.f;
	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor", CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	m_fLoadingProgress += 1.f;

	CResourceMgr<CFbx>::Load(L"killedspider", CFilePathMgr::GetPath(L"Animation", L"killedspider.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	m_fLoadingProgress += 1.f;

	//나무 fbx 로드
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush01.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush02.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush03.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush04.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush05.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"Bush06.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"SM_SlaughterTree01.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"LV_Campagna_Tree04.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;
	CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Architecture", L"LV_Campagna_Tree05.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"" });
	m_fLoadingProgress += 1.f;


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
		m_fLoadingProgress += 1.f;
		result = _findnext(handle, &fd);
	}
}

void CSelectScene::LoadSound()
{
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back1.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back2.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back3.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back4.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back5.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Back6.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Chase.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Click.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"DoorOpen.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Generator.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"GeneratorOn.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"KBlood.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"KHitFail.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"KillerNear.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"KKillerHit.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"KPlayerHang.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"LobbyScene.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PBreath.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PBreath2.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PFirstHitidle.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PHang.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PHangIdle.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PHeal.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PHit.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PlayScene.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PSecondHit.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PSecondHitIdle.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"Button.mp3"));
	m_fLoadingProgress += 1.f;
	CResourceMgr<CSound>::Load(CFilePathMgr::GetPath(L"Sound", L"PDead.mp3"));
	m_fLoadingProgress += 1.f;
}

void CSelectScene::WakeUp()
{
}

void CSelectScene::ShutDown()
{
}
