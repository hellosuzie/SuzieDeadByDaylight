#pragma once
#include <Script.h>

class CPlayerCamScript
	: public CScript
{
private:
	SPTR<class CActor>					m_PlayerCamActor;
	SPTR<class CCamera>					m_PlayerCam;


public:
	CPlayerCamScript();
	~CPlayerCamScript();

public:
	const bool Init(SPTR<class CActor> _ParentActor);
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender() override;
};

