#pragma once
#include "Base3DCollider.h"


class COBBCollider : public CBase3DCollider
{

public:
	const bool Init(const COLDATA& _InitData);
	const bool Init(const COL3D_DATA& _InitData);

public:
	void CollisionSyncro() override;
	const Vec3 GetColPos() override;
public:
	const float GetRayCollisionLength();

public:
	void ObbDebugOff();
	void ObbDebugOn();

public:
	COBBCollider();
	~COBBCollider();
};

