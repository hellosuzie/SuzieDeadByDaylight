#include "PreHeader.h"
#include "RoomSelectScene.h"
#include <DXFont.h>
#include <Terrain.h>
#include <Skybox.h>
#include <FreeCameraScript.h>
#include <TransForm.h>
#include <Fbx.h>
#include <SkeletalFbxRenderer.h>
#include <KeyMgr.h>
#include <FbxRenderer.h>
#include <SSDRandom.h>
#include <FogFilter.h>
#include <DepthFieldFilter.h>
#include <ParticleRenderer.h>
#include <UIBase.h>
#include <SoundPlayer.h>


Vec3					CRoomSelectScene::RoomUISize = Vec3(300.f, 100.f, 0.f);
Vec3					CRoomSelectScene::RoomUIPosition[5] = { Vec3(-400.f, 205.f, 99.f) , Vec3(-400.f, 100.f, 99.f) , Vec3(-400.f, -5.f, 99.f) , Vec3(-400.f, -110.f, 99.f) , Vec3(-400.f, -215.f, 99.f) };

void RoomSelectUI::SetRoomData(const UINT& _RoomNo, const UINT& _UserCount) 
{
	RoomNumber = _RoomNo;

	switch (_RoomNo)
	{
	case 1:
		RoomNo->SetUITexture(L"Number1");
		break;
	case 2:
		RoomNo->SetUITexture(L"Number2");
		break;
	case 3:
		RoomNo->SetUITexture(L"Number3");
		break;
	case 4:
		RoomNo->SetUITexture(L"Number4");
		break;
	case 5:
		RoomNo->SetUITexture(L"Number5");
		break;
	default:
		break;
	}

	switch (_UserCount)
	{
	case 1:
		RoomUserCount->SetUITexture(L"Number1");
		break;
	case 2:
		RoomUserCount->SetUITexture(L"Number2");
		break;
	case 3:
		RoomUserCount->SetUITexture(L"Number3");
		break;
	case 4:
		RoomUserCount->SetUITexture(L"Number4");
		break;
	case 5:
		RoomUserCount->SetUITexture(L"Number5");
		break;
	default:
		break;
	}
	
	RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
}
void RoomSelectUI::SetEmpty() 
{

}
void RoomSelectUI::SetUIPos(const Vec3& _Pos)
{
	RoomFrame->SetSubPivot(_Pos);
	RoomFrame->SetCollPivot(Vec3(_Pos.x, _Pos.y, 0.f));
	RoomFocusing->SetSubPivot(_Pos);
	EmptyUI->SetSubPivot(_Pos);
	RoomNo->SetSubPivot(_Pos + Vec3(17.f, 13.f, 0.f));
	RoomUserCount->SetSubPivot(_Pos + Vec3(80.f, -13.f, 0.f));
	RoomSlash->SetSubPivot(_Pos + Vec3(100.f, -13.f, 0.f));
	RoomMaxUserCount->SetSubPivot(_Pos + Vec3(120.f, -13.f, 0.f));
}

void RoomSelectUI::On() 
{
	RoomFrame->On();
	RoomFocusing->On();
	RoomNo->On();
	RoomUserCount->On();
	RoomSlash->On();
	RoomMaxUserCount->On();
	EmptyUI->Off();
}
void RoomSelectUI::Off() 
{
	RoomFrame->Off();
	RoomFocusing->Off();
	RoomNo->Off();
	RoomUserCount->Off();
	RoomSlash->Off();
	RoomMaxUserCount->Off();
	EmptyUI->On();
}

CRoomSelectScene::CRoomSelectScene(CScene* _PairScene)
	: CSceneScript(_PairScene), RoomSelectCheck(false), m_CurSelectIdx(-1)
	, m_MaxSelectRoom(0)
{
}


