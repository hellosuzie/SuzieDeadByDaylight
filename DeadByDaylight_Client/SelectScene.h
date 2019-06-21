#pragma once
#include <Scene.h>

#define INTRORESOURCECOUNT 211.f

// 게임 인트로 + 생존자 살인자 선택씬
enum INTRO_SELECT_UI
{
	INTRO_BACKUI,
	INTRO_UI,
	SELECT_SURVIVOR_UI,
	SELECT_KILLER_UI,
	SELECT_BACKUI,
	MAX_INTRO_SELECT_UI,
};

enum INTRO_SELECT_SCENE
{
	INTRO_SCENE,
	SELECT_SCENE,
	MAX_INTRO_SELECT_SCENE,
};

enum SCENE_BUILD_STEP 
{
	LOAD_GAMERESOURCE,
	BUILD_ROOMSELECT_SCENE,
	BUILD_SURVIVORLOBBY_SCENE,
	BUILD_KILLERLOBBY_SCENE,
	BUILD_SURVIVORVICTORY_SCENE,
	BUILD_KILLERVICTORY_SCENE,
	BUILD_ENDING_SCENE,
	BUILD_MAINPLAY_SCENE,
	BUILD_MAINLOADING_SCENE,
	TRY_CONNECT_SERVER,
	COMPLETE_CONNECT_SERVER,
	MAX_SCENE_BUILD_STEP
};
class CUIBase;
class CSelectScene : public CSceneScript
{
private:
	INTRO_SELECT_SCENE										m_eIntroSelectScene;
	float													m_fAccTime;
	SPTR<CUIBase>											m_IntroSelectUI[MAX_INTRO_SELECT_UI];
	float													m_fLoadingProgress;
	SPTR<class CParticleRenderer>							m_ParticleRedner;

public:
	int														m_BuildCount;


public:
	const bool Loading() override;

public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void SceneRender()override;


public:
	void IntroLoadingProgress(const float& _DeltaTime);
	void SelectProgress(const float& _DeltaTime);

public:
	float LoadingGauge(float _Percent);

public:
	void KillerEnter();
	void KillerStay();
	void KillerExit();

	void SurvivalEnter();
	void SurvivalStay();
	void SurvivalExit();

public:
	void LoadGameResource();
	void LoadTexture();
	void LoadFBX();
	void LoadSound();

public:
	void WakeUp();
	void ShutDown();

public:
	CSelectScene(CScene* _PairScene);
	~CSelectScene();
};

