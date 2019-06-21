#pragma once
#include <Scene.h>

class CKillerVictoryScene : public CSceneScript
{


public:
	const bool Loading() override;

	void DebugRender()override;
public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	CKillerVictoryScene(CScene* _PairScene);
	~CKillerVictoryScene();
};
