#include "PreHeader.h"
#include "TestScene2.h"
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

CTestScene2::CTestScene2(CScene* _pScene)
	:CSceneScript(_pScene), m_PlayerTrace(nullptr), m_Player(nullptr)
{

}

CTestScene2::~CTestScene2()
{

}

const bool CTestScene2::Loading()
{
	if (false == m_bLoading)
	{
		LoadTexture();
		LoadFBX();
	}



	SPTR<CCamera> MainCam = GetMainCamera();

	MainCam->ACTOR->AddCom<CFreeCameraScript>();
	MainCam->ACTOR->AddCom<CMouseScript>();

	MainCam->SetPos(Vec3(0.f, 450.f, -1080.f));
	MainCam->SetPerspectiveMode();

	LinkCollsionChannel(COLLISION_3D, L"Player", L"Monster");

	SPTR<CActor> SkyActor = CreateActor(L"SkyActor", UPDATE_LAYER_NORMAL);
	SPTR<CSkybox> SkyBox = SkyActor->AddCom<CSkybox>(RENDATA{ RENDER_LAYER_MAP });
	SkyBox->SetSkyBoxSize(10000.f);
	SkyBox->SetTexture(L"SkyboxNight");

	//디폴트 라이트
	//SPTR<CActor> DeLightActor = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	//SPTR<CLight> DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	//DLight->SetLightColor(Vec4::Mint);
	//DLight->SetLightDir(Vec3(-1.f, -1.f, 1.f));
	//DLight->SetLightPower(0.5f);
	//DLight->SetSpecPower(200.f);

	//DLight = DeLightActor->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	//DLight->SetLightColor(Vec4::Mint);
	//DLight->SetLightDir(Vec3(1.f, 1.f, 1.f));
	//DLight->SetLightPower(0.5f);
	//DLight->SetSpecPower(200.f);

	// 터레인 (터레인 랜더러 + 기타 객체들) 
	SPTR<CActor> TerrainActor = CreateActor(UPDATE_LAYER_NORMAL, L"TerrainActor");
	SPTR<CTerrain> Terrain = TerrainActor->AddCom<CTerrain>();
	Terrain->CreateTerrain(L"BaseFloor", L"TerrainDiff", L"TerrainNormal", 128, 128, 300.f);
	Terrain->SetCellSize(100.f);
	Terrain->CreateFloor(L"Floor2");
	//Terrain->BakeNavigationMesh();

	DBD::pMainTerrain = Terrain;

	
	SPTR<CActor> LightActor1 = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor");
	SPTR<CLight> Light1 = LightActor1->AddCom<CLight>(LIGHT_DATA{ RENDER_LAYER_PLAYER, DIRECTION_LIGHT, false });
	Light1->SetLightColor(Vec4::White);
	Light1->SetLightDir(Vec3(1.f, -1.f, 1.f));
	Light1->SetLightPower(1.f);

	SPTR<CTest> Temp = CreateActor(UPDATE_LAYER_NORMAL, L"Test")->AddCom<CTest>();

	// Navigation Object (스테틱 랜더러 + 네비메쉬 + 기타 객체)
	//SPTR<CActor> Test1 = CreateActor(L"MeshTestActor", UPDATE_LAYER_NORMAL);
	//SPTR<CNavigationObject> NaviObj = Test1->AddCom<CNavigationObject>();
	//Test1->TRANS->SetPos(DBD::pMainTerrain->GetMiddlePos());
	//NaviObj->CreateFbxRenderer(RENDATA{ RENDER_LAYER_PLAYER });
	//NaviObj->CreateNavigationRenderer();
	//NaviObj->CreateNaviArea(DBD::pMainTerrain);

	//NaviObj->SetDrawFbx(L"SM_ST_OfficeRoom");
	//NaviObj->SettingFbxMesh(0);
	//NaviObj->CreateNavigationMesh(L"SM_ST_OfficeRoomNavi", NAVIMESH_OBJ);

	//NaviObj->GetNaviArea()->AddRectArea(Vec2(), Vec2(1700.f, 1700.f));
	//NaviObj->GetNaviArea()->AddCircleArea(Vec2(), 1300.f);
	//NaviObj->SetGizmoPivot(Vec3(0.f, 800.f, 0.f));

	//SPTR<CActor> PlayerActor = CreateActor(L"PlayerActor", UPDATE_LAYER_NORMAL);
	//m_Player = PlayerActor->AddCom<CPlayer>(PlayerInitData{ false });
	//CSurvivorFSM* Fsm = m_Player->MountPlayerFSM<CSurvivorFSM>();
	//m_PlayerNavigator = PlayerActor->AddCom<CNavigator>();

	//PlayerActor->TRANS->SetPos(DBD::pMainTerrain->GetMiddlePos());
	//m_Player->SetLocalPlayer();
	//Fsm->LocalPlayer();
	//
	//m_PlayerNavigator->SetCurNaviMesh(NaviObj->GetNaviRen());
	//m_PlayerNavigator->SetCurNaviMesh(DBD::pMainTerrain->GetTerrainNaviMesh());
	return true;
}

