#include "DeadByDaylightBuilder.h"
#include <ClientConnectMgr.h>
#include <Collider.h>
#include <Scene.h>
#include <FilePathMgr.h>
#include <DeadByDaylightGlobal.h>
#include "IntroScene.h"
#include "PlayScene.h"
#include "TestScene.h"
#include <GameWindow.h>

#include "SelectScene.h"
#include "RoomSelectScene.h"
#include "MainPlayScene.h"
#include "EndingScene.h"
#include "KillerLobbyScene.h"
#include "KillerVictoryScene.h"
#include "SurvivorLobbyScene.h"
#include "SurvivorVictoryScene.h"

#include <SJ_SplashWindow.h>

DeadByDaylightBuilder::DeadByDaylightBuilder()
	: m_bOpenServer(false)
{
}


DeadByDaylightBuilder::~DeadByDaylightBuilder()
{
}
const BOOL DeadByDaylightBuilder::PreBuild()
{
	SPLASHDEVICEINFO Info;
	Info.ClassName = L"SplashWindow";
	Info.Title = L"Title";
	Info.WndPos = POINT{ 0,0 };
	Info.WndSize = POINT{ 810,460 };
	Info.BackBuffColor = RGB(255, 0, 255);
	Info.ImagePath = CFilePathMgr::GetPath(L"Texture") + L"DBDSplash.png";

	CSplashDevice::Inst()->InitDevice(Info);
	return TRUE;
}
const BOOL DeadByDaylightBuilder::Build()
{	
	GlobalDBD::Init();
	CCollider::ColliderInit();

	// 1. 접속 절차가 있는 서버 - 클라
	DeadClientBuild();
	
	// 2. 접속 절차가 생략된 서버 - 클라 
	//ClientBuild();
	
	// 3. 서버 비접속 클라이언트  
	//TestClientBuild();

	return TRUE;
}
void DeadByDaylightBuilder::DeadClientBuild()
{
	m_bOpenServer = true;
	GETSINGLE(CClientConnectMgr).Init();
	CScene::CreateScene<CSelectScene>(L"SelectScene", true);
	//CScene::CreateScene<CRoomSelectScene>(L"RoomSelectScene", true);
	//CScene::CreateScene<CMainPlayScene>(L"MainPlayScene", true);
	//CScene::CreateScene<CEndingScene>(L"EndingScene", true);
	//CScene::CreateScene<CKillerLobbyScene>(L"KillerLobbyScene", true);
	//CScene::CreateScene<CKillerVictoryScene>(L"KillerVictoryScene", true);
	//CScene::CreateScene<CSurvivorLobbyScene>(L"SurvivorLobbyScene", true);
	//CScene::CreateScene<CSurvivorVictoryScene>(L"SurvivorVictoryScene", true);

	CScene::ChangeScene(L"SelectScene");

	CSplashDevice::Inst()->EnableSplashWindow(false);
	CSplashDevice::Inst()->SetDeviceProcess(false);
	Core::ShowGameWindow(true);
}
void DeadByDaylightBuilder::ClientBuild() 
{
	// 씬로딩과  접속 로직은 쓰레드로 돌리고
	// 두 작업이 끝나면 로비 씬으로 이동
	// 1. 인트로 씬 로딩 : 텍스쳐,스프라이트 와같은 그래픽 리소스 로드
	// 2. 로비 씬 로딩 : 로비 UI 및  로비씬용 캐릭터 미리 만들기
	// 3. 메인(게임) 씬 로딩 : 프리팹 데이터 로드 및 오브젝트 미리 생성
	// 서버가 연결 된 씬 
	
	Core::SetProcessPrioriy(REALTIME_PRIORITY_CLASS);
	m_bOpenServer = true;
	GETSINGLE(CClientConnectMgr).Init();
	CScene::CreateScene<CPlayScene>(L"PlayScene", true);
	CScene::CreateScene<CIntroScene>(L"IntroScene", true);
	CScene::CreateScene<CEndingScene>(L"EndingScene", true);

	CScene::ChangeScene(L"IntroScene");
}
void DeadByDaylightBuilder::TestClientBuild() 
{
	// 서버가 연결 안된 씬
	m_bOpenServer = false;
	CScene::CreateScene<CTestScene>(L"TestScene", true);
	//CScene::CreateScene<CSelectScene>(L"SelectScene", true);
	//CScene::CreateScene<CRoomSelectScene>(L"RoomSelectScene", true);
	//CScene::CreateScene<CKillerLobbyScene>(L"KillerLobbyScene", true);
	//CScene::CreateScene<CSurvivorLobbyScene>(L"SurvivorLobbyScene", true);

	CScene::ChangeScene(L"TestScene");
	//CScene::ChangeScene(L"SelectScene");
	//CScene::ChangeScene(L"KillerLobbyScene");
}
void DeadByDaylightBuilder::BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_Msg)
	{
	case WM_MOVE:
	case WM_SIZE:
	{
		CGameWindow::SyncroWindowScreen();
	}
	break;
	case WM_ACTIVATE:
	{
		Core::FocusingClient((bool)_wParam);
	}
		break;
	default:
		break;
	}
}

const BOOL DeadByDaylightBuilder::Release()
{
	GlobalDBD::Release();

	if (true == m_bOpenServer) 
	{
		GETSINGLE(ClientConnector).DisConnectLogicServer();
	}
	return TRUE;
}      

CORESTART_HIDE(DeadByDaylightBuilder, L"Client", Vec2(1600 , 900), Vec2(0, 15))