CRoomSelectScene::~CRoomSelectScene()
{
}
void CRoomSelectScene::WakeUp()
{
	//CSoundPlayer::PlayBGM(L"LobbyScene", 0.1f);
	UpdateRoomUI();
}
void CRoomSelectScene::ShutDown()
{
}
const bool CRoomSelectScene::Loading()
{
	SPTR<CCamera> MainCam = GetMainCamera();
	MainCam->TRANS->SetPos(Vec3(894.f, 83.f, 32.f));
	MainCam->TRANS->SetRotate(Vec3(0.5f, -47.5f, 0.f));
	SPTR<CFogFilter> TempFog = MainCam->AddFilter<CFogFilter>(MainCam);
	TempFog->SetFogStartDepth(600.f);
	MainCam->AddFilter<CDepthFieldFilter>();


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

	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeBlack.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeRed.png"));
	CResourceMgr<CTexture>::Load(CFilePathMgr::GetPath(L"Texture", L"FadeWhite.png"));
	SPTR<CMultiTexture> TempTex = CResourceMgr<CMultiTexture>::Create(L"FadeParticle", { D3D11_USAGE::D3D11_USAGE_DEFAULT, 3, L"FadeWhite", L"FadeBlack", L"FadeRed" });

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

	SPTR<CCamera> UICam = GetUICamera();


	//룸 선택 UI 배경
	SPTR<CUIBase> BackGroundUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomScene" });
	BackGroundUI->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	BackGroundUI->SetSubPivot(Vec3(0.f, 0.f, 110.f));

	SelectUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"None" });
	SelectUI->SetSubSize(Vec3(200.f, 59.f, 0.f));
	SelectUI->SetSubPivot(Vec3(-39.f - 48.f, -304.f, 99.f));
	SelectUI->SetCollSize(Vec3(200.f, 59.f, 0.f));
	SelectUI->SetCollPivot(Vec3(-39.f - 48.f, -304.f, 0.f));
	SelectUI->SetEnterCallBack(this, &CRoomSelectScene::SelectEnter);
	SelectUI->SetStayCallBack(this, &CRoomSelectScene::SelectStay);
	SelectUI->SetExitCallBack(this, &CRoomSelectScene::SelectExit);

	BackUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"None" });
	BackUI->SetSubSize(Vec3(200.f, 59.f, 0.f));
	BackUI->SetSubPivot(Vec3(-56.f - 48.f, -380.f, 99.f));
	BackUI->SetCollSize(Vec3(200.f, 59.f, 0.f));
	BackUI->SetCollPivot(Vec3(-56.f - 48.f, -380.f, 0.f));
	BackUI->SetEnterCallBack(this, &CRoomSelectScene::BackEnter);
	BackUI->SetStayCallBack(this, &CRoomSelectScene::BackStay);
	BackUI->SetExitCallBack(this, &CRoomSelectScene::BackExit);


	m_RoomUI[0].RoomFrame = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	m_RoomUI[0].RoomFrame->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[0].RoomFrame->SetCollSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[0].RoomFrame->SetEnterCallBack(this, &CRoomSelectScene::Room1Enter);
	m_RoomUI[0].RoomFrame->SetStayCallBack(this, &CRoomSelectScene::Room1Stay);
	m_RoomUI[0].RoomFrame->SetExitCallBack(this, &CRoomSelectScene::Room1Exit);
	m_RoomUI[0].RoomFocusing = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"RoomSelect" });
	m_RoomUI[0].RoomFocusing->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[0].RoomFocusing->SetUIAplha(0.f);
	m_RoomUI[0].RoomNo = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	m_RoomUI[0].RoomNo->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[0].RoomUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number2" });
	m_RoomUI[0].RoomUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[0].RoomMaxUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[0].RoomMaxUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[0].RoomSlash = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Slash" });
	m_RoomUI[0].RoomSlash->SetSubSize(Vec3(25.f, 30.f, 0.f));
	m_RoomUI[0].EmptyUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	m_RoomUI[0].EmptyUI->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[0].SetUIPos(RoomUIPosition[0]);
	m_RoomUI[0].On();

	m_RoomUI[1].RoomFrame = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	m_RoomUI[1].RoomFrame->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[1].RoomFrame->SetCollSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[1].RoomFrame->SetEnterCallBack(this, &CRoomSelectScene::Room2Enter);
	m_RoomUI[1].RoomFrame->SetStayCallBack(this, &CRoomSelectScene::Room2Stay);
	m_RoomUI[1].RoomFrame->SetExitCallBack(this, &CRoomSelectScene::Room2Exit);
	m_RoomUI[1].RoomFocusing = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"RoomSelect" });
	m_RoomUI[1].RoomFocusing->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[1].RoomFocusing->SetUIAplha(0.f);
	m_RoomUI[1].RoomNo = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	m_RoomUI[1].RoomNo->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[1].RoomUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number4" });
	m_RoomUI[1].RoomUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[1].RoomMaxUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[1].RoomMaxUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[1].RoomSlash = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Slash" });
	m_RoomUI[1].RoomSlash->SetSubSize(Vec3(25.f, 30.f, 0.f));
	m_RoomUI[1].EmptyUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	m_RoomUI[1].EmptyUI->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[1].SetUIPos(RoomUIPosition[1]);
	m_RoomUI[1].On();

	m_RoomUI[2].RoomFrame = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	m_RoomUI[2].RoomFrame->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[2].RoomFrame->SetCollSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[2].RoomFrame->SetEnterCallBack(this, &CRoomSelectScene::Room3Enter);
	m_RoomUI[2].RoomFrame->SetStayCallBack(this, &CRoomSelectScene::Room3Stay);
	m_RoomUI[2].RoomFrame->SetExitCallBack(this, &CRoomSelectScene::Room3Exit);
	m_RoomUI[2].RoomFocusing = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"RoomSelect" });
	m_RoomUI[2].RoomFocusing->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[2].RoomFocusing->SetUIAplha(0.f);
	m_RoomUI[2].RoomNo = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number3" });
	m_RoomUI[2].RoomNo->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[2].RoomUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number3" });
	m_RoomUI[2].RoomUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[2].RoomMaxUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[2].RoomMaxUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[2].RoomSlash = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Slash" });
	m_RoomUI[2].RoomSlash->SetSubSize(Vec3(25.f, 30.f, 0.f));
	m_RoomUI[2].EmptyUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	m_RoomUI[2].EmptyUI->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[2].SetUIPos(RoomUIPosition[2]);
	m_RoomUI[2].On();

	m_RoomUI[3].RoomFrame = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	m_RoomUI[3].RoomFrame->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[3].RoomFrame->SetCollSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[3].RoomFrame->SetEnterCallBack(this, &CRoomSelectScene::Room4Enter);
	m_RoomUI[3].RoomFrame->SetStayCallBack(this, &CRoomSelectScene::Room4Stay);
	m_RoomUI[3].RoomFrame->SetExitCallBack(this, &CRoomSelectScene::Room4Exit);
	m_RoomUI[3].RoomFocusing = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"RoomSelect" });
	m_RoomUI[3].RoomFocusing->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[3].RoomFocusing->SetUIAplha(0.f);
	m_RoomUI[3].RoomNo = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[3].RoomNo->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[3].RoomUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	m_RoomUI[3].RoomUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[3].RoomMaxUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[3].RoomMaxUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[3].RoomSlash = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Slash" });
	m_RoomUI[3].RoomSlash->SetSubSize(Vec3(25.f, 30.f, 0.f));
	m_RoomUI[3].EmptyUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	m_RoomUI[3].EmptyUI->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[3].SetUIPos(RoomUIPosition[3]);
	m_RoomUI[3].Off();

	m_RoomUI[4].RoomFrame = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	m_RoomUI[4].RoomFrame->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[4].RoomFrame->SetCollSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[4].RoomFrame->SetEnterCallBack(this, &CRoomSelectScene::Room5Enter);
	m_RoomUI[4].RoomFrame->SetStayCallBack(this, &CRoomSelectScene::Room5Stay);
	m_RoomUI[4].RoomFrame->SetExitCallBack(this, &CRoomSelectScene::Room5Exit);
	m_RoomUI[4].RoomFocusing = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"RoomSelect" });
	m_RoomUI[4].RoomFocusing->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[4].RoomFocusing->SetUIAplha(0.f);
	m_RoomUI[4].RoomNo = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number1" });
	m_RoomUI[4].RoomNo->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[4].RoomUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number2" });
	m_RoomUI[4].RoomUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[4].RoomMaxUserCount = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number5" });
	m_RoomUI[4].RoomMaxUserCount->SetSubSize(Vec3(20.f, 18.f, 0.f));
	m_RoomUI[4].RoomSlash = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Slash" });
	m_RoomUI[4].RoomSlash->SetSubSize(Vec3(25.f, 30.f, 0.f));
	m_RoomUI[4].EmptyUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	m_RoomUI[4].EmptyUI->SetSubSize(Vec3(300.f, 100.f, 0.f));
	m_RoomUI[4].SetUIPos(RoomUIPosition[4]);
	m_RoomUI[4].Off();

	//RoomUI1 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Room" });
	//RoomUI1->SetEnterCallBack(this, &CRoomSelectScene::Room1Enter);
	//RoomUI1->SetStayCallBack(this, &CRoomSelectScene::Room1Stay);
	//RoomUI1->SetExitCallBack(this, &CRoomSelectScene::Room1Exit);

	//RoomUI2 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	//RoomUI2->SetSubSize(Vec3(300.f, 100.f, 0.f));
	//RoomUI2->SetSubPivot(Vec3(-400.f, 205.f - 105.f, 100.f));

	//RoomUI3 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	//RoomUI3->SetSubSize(Vec3(300.f, 100.f, 0.f));
	//RoomUI3->SetSubPivot(Vec3(-400.f, 205.f - (105.f * 2.f), 100.f));

	//RoomUI4 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	//RoomUI4->SetSubSize(Vec3(300.f, 100.f, 0.f));
	//RoomUI4->SetSubPivot(Vec3(-400.f, 205.f - (105.f * 3.f), 100.f));

	//RoomUI5 = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RoomEmpty" });
	//RoomUI5->SetSubSize(Vec3(300.f, 100.f, 0.f));
	//RoomUI5->SetSubPivot(Vec3(-400.f, 205.f - (105.f * 4.f), 100.f));


	//RoomUISelect = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"None" });
	//RoomUISelect->SetSubSize(Vec3(300.f, 100.f, 0.f));
	//RoomUISelect->SetSubPivot(Vec3(-400.f, 205.f, 10.f));

	ResetUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BUTTON, L"Reset" });
	ResetUI->SetSubSize(Vec3(90.f, 90.f, 0.f));
	ResetUI->SetSubPivot(Vec3(-636.f, 234.f, 99.f));
	ResetUI->SetCollSize(Vec3(90.f, 90.f, 0.f));
	ResetUI->SetCollPivot(Vec3(-636.f, 234.f, 0.f));
	ResetUI->SetEnterCallBack(this, &CRoomSelectScene::ResetEnter);
	ResetUI->SetStayCallBack(this, &CRoomSelectScene::ResetStay);
	ResetUI->SetExitCallBack(this, &CRoomSelectScene::ResetExit);

	return true;
}

