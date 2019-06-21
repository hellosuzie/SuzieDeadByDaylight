#pragma once
#include "PlayerFSM.h"



class CSurvivorFSM : public CPlayerFSM
{
private:
	friend CPlayerFSM;

private:
	static bool											SurvivorStateMap[MAX_PLAYER_STATE][MAX_PLAYER_STATE];

private:
	SPTR<class CDissolveSkeletalRenderer>				m_SurvivorRen;
	SPTR<class CHook>									m_SurvivorHook;
	bool												m_bGroggyMotion;


public:
	void Init() override;
	void PostUpdate(const float& _DeltaTime);

public:
	const bool CheckTransferableState(const PLAYER_STATE& _State) override;

public:
	void Idle(const float& _DeltaTime)override;
	void Walk(const float& _DeltaTime)override;
	void Run(const float& _DeltaTime)override;
	void Repair(const float& _DeltaTime)override;
	void Damaged(const float& _DeltaTime)override;
	void Groggy(const float& _DeltaTime)override;
	void Summons(const float& _DeltaTime)override;
	void Dead(const float& _DeltaTime)override;
	void Healing(const float& _DeltaTime)override;
	void Hook(const float& _DeltaTime)override;
	void Rescue(const float& _DeltaTime) override;
	void Rescued(const float& _DeltaTime) override;
	
protected:
	void ChangeIdleEvent()override;
	void ChangeWalkEvent()override;
	void ChangeRunEvent()override;
	void ChangeRepairEvent()override;
	void ChangeDamagedEvent()override;
	void ChangeGroggyEvent()override;
	void ChangeSummonsEvent()override;
	void ChangeHealingEvent()override;
	void ChangeSelfHealingEvent()override;
	void ChangeHookEvent()override;
	void ChangeDeadEvent()override;
	void ChangeRescueEvent()override;
	void ChangeRescuedEvent()override;

public:
	void DissolveEvent()override;
	void HalfHookEvent() override;
	void RescuedEvent() override;


public:
	CSurvivorFSM();
	CSurvivorFSM(CPlayer* _pPlayer);
	~CSurvivorFSM();
};

