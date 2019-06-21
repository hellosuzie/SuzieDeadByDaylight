#pragma once
#include "DXEnum.h"
#include "DXHeader.h"
#include "ReferenceBase.h"

class CFrustum : public CReferenceBase
{
private:
	class CCamera*				m_Camera;
	Vec3						m_ProjRectPos[8];
	DirectX::XMVECTOR			m_FrustumNormal[6];
	DirectX::BoundingFrustum	m_BoundingFrustum;

public:
	bool Init();
	void Update();

	bool FrustumPointCheck(Vec3 _Pos);
	bool FrustumTriangleCheck(const Vec3& _V1, const Vec3& _V2, const Vec3& _V3);
	bool FrustumSphereCheck(Vec3 _Center, float _Radius);

public:
	CFrustum(CCamera* _Camera);
	~CFrustum();
};