//취소 시작버튼
//리셋버튼
//룸 선택

void CRoomSelectScene::PreUpdate(const float & _DeltaTime)
{
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
			UpdateRoomUI();
		}
		break;
		case FLAG_ENTER_ROOM_SUCCESS:
		{
			LobbyEnterData LData;
			LData = ExtractionData<LobbyEnterData>(Pack);
			DBD::RecordRecvRoomData(LData);
			CScene::ChangeScene(L"SurvivorLobbyScene");
		}

		break;
		default:
			break;
		}
	}
}

void CRoomSelectScene::PostUpdate(const float & _DeltaTime)
{
}

void CRoomSelectScene::LastUpdate(const float & _DeltaTime)
{
}

void CRoomSelectScene::SelectEnter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	SelectUI->SetUITexture(L"Select");
}

void CRoomSelectScene::SelectStay()
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx <= 5)
		{
			GETSINGLE(CClientConnectMgr).RequestEnterLobbyRoom(PLAYER_SURVIVOR, m_RoomUI[m_CurSelectIdx].RoomNumber);
			//CScene::ChangeScene(L"SurvivorLobbyScene");
		}

	}
}

void CRoomSelectScene::SelectExit()
{
	SelectUI->SetUITexture(L"None");
}

void CRoomSelectScene::BackEnter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	BackUI->SetUITexture(L"Back");
}

