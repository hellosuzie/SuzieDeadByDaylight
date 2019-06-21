#include "BloodScript.h"
#include <UIBase.h>
#include <Actor.h>
#include <TimerMgr.h>

CBloodScript::CBloodScript() :m_AccTime(0.f), m_AccTime1(0.f), m_FadeTime(0.8f), m_MoveBloodBase(nullptr), m_BloodBase(nullptr)
{
}


CBloodScript::~CBloodScript()
{
}


const bool CBloodScript::Init(BLOODDATA _Data)
{
	if (BT_SURVIVOR == _Data.BloodType)
	{
		m_BloodBase = ACTOR->AddUICom(UI_DATA{ UL_BASE, _Data.UICamera, UCT_BASE, L"PlayerBlood" });
		m_BloodBase->SetSubSize(Vec3(1600.f, 900.f, 0.f));
		m_BloodBase->SetSubPivot(Vec3(0.f, 0.f, 80.f));
	}
	else if (BT_KILLER == _Data.BloodType)
	{
		m_BloodBase = ACTOR->AddUICom(UI_DATA{ UL_BASE, _Data.UICamera, UCT_BASE, L"KillerBlood" });
		m_BloodBase->SetSubSize(Vec3(1600.f, 900.f, 0.f));
		m_BloodBase->SetSubPivot(Vec3(0.f, 0.f, 80.f));

		m_MoveBloodBase = ACTOR->AddUICom(UI_DATA{ UL_BASE, _Data.UICamera, UCT_BASE, L"MoveBlood" });
		m_MoveBloodBase->SetSubSize(Vec3(1600.f, 900.f, 0.f));
		m_MoveBloodBase->SetSubPivot(Vec3(-1600.f, -900.f, 80.f));
	}
	return true;
}

void CBloodScript::Update(const float & _DeltaTime)
{
	if (nullptr == m_BloodBase)
	{
		return;
	}

	if (nullptr == m_MoveBloodBase)
	{
		return;
	}

	m_AccTime += _DeltaTime;
	if (m_AccTime <= 1.5f)
	{
		m_MoveBloodBase->SetSubPivot(m_MoveBloodBase->GetSubPivot() + (Vec3(0.5f, 0.5f, 0.f) * 500.f * m_AccTime));
		return;
	}

	if (nullptr != m_MoveBloodBase)
	{
		m_MoveBloodBase->Death();
	}

	if (m_FadeTime <= m_AccTime1)
	{
		m_BloodBase->Death();
		this->Death();
		return;
	}
	m_AccTime1 += _DeltaTime;
	float Alpha = 1 - min(m_AccTime1 / m_FadeTime, 1.f);
	m_BloodBase->SetUIColorFactor(Vec4(1.f, 1.f, 1.f, Alpha));
}

void CBloodScript::LastUpdate(const float & _DeltaTime)
{
}

void CBloodScript::DebugRender()
{
}
