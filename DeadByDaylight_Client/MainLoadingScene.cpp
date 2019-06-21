#include "MainLoadingScene.h"
#include <Animator2D.h>
#include <DeadByDaylightGlobal.h>
#include <SSDCore.h>
#include <DXFont.h>
#include "MainPlayScene.h"
#include <SSDThread.h>
#include <ClientConnectMgr.h>
#include <TimerMgr.h>

unsigned _stdcall BuildMainPlayerScene(void* _Arg)
{
	if (nullptr == _Arg)
		return 1;

	CMainPlayScene* MainScene = reinterpret_cast<CMainPlayScene*>(_Arg);

	MainScene->AsyncLoading(nullptr);
	GETSINGLE(CClientConnectMgr).RequestSceneObject();
	return 0;
}

CMainLoadingScene::CMainLoadingScene(CScene* _pScene)
	:CSceneScript(_pScene), m_Loaing2DAni(nullptr), m_pPlayScene(nullptr)
	, m_LoadingUI(nullptr)
{

}


CMainLoadingScene::~CMainLoadingScene()
{
}
void CMainLoadingScene::WakeUp()
{
	if (nullptr == m_pPlayScene) 
	{
		m_pPlayScene = CScene::FindScene(L"MainPlayScene")->FindSceneScript<CMainPlayScene>();
		CTimerMgr::SetFixFrame(60.f);
	}

	SSDThread::CreateThread(L"LoadingThread", &BuildMainPlayerScene, m_pPlayScene);
}
const bool CMainLoadingScene::Loading()
{
	//m_pPlayScene = CScene::FindScene(L"MainPlayScene")->FindSceneScript<CMainPlayScene>();

	//SSDThread::CreateThread(L"LoadingThread", &BuildMainPlayerScene, &m_pPlayScene);

	SPTR<CCamera> UICam = GetUICamera();
	m_LoadingUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"MainLoading" });
	m_LoadingUI->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_LoadingUI->SetSubPivot(Vec3(0.f, 0.f, 100.f));

	return true;
}
void CMainLoadingScene::PreUpdate(const float& _DeltaTime)
{
	if (GETSINGLE(CClientConnectMgr).EmptyPacketQueue())
		return;

	PACKET Pack;

	while (GETSINGLE(CClientConnectMgr).ExistPacket())
	{
		// 1. 서버에서 전달받은 패킷을 가져온다
		GETSINGLE(CClientConnectMgr).GetPacket(Pack);

		// 서버에서 메신씬으로 이동하라는 패킷이 온 경우
		if (FLAG_GAME_PLAYING == Pack.Header.HeaderFlag) 
		{
			CScene::ChangeScene(L"MainPlayScene");
			return;
		}

		if (FLAG_LOBBY_WAITING != Pack.Header.HeaderFlag )
			continue;


		switch (Pack.Header.DataType)
		{
		case FLAG_DATA_WORLD_OBJECT: 
		{
			WorldObjData ObjData;
			unsigned int* DataSize = (unsigned int*)(Pack.Packet);
			ObjData = ExtractionData<WorldObjData>(Pack, *DataSize);
			RecvWorldObjData(ObjData);
		}
			break;
		default:
			break;
		}
	}

}



void CMainLoadingScene::RecvWorldObjData(const WorldObjData& _Data)
{
	m_pPlayScene->CreateWorldObjData(_Data);

	if (true == _Data.Header.LastData) 
	{
		GETSINGLE(CClientConnectMgr).NotifyWorldBuildComplete();
	}
}