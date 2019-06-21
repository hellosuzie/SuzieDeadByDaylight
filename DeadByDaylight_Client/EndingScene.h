#pragma once
#include <Scene.h>

class CEndingScene : public CSceneScript
{
	SPTR<CUIBase>							m_EndingUI;
	SPTR<CUIBase>							m_WinUI;
	SPTR<CUIBase>							m_DefeatUI;

public:
	void WakeUp() override;
	const bool Loading() override;


	void DebugRender()override;
public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;


public:
	CEndingScene(CScene* _PairScene);
	~CEndingScene();
};

