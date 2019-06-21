#include "OBBCollider.h"
#include "DebugRenderer.h"
#include "Actor.h"
#include "Material.h"

COBBCollider::COBBCollider()
{
	m_pFi = new OBB_FI();
	
}


COBBCollider::~COBBCollider()
{
}


const bool COBBCollider::Init(const COLDATA& _InitData)
{
	m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG,DF_CUBE });
	
	SetColFi(Vec3(), Vec3(1.f, 1.f, 1.f));

	return CCollider::Init(_InitData);
}
const bool COBBCollider::Init(const COL3D_DATA& _InitData)
{
	if (true == _InitData.DebugRen) 
	{
		m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG,DF_CUBE });
	}

	SetColFi(Vec3(), Vec3(1.f, 1.f, 1.f));

	return CCollider::Init(_InitData.ColData);
}
void COBBCollider::CollisionSyncro()
{
	SubTransUpdate();
	Mat World = GetSubTransCRWorld();
	Vec3 ObbSize = World.GetScaleFactor().vec3 * 0.5f;

	((OBB_FI*)m_pFi)->ObbFi.Center = World.Row[3];
	((OBB_FI*)m_pFi)->ObbFi.Extents = ObbSize.XMF3;
	((OBB_FI*)m_pFi)->ObbFi.Orientation = World.GetAxisQuaternion().XMF4;
}
const Vec3 COBBCollider::GetColPos()
{
	DirectX::XMFLOAT3 Center = ((OBB_FI*)m_pFi)->ObbFi.Center;
	return Vec3(Center.x, Center.y, Center.z);
}
const float COBBCollider::GetRayCollisionLength()
{
	return reinterpret_cast<OBB_FI*>(m_pFi)->CollisionLength;
}

void COBBCollider::ObbDebugOff()
{
	m_DebugRen->Off();
}

void COBBCollider::ObbDebugOn()
{
	m_DebugRen->On();
}