void CTestScene2::PostUpdate(const float& _DeltaTime)
{
	if (true == KEYDOWN("CameraKey")) 
	{
		m_pPairScene->ChangeMainCamera();
	}

	if (true == KEYDOWN("NaviBakeKey"))
	{
		DBD::pMainTerrain->BakeNavigationMesh();

		m_Player->SetPos(Vec3(6400.f, 0.f,5500.f));
		m_PlayerNavigator->SetCurNaviMesh(DBD::pMainTerrain->GetTerrainRen()->GetTerrainNaviMesh());
	}


	if (true == KEYDOWN("NaviAreaKey"))
	{
		CNaviMeshArea::ChangeAreaRendering();
	}
}
void CTestScene2::LoadTexture()
{
	// 텍스쳐 로드
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"Sky1.jpg"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"SkyboxNight.dds"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"BaseFloor_2.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainDiff_2.tga"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"TerrainNormal_2.tga"));
	// 스프라이트 생성
	//CResourceMgr<CSprite>::Create(L"spectiles", { L"spectiles" , POINTSAMPLER, Vec2(0.f,0.f), Vec2(0.f,0.f) ,16,16,0, ALLFRAME });

	CResourceMgr<CMultiTexture>::Create(L"Floor2", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"BaseFloor_2", L"TerrainDiff_2", L"TerrainNormal_2" });

}
void CTestScene2::LoadFBX()
{
	// 살인자 FBX 로드
	SPTR<CFbx> KillerFbx = CResourceMgr<CFbx>::Load(L"killer", CFilePathMgr::GetPath(L"Animation", L"killer.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });

	// 생존자 FBX 로드
	SPTR<CFbx> SurvivorFbx = CResourceMgr<CFbx>::Load(L"survivor", CFilePathMgr::GetPath(L"Animation", L"survivor.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION });

	// 갈고리 FBX 로드
	SPTR<CFbx> Hook = CResourceMgr<CFbx>::Load(L"killedspider", CFilePathMgr::GetPath(L"Animation", L"killedspider.sfbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION  });
	//SPTR<CFbx> Hook = CResourceMgr<CFbx>::Load(L"killedspider", CFilePathMgr::GetPath(L"Animation", L"killedspiderin.fbx"), CFbx::LOAD_DESC{ FBX_MULTITAKE_ANIMATION,L"In" });
	//Hook->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killedspiderout.fbx"), L"Out");
	//Hook->LoadFbxAnimation(CFilePathMgr::GetPath(L"Animation", L"killedspiderloop.fbx"), L"Loop");

	//Hook->BinarySave(CFilePathMgr::GetPath(L"Animation", L"killedspider.sfbx").c_str());
}

void CTestScene2::DebugRender()
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

	//swprintf_s(DebugBuff, L"Player Camera Pivot : ( %.2f ,%.2f ,%.2f )  , Player Camera Rotate : ( %.2f ,%.2f ,%.2f )"
	//	, m_PlayerTrace->GetTracePivot().x, m_PlayerTrace->GetTracePivot().y, m_PlayerTrace->GetTracePivot().z
	//	, m_PlayerTrace->TRANS->GetRotate().x, m_PlayerTrace->TRANS->GetRotate().y, m_PlayerTrace->TRANS->GetRotate().z);
	//DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 20.f), Vec4::Pink);

	//swprintf_s(DebugBuff, L"Player Pos : ( %.2f ,%.2f ,%.2f ) ",
	//	m_Player->TRANS->GetWorldPos().x, m_Player->TRANS->GetWorldPos().y, m_Player->TRANS->GetWorldPos().z);
	//DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 40.f), Vec4::Pink);

}