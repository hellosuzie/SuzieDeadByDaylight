#include "SphereCollider.h"
#include "DebugRenderer.h"
#include "Actor.h"



CSphereCollider::CSphereCollider()
{
	m_pFi = new SPHERE_FI();
}


CSphereCollider::~CSphereCollider()
{

}

const bool CSphereCollider::Init(const COLDATA& _InitData)
{
	m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_SPHERE });
	SetColFi(Vec3(), Vec3(1.f, 1.f, 1.f));

	return CCollider::Init(_InitData);
}
const bool CSphereCollider::Init(const COL3D_DATA& _InitData)
{
	if (true == _InitData.DebugRen) 
	{
		m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_SPHERE });
	}

	SetColFi(Vec3(), Vec3(1.f, 1.f, 1.f));

	return CCollider::Init(_InitData.ColData);
}
void CSphereCollider::SetColRadius(const float& _Radius)
{
	SetColSize(Vec3(_Radius, _Radius, _Radius));
}
void CSphereCollider::CollisionSyncro()
{
	SubTransUpdate(); 

	Mat World = GetSubTransCRWorld();

	((SPHERE_FI*)m_pFi)->SphereFi.Center = World.Row[3];
	((SPHERE_FI*)m_pFi)->SphereFi.Radius = World.GetScaleFactor().x ;

}