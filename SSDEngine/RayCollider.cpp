#include "RayCollider.h"



CRayCollider::CRayCollider()
{
	m_pFi = new RAY_FI();
}


CRayCollider::~CRayCollider()
{
}

void CRayCollider::LastUpdate(const float& _DeltaTime)
{
	reinterpret_cast<RAY_FI*>(m_pFi)->bCollsion = false;
}
void CRayCollider::SetRayOrigin(const Vec4& _Origin)
{
	((RAY_FI*)m_pFi)->RayOrigin = _Origin;

}
void CRayCollider::SetRayDir(const Vec4& _Dir) 
{
	((RAY_FI*)m_pFi)->RayDir = _Dir;
}
void CRayCollider::SetRayLength(const float& _Len) 
{
	((RAY_FI*)m_pFi)->RayLength = _Len;
}

const Vec4 CRayCollider::GetRayOrigin()
{
	return reinterpret_cast<RAY_FI*>(m_pFi)->RayOrigin;
}
const Vec4 CRayCollider::GetRayDir()
{
	return reinterpret_cast<RAY_FI*>(m_pFi)->RayDir;
}
const float CRayCollider::GetRayLength()
{
	return reinterpret_cast<RAY_FI*>(m_pFi)->RayLength;
}
const bool CRayCollider::IsCollision()
{
	return reinterpret_cast<RAY_FI*>(m_pFi)->bCollsion;
}