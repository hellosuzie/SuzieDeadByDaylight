#include "FootPrints.h"
#include <DecalRenderer.h>
#include <Actor.h>

CFootPrints::CFootPrints() : m_DecalRendrer(nullptr), m_FootTimer(8.f)
{
}


CFootPrints::~CFootPrints()
{
}

const bool CFootPrints::Init(Vec3 _FootPos)
{
	m_DecalRendrer = ACTOR->AddCom<CDecalRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	m_DecalRendrer->SetRenSize(Vec3(200.f, 25.f, 200.f));
	m_DecalRendrer->SetRenPivot(Vec3(_FootPos));
	m_DecalRendrer->SetDecalTex(L"FootPrints");
	m_DecalRendrer->SetDecalNormalTex(L"FootPrints_N");
	return true;
}

void CFootPrints::Update(const float & _DeltaTime)
{
	m_FootTimer.Update(CTimerMgr::DeltaTime());
	if (true == m_FootTimer.CheckPulse())
	{
		m_DecalRendrer->Off();
		Death();
	}
}

void CFootPrints::LastUpdate(const float & _DeltaTime)
{
}

void CFootPrints::DebugRender()
{
}


