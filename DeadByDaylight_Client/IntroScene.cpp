#include "PreHeader.h"
#include "IntroScene.h"
#include <ClientConnectMgr.h>
#include <Sprite.h>
#include <ResourceMgr.h>
#include <Texture.h>
#include <Fbx.h>
#include <SSDCore.h>
#include <SSDThread.h>
#include <DXFont.h>
#include <GlobalDebug.h>
#include <DeadByDaylightGlobal.h>
#include "PlayScene.h"
#include <Camera.h>
#include <GaussianBlur.h>
#include <MiddleStruct.h>
#include <tuple>

#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>



unsigned _stdcall ConnectServer(void* _Arg) 
{
	GETSINGLE(ClientConnector).ConnectLogicServer();
	
	return 0;
}

unsigned _stdcall LoadingGameScene(void* _Arg)
{
	if (nullptr == _Arg)
		return 1;

	std::tuple<SPTR<CPlayScene>, WorldData>* Arg = (std::tuple<SPTR<CPlayScene>, WorldData>*)_Arg;

	SPTR<CPlayScene>  Scene = std::get<0>(*Arg);
	WorldData LoadData = std::get<1>(*Arg);
	Scene->AsyncLoading((&LoadData));

	return 0;
}

CIntroScene::CIntroScene(CScene* _PairScene)
	: CSceneScript(_PairScene), m_fAccTime(0.f), m_GameScene(nullptr)
	, m_eCurStep(INTRO_LOADING)
{
}


CIntroScene::~CIntroScene()
{
}


const bool CIntroScene::Loading()
{
	m_GameScene = CResourceMgr<CScene>::Find(L"PlayScene")->FindSceneScript<CPlayScene>();


	LoadTexture();
	LoadFBX();

	CORE::ChangeWindowTitle(L"TryConnect");
	

	GETSINGLE(ClientConnector).ConnectLogicServer();

	m_eCurStep = INTRO_WAIT_CONNECT;
	return true;
}
void CIntroScene::PreUpdate(const float& _DeltaTime)
{
	switch (m_eCurStep)
	{
	case INTRO_WAIT_CONNECT: 
	{
		GETSINGLE(CClientConnectMgr).CheckConnectServer(_DeltaTime);

		if (true == GETSINGLE(CClientConnectMgr).IsCompleteConnect()) 
		{
			m_eCurStep = INTRO_CONNECT_COMPLETE;
		}
	}
		break;
	case INTRO_CONNECT_COMPLETE: 
	{
		m_eCurStep = INTRO_SELECT_ROOM;
	}
		break;
	case INTRO_SELECT_ROOM:
	{
		// 연결 된 이후에는  생존자 & 살인자를 결정
		// 플레이어 타입을 결정한 다음에는 룸에 접속하기 위해 서버에 정보를 전달한다
		//GETSINGLE(CClientConnectMgr).RequestEnterRoom(PLAYER_SURVIVOR);
		GETSINGLE(CClientConnectMgr).RequestEnterTestRoom();
		m_eCurStep = INTRO_WAIT_ROOM_ENTER;
	}
	break;
	case INTRO_WAIT_ROOM_ENTER: 
	{
		WorldData Data;
		int Check = GETSINGLE(CClientConnectMgr).ReceiveRoomPacket(&Data);

		if (FLAG_ENTER_ROOM_FAIL == Check) 
		{
			//GETSINGLE(CClientConnectMgr).m_LogMgr.PrintLog(CClientLogMgr::MakeLogString("Fail Room Enter."));
			m_eCurStep = INTRO_SELECT_ROOM;
		}
		else if (FLAG_ENTER_ROOM_SUCCESS == Check) 
		{
			// 게임씬을 쓰레드 로딩 한다.
			//GETSINGLE(CClientConnectMgr).m_LogMgr.PrintLog(CClientLogMgr::MakeLogString("Success Room Enter."));
			m_LoadingData = std::make_tuple(m_GameScene, Data);
			SSDThread::CreateThread(L"LoadingThread", &LoadingGameScene, &m_LoadingData);
			m_eCurStep = INTRO_WAIT_ROOM_BUILD;
		}
	}
		break;
	case INTRO_WAIT_ROOM_BUILD:
	{
		PACKET Pack;

		while (true == GETSINGLE(CClientConnectMgr).ExistPacket())
		{

			GETSINGLE(CClientConnectMgr).GetPacket(Pack);

			switch (Pack.Header.DataType)
			{
			case FLAG_DATA_WORLD_OBJECT:
			{
				WorldObjData ObjData;
				unsigned int* DataSize = (unsigned int*)(Pack.Packet);
				ObjData = ExtractionData<WorldObjData>(Pack, *DataSize);
				m_GameScene->CreateWorldObjData(ObjData);

				if (true == ObjData.Header.LastData) 
				{
					Core::ChangeWindowTitle(L"Dead By Daylight");
					CScene::ChangeScene(L"PlayScene");
					return;
				}
			}
			break;
			default:
				break;
			}

		}

		//if (true == m_GameScene->IsAsyncLoadingComplete())
		//{
		//	Core::ChangeWindowTitle(L"Dead By Daylight");
		//	CScene::ChangeScene(L"PlayScene");
		//	return;
		//}
	}
		break;
	default:
		break;
	}

}
void CIntroScene::PostUpdate(const float& _DeltaTime)
{
	/*if (m_eCurStep >= INTRO_CONNECT_COMPLETE) 
	{
		GETSINGLE(CClientConnectMgr).ConnectUpdate(_DeltaTime);
	}*/
}
void CIntroScene::LoadTexture()
{

}
void CIntroScene::LoadFBX()
{

}

