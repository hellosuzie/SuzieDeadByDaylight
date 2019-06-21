#pragma once
#include <Scene.h>

enum PLAYSCENE_SOUND_TYPE
{
	BACK1,
	BACK2,
	BACK3,
	BACK4,
	BACK5,
	BACK6,
	BACK_MAX,
};


class CMainPlayScene : public CSceneScript
{
private:
	SPTR<class CDBDRule>											m_GameRule;
	std::wstring													TreeTypeName[TREETYPEMAX];
	SPTR<class CParticleRenderer>									m_SceneParticle;

public:
	float															m_SoundAccTime;
	std::wstring													SoundTypeName[BACK_MAX];

private:


public:
	const bool Loading() override;
	const bool AsyncLoading(void* _Arg) override;
	void DebugRender()override;
	void CreateWorldObjData(const WorldObjData& _Data);

public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;


	void ScenePacketProcessing(const float& _DeltaTime);
	void WakeUp() override;

public:
	CMainPlayScene(CScene* _PairScene);
	~CMainPlayScene();
};

