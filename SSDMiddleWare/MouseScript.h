#pragma once
#include <CameraScript.h>
class CMouseScript : public CCameraScript
{
private:
	SPTR<class CRayCollider>					m_MouseCol;

public:
	const bool Init()override;
	void MountingCameraEvent()override;
	void LastCamScriptUpdate(const float& _DeltaTime)override;

public:
	SPTR<CRayCollider> GetMouseRay();
public:
	CMouseScript();
	~CMouseScript();
};

