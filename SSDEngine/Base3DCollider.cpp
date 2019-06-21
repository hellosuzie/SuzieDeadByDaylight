#include "Base3DCollider.h"
#include "DebugRenderer.h"


CBase3DCollider::CBase3DCollider()
	:m_DebugRen(nullptr)
{

}


CBase3DCollider::~CBase3DCollider()
{
}


 void CBase3DCollider::CollisionEvent()
 {
	 if (nullptr != m_DebugRen) 
	 {
		 m_DebugRen->SetColorFactor(Vec3(1.f, 0.f, 0.f));
	 }


	 return CCollider::CollisionEvent();
 }
 void CBase3DCollider::CollisionEndEvent() 
 {
	 if (nullptr != m_DebugRen)
	 {
		 m_DebugRen->SetColorFactor(m_DebugColor);
	 }

 
	 return CCollider::CollisionEndEvent();
 }

 void CBase3DCollider::SetDebugColor(const Vec3& _Color)
 {
	 CCollider::SetDebugColor(_Color);
	 if (nullptr != m_DebugRen)
	 {
		 m_DebugRen->SetColorFactor(m_DebugColor);
	 }
 }


void CBase3DCollider::SetColPivot(const Vec3& _Pivot) 
{
	CCollider::SetColPivot(_Pivot);

	if (nullptr != m_DebugRen)
		m_DebugRen->SetRenPivot(_Pivot);
}
void CBase3DCollider::SetColSize(const Vec3& _Size) 
{
	CCollider::SetColSize(_Size);

	if (nullptr != m_DebugRen)
		m_DebugRen->SetRenSize(_Size);
}
void CBase3DCollider::SetColFi(const Vec3& _Pivot, const Vec3& _Size)
{
	CCollider::SetColFi(_Pivot, _Size);

	if (nullptr != m_DebugRen) 
	{
		m_DebugRen->SetRenPivot(_Pivot);
		m_DebugRen->SetRenSize(_Size);
	}
}

void CBase3DCollider::SetBorderSize(const float& _BoarderSize)
{
	CCollider::SetBorderSize(_BoarderSize);

	if (nullptr != m_DebugRen) 
	{
		m_DebugRen->SetBoaderSize(m_fBorderSize);
	}
}

void CBase3DCollider::RemoveDebugRenderer()
{
	m_DebugRen->Death();
	m_DebugRen = nullptr;
}

void CBase3DCollider::On()
{
	CCollider::On();

	if (nullptr != m_DebugRen) 
	{
		m_DebugRen->On();
	}
}
void CBase3DCollider::Off()
{
	CCollider::Off();

	if (nullptr != m_DebugRen)
	{
		m_DebugRen->Off();
	}
}