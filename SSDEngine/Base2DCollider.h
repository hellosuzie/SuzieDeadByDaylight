#pragma once
#include "Collider.h"

struct BASE2D_FI :public BASE_FI
{
public:
	Vec4						m_Fi;

public:
	BASE2D_FI() : BASE_FI(MAXCOLTYPE), m_Fi(Vec4()) {}
	BASE2D_FI(const COLTYPE& _Type) : BASE_FI(_Type), m_Fi(Vec4()) {}
	~BASE2D_FI() {}
};

class CBase2DCollider : public CCollider
{
protected:
	Vec2			m_ColSize;
	Vec3			m_ColPivot;
	Vec3			m_DebugColor;
	float			m_fBorderSize;
	
public:
	void SetColSize(const Vec2& _Size) { m_ColSize = _Size; SetSubSize(Vec3(_Size.x, _Size.y,1.f)); }
	void SetColPivot(const Vec3& _Pivot) { m_ColPivot = _Pivot; SetSubPivot(m_ColPivot);}
	void SetColFi(const Vec3& _Pivot, const Vec2& _Size)
	{
		m_ColPivot = _Pivot; m_ColSize = _Size;
		SetSubSize(Vec3(_Size.x, _Size.y, 1.f));
		SetSubPivot(m_ColPivot);
	}
	void SetDebugColor(const Vec3& _Color) { m_DebugColor = _Color; }
	void SetBorderSize(const float& _Border) {m_fBorderSize = _Border;}
	
public:
	void CollisionSyncro() override;
	const Vec3 GetColliderPivot() override;
	const Vec3 GetColliderSize() override;

public:
	CBase2DCollider();
	~CBase2DCollider();
};

