#pragma once
#include "Base2DCollider.h"

class CCircleCollider : public CBase2DCollider
{


public:
	void DebugRender() override;


public:
	CCircleCollider();
	~CCircleCollider();
};

