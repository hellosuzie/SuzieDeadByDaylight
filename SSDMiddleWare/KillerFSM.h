#pragma once
#include "PlayerFSM.h"
class CKillerFSM : public CPlayerFSM
{
private:
	friend CPlayerFSM;
private:
	static bool											KillerStateMap[MAX_PLAYER_STATE][MAX_PLAYER_STATE];

private:
	SPTR<class CSkeletalFbxRenderer>					m_KillerRen;
	

public:
	void Init() override;

public:
	const bool CheckTransferableState(const PLAYER_STATE& _State) override;

public:
	void Idle(const float& _DeltaTime)override;
	void Run(const float& _DeltaTime)override;
	void Attack(const float& _DeltaTime)override;
	void Damaged(const float& _DeltaTime)override;
	void GenBreak(const float& _DeltaTime)override;
	void Summons(const float& _DeltaTime)override;

protected:
	void ChangeIdleEvent()override;
	void ChangeRunEvent()override;
	void ChangeAttackEvent()override;
	void ChangeDamagedEvent()override;
	void ChangeGenBreakEvent()override;
	void ChangeSummonsEvent()override;


public:
	CKillerFSM();
	CKillerFSM(CPlayer* _pPlayer);
	~CKillerFSM();
};

