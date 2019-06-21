#include "Test.h"
#include <KeyMgr.h>
#include <TransForm.h>
#include <Actor.h>
#include <SphereCollider.h>
#include <OBBCollider.h>
#include <Scene.h>
#include <SkeletalFbxRenderer.h>
#include <Navigator.h>

CTest::CTest()
	:m_AnimRen(nullptr), m_Sphere(nullptr), m_Obb(nullptr), m_Navi(nullptr)
{
}


CTest::~CTest()
{
}

const bool CTest::Init() 
{

	m_Navi = ACTOR->AddCom<CNavigator>();
	

	//SPTR<CLight> Light = CreateActor(UPDATE_LAYER_NORMAL, L"LightActor")->AddCom<CLight>(LIGHT_DATA{RENDER_LAYER_PLAYER});
	//Light->SetLightColor(Vec4::White);
	//Light->SetLightType(DIRECTION_LIGHT);
	//Light->SetLightDir(Vec3(1.f, -1.f, 1.f));


	m_AnimRen = ACTOR->AddCom<CSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	
	//KillerInit();
	SurvivorInit();


	TRANS->SetPos(Vec3(0.f, 100.f, -200.f));
	TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));

	//m_Obb = CreateActor(UPDATE_LAYER_NORMAL, L"test")->AddCom<COBBCollider>(COLDATA{ L"Monster",COLLISION_3D });
	//m_Obb->SetColSize(Vec3(10.f,10.f,50.f));
	//m_Obb->SetColPivot(Vec3(0.f, 0.f, 50.f));
	//m_AnimRen->AddAnimSocket("Knife01", m_Obb->TRANS);


	//m_Sphere = CreateActor(UPDATE_LAYER_NORMAL, L"test")->AddCom<CSphereCollider>(COLDATA{ L"Monster",COLLISION_3D });
	//m_Sphere->SetColRadius(50.f);

	//m_Obb2 = CreateActor(UPDATE_LAYER_NORMAL, L"test")->AddCom<COBBCollider>(COLDATA{ L"Monster",COLLISION_3D });
	//m_Obb2->TRANS->SetPos(Vec3(-100.f, 0.f, -49.f));
	//m_Obb2->SetColSize(Vec3(50.f, 50.f, 50.f));
	//m_Obb2->TRANS->SetRotate(Vec3(20.f, 30.f, 50.f));
	

	/*m_Obb = ACTOR->AddCom<COBBCollider>(COLDATA{ L"Player",COLLISION_3D });
	TRANS->SetPos(Vec3(100.f, 0.f, 0.f));
	m_Obb->SetColSize(Vec3(50.f,50.f,50.f));
	m_Obb->SetEnterCallBack(this, &CTest::Enter);*/

	return true;
}
void CTest::Update(const float& _DeltaTime) 
{
	Vec3 MoveDir = Vec3();

	if (true == KEY("CAM_UP"))
		TRANS->AccRotate(Vec3(0.f, 0.f, 360.f*_DeltaTime));

	if (true == KEY("CAM_DOWN"))
		TRANS->AccRotate(Vec3(0.f, 0.f, -360.f*_DeltaTime));

	if (true == KEY("UP"))
		MoveDir -= TRANS->GetUp();

	if (true == KEY("DOWN"))
		MoveDir -= TRANS->GetDown();

	if (true == KEY("LEFT"))
		MoveDir += TRANS->GetLeft();

	if (true == KEY("RIGHT"))
		MoveDir += TRANS->GetRight();


	if (MoveDir == Vec3())
		return;
	

	TRANS->Move(MoveDir, 1000.f, _DeltaTime);

}
void CTest::LastUpdate(const float& _DeltaTime)
{
	if (TRUE == KEYDOWN("NextAni")) 
	{
		m_AnimRen->ChangeNextAnimation();
	}

	if (TRUE == KEYDOWN("PrevAni"))
	{
		m_AnimRen->ChangePrevAnimation();
	}

}
void CTest::Enter(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{

}

void CTest::KillerInit()
{
	m_AnimRen->SetFbx(L"killer");

	m_AnimRen->CreateBoneAnimation(L"walk", 1);
	m_AnimRen->CreateBoneAnimation(L"run", 2);
	m_AnimRen->CreateBoneAnimation(L"atk", 3);
	m_AnimRen->CreateBoneAnimation(L"genatk", 4);
	m_AnimRen->CreateBoneAnimation(L"stunpallet", 5);
	m_AnimRen->CreateBoneAnimation(L"grap", 6);
	m_AnimRen->CreateBoneAnimation(L"pickup", 7);
	m_AnimRen->CreateBoneAnimation(L"carryidle", 8);
	m_AnimRen->CreateBoneAnimation(L"carrydrop", 9);
	m_AnimRen->CreateBoneAnimation(L"hookin", 10);
	m_AnimRen->CreateBoneAnimation(L"killing", 11);
	m_AnimRen->CreateBoneAnimation(L"exceedwindow", 12);
	m_AnimRen->CreateBoneAnimation(L"settrap", 13);
	m_AnimRen->CreateBoneAnimation(L"trapaction", 14);

	m_AnimRen->ChangeDefaultAnimation();

}

void CTest::SurvivorInit() 
{
	m_AnimRen->SetFbx(L"killedspider");
	m_AnimRen->CreateBoneAnimation(L"Out", 1);
	m_AnimRen->CreateBoneAnimation(L"Loop", 2);

	//m_AnimRen->SetFbx(L"survivor");

	/*m_AnimRen->CreateBoneAnimation(L"walkcrouch", 1);
	m_AnimRen->CreateBoneAnimation(L"walkherted", 2);
	m_AnimRen->CreateBoneAnimation(L"walkslow",3);
	m_AnimRen->CreateBoneAnimation(L"sprint", 4);
	m_AnimRen->CreateBoneAnimation(L"hertedsprint1", 5);
	m_AnimRen->CreateBoneAnimation(L"hertedsprint2", 6);
	m_AnimRen->CreateBoneAnimation(L"hitcrawl1", 7);
	m_AnimRen->CreateBoneAnimation(L"hitcrawl2", 8);
	m_AnimRen->CreateBoneAnimation(L"healing", 9);
	m_AnimRen->CreateBoneAnimation(L"healdloop", 10);
	m_AnimRen->CreateBoneAnimation(L"selfhealing", 11);
	m_AnimRen->CreateBoneAnimation(L"lockerhide", 12);
	m_AnimRen->CreateBoneAnimation(L"hurtedidle", 13);
	m_AnimRen->CreateBoneAnimation(L"hurtedcrouch", 14);
	m_AnimRen->CreateBoneAnimation(L"hookedfree", 15);
	m_AnimRen->CreateBoneAnimation(L"hookedidle", 16);
	m_AnimRen->CreateBoneAnimation(L"hookedstruggle", 17);
	m_AnimRen->CreateBoneAnimation(L"genrefair", 18);
	m_AnimRen->CreateBoneAnimation(L"genfail", 19);
	m_AnimRen->CreateBoneAnimation(L"killedspiderin", 20);
	m_AnimRen->CreateBoneAnimation(L"killedspiderloop", 21);
	m_AnimRen->CreateBoneAnimation(L"killedspiderout", 22);
	m_AnimRen->CreateBoneAnimation(L"rescue", 23);
	m_AnimRen->CreateBoneAnimation(L"rescuehooked", 24);*/


	//m_AnimRen->CreateBoneAnimation(L"carryidle", 1);
	//m_AnimRen->CreateBoneAnimation(L"carrydrop", 2);
	//m_AnimRen->CreateBoneAnimation(L"grap", 3);
	//m_AnimRen->CreateBoneAnimation(L"hookin", 4);
	//m_AnimRen->CreateBoneAnimation(L"pickup", 5);
	//m_AnimRen->CreateBoneAnimation(L"killing", 6);


	//m_AnimRen->CreateBoneAnimation(L"killedspiderloop", 21);
	//m_AnimRen->SetAnimationSubTrans(L"killedspiderloop", Vec3(0.f, -320.f, 0.f), Vec3());

	//m_AnimRen->CreateBoneAnimation(L"rescuehooked", 24);
	//m_AnimRen->SetAnimationSubTrans(L"rescuehooked", Vec3(0.f, 180.f,0.f), Vec3(0.f, 180.f, 0.f));
	m_AnimRen->ChangeDefaultAnimation();
}
