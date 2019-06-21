#include "PreHeader.h"
#include "TestScene.h"
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


CTestScene::CTestScene(CScene* _PairScene)
	:CSceneScript(_PairScene), m_fAccTime(0.f)
	, m_eLoadStep(ASYNC_WAIT), m_LocalPlayerID(0), m_UpdateCount(0)
	, m_LocalPlayer(nullptr)
{

}


CTestScene::~CTestScene()
{
}
const bool CTestScene::IsAsyncLoadingComplete()
{
	CCriSectionObject<CTestScene>();
	return (m_eLoadStep == ASYNC_LOAD_COMPLETE);
}
const ASYNC_LOADING_STEP CTestScene::GetLoadingStep()
{
	CCriSectionObject<CTestScene>();
	return m_eLoadStep;
}

const void CTestScene::CompleteAsyncLoading()
{
	CCriSectionObject<CTestScene>();
	m_eLoadStep = ASYNC_LOAD_COMPLETE;
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

	MainCam->SetPos(Vec3(0.f, 350.f, -1350.f));
	MainCam->SetPerspectiveMode();
	
	LinkCollsionChannel(COLLISION_3D, L"Player", L"Monster");

	SPTR<CActor> SkyActor = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL);
	SPTR<CSkybox> SkyBox =  SkyActor->AddCom<CSkybox>(RENDATA{ RENDER_LAYER_MAP }); 
	SkyBox->SetTexture(L"Sky1");

	SPTR<CActor> MeshTestActor = CreateActor(L"TestMeshActor", UPDATE_LAYER_NORMAL);
	SPTR<Grid3DRenderer> TempMeshRender = MeshTestActor->AddCom<Grid3DRenderer>(RENDATA{ RENDER_LAYER_MAP });
	TempMeshRender->SetColorFactor(VEC4::Pink);

	SPTR<CActor> LightActor1 = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> Light1 = LightActor1->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	Light1->SetLightColor(Vec4::White);
	Light1->SetLightDir(Vec3(1.f, -1.f, 1.f));
	Light1->SetLightPower(1.f);


	m_mapPlayer.clear();

	return true;
}


const bool CTestScene::AsyncLoading(void* _Arg)
{
	InitializeScene();
	Loading();
	
	if (nullptr == _Arg) 
	{
		TASSERT(true);
		return false;
	}

	WorldData* Data = (WorldData*)_Arg;
	
	for (unsigned int i = 0; i < Data->Header.CurPlayerCount; ++i)
	{
		if (Data->Header.LocalPlayerID == Data->arrPlayerData[i].PlayerID) 
		{
			CreatePlayer(Data->arrPlayerData[i], true);
		}
		else
		{
			CreatePlayer(Data->arrPlayerData[i]);
		}
	}

	CompleteAsyncLoading();
	return true;
}

CPlayer* CTestScene::CreatePlayer(const PlayerData& _Data,const bool& _Local /*= false*/)
{
	CPlayer* NewPlayer = nullptr;

	switch (_Data.PlayerRole)
	{
	case PLAYER_SURVIVOR:
	{
		NewPlayer = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL)->AddCom<CPlayer>(PlayerInitData{ false });
		CSurvivorFSM* Fsm = NewPlayer->MountPlayerFSM<CSurvivorFSM>();

		// 로컬 플레이어 생성
		if (true == _Local) 
		{
			NewPlayer->SetLocalPlayer();
			m_LocalPlayer = NewPlayer;
			GETSINGLE(CClientConnectMgr).SetLocalPlayer(NewPlayer);
		}
		else 
		{
			NewPlayer->SetRemotePlayer();
		}
	}

	break;
	case PLAYER_KILLER:
	{

	}
	break;
	default:
		break;
	}


	if (nullptr == NewPlayer)
		return nullptr;

	NewPlayer->InitializePlayer(_Data);
	InsertPlayer(NewPlayer);
	return NewPlayer;
}

void CTestScene::WakeUp()
{
	//GETSINGLE(CClientConnectMgr).ReceivePacket(true);
	GETSINGLE(CClientConnectMgr).NotifyBuildComplete();
}
void CTestScene::PreUpdate(const float & _DeltaTime)
{
	PacketProcessing(_DeltaTime);
}

void CTestScene::PostUpdate(const float & _DeltaTime)
{
	GETSINGLE(ClientConnector).SendPlayerState(_DeltaTime);
	GETSINGLE(ClientConnector).ConnectUpdate(_DeltaTime);

}

void CTestScene::LastUpdate(const float & _DeltaTime)
{

}


