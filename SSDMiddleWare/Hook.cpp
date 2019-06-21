#include "Hook.h"
#include <SkeletalFbxRenderer.h>
#include <DissolveSkeletalRenderer.h>
#include <Actor.h>
#include <Transform.h>
#include <KeyMgr.h>
#include <OBBCollider.h>
#include "DeadByDaylightGlobal.h"
#include "DBDRule.h"
#include "Player.h"

CHook::CHook()
	:m_HookRen(nullptr), m_HookSpiderRen(nullptr), m_HookCollider(nullptr)
	, m_eCurState(HOOK_UNACTIVE), m_fAccTime(0.f), m_pRule(nullptr)
{
}


CHook::~CHook()
{
}

const bool CHook::Init()
{
	TRANS->SetRotate(Vec3(-90.f, 0.f, 0.f));
	TRANS->SetScale(Vec3(0.25f, 0.25f, 0.25f));

	//m_HookRen = ACTOR->AddCom<CSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//m_HookRen->SetFbx(L"killedspider");
	//m_HookRen->CreateBoneAnimation(L"Idle", 0, 0, 3)->SetAnimationSpeed(0.05f);
	//m_HookRen->CreateBoneAnimation(L"Out", 1);
	//m_HookRen->CreateBoneAnimation(L"Loop", 2);

	m_HookRen = ACTOR->AddCom<CDissolveSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_HookRen->SetFbx(L"killedspider");
	m_HookRen->PushFbxMesh(HOOK);
	m_HookRen->CreateBoneAnimation(L"Idle", 0, 0, 3)->SetAnimationSpeed(0.05f);
	m_HookRen->CreateBoneAnimation(L"Out", 1);
	m_HookRen->CreateBoneAnimation(L"Loop", 2);

	m_HookSpiderRen = ACTOR->AddCom<CDissolveSkeletalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_HookSpiderRen->SetFbx(L"killedspider");
	m_HookSpiderRen->PushFbxMesh(SPIDER);
	m_HookSpiderRen->PushFbxMesh(SPIDERCELING);
	m_HookSpiderRen->CreateBoneAnimation(L"Idle", 0, 0, 3)->SetAnimationSpeed(0.05f);
	m_HookSpiderRen->CreateBoneAnimation(L"Out", 1);
	m_HookSpiderRen->CreateBoneAnimation(L"Loop", 2);


	//m_HookRen->DrawOff(SPIDER);
	//m_HookRen->DrawOff(SPIDERCELING);
	//m_HookRen->DrawOff(HOOK);
	

	m_HookRen->ChangeBoneAnimation(L"Idle");
	m_HookSpiderRen->ChangeBoneAnimation(L"Idle");
	m_HookSpiderRen->Off();
	return true;
}

void CHook::Update(const float& _DeltaTime)
{
	switch (m_eCurState)
	{
	case HOOK_SUMMONS:
		Summons(_DeltaTime);
		break;
	case HOOK_IDLE:
		Idle(_DeltaTime);
		break;
	case HOOK_SPIDERIN:
		SpiderIn(_DeltaTime);
		break;
	case HOOK_SPIDERLOOP:
		SpiderLoop(_DeltaTime);
		break;
	case HOOK_SPIDEROUT:
		SpiderOut(_DeltaTime);
		break;
	case HOOK_EXTINCTION:
		Extinction(_DeltaTime);
		break;
	case MAX_HOOK_STATE:
		return;
	default:
		return;
	}

	m_fAccTime += _DeltaTime;
}

void CHook::ChangeHookState(const HOOK_STATE& _State)
{
	if (m_eCurState == _State)
	{
		return;
	}

	m_eCurState = _State;

	switch (m_eCurState)
	{
	case HOOK_UNACTIVE:
		break;
	case HOOK_SUMMONS:
		break;
	case HOOK_IDLE:
		break;
	case HOOK_SPIDERIN:
		m_HookRen->ChangeBoneAnimation(DEFANIM);
		m_HookSpiderRen->ChangeBoneAnimation(DEFANIM);
		break;
	case HOOK_SPIDERLOOP:
		m_HookRen->ChangeBoneAnimation(L"Loop");
		m_HookSpiderRen->ChangeBoneAnimation(L"Loop");
		break;
	case HOOK_SPIDEROUT:
		m_HookRen->ChangeBoneAnimation(L"Out");
		m_HookSpiderRen->ChangeBoneAnimation(L"Out");
		break;
	case HOOK_EXTINCTION:
		break;
	case MAX_HOOK_STATE:
		break;
	default:
		break;
	}
	m_fAccTime = 0.f;
}
void CHook::Summons(const float& _DeltaTime) 
{
	if (m_fAccTime >= 1.51f) 
	{
		ChangeHookState(HOOK_IDLE);
		return;
	}

}
void CHook::Idle(const float& _DeltaTime) 
{

}
void CHook::SpiderIn(const float& _DeltaTime) 
{
	if (m_fAccTime >= 3.f) 
	{
		ChangeHookState(HOOK_SPIDERLOOP);
		return;
	}
}
void CHook::SpiderLoop(const float& _DeltaTime) 
{
	if (m_fAccTime >= 2.f)
	{
		ChangeHookState(HOOK_SPIDEROUT);
		return;
	}
}
void CHook::SpiderOut(const float& _DeltaTime) 
{
	if (m_fAccTime >= 8.f)
	{
		HookExtinction();
		return;
	}


}
void CHook::Extinction(const float& _DeltaTime) 
{
	if (m_fAccTime >= 2.f) 
	{
		ChangeHookState(HOOK_UNACTIVE);
		ActorOff();
		m_pRule->PushHook(this);
		return;
	}
}

void CHook::HookActive(const UINT& _DeathCount, CPlayer* _pPlayer)
{
	ActorOn();
	Vec3 Back = _pPlayer->TRANS->GetDown();
	Vec3 Down = _pPlayer->TRANS->GetBack();
	TRANS->SetPos(_pPlayer->GetWorldPos() + Back * 80.f + Down * 20.f);
	TRANS->SetRotate(_pPlayer->TRANS->GetRotate());
	ChangeHookState(HOOK_SUMMONS);

	switch (_DeathCount)
	{
	case 1:
	case 2:
	{
		m_HookRen->ChangeBoneAnimation(L"Idle");
		m_HookRen->SetReverseDissolve(1.5f);
		m_HookSpiderRen->Off();
	}
		break;
	case 3: 
	{
		m_HookRen->ChangeBoneAnimation(L"Idle");
		m_HookRen->SetReverseDissolve(1.5f);
		m_HookSpiderRen->ChangeBoneAnimation(L"Idle");
		m_HookSpiderRen->SetReverseDissolve(1.5f);
	}
		break;
	default:
		break;
	}

}
void CHook::SpiderActive()
{
	m_HookSpiderRen->ChangeBoneAnimation(L"Idle");
	m_HookSpiderRen->SetReverseDissolve(20.f);
}
void CHook::HookExtinction() 
{
	m_HookRen->SetDissolve(2.f);
	ChangeHookState(HOOK_EXTINCTION);


	if (true == m_HookSpiderRen->IsActive()) 
	{
		m_HookSpiderRen->SetDissolve(2.f);
	}
}

