#pragma once
#include <BaseFSM.h>
#include <DXHeader.h>
#include "MiddleStruct.h"
#include <list>

#define INPUTINTERVAL 0.05f 
#define VISIONROTSPEEDX 15.f
#define VISIONROTSPEEDY 30.f
#define PLAYERROTSPEED 270.f

#define COLCHECKUNIT 0.1f
#define LOOKATMAGICNUMBER 0.65f


#define SURVIVORWALK 80.f
#define SURVIVORRUN 200.f
#define SURVIVORSLOWRUN 130.f
#define KILLERRUN 250.f


class CCollider;
class CPlayerFSM : public CBaseFSM
{

private:
	friend class CPlayer;
	friend class CRoom;
	friend class CDBDRule;
	
protected:
	CPlayer*								m_pPairPlayer;
	PLAYER_STATE							m_CurPlayerState;
	Vec3									m_PlayerMoveDir;
	float									m_fInputInterval;

	Vec3									m_CurCamDir;
	Vec2									m_CurMoveFlag; //(x : ÁÂ¿ì , y : ÀüÈÄ,)
	Vec3									m_CurFront;
	Vec3									m_CurRight;
	float									m_fCurSpeed;
	float									m_fAccTime;

protected:
	CPlayer*								m_TreatPlayer;
	CPlayer*								m_HookPlayer;
	CPlayer*								m_RescuedPlayer;
	class CHook*							m_SurvivorHook;
public:
	Vec3									m_GenViewDir;				


public:
	void SetLocalPlayer();
	void SetRemotePlayer();

public:
	void Init() override;
	void Update(const float& _DeltaTime) override;
	void PreUpdate(const float& _DeltaTime) override;
	virtual void DissolveEvent() {}
	virtual void HalfHookEvent() {}
	virtual void RescuedEvent() {}


protected:
	void InitializeSurvivorCollider(const bool& _Debug = true);
	void InitializeKillerCollider(const bool& _Debug = true);

protected:
	const bool CameraVisionUpdate(const float& _DeltaTime);
	const bool CheckMoveKeyInput();
	const bool CheckSelfHealing();

protected:
	const bool ChangePlayerState(const PLAYER_STATE& _State);

public:
	virtual const bool CheckTransferableState(const PLAYER_STATE& _State);

public:
	virtual void Idle(const float& _DeltaTime);
	virtual void Walk(const float& _DeltaTime);
	virtual void Run(const float& _DeltaTime);
	virtual void Attack(const float& _DeltaTime);
	virtual void Damaged(const float& _DeltaTime);
	virtual void Repair(const float& _DeltaTime);
	virtual void Healing(const float& _DeltaTime);
	virtual void Groggy(const float& _DeltaTime);
	virtual void Summons(const float& _DeltaTime);
	virtual void GenBreak(const float& _DeltaTime);
	virtual void SelfHealing(const float& _DeltaTime);
	virtual void Hook(const float& _DeltaTime);
	virtual void Rescue(const float& _DeltaTime);
	virtual void Rescued(const float& _DeltaTime);
	virtual void Dead(const float& _DeltaTime);

protected:
	virtual void ChangeIdleEvent();
	virtual void ChangeWalkEvent();
	virtual void ChangeRunEvent();
	virtual void ChangeAttackEvent();
	virtual void ChangeDamagedEvent();
	virtual void ChangeRepairEvent();
	virtual void ChangeHealingEvent();
	virtual void ChangeGroggyEvent();
	virtual void ChangeSummonsEvent();
	virtual void ChangeGenBreakEvent();
	virtual void ChangeSelfHealingEvent();
	virtual void ChangeHookEvent();
	virtual void ChangeRescueEvent();
	virtual void ChangeRescuedEvent();
	virtual void ChangeDeadEvent();

protected: 
	const bool CheckGeneratorRepair();
	const bool CheckGeneratorAttack();
	const bool CheckBoardTrapActive();
	const bool CheckBoardTrapAttack();
	const bool CheckSurvivorHealing();
	const bool CheckHookSummons();
	const bool CheckSurvivorRescued();
	const bool CheckLookAtObject(const Vec3& _ObjPos);

protected:
	void ActiveFocusBoardTrap();
	void AttackFocusBoardTrap();

private:
	void UpdatePlayerDir(const float& _DeltaTime);
	const Vec3 UpdateMoveDir(const float& _DeltaTime);

private:
	const bool UpdatePlayerData(const PlayerData& _Data);
	void SynchronizationPlayer(const PlayerData& _Data);

public:
	void ObstacleCollsionEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void ObstacleCollsionEnd(SPTR<CCollider> _This, SPTR<CCollider> _Other);

public:
	void CamObstacleCollsionEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other);

public:
	void SurvivorDamagedEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void KillerDamagedEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other);

public:
	void FocusingPlayerEvent(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void FocusingPlayerEnd(SPTR<CCollider> _This, SPTR<CCollider> _Other);

public:
	void DetectionGenerator(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void DetectionPlayer(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void DetectionTrap(SPTR<CCollider> _This, SPTR<CCollider> _Other);

public:
	CPlayer* FindTreatablePlayer();
	CPlayer* FindHookPlayer();
	CPlayer* FindRescuedPlayer();
	const bool IsFocusingPlayer(CPlayer* _Player);

public:
	const UINT GetLifeCount();
	const UINT GetDeathCount();
	const float GetHealingCount();
	const float GetHealingGauge();
	const float GetHookCount();
	const float GetRescuedCount();

public:
	CPlayerFSM();
	CPlayerFSM(CPlayer* _pPlayer);
	~CPlayerFSM();
};

#define PLAYERSCRIPT m_pPairPlayer
#define PLAYERTRANS m_pPairPlayer->GetScriptTrans()
#define PLAYERACTOR m_pPairPlayer->GetScriptActor()
