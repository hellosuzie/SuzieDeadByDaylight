#pragma once
#include "Base3DCollider.h"



class CRayCollider : public CBase3DCollider
{

public:
	void LastUpdate(const float& _DeltaTime);

public:
	void SetRayOrigin(const Vec4& _Origin);
	void SetRayDir(const Vec4& _Dir);
	void SetRayLength(const float& _Len);

	const Vec4 GetRayOrigin();
	const Vec4 GetRayDir();
	const float GetRayLength();
	const bool IsCollision();
public:
	CRayCollider();
	~CRayCollider();
};

