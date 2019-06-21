#pragma once
#include <Script.h>

#define SPIDER		 0
#define SPIDERCELING 1
#define HOOK		 2	


enum HOOK_STATE 
{
	HOOK_UNACTIVE,
	HOOK_SUMMONS,
	HOOK_IDLE,
	HOOK_SPIDERIN,
	HOOK_SPIDERLOOP,
	HOOK_SPIDEROUT,
	HOOK_EXTINCTION,
	MAX_HOOK_STATE,
};

class CHook : public CScript
{
private:
	friend class CDBDRule;

private:
	CDBDRule*											m_pRule;
	SPTR<class CDissolveSkeletalRenderer>				m_HookRen;
	SPTR<class CDissolveSkeletalRenderer>				m_HookSpiderRen;
	SPTR<class COBBCollider>							m_HookCollider;
	HOOK_STATE											m_eCurState;
	float												m_fAccTime;

public:
	const bool Init()override;
	void Update(const float& _DeltaTime)override;
	void ChangeHookState(const HOOK_STATE& _State);

public:
	void Summons(const float& _DeltaTime);
	void Idle(const float& _DeltaTime);
	void SpiderIn(const float& _DeltaTime);
	void SpiderLoop(const float& _DeltaTime);
	void SpiderOut(const float& _DeltaTime);
	void Extinction(const float& _DeltaTime);

public:
	void HookActive(const UINT& _DeathCount,class CPlayer* _pPlayer);
	void SpiderActive();
	void HookExtinction();

public:
	CHook();
	~CHook();
};

