#pragma once
#include<CameraScript.h>

#define ROTBOOST 2.f
#define MOVEBOOST 10.f

class CFreeCameraScript : public CCameraScript
{
private:
	SPTR<class CLight>			m_CamLight;
	float						m_fRotSpeed;
	float						m_fMoveSpeed;

public:
	const bool Init()override;
	void Update(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;

private:
	void FreeMoveUpdate(const float& _DeltaTime);
	void FreeRotateUpdate(const float& _DeltaTime);

public:
	CFreeCameraScript();
	~CFreeCameraScript();
};

