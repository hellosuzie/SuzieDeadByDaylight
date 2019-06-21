#pragma once
#include "Base3DCollider.h"

class CFrustumCollider : public CBase3DCollider
{
	SPTR<class CCamera>				m_FrustumCamera;
	Mat								m_FrustumProj;


public:
	const bool Init(const COLDATA& _InitData);
	const bool Init(const COL3D_DATA& _InitData);
	void PostUpdate(const float& _DeltaTime)override;

public:
	void SupplyFrustumMatrix();

public:
	void CollisionSyncro() override;

public:
	void SetFrustumFi(SPTR<class CCamera> _Cam,const float& FovyAngle, const float& Aspect,const float& _Near, const float& _Far);
	void SetFrustumNear(const float& _Near);
	void SetFrustumFar(const float& _Far);

public:
	CFrustumCollider();
	~CFrustumCollider();
};

