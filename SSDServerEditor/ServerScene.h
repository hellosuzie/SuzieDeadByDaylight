#pragma once
#include <Scene.h>

class CServerScene: public CSceneScript
{
	SPTR<class CDXFont>					m_DebugFont;


public:
	const bool Loading() override;
	void DebugRender()override;

public:
	CServerScene(CScene* _PairScene);
	~CServerScene();

};

