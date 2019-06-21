#pragma once
#include <Script.h>
#include <TimerMgr.h>

class CFootPrints
	: public CScript
{
private:
	SPTR<class CDecalRenderer>		m_DecalRendrer;
	CTimerMgr::CPulse				m_FootTimer;

public:
	const bool Init(Vec3 _FootPos);
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender() override;

public:
	CFootPrints();
	~CFootPrints();
};

