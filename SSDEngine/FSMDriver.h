#pragma once
#include"GameObject.h"
#include"Engine.h"
#include"BaseFSM.h"


class CFSMDriver : public CGameObject
{
private:
	friend Engine;
	friend class CScript;

private:
	SPTR<class CBaseFSM>					m_MainFSM;


private:
	template<typename FSM>
	FSM* SettingFSM() 
	{
		if (false == Dynamic_Cast_Check<CBaseFSM, FSM>::IsParent)
			return nullptr;

		FSM* NewFSM = Engine::CreateGameObject<FSM>();
		
		if (nullptr == NewFSM)
			return nullptr;


		m_MainFSM = (CBaseFSM*)NewFSM;
		m_MainFSM->Init();
		return NewFSM;
	}

	template<typename FSM,typename INITPOINTER>
	FSM* SettingFSM(INITPOINTER* _pPara)
	{
		if (false == Dynamic_Cast_Check<CBaseFSM, FSM>::IsParent)
			return nullptr;

		FSM* NewFSM = Engine::CreateGameObject<FSM>(_pPara);

		if (nullptr == NewFSM)
			return nullptr;

		m_MainFSM = (CBaseFSM*)NewFSM;
		m_MainFSM->Init();
		return NewFSM;
	}


public:
	void Update(const float& _DeltaTime);

private:
	CFSMDriver();
	~CFSMDriver();
};

