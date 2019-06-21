#pragma once
#include <Scene.h>
class CMainLoadingScene : public CSceneScript
{

private:
	class CMainPlayScene*								m_pPlayScene;
	class CAnimator2D*									m_Loaing2DAni;
	std::tuple<SPTR<class CMainPlayScene>, WorldData>	m_LoadingData;

	SPTR<class CUIBase>									m_LoadingUI;

public:
	void WakeUp()override;
	const bool Loading()override;
	void PreUpdate(const float& _DeltaTime)override;


public:
	void RecvWorldObjData(const WorldObjData& _Data);

public:
	CMainLoadingScene(CScene* _pScene);
	~CMainLoadingScene();
};

