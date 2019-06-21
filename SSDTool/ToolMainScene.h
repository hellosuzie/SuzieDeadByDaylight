#pragma once
#include <Scene.h>

class CToolMainScene : public CSceneScript
{
	SPTR<class CDXFont>							m_DebugFont;
	SPTR<class CSkeletalFbxRenderer>			m_AnimRen;
	SPTR<class CLight>							m_Light;
	SPTR<class CBloomFilter>					TempBloom;

	SPTR<CActor>								LightActor;
	SPTR<class CTraceScript>					m_PlayerTrace;

private:
	SPTR<class CPlayer>							m_ToolPlayer;
	SPTR<class CNavigator>						m_PlayerNavigator;
	SPTR<class CFbxRenderer>					FbxRen1;

	SPTR<class CPlayer>							m_SurvivorPlayer;
	SPTR<class CSurvivorFSM>					m_Survivor;
	SPTR<class CDBDRule>						m_GameRule;
	SPTR<CCamera>								m_ControlableCamera;

public:
	const bool Loading() override;
	void DebugRender()override;

	void TestFunc(void* _Para);
	void TestFunc2(void* _Para);

	void PreUpdate(const float& _DeltaTime);
	void PostUpdate(const float& _DeltaTime);
	void LastUpdate(const float& _DeltaTime);


public:
	void ChangePlayerVision();
	void ChangeFreeCameraVision();

public:
	void CreateToolPlayer();

public:
	CToolMainScene(class CScene* _PairScene);
	~CToolMainScene();
};

