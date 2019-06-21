#pragma once
#include <Scene.h>
#include <MiddleEnum.h>

class CTestScene;
class CIntroScene : public CSceneScript
{
private:
	INTRO_STEP											m_eCurStep;
	SPTR<CTestScene>									m_GameScene;
	float												m_fAccTime;
	std::tuple<SPTR<CTestScene>, WorldData>				m_LoadingData;



public:
	const bool Loading() override;
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void DebugRender()override;

public:



public:
	void LoadTexture();
	void LoadFBX();

public:
	CIntroScene(CScene* _PairScene);
	~CIntroScene();
};

