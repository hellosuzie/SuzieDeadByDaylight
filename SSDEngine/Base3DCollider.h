#pragma once
#include "Collider.h"


struct COL3D_DATA 
{
	COLDATA			ColData;
	bool			DebugRen;

public:
	COL3D_DATA():ColData(COLDATA()), DebugRen(true) {}
	COL3D_DATA(const COLDATA& _Col, const bool DebugRen = true)
		: ColData(_Col), DebugRen(DebugRen)
	{}
	~COL3D_DATA() {}
};

class CBase3DCollider : public CCollider
{
protected:
	SPTR<class CDebugRenderer>						m_DebugRen;

protected:
	void CollisionEvent()override;
	void CollisionEndEvent()override;

public:
	void SetDebugColor(const Vec3& _Color)override;
	void SetColPivot(const Vec3& _Pivot) override;
	void SetColSize(const Vec3& _Size) override;
	void SetColFi(const Vec3& _Pivot, const Vec3& _Size) override;
	void SetBorderSize(const float& _BoarderSize) override;

	const Vec3 GetColliderPivot() { return m_ColPivot; }
	const Vec3 GetColliderSize() { return m_ColSize; }

public:
	void RemoveDebugRenderer();

public:
	void On()override;
	void Off()override;

public:
	CBase3DCollider();
	~CBase3DCollider();
};

