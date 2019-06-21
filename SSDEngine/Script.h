#pragma once
#include "BaseComponent.h"
#include"FSMDriver.h"
#include"DXHeader.h"
#include"DXEnum.h"

class CScript : public CBaseComponent
{

protected:
	SPTR<CFSMDriver>						m_FSMDriver;


protected:
	void CreateFSM();
	

	template<typename FSM>
	FSM* SettingFSM()
	{
		return	m_FSMDriver->SettingFSM<FSM>();
	}
	template<typename FSM, typename INITPOINTER>
	FSM* SettingFSM(INITPOINTER* _pPara)
	{
		return	m_FSMDriver->SettingFSM<FSM>(_pPara);
	}

public:
	const bool Init() override;
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	virtual void DebugRender() {}


protected:
	void SetSize(const Vec3& _Pos);
	void SetPos(const Vec3& _Size);
	void ActorMove(const Vec3& _MoveVec,const float& _DeltaTime);

public:
	CTransForm* GetScriptTrans();
	CActor* GetScriptActor();

protected:
	CActor * CreateActor(const ACTOR_UPDATE_LAYER& _Layer, const std::wstring& _Name);

public:
	CScript();
	CScript(CActor* _Actor);
	~CScript();
};

