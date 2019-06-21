#pragma once
#include <Scene.h>

#define SURVIVORINITPOS Vec3(3200.f,0.f,2500.f)
#define KILLERINITPOS Vec3(3200.f,0.f,3500.f)

enum TEST_CONTROL 
{
	CONTROL_SURVIVOR,	
	CONTROL_KILLER,
	MAX_TEST_CONTROL,
};

//enum PLAYSCENE_SOUND_TYPE
//{
//	BACK1,
//	BACK2,
//	BACK3,
//	BACK4,
//	BACK5,
//	BACK6,
//	BACK_MAX,
//};

class CTestScene : public CSceneScript
{
private:
	SPTR<class CPlayer>								m_SurvivorPlayer;
	SPTR<class CSurvivorFSM>						m_Survivor;

	SPTR<class CPlayer>								m_TestSurvivorPlayer;
	SPTR<class CSurvivorFSM>						m_TestSurvivor;

	SPTR<class CUIBase>								Tem;

	SPTR<CPlayer>									m_KillerPlayer;
	SPTR<class CKillerFSM>							m_Killer;

	SPTR<class CDBDRule>							m_GameRule;

	SPTR<CCamera>									m_ControlableCamera;
	std::vector<SPTR<ActorSaveData>>				m_vecLoadActor;
	std::map<int, std::list<SPTR<CActor>>>			m_mapAllLoadActor;

	SPTR<class CParticleRenderer>					m_TestSceneParticle;

private:
	SPTR<class CMagicCircle>						m_GenMagicCircle;
	SPTR<class CMagicCircle>						m_TrapMagicCircle;

private:
	TEST_CONTROL									m_eSceneControl;

private:
	std::wstring									TreeTypeName[TREETYPEMAX];

	float											m_AccTime;
	float											m_SoundAccTime;
	//std::wstring									SoundTypeName[BACK_MAX];

public:
	void WakeUp()override;
	const bool Loading() override;
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void LoadTexture();
	void LoadFBX();

public:
	void DebugRender()override;

	void EnterTest();
	void StayTest();
	void ExitTest();

public:
	CTestScene(CScene* _pScene);
	~CTestScene();
};