void CRoomSelectScene::BackStay()
{
	if (true == KEYDOWN("GizmoHold"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		CScene::ChangeScene(L"SelectScene");
	}
}

void CRoomSelectScene::BackExit()
{
	BackUI->SetUITexture(L"None");
}

void CRoomSelectScene::Room1Enter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomUI[0].RoomFocusing->SetUIAplha(0.5f);
}

void CRoomSelectScene::Room1Stay()
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx < 5)
		{
			m_RoomUI[m_CurSelectIdx].RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
		}
		m_RoomUI[0].RoomFocusing->SetUIColorFactor(Vec4::LightYellow.vec3);
		m_CurSelectIdx = 0;
	}
}

void CRoomSelectScene::Room1Exit()
{
	if (m_CurSelectIdx != 0) 
	{
		m_RoomUI[0].RoomFocusing->SetUIAplha(0.f);
	}
}

void CRoomSelectScene::Room2Enter() 
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomUI[1].RoomFocusing->SetUIAplha(0.5f);
}
void CRoomSelectScene::Room2Stay() 
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx < 5)
		{
			m_RoomUI[m_CurSelectIdx].RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
		}
		m_RoomUI[1].RoomFocusing->SetUIColorFactor(Vec4::LightYellow.vec3);
		m_CurSelectIdx = 1;
	}
}
void CRoomSelectScene::Room2Exit() 
{
	if (m_CurSelectIdx != 1)
	{
		m_RoomUI[1].RoomFocusing->SetUIAplha(0.f);
	}

}

