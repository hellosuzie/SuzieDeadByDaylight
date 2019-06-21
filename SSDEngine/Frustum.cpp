#include "Frustum.h"
#include "Camera.h"
#include "TransForm.h"

CFrustum::CFrustum(CCamera* _Camera) : m_Camera(_Camera), m_ProjRectPos{}
{
}


CFrustum::~CFrustum()
{
}


bool CFrustum::Init()
{
	m_ProjRectPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_ProjRectPos[1] = Vec3(1.f, 1.f, 0.f);
	m_ProjRectPos[2] = Vec3(1.f, -1.f, 0.f);
	m_ProjRectPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_ProjRectPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_ProjRectPos[5] = Vec3(1.f, 1.f, 1.f);
	m_ProjRectPos[6] = Vec3(1.f, -1.f, 1.f);
	m_ProjRectPos[7] = Vec3(-1.f, -1.f, 1.f);
	return true;
}

void CFrustum::Update()
{
	const Mat& View = m_Camera->GetViewMatrix();
	const Mat& Proj = m_Camera->GetFrustumProjMatrix();

	m_BoundingFrustum = DirectX::BoundingFrustum(Proj);
	m_BoundingFrustum.Origin = m_Camera->TRANS->GetWorldPos();
	m_BoundingFrustum.Orientation = m_Camera->TRANS->GetCRWorldMat().GetAxisQuaternion().XMF4;


	Mat InverseVP = View * Proj;
	InverseVP = DirectX::XMMatrixInverse(NULL, InverseVP);

	Vec3 WorldPos[8] = {};
	for (size_t i = 0; i < 8; ++i)
	{
		WorldPos[i] = DirectX::XMVector3TransformCoord(m_ProjRectPos[i].GetVec4_Coord(), InverseVP);
	}
	m_FrustumNormal[FT_NEAR] = DirectX::XMPlaneFromPoints(WorldPos[0].GetVec4_Coord(), WorldPos[1].GetVec4_Coord(), WorldPos[2].GetVec4_Coord());
	m_FrustumNormal[FT_FAR] = DirectX::XMPlaneFromPoints(WorldPos[6].GetVec4_Coord(), WorldPos[5].GetVec4_Coord(), WorldPos[4].GetVec4_Coord());
	m_FrustumNormal[FT_LEFT] = DirectX::XMPlaneFromPoints(WorldPos[4].GetVec4_Coord(), WorldPos[0].GetVec4_Coord(), WorldPos[7].GetVec4_Coord());
	m_FrustumNormal[FT_RIGHT] = DirectX::XMPlaneFromPoints(WorldPos[1].GetVec4_Coord(), WorldPos[5].GetVec4_Coord(), WorldPos[6].GetVec4_Coord());
	m_FrustumNormal[FT_TOP] = DirectX::XMPlaneFromPoints(WorldPos[0].GetVec4_Coord(), WorldPos[4].GetVec4_Coord(), WorldPos[5].GetVec4_Coord());
	m_FrustumNormal[FT_BOT] = DirectX::XMPlaneFromPoints(WorldPos[2].GetVec4_Coord(), WorldPos[6].GetVec4_Coord(), WorldPos[7].GetVec4_Coord());
}

bool CFrustum::FrustumPointCheck(Vec3 _Pos)
{
	DirectX::XMVECTOR vRet;
	for (UINT i = 0; i < (UINT)FRUSTUM_TYPE::FT_END; ++i)
	{
		vRet = DirectX::XMPlaneDotCoord(m_FrustumNormal[i], _Pos.GetVec4_Coord());
		if (0.f < vRet.m128_f32[0])
		{
			return false;
		}
	}
	return true;
}
bool CFrustum::FrustumTriangleCheck(const Vec3& _V1, const Vec3& _V2, const Vec3& _V3)
{
	return m_BoundingFrustum.Intersects(_V1, _V2, _V3);
}
bool CFrustum::FrustumSphereCheck(Vec3 _Center, float _Radius)
{
	DirectX::XMVECTOR vRet;
	for (UINT i = 0; i < (UINT)FRUSTUM_TYPE::FT_END; ++i)
	{
		vRet = DirectX::XMPlaneDotCoord(m_FrustumNormal[i], _Center.GetVec4_Coord());
		if (_Radius < vRet.m128_f32[0])
		{
			return false;
		}
	}
	return true;
}