void CIntroScene::DebugRender()
{
	wchar_t DebugBuff[MAXSTRING] ;
	ZeroMemory(DebugBuff, sizeof(DebugBuff));

	switch (m_eCurStep)
	{
	case INTRO_LOADING:
		break;
	case INTRO_WAIT_CONNECT:
		swprintf_s(DebugBuff, L"1단계 : 서버 연결 대기중 ");
		break;
	case INTRO_CONNECT_COMPLETE:
		swprintf_s(DebugBuff, L"2단계 : 서버 연결 완료,게임 시작 로직 구동");
		break;
	case INTRO_SELECT_ROOM:
		swprintf_s(DebugBuff, L"3단계 : 방 선택 대기중");
		break;
	case INTRO_WAIT_ROOM_ENTER:
		swprintf_s(DebugBuff, L"4단계 : 방 입장 대기중");
		break;
	case INTRO_WAIT_ROOM_BUILD:
		swprintf_s(DebugBuff, L"5단계 : 플레이 씬 로딩중");
		break;
	case MAX_INTRO_STEP:
		break;
	default:
		break;
	}

	DBD::pPrettyFont->DrawString(DebugBuff, 20.f, Vec2(600.f, 700.f), Vec4::Pink);


	//SPTR<CFbx> GeneratorFbx = CResourceMgr<CFbx>::Load(L"generator", CFilePathMgr::GetPath(L"Animation", L"generator_fail_test.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION, L"Idle" });
	//GeneratorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"generator_fail.fbx"), L"fail");
	//KillerFbx->BinarySave(CFilePathMgr::GetPath(L"Animation", L"generator.sfbx").c_str());
	//SPTR<CFbx> BoardFbx = CResourceMgr<CFbx>::Load(L"board", CFilePathMgr::GetPath(L"Animation", L"board_fall.fbx"), CFbx::LOAD_DESC{ FBX_ANIMATION_AUTOSAVE, L"fall" });
	//FerryFbx->BinarySave(CFilePathMgr::GetPath(L"Architecture", L"FerryBoat.sfbx").c_str());

	//SPTR<CFbx> FerryFbx = CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Architecture", L"SM_SwampFerryBoat01.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"Idle" });
	//SPTR<CFbx> FerryFbx = CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Architecture", L"SM_SlaughterTree01.sfbx"), CFbx::LOAD_DESC{ FBX_STATIC, L"Idle" });
	//SPTR<CFbx> FerryFbx = CResourceMgr<CFbx>::Load(L"Test", CFilePathMgr::GetPath(L"Architecture", L"LV_BurningSands_Object_BreakWall03.fbx"), CFbx::LOAD_DESC{ FBX_STATIC_NONAUTO, L"Idle" });


}


