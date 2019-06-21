#pragma once
#include "Base3DCollider.h"


class CSphereCollider : public CBase3DCollider
{

public:
	const bool Init(const COLDATA& _InitData);
	const bool Init(const COL3D_DATA& _InitData);

public:
	void SetColRadius(const float& _Radius);

public:
	void CollisionSyncro() override;


public:
	CSphereCollider();
	~CSphereCollider();
};