void CRoomSelectScene::Room3Enter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomUI[2].RoomFocusing->SetUIAplha(0.5f);
}
void CRoomSelectScene::Room3Stay() 
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx < 5)
		{
			m_RoomUI[m_CurSelectIdx].RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
		}
		m_RoomUI[2].RoomFocusing->SetUIColorFactor(Vec4::LightYellow.vec3);
		m_CurSelectIdx = 2;
	}
}
void CRoomSelectScene::Room3Exit() 
{
	if (m_CurSelectIdx != 2)
	{
		m_RoomUI[2].RoomFocusing->SetUIAplha(0.f);
	}
}

void CRoomSelectScene::Room4Enter() 
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomUI[3].RoomFocusing->SetUIAplha(0.5f);
}
void CRoomSelectScene::Room4Stay() 
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx < 5)
		{
			m_RoomUI[m_CurSelectIdx].RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
		}
		m_RoomUI[3].RoomFocusing->SetUIColorFactor(Vec4::LightYellow.vec3);
		m_CurSelectIdx = 3;
	}
}
void CRoomSelectScene::Room4Exit() 
{
	if (m_CurSelectIdx != 3)
	{
		m_RoomUI[3].RoomFocusing->SetUIAplha(0.f);
	}
}

void CRoomSelectScene::Room5Enter() 
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	m_RoomUI[4].RoomFocusing->SetUIAplha(0.5f);
}
void CRoomSelectScene::Room5Stay() 
{
	if (true == KEYDOWN("LMouseClick"))
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		if (m_CurSelectIdx < 5)
		{
			m_RoomUI[m_CurSelectIdx].RoomFocusing->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, 0.f));
		}
		m_RoomUI[4].RoomFocusing->SetUIColorFactor(Vec4::LightYellow.vec3);
		m_CurSelectIdx = 4;
	}
}
void CRoomSelectScene::Room5Exit() 
{
	if (m_CurSelectIdx != 4)
	{
		m_RoomUI[4].RoomFocusing->SetUIAplha(0.f);
	}
}

void CRoomSelectScene::ResetEnter()
{
	CSoundPlayer::GlobalPlaySound(L"Button");
	ResetUI->SetUITexture(L"ResetSelect");
}

void CRoomSelectScene::ResetStay()
{
	if (true == KEYDOWN("LMouseClick")) 
	{
		CSoundPlayer::GlobalPlaySound(L"Click");
		// 현재 활성화된 Room의 정보를 요구하는 패킷을 전송
		GETSINGLE(CClientConnectMgr).RequestActiveRoomList();
	}


}

void CRoomSelectScene::ResetExit()
{
	ResetUI->SetUITexture(L"Reset");
}

void CRoomSelectScene::DebugRender()
{
	wchar_t	DebugBuff[MAXSTRING];
	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);

	swprintf_s(DebugBuff, L"Room Select Scene");
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(5.f, 0.f), Vec4::Pink);

}

void CRoomSelectScene::UpdateRoomUI()
{
	RoomList RLData = DBD::m_RecvRoomList;

	for (size_t i = 0; i < 5; i++)
	{
		m_RoomUI[i].Off();
	}

	UINT Count = 0;

	for (size_t i = 0; i < RLData.Header.ActiveRoomCount; i++)
	{
		if (true == RLData.ArrRoomData[i].bGamePlaying)
			continue;

		m_RoomUI[Count].SetRoomData(RLData.ArrRoomData[i].RoomNumber, RLData.ArrRoomData[i].RoomUserCount);
		m_RoomUI[Count++].On();
	}

	m_MaxSelectRoom = Count;
	m_CurSelectIdx = -1;
}