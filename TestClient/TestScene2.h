#pragma once
#include <Scene.h>
class CTestScene2 : public CSceneScript
{
	SPTR<class CTraceScript>						m_PlayerTrace;
	SPTR<class CPlayer>								m_Player;
	SPTR<class CNavigator>							m_PlayerNavigator;

public:
	const bool Loading() override;
	void PostUpdate(const float& _DeltaTime) override;

public:
	void LoadTexture();
	void LoadFBX();

public:
	void DebugRender()override;

public:
	CTestScene2(CScene* _pScene);
	~CTestScene2();
};

