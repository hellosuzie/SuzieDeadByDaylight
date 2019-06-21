#pragma once
#include <CameraScript.h>

class CTraceScript : public CCameraScript
{
private:
	Vec3									m_TracePivot;
	SPTR<CTransForm>						m_TraceTrans;
	SPTR<class CGizmo>						m_CameraGizmo;

public:
	const bool Init()override;
	void Update(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;

public:
	void SetTraceTrans(SPTR<CTransForm> _pTrans, const Vec3& _Pivot = Vec3());
	void GizmoTransEvent(const Vec3& _Dir);
	const Vec3 GetTracePivot() { return m_TracePivot; }


public:
	CTraceScript();
	~CTraceScript();
};