void CTestScene::PacketProcessing(const float& _DeltaTime)
{
	if (GETSINGLE(CClientConnectMgr).EmptyPacketQueue())
		return;

	PACKET Pack;

	while (GETSINGLE(CClientConnectMgr).ExistPacket())
	{
		// 1. 서버에서 전달받은 패킷을 가져온다
		GETSINGLE(CClientConnectMgr).GetPacket(Pack);

		// 2. 패킷에서 게임 동작에 관련된 정보를 뽑아낸다.
		switch (Pack.Header.DataType )
		{
		// 현재 접속중인 리모트 플레이어들의 상태들을 업데이트
		case FLAG_DATA_ROOM_WORLD: 
		{
			WorldData UpdateData;
			
			unsigned int* DataSize = (unsigned int*)(Pack.Packet);
			
			UpdateData = ExtractionData<WorldData>(Pack, *DataSize);

			for (unsigned int i = 0; i < UpdateData.Header.CurPlayerCount; ++i)
			{
				// 로컬 플레이어의 데이터는 여기서 갱신하지 않는다.
				if (UpdateData.Header.LocalPlayerID == UpdateData.arrPlayerData[i].PlayerID)
					continue;

				m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);

				// 맵안에 없는 플레이어가 생긴경우 새로 접속한 플레이어로 간주 플레이어를 만든다.
				if (m_mapPlayerIter == m_mapPlayerEndIter)
				{
					CreatePlayer(UpdateData.arrPlayerData[i]);
					continue;
				}
			
				m_mapPlayerIter->second->UpdatePlayer(UpdateData.arrPlayerData[i]);
			}

			++m_UpdateCount;
		}
			break;
		// 게임 중 접속이 끊기거나 게임종료을 한 플레이어 처리
		case FLAG_GAME_EXIT_USER:
		{
			unsigned __int64 ExitPalyerID = ExtractionData<unsigned __int64>(Pack);
	
			m_mapPlayerIter = m_mapPlayer.find(ExitPalyerID);

			if (m_mapPlayerIter != m_mapPlayerEndIter) 
			{
				m_mapPlayerIter->second->ACTOR->Death();
				m_mapPlayer.erase(m_mapPlayerIter);
				m_mapPlayerEndIter = m_mapPlayer.end();
			}

		}
		default:
			break;	
		}
	}


}
void CTestScene::LoadTexture()
{
	// 텍스쳐 로드
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Sky1.jpg"));

	// 스프라이트 생성
	//CResourceMgr<CSprite>::Create(L"spectiles", { L"spectiles" , POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f) ,16,16,0, ALLFRAME });

}
void CTestScene::LoadFBX()
{
	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx =  CResourceMgr<CFbx>::Load(L"killer",CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION});
	//SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer_idle.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION, L"idle" });
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_walk.fbx"), L"walk");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_run.fbx"), L"run");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_atk.fbx"), L"atk");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_genatk.fbx"), L"genatk");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_stunpallet.fbx"), L"stunpallet");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_grap.fbx"), L"grap");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_pickup.fbx"), L"pickup");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_carryidle.fbx"), L"carryidle");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_carrydrop.fbx"), L"carrydrop");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_hookin.fbx"), L"hookin");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_killing.fbx"), L"killing");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_exceedwindow.fbx"), L"exceedwindow");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_settrap.fbx"), L"settrap");
	//KillerFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killer_trapaction.fbx"), L"trapaction");

	//KillerFbx->BinarySave(CFilePathMgr::GetPath(L"Animation", L"killer.sfbx").c_str());


	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor",CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });
	//SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor",CFilePathMgr::GetPath(L"Animation", L"survivor_idle.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION ,L"idle" });
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkcrouch.fbx"), L"walkcrouch");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkherted.fbx"), L"walkherted");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkslow.fbx"), L"walkslow");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_sprint.fbx"), L"sprint");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hertedsprint1.fbx"), L"hertedsprint1");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hertedsprint2.fbx"), L"hertedsprint2");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hitcrawl1.fbx"), L"hitcrawl1");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hitcrawl2.fbx"), L"hitcrawl2");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_healing.fbx"), L"healing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_healdloop.fbx"), L"healdloop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_selfhealing.fbx"), L"selfhealing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_lockerhide.fbx"), L"lockerhide");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hurtedidle.fbx"), L"hurtedidle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hurtedcrouch.fbx"), L"hurtedcrouch");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedfree.fbx"), L"hookedfree");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedidle.fbx"), L"hookedidle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedstruggle.fbx"), L"hookedstruggle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_genrefair.fbx"), L"genrefair");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_genfail.fbx"), L"genfail");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderin.fbx"), L"killedspiderin");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderloop.fbx"), L"killedspiderloop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderout.fbx"), L"killedspiderout");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_rescue.fbx"), L"hookedrescue");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_rescuehooked.fbx"), L"rescuehooked");
	
	//SurvivorFbx->BinarySave(CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx").c_str());

	
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_grap.fbx"), L"grap");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killing.fbx"), L"killing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookin.fbx"), L"hookin");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_pickup.fbx"), L"pickup");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_carrydrop.fbx"), L"carrydrop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_carryidle.fbx"), L"carryidle"); 
	
	//SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(CFilePathMgr::GetPath(L"Animation", L"TestWalk.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION ,L"idle" });
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkcrouch.fbx"), L"walkcrouch");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkherted.fbx"), L"walkherted");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_walkslow.fbx"), L"walkslow");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_sprint.fbx"), L"sprint");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hertedsprint1.fbx"), L"hertedsprint1");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hertedsprint2.fbx"), L"hertedsprint2");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hitcrawl1.fbx"), L"hitcrawl1");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hitcrawl2.fbx"), L"hitcrawl2");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_healing.fbx"), L"healing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_healdloop.fbx"), L"healdloop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_selfhealing.fbx"), L"selfhealing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_lockerhide.fbx"), L"lockerhide");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hurtedidle.fbx"), L"hurtedidle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hurtedcrouch.fbx"), L"hurtedcrouch");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedfree.fbx"), L"hookedfree");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedidle.fbx"), L"hookedidle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookedstruggle.fbx"), L"hookedstruggle");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_genrefair.fbx"), L"genrefair");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_genfail.fbx"), L"genfail");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderin.fbx"), L"killedspiderin");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderloop.fbx"), L"killedspiderloop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killedspiderout.fbx"), L"killedspiderout");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_rescue.fbx"), L"hookedrescue");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_rescuehooked.fbx"), L"rescuehooked");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_grap.fbx"), L"grap");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_killing.fbx"), L"killing");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_hookin.fbx"), L"hookin");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_pickup.fbx"), L"pickup");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_carrydrop.fbx"), L"carrydrop");
	//SurvivorFbx->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"survivor_carryidle.fbx"), L"carryidle"); 

}
void CTestScene::DebugRender()
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

	swprintf_s(DebugBuff, L"FPS : %.f", CTimerMgr::FPS);

	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 0.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Camera Pos  : (%.1f , %.1f , %.1f)", CamPos.x, CamPos.y, CamPos.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(250.f, 0.f), Vec4::Pink);

	swprintf_s(DebugBuff, L"Current Connected User : %d Update Count : %ld", (int)m_mapPlayer.size(), m_UpdateCount);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 20.f), Vec4::Pink);
	
	
	Vec3 Pos = m_LocalPlayer->GetWorldPos();
	swprintf_s(DebugBuff, L"Local Player ID : %llu , Player Pos : (%.2f , %.2f , %.2f)", m_LocalPlayer->GetPlayerID() , Pos.x, Pos.y, Pos.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 40.f ), Vec4::Pink);

	m_mapPlayerIter = m_mapPlayer.begin();
	m_mapPlayerEndIter = m_mapPlayer.end();

	for (size_t i = 0; m_mapPlayerIter != m_mapPlayerEndIter; ++m_mapPlayerIter)
	{
		if (m_mapPlayerIter->second == m_LocalPlayer)
			continue;

		Vec3 Pos = m_mapPlayerIter->second->GetWorldPos();
		swprintf_s(DebugBuff, L"Remote Player ID : %llu , Player Pos : (%.2f , %.2f , %.2f)",m_mapPlayerIter->second->GetPlayerID(), Pos.x, Pos.y, Pos.z);
		DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 60.f + (float)i * 20.f), Vec4::Pink);
		++i;
	}

}

void CTestScene::InsertPlayer(CPlayer* _pPlayer) 
{
	if (nullptr == _pPlayer)
		return;

	m_mapPlayer.insert(std::unordered_map<unsigned __int64, SPTR<CPlayer>>::value_type(_pPlayer->GetPlayerID(), _pPlayer));
	m_mapPlayerEndIter = m_mapPlayer.end();
}
void CTestScene::DeletePlayer(CPlayer* _pPlayer)
{
	if (nullptr == _pPlayer)
		return;

	m_mapPlayerIter = m_mapPlayer.find(_pPlayer->GetPlayerID());

	if (m_mapPlayerIter != m_mapPlayerEndIter) 
	{
		m_mapPlayerIter->second->ACTOR->Death();
		m_mapPlayer.erase(m_mapPlayerIter);
		m_mapPlayerEndIter =  m_mapPlayer.end();
	}
}