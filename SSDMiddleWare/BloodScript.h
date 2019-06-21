#pragma once
#include <Script.h>

enum BLOOD_TYPE
{
	BT_SURVIVOR,
	BT_KILLER,
};

struct BLOODDATA
{
	SPTR<class CCamera>		UICamera;
	BLOOD_TYPE				BloodType;
};

class CBloodScript
	: public CScript
{
private:
	SPTR<class CUIBase>		m_BloodBase;
	SPTR<class CUIBase>		m_MoveBloodBase;

private:
	float					m_AccTime;
	float					m_AccTime1;
	float					m_FadeTime;

public:
	const bool Init(BLOODDATA _Data);
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender() override;

public:
	CBloodScript();
	~CBloodScript();
};